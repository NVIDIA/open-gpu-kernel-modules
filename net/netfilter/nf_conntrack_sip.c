// SPDX-License-Identifier: GPL-2.0-only
/* SIP extension for IP connection tracking.
 *
 * (C) 2005 by Christian Hentschel <chentschel@arnet.com.ar>
 * based on RR's ip_conntrack_ftp.c and other modules.
 * (C) 2007 United Security Providers
 * (C) 2007, 2008 Patrick McHardy <kaber@trash.net>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/skbuff.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_zones.h>
#include <linux/netfilter/nf_conntrack_sip.h>

#define HELPER_NAME "sip"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Hentschel <chentschel@arnet.com.ar>");
MODULE_DESCRIPTION("SIP connection tracking helper");
MODULE_ALIAS("ip_conntrack_sip");
MODULE_ALIAS_NFCT_HELPER(HELPER_NAME);

#define MAX_PORTS	8
static unsigned short ports[MAX_PORTS];
static unsigned int ports_c;
module_param_array(ports, ushort, &ports_c, 0400);
MODULE_PARM_DESC(ports, "port numbers of SIP servers");

static unsigned int sip_timeout __read_mostly = SIP_TIMEOUT;
module_param(sip_timeout, uint, 0600);
MODULE_PARM_DESC(sip_timeout, "timeout for the master SIP session");

static int sip_direct_signalling __read_mostly = 1;
module_param(sip_direct_signalling, int, 0600);
MODULE_PARM_DESC(sip_direct_signalling, "expect incoming calls from registrar "
					"only (default 1)");

static int sip_direct_media __read_mostly = 1;
module_param(sip_direct_media, int, 0600);
MODULE_PARM_DESC(sip_direct_media, "Expect Media streams between signalling "
				   "endpoints only (default 1)");

static int sip_external_media __read_mostly = 0;
module_param(sip_external_media, int, 0600);
MODULE_PARM_DESC(sip_external_media, "Expect Media streams between external "
				     "endpoints (default 0)");

const struct nf_nat_sip_hooks *nf_nat_sip_hooks;
EXPORT_SYMBOL_GPL(nf_nat_sip_hooks);

static int string_len(const struct nf_conn *ct, const char *dptr,
		      const char *limit, int *shift)
{
	int len = 0;

	while (dptr < limit && isalpha(*dptr)) {
		dptr++;
		len++;
	}
	return len;
}

static int digits_len(const struct nf_conn *ct, const char *dptr,
		      const char *limit, int *shift)
{
	int len = 0;
	while (dptr < limit && isdigit(*dptr)) {
		dptr++;
		len++;
	}
	return len;
}

static int iswordc(const char c)
{
	if (isalnum(c) || c == '!' || c == '"' || c == '%' ||
	    (c >= '(' && c <= '+') || c == ':' || c == '<' || c == '>' ||
	    c == '?' || (c >= '[' && c <= ']') || c == '_' || c == '`' ||
	    c == '{' || c == '}' || c == '~' || (c >= '-' && c <= '/') ||
	    c == '\'')
		return 1;
	return 0;
}

static int word_len(const char *dptr, const char *limit)
{
	int len = 0;
	while (dptr < limit && iswordc(*dptr)) {
		dptr++;
		len++;
	}
	return len;
}

static int callid_len(const struct nf_conn *ct, const char *dptr,
		      const char *limit, int *shift)
{
	int len, domain_len;

	len = word_len(dptr, limit);
	dptr += len;
	if (!len || dptr == limit || *dptr != '@')
		return len;
	dptr++;
	len++;

	domain_len = word_len(dptr, limit);
	if (!domain_len)
		return 0;
	return len + domain_len;
}

/* get media type + port length */
static int media_len(const struct nf_conn *ct, const char *dptr,
		     const char *limit, int *shift)
{
	int len = string_len(ct, dptr, limit, shift);

	dptr += len;
	if (dptr >= limit || *dptr != ' ')
		return 0;
	len++;
	dptr++;

	return len + digits_len(ct, dptr, limit, shift);
}

static int sip_parse_addr(const struct nf_conn *ct, const char *cp,
			  const char **endp, union nf_inet_addr *addr,
			  const char *limit, bool delim)
{
	const char *end;
	int ret;

	if (!ct)
		return 0;

	memset(addr, 0, sizeof(*addr));
	switch (nf_ct_l3num(ct)) {
	case AF_INET:
		ret = in4_pton(cp, limit - cp, (u8 *)&addr->ip, -1, &end);
		if (ret == 0)
			return 0;
		break;
	case AF_INET6:
		if (cp < limit && *cp == '[')
			cp++;
		else if (delim)
			return 0;

		ret = in6_pton(cp, limit - cp, (u8 *)&addr->ip6, -1, &end);
		if (ret == 0)
			return 0;

		if (end < limit && *end == ']')
			end++;
		else if (delim)
			return 0;
		break;
	default:
		BUG();
	}

	if (endp)
		*endp = end;
	return 1;
}

/* skip ip address. returns its length. */
static int epaddr_len(const struct nf_conn *ct, const char *dptr,
		      const char *limit, int *shift)
{
	union nf_inet_addr addr;
	const char *aux = dptr;

	if (!sip_parse_addr(ct, dptr, &dptr, &addr, limit, true)) {
		pr_debug("ip: %s parse failed.!\n", dptr);
		return 0;
	}

	/* Port number */
	if (*dptr == ':') {
		dptr++;
		dptr += digits_len(ct, dptr, limit, shift);
	}
	return dptr - aux;
}

/* get address length, skiping user info. */
static int skp_epaddr_len(const struct nf_conn *ct, const char *dptr,
			  const char *limit, int *shift)
{
	const char *start = dptr;
	int s = *shift;

	/* Search for @, but stop at the end of the line.
	 * We are inside a sip: URI, so we don't need to worry about
	 * continuation lines. */
	while (dptr < limit &&
	       *dptr != '@' && *dptr != '\r' && *dptr != '\n') {
		(*shift)++;
		dptr++;
	}

	if (dptr < limit && *dptr == '@') {
		dptr++;
		(*shift)++;
	} else {
		dptr = start;
		*shift = s;
	}

	return epaddr_len(ct, dptr, limit, shift);
}

/* Parse a SIP request line of the form:
 *
 * Request-Line = Method SP Request-URI SP SIP-Version CRLF
 *
 * and return the offset and length of the address contained in the Request-URI.
 */
int ct_sip_parse_request(const struct nf_conn *ct,
			 const char *dptr, unsigned int datalen,
			 unsigned int *matchoff, unsigned int *matchlen,
			 union nf_inet_addr *addr, __be16 *port)
{
	const char *start = dptr, *limit = dptr + datalen, *end;
	unsigned int mlen;
	unsigned int p;
	int shift = 0;

	/* Skip method and following whitespace */
	mlen = string_len(ct, dptr, limit, NULL);
	if (!mlen)
		return 0;
	dptr += mlen;
	if (++dptr >= limit)
		return 0;

	/* Find SIP URI */
	for (; dptr < limit - strlen("sip:"); dptr++) {
		if (*dptr == '\r' || *dptr == '\n')
			return -1;
		if (strncasecmp(dptr, "sip:", strlen("sip:")) == 0) {
			dptr += strlen("sip:");
			break;
		}
	}
	if (!skp_epaddr_len(ct, dptr, limit, &shift))
		return 0;
	dptr += shift;

	if (!sip_parse_addr(ct, dptr, &end, addr, limit, true))
		return -1;
	if (end < limit && *end == ':') {
		end++;
		p = simple_strtoul(end, (char **)&end, 10);
		if (p < 1024 || p > 65535)
			return -1;
		*port = htons(p);
	} else
		*port = htons(SIP_PORT);

	if (end == dptr)
		return 0;
	*matchoff = dptr - start;
	*matchlen = end - dptr;
	return 1;
}
EXPORT_SYMBOL_GPL(ct_sip_parse_request);

/* SIP header parsing: SIP headers are located at the beginning of a line, but
 * may span several lines, in which case the continuation lines begin with a
 * whitespace character. RFC 2543 allows lines to be terminated with CR, LF or
 * CRLF, RFC 3261 allows only CRLF, we support both.
 *
 * Headers are followed by (optionally) whitespace, a colon, again (optionally)
 * whitespace and the values. Whitespace in this context means any amount of
 * tabs, spaces and continuation lines, which are treated as a single whitespace
 * character.
 *
 * Some headers may appear multiple times. A comma separated list of values is
 * equivalent to multiple headers.
 */
static const struct sip_header ct_sip_hdrs[] = {
	[SIP_HDR_CSEQ]			= SIP_HDR("CSeq", NULL, NULL, digits_len),
	[SIP_HDR_FROM]			= SIP_HDR("From", "f", "sip:", skp_epaddr_len),
	[SIP_HDR_TO]			= SIP_HDR("To", "t", "sip:", skp_epaddr_len),
	[SIP_HDR_CONTACT]		= SIP_HDR("Contact", "m", "sip:", skp_epaddr_len),
	[SIP_HDR_VIA_UDP]		= SIP_HDR("Via", "v", "UDP ", epaddr_len),
	[SIP_HDR_VIA_TCP]		= SIP_HDR("Via", "v", "TCP ", epaddr_len),
	[SIP_HDR_EXPIRES]		= SIP_HDR("Expires", NULL, NULL, digits_len),
	[SIP_HDR_CONTENT_LENGTH]	= SIP_HDR("Content-Length", "l", NULL, digits_len),
	[SIP_HDR_CALL_ID]		= SIP_HDR("Call-Id", "i", NULL, callid_len),
};

static const char *sip_follow_continuation(const char *dptr, const char *limit)
{
	/* Walk past newline */
	if (++dptr >= limit)
		return NULL;

	/* Skip '\n' in CR LF */
	if (*(dptr - 1) == '\r' && *dptr == '\n') {
		if (++dptr >= limit)
			return NULL;
	}

	/* Continuation line? */
	if (*dptr != ' ' && *dptr != '\t')
		return NULL;

	/* skip leading whitespace */
	for (; dptr < limit; dptr++) {
		if (*dptr != ' ' && *dptr != '\t')
			break;
	}
	return dptr;
}

static const char *sip_skip_whitespace(const char *dptr, const char *limit)
{
	for (; dptr < limit; dptr++) {
		if (*dptr == ' ' || *dptr == '\t')
			continue;
		if (*dptr != '\r' && *dptr != '\n')
			break;
		dptr = sip_follow_continuation(dptr, limit);
		break;
	}
	return dptr;
}

/* Search within a SIP header value, dealing with continuation lines */
static const char *ct_sip_header_search(const char *dptr, const char *limit,
					const char *needle, unsigned int len)
{
	for (limit -= len; dptr < limit; dptr++) {
		if (*dptr == '\r' || *dptr == '\n') {
			dptr = sip_follow_continuation(dptr, limit);
			if (dptr == NULL)
				break;
			continue;
		}

		if (strncasecmp(dptr, needle, len) == 0)
			return dptr;
	}
	return NULL;
}

int ct_sip_get_header(const struct nf_conn *ct, const char *dptr,
		      unsigned int dataoff, unsigned int datalen,
		      enum sip_header_types type,
		      unsigned int *matchoff, unsigned int *matchlen)
{
	const struct sip_header *hdr = &ct_sip_hdrs[type];
	const char *start = dptr, *limit = dptr + datalen;
	int shift = 0;

	for (dptr += dataoff; dptr < limit; dptr++) {
		/* Find beginning of line */
		if (*dptr != '\r' && *dptr != '\n')
			continue;
		if (++dptr >= limit)
			break;
		if (*(dptr - 1) == '\r' && *dptr == '\n') {
			if (++dptr >= limit)
				break;
		}

		/* Skip continuation lines */
		if (*dptr == ' ' || *dptr == '\t')
			continue;

		/* Find header. Compact headers must be followed by a
		 * non-alphabetic character to avoid mismatches. */
		if (limit - dptr >= hdr->len &&
		    strncasecmp(dptr, hdr->name, hdr->len) == 0)
			dptr += hdr->len;
		else if (hdr->cname && limit - dptr >= hdr->clen + 1 &&
			 strncasecmp(dptr, hdr->cname, hdr->clen) == 0 &&
			 !isalpha(*(dptr + hdr->clen)))
			dptr += hdr->clen;
		else
			continue;

		/* Find and skip colon */
		dptr = sip_skip_whitespace(dptr, limit);
		if (dptr == NULL)
			break;
		if (*dptr != ':' || ++dptr >= limit)
			break;

		/* Skip whitespace after colon */
		dptr = sip_skip_whitespace(dptr, limit);
		if (dptr == NULL)
			break;

		*matchoff = dptr - start;
		if (hdr->search) {
			dptr = ct_sip_header_search(dptr, limit, hdr->search,
						    hdr->slen);
			if (!dptr)
				return -1;
			dptr += hdr->slen;
		}

		*matchlen = hdr->match_len(ct, dptr, limit, &shift);
		if (!*matchlen)
			return -1;
		*matchoff = dptr - start + shift;
		return 1;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(ct_sip_get_header);

/* Get next header field in a list of comma separated values */
static int ct_sip_next_header(const struct nf_conn *ct, const char *dptr,
			      unsigned int dataoff, unsigned int datalen,
			      enum sip_header_types type,
			      unsigned int *matchoff, unsigned int *matchlen)
{
	const struct sip_header *hdr = &ct_sip_hdrs[type];
	const char *start = dptr, *limit = dptr + datalen;
	int shift = 0;

	dptr += dataoff;

	dptr = ct_sip_header_search(dptr, limit, ",", strlen(","));
	if (!dptr)
		return 0;

	dptr = ct_sip_header_search(dptr, limit, hdr->search, hdr->slen);
	if (!dptr)
		return 0;
	dptr += hdr->slen;

	*matchoff = dptr - start;
	*matchlen = hdr->match_len(ct, dptr, limit, &shift);
	if (!*matchlen)
		return -1;
	*matchoff += shift;
	return 1;
}

/* Walk through headers until a parsable one is found or no header of the
 * given type is left. */
static int ct_sip_walk_headers(const struct nf_conn *ct, const char *dptr,
			       unsigned int dataoff, unsigned int datalen,
			       enum sip_header_types type, int *in_header,
			       unsigned int *matchoff, unsigned int *matchlen)
{
	int ret;

	if (in_header && *in_header) {
		while (1) {
			ret = ct_sip_next_header(ct, dptr, dataoff, datalen,
						 type, matchoff, matchlen);
			if (ret > 0)
				return ret;
			if (ret == 0)
				break;
			dataoff += *matchoff;
		}
		*in_header = 0;
	}

	while (1) {
		ret = ct_sip_get_header(ct, dptr, dataoff, datalen,
					type, matchoff, matchlen);
		if (ret > 0)
			break;
		if (ret == 0)
			return ret;
		dataoff += *matchoff;
	}

	if (in_header)
		*in_header = 1;
	return 1;
}

/* Locate a SIP header, parse the URI and return the offset and length of
 * the address as well as the address and port themselves. A stream of
 * headers can be parsed by handing in a non-NULL datalen and in_header
 * pointer.
 */
int ct_sip_parse_header_uri(const struct nf_conn *ct, const char *dptr,
			    unsigned int *dataoff, unsigned int datalen,
			    enum sip_header_types type, int *in_header,
			    unsigned int *matchoff, unsigned int *matchlen,
			    union nf_inet_addr *addr, __be16 *port)
{
	const char *c, *limit = dptr + datalen;
	unsigned int p;
	int ret;

	ret = ct_sip_walk_headers(ct, dptr, dataoff ? *dataoff : 0, datalen,
				  type, in_header, matchoff, matchlen);
	WARN_ON(ret < 0);
	if (ret == 0)
		return ret;

	if (!sip_parse_addr(ct, dptr + *matchoff, &c, addr, limit, true))
		return -1;
	if (*c == ':') {
		c++;
		p = simple_strtoul(c, (char **)&c, 10);
		if (p < 1024 || p > 65535)
			return -1;
		*port = htons(p);
	} else
		*port = htons(SIP_PORT);

	if (dataoff)
		*dataoff = c - dptr;
	return 1;
}
EXPORT_SYMBOL_GPL(ct_sip_parse_header_uri);

static int ct_sip_parse_param(const struct nf_conn *ct, const char *dptr,
			      unsigned int dataoff, unsigned int datalen,
			      const char *name,
			      unsigned int *matchoff, unsigned int *matchlen)
{
	const char *limit = dptr + datalen;
	const char *start;
	const char *end;

	limit = ct_sip_header_search(dptr + dataoff, limit, ",", strlen(","));
	if (!limit)
		limit = dptr + datalen;

	start = ct_sip_header_search(dptr + dataoff, limit, name, strlen(name));
	if (!start)
		return 0;
	start += strlen(name);

	end = ct_sip_header_search(start, limit, ";", strlen(";"));
	if (!end)
		end = limit;

	*matchoff = start - dptr;
	*matchlen = end - start;
	return 1;
}

/* Parse address from header parameter and return address, offset and length */
int ct_sip_parse_address_param(const struct nf_conn *ct, const char *dptr,
			       unsigned int dataoff, unsigned int datalen,
			       const char *name,
			       unsigned int *matchoff, unsigned int *matchlen,
			       union nf_inet_addr *addr, bool delim)
{
	const char *limit = dptr + datalen;
	const char *start, *end;

	limit = ct_sip_header_search(dptr + dataoff, limit, ",", strlen(","));
	if (!limit)
		limit = dptr + datalen;

	start = ct_sip_header_search(dptr + dataoff, limit, name, strlen(name));
	if (!start)
		return 0;

	start += strlen(name);
	if (!sip_parse_addr(ct, start, &end, addr, limit, delim))
		return 0;
	*matchoff = start - dptr;
	*matchlen = end - start;
	return 1;
}
EXPORT_SYMBOL_GPL(ct_sip_parse_address_param);

/* Parse numerical header parameter and return value, offset and length */
int ct_sip_parse_numerical_param(const struct nf_conn *ct, const char *dptr,
				 unsigned int dataoff, unsigned int datalen,
				 const char *name,
				 unsigned int *matchoff, unsigned int *matchlen,
				 unsigned int *val)
{
	const char *limit = dptr + datalen;
	const char *start;
	char *end;

	limit = ct_sip_header_search(dptr + dataoff, limit, ",", strlen(","));
	if (!limit)
		limit = dptr + datalen;

	start = ct_sip_header_search(dptr + dataoff, limit, name, strlen(name));
	if (!start)
		return 0;

	start += strlen(name);
	*val = simple_strtoul(start, &end, 0);
	if (start == end)
		return 0;
	if (matchoff && matchlen) {
		*matchoff = start - dptr;
		*matchlen = end - start;
	}
	return 1;
}
EXPORT_SYMBOL_GPL(ct_sip_parse_numerical_param);

static int ct_sip_parse_transport(struct nf_conn *ct, const char *dptr,
				  unsigned int dataoff, unsigned int datalen,
				  u8 *proto)
{
	unsigned int matchoff, matchlen;

	if (ct_sip_parse_param(ct, dptr, dataoff, datalen, "transport=",
			       &matchoff, &matchlen)) {
		if (!strncasecmp(dptr + matchoff, "TCP", strlen("TCP")))
			*proto = IPPROTO_TCP;
		else if (!strncasecmp(dptr + matchoff, "UDP", strlen("UDP")))
			*proto = IPPROTO_UDP;
		else
			return 0;

		if (*proto != nf_ct_protonum(ct))
			return 0;
	} else
		*proto = nf_ct_protonum(ct);

	return 1;
}

static int sdp_parse_addr(const struct nf_conn *ct, const char *cp,
			  const char **endp, union nf_inet_addr *addr,
			  const char *limit)
{
	const char *end;
	int ret;

	memset(addr, 0, sizeof(*addr));
	switch (nf_ct_l3num(ct)) {
	case AF_INET:
		ret = in4_pton(cp, limit - cp, (u8 *)&addr->ip, -1, &end);
		break;
	case AF_INET6:
		ret = in6_pton(cp, limit - cp, (u8 *)&addr->ip6, -1, &end);
		break;
	default:
		BUG();
	}

	if (ret == 0)
		return 0;
	if (endp)
		*endp = end;
	return 1;
}

/* skip ip address. returns its length. */
static int sdp_addr_len(const struct nf_conn *ct, const char *dptr,
			const char *limit, int *shift)
{
	union nf_inet_addr addr;
	const char *aux = dptr;

	if (!sdp_parse_addr(ct, dptr, &dptr, &addr, limit)) {
		pr_debug("ip: %s parse failed.!\n", dptr);
		return 0;
	}

	return dptr - aux;
}

/* SDP header parsing: a SDP session description contains an ordered set of
 * headers, starting with a section containing general session parameters,
 * optionally followed by multiple media descriptions.
 *
 * SDP headers always start at the beginning of a line. According to RFC 2327:
 * "The sequence CRLF (0x0d0a) is used to end a record, although parsers should
 * be tolerant and also accept records terminated with a single newline
 * character". We handle both cases.
 */
static const struct sip_header ct_sdp_hdrs_v4[] = {
	[SDP_HDR_VERSION]	= SDP_HDR("v=", NULL, digits_len),
	[SDP_HDR_OWNER]		= SDP_HDR("o=", "IN IP4 ", sdp_addr_len),
	[SDP_HDR_CONNECTION]	= SDP_HDR("c=", "IN IP4 ", sdp_addr_len),
	[SDP_HDR_MEDIA]		= SDP_HDR("m=", NULL, media_len),
};

static const struct sip_header ct_sdp_hdrs_v6[] = {
	[SDP_HDR_VERSION]	= SDP_HDR("v=", NULL, digits_len),
	[SDP_HDR_OWNER]		= SDP_HDR("o=", "IN IP6 ", sdp_addr_len),
	[SDP_HDR_CONNECTION]	= SDP_HDR("c=", "IN IP6 ", sdp_addr_len),
	[SDP_HDR_MEDIA]		= SDP_HDR("m=", NULL, media_len),
};

/* Linear string search within SDP header values */
static const char *ct_sdp_header_search(const char *dptr, const char *limit,
					const char *needle, unsigned int len)
{
	for (limit -= len; dptr < limit; dptr++) {
		if (*dptr == '\r' || *dptr == '\n')
			break;
		if (strncmp(dptr, needle, len) == 0)
			return dptr;
	}
	return NULL;
}

/* Locate a SDP header (optionally a substring within the header value),
 * optionally stopping at the first occurrence of the term header, parse
 * it and return the offset and length of the data we're interested in.
 */
int ct_sip_get_sdp_header(const struct nf_conn *ct, const char *dptr,
			  unsigned int dataoff, unsigned int datalen,
			  enum sdp_header_types type,
			  enum sdp_header_types term,
			  unsigned int *matchoff, unsigned int *matchlen)
{
	const struct sip_header *hdrs, *hdr, *thdr;
	const char *start = dptr, *limit = dptr + datalen;
	int shift = 0;

	hdrs = nf_ct_l3num(ct) == NFPROTO_IPV4 ? ct_sdp_hdrs_v4 : ct_sdp_hdrs_v6;
	hdr = &hdrs[type];
	thdr = &hdrs[term];

	for (dptr += dataoff; dptr < limit; dptr++) {
		/* Find beginning of line */
		if (*dptr != '\r' && *dptr != '\n')
			continue;
		if (++dptr >= limit)
			break;
		if (*(dptr - 1) == '\r' && *dptr == '\n') {
			if (++dptr >= limit)
				break;
		}

		if (term != SDP_HDR_UNSPEC &&
		    limit - dptr >= thdr->len &&
		    strncasecmp(dptr, thdr->name, thdr->len) == 0)
			break;
		else if (limit - dptr >= hdr->len &&
			 strncasecmp(dptr, hdr->name, hdr->len) == 0)
			dptr += hdr->len;
		else
			continue;

		*matchoff = dptr - start;
		if (hdr->search) {
			dptr = ct_sdp_header_search(dptr, limit, hdr->search,
						    hdr->slen);
			if (!dptr)
				return -1;
			dptr += hdr->slen;
		}

		*matchlen = hdr->match_len(ct, dptr, limit, &shift);
		if (!*matchlen)
			return -1;
		*matchoff = dptr - start + shift;
		return 1;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(ct_sip_get_sdp_header);

static int ct_sip_parse_sdp_addr(const struct nf_conn *ct, const char *dptr,
				 unsigned int dataoff, unsigned int datalen,
				 enum sdp_header_types type,
				 enum sdp_header_types term,
				 unsigned int *matchoff, unsigned int *matchlen,
				 union nf_inet_addr *addr)
{
	int ret;

	ret = ct_sip_get_sdp_header(ct, dptr, dataoff, datalen, type, term,
				    matchoff, matchlen);
	if (ret <= 0)
		return ret;

	if (!sdp_parse_addr(ct, dptr + *matchoff, NULL, addr,
			    dptr + *matchoff + *matchlen))
		return -1;
	return 1;
}

static int refresh_signalling_expectation(struct nf_conn *ct,
					  union nf_inet_addr *addr,
					  u8 proto, __be16 port,
					  unsigned int expires)
{
	struct nf_conn_help *help = nfct_help(ct);
	struct nf_conntrack_expect *exp;
	struct hlist_node *next;
	int found = 0;

	spin_lock_bh(&nf_conntrack_expect_lock);
	hlist_for_each_entry_safe(exp, next, &help->expectations, lnode) {
		if (exp->class != SIP_EXPECT_SIGNALLING ||
		    !nf_inet_addr_cmp(&exp->tuple.dst.u3, addr) ||
		    exp->tuple.dst.protonum != proto ||
		    exp->tuple.dst.u.udp.port != port)
			continue;
		if (mod_timer_pending(&exp->timeout, jiffies + expires * HZ)) {
			exp->flags &= ~NF_CT_EXPECT_INACTIVE;
			found = 1;
			break;
		}
	}
	spin_unlock_bh(&nf_conntrack_expect_lock);
	return found;
}

static void flush_expectations(struct nf_conn *ct, bool media)
{
	struct nf_conn_help *help = nfct_help(ct);
	struct nf_conntrack_expect *exp;
	struct hlist_node *next;

	spin_lock_bh(&nf_conntrack_expect_lock);
	hlist_for_each_entry_safe(exp, next, &help->expectations, lnode) {
		if ((exp->class != SIP_EXPECT_SIGNALLING) ^ media)
			continue;
		if (!nf_ct_remove_expect(exp))
			continue;
		if (!media)
			break;
	}
	spin_unlock_bh(&nf_conntrack_expect_lock);
}

static int set_expected_rtp_rtcp(struct sk_buff *skb, unsigned int protoff,
				 unsigned int dataoff,
				 const char **dptr, unsigned int *datalen,
				 union nf_inet_addr *daddr, __be16 port,
				 enum sip_expectation_classes class,
				 unsigned int mediaoff, unsigned int medialen)
{
	struct nf_conntrack_expect *exp, *rtp_exp, *rtcp_exp;
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	struct net *net = nf_ct_net(ct);
	enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
	union nf_inet_addr *saddr;
	struct nf_conntrack_tuple tuple;
	int direct_rtp = 0, skip_expect = 0, ret = NF_DROP;
	u_int16_t base_port;
	__be16 rtp_port, rtcp_port;
	const struct nf_nat_sip_hooks *hooks;

	saddr = NULL;
	if (sip_direct_media) {
		if (!nf_inet_addr_cmp(daddr, &ct->tuplehash[dir].tuple.src.u3))
			return NF_ACCEPT;
		saddr = &ct->tuplehash[!dir].tuple.src.u3;
	} else if (sip_external_media) {
		struct net_device *dev = skb_dst(skb)->dev;
		struct net *net = dev_net(dev);
		struct flowi fl;
		struct dst_entry *dst = NULL;

		memset(&fl, 0, sizeof(fl));

		switch (nf_ct_l3num(ct)) {
			case NFPROTO_IPV4:
				fl.u.ip4.daddr = daddr->ip;
				nf_ip_route(net, &dst, &fl, false);
				break;

			case NFPROTO_IPV6:
				fl.u.ip6.daddr = daddr->in6;
				nf_ip6_route(net, &dst, &fl, false);
				break;
		}

		/* Don't predict any conntracks when media endpoint is reachable
		 * through the same interface as the signalling peer.
		 */
		if (dst) {
			bool external_media = (dst->dev == dev);

			dst_release(dst);
			if (external_media)
				return NF_ACCEPT;
		}
	}

	/* We need to check whether the registration exists before attempting
	 * to register it since we can see the same media description multiple
	 * times on different connections in case multiple endpoints receive
	 * the same call.
	 *
	 * RTP optimization: if we find a matching media channel expectation
	 * and both the expectation and this connection are SNATed, we assume
	 * both sides can reach each other directly and use the final
	 * destination address from the expectation. We still need to keep
	 * the NATed expectations for media that might arrive from the
	 * outside, and additionally need to expect the direct RTP stream
	 * in case it passes through us even without NAT.
	 */
	memset(&tuple, 0, sizeof(tuple));
	if (saddr)
		tuple.src.u3 = *saddr;
	tuple.src.l3num		= nf_ct_l3num(ct);
	tuple.dst.protonum	= IPPROTO_UDP;
	tuple.dst.u3		= *daddr;
	tuple.dst.u.udp.port	= port;

	do {
		exp = __nf_ct_expect_find(net, nf_ct_zone(ct), &tuple);

		if (!exp || exp->master == ct ||
		    nfct_help(exp->master)->helper != nfct_help(ct)->helper ||
		    exp->class != class)
			break;
#if IS_ENABLED(CONFIG_NF_NAT)
		if (!direct_rtp &&
		    (!nf_inet_addr_cmp(&exp->saved_addr, &exp->tuple.dst.u3) ||
		     exp->saved_proto.udp.port != exp->tuple.dst.u.udp.port) &&
		    ct->status & IPS_NAT_MASK) {
			*daddr			= exp->saved_addr;
			tuple.dst.u3		= exp->saved_addr;
			tuple.dst.u.udp.port	= exp->saved_proto.udp.port;
			direct_rtp = 1;
		} else
#endif
			skip_expect = 1;
	} while (!skip_expect);

	base_port = ntohs(tuple.dst.u.udp.port) & ~1;
	rtp_port = htons(base_port);
	rtcp_port = htons(base_port + 1);

	if (direct_rtp) {
		hooks = rcu_dereference(nf_nat_sip_hooks);
		if (hooks &&
		    !hooks->sdp_port(skb, protoff, dataoff, dptr, datalen,
				     mediaoff, medialen, ntohs(rtp_port)))
			goto err1;
	}

	if (skip_expect)
		return NF_ACCEPT;

	rtp_exp = nf_ct_expect_alloc(ct);
	if (rtp_exp == NULL)
		goto err1;
	nf_ct_expect_init(rtp_exp, class, nf_ct_l3num(ct), saddr, daddr,
			  IPPROTO_UDP, NULL, &rtp_port);

	rtcp_exp = nf_ct_expect_alloc(ct);
	if (rtcp_exp == NULL)
		goto err2;
	nf_ct_expect_init(rtcp_exp, class, nf_ct_l3num(ct), saddr, daddr,
			  IPPROTO_UDP, NULL, &rtcp_port);

	hooks = rcu_dereference(nf_nat_sip_hooks);
	if (hooks && ct->status & IPS_NAT_MASK && !direct_rtp)
		ret = hooks->sdp_media(skb, protoff, dataoff, dptr,
				       datalen, rtp_exp, rtcp_exp,
				       mediaoff, medialen, daddr);
	else {
		/* -EALREADY handling works around end-points that send
		 * SDP messages with identical port but different media type,
		 * we pretend expectation was set up.
		 * It also works in the case that SDP messages are sent with
		 * identical expect tuples but for different master conntracks.
		 */
		int errp = nf_ct_expect_related(rtp_exp,
						NF_CT_EXP_F_SKIP_MASTER);

		if (errp == 0 || errp == -EALREADY) {
			int errcp = nf_ct_expect_related(rtcp_exp,
						NF_CT_EXP_F_SKIP_MASTER);

			if (errcp == 0 || errcp == -EALREADY)
				ret = NF_ACCEPT;
			else if (errp == 0)
				nf_ct_unexpect_related(rtp_exp);
		}
	}
	nf_ct_expect_put(rtcp_exp);
err2:
	nf_ct_expect_put(rtp_exp);
err1:
	return ret;
}

static const struct sdp_media_type sdp_media_types[] = {
	SDP_MEDIA_TYPE("audio ", SIP_EXPECT_AUDIO),
	SDP_MEDIA_TYPE("video ", SIP_EXPECT_VIDEO),
	SDP_MEDIA_TYPE("image ", SIP_EXPECT_IMAGE),
};

static const struct sdp_media_type *sdp_media_type(const char *dptr,
						   unsigned int matchoff,
						   unsigned int matchlen)
{
	const struct sdp_media_type *t;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(sdp_media_types); i++) {
		t = &sdp_media_types[i];
		if (matchlen < t->len ||
		    strncmp(dptr + matchoff, t->name, t->len))
			continue;
		return t;
	}
	return NULL;
}

static int process_sdp(struct sk_buff *skb, unsigned int protoff,
		       unsigned int dataoff,
		       const char **dptr, unsigned int *datalen,
		       unsigned int cseq)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	unsigned int matchoff, matchlen;
	unsigned int mediaoff, medialen;
	unsigned int sdpoff;
	unsigned int caddr_len, maddr_len;
	unsigned int i;
	union nf_inet_addr caddr, maddr, rtp_addr;
	const struct nf_nat_sip_hooks *hooks;
	unsigned int port;
	const struct sdp_media_type *t;
	int ret = NF_ACCEPT;

	hooks = rcu_dereference(nf_nat_sip_hooks);

	/* Find beginning of session description */
	if (ct_sip_get_sdp_header(ct, *dptr, 0, *datalen,
				  SDP_HDR_VERSION, SDP_HDR_UNSPEC,
				  &matchoff, &matchlen) <= 0)
		return NF_ACCEPT;
	sdpoff = matchoff;

	/* The connection information is contained in the session description
	 * and/or once per media description. The first media description marks
	 * the end of the session description. */
	caddr_len = 0;
	if (ct_sip_parse_sdp_addr(ct, *dptr, sdpoff, *datalen,
				  SDP_HDR_CONNECTION, SDP_HDR_MEDIA,
				  &matchoff, &matchlen, &caddr) > 0)
		caddr_len = matchlen;

	mediaoff = sdpoff;
	for (i = 0; i < ARRAY_SIZE(sdp_media_types); ) {
		if (ct_sip_get_sdp_header(ct, *dptr, mediaoff, *datalen,
					  SDP_HDR_MEDIA, SDP_HDR_UNSPEC,
					  &mediaoff, &medialen) <= 0)
			break;

		/* Get media type and port number. A media port value of zero
		 * indicates an inactive stream. */
		t = sdp_media_type(*dptr, mediaoff, medialen);
		if (!t) {
			mediaoff += medialen;
			continue;
		}
		mediaoff += t->len;
		medialen -= t->len;

		port = simple_strtoul(*dptr + mediaoff, NULL, 10);
		if (port == 0)
			continue;
		if (port < 1024 || port > 65535) {
			nf_ct_helper_log(skb, ct, "wrong port %u", port);
			return NF_DROP;
		}

		/* The media description overrides the session description. */
		maddr_len = 0;
		if (ct_sip_parse_sdp_addr(ct, *dptr, mediaoff, *datalen,
					  SDP_HDR_CONNECTION, SDP_HDR_MEDIA,
					  &matchoff, &matchlen, &maddr) > 0) {
			maddr_len = matchlen;
			memcpy(&rtp_addr, &maddr, sizeof(rtp_addr));
		} else if (caddr_len)
			memcpy(&rtp_addr, &caddr, sizeof(rtp_addr));
		else {
			nf_ct_helper_log(skb, ct, "cannot parse SDP message");
			return NF_DROP;
		}

		ret = set_expected_rtp_rtcp(skb, protoff, dataoff,
					    dptr, datalen,
					    &rtp_addr, htons(port), t->class,
					    mediaoff, medialen);
		if (ret != NF_ACCEPT) {
			nf_ct_helper_log(skb, ct,
					 "cannot add expectation for voice");
			return ret;
		}

		/* Update media connection address if present */
		if (maddr_len && hooks && ct->status & IPS_NAT_MASK) {
			ret = hooks->sdp_addr(skb, protoff, dataoff,
					      dptr, datalen, mediaoff,
					      SDP_HDR_CONNECTION,
					      SDP_HDR_MEDIA,
					      &rtp_addr);
			if (ret != NF_ACCEPT) {
				nf_ct_helper_log(skb, ct, "cannot mangle SDP");
				return ret;
			}
		}
		i++;
	}

	/* Update session connection and owner addresses */
	hooks = rcu_dereference(nf_nat_sip_hooks);
	if (hooks && ct->status & IPS_NAT_MASK)
		ret = hooks->sdp_session(skb, protoff, dataoff,
					 dptr, datalen, sdpoff,
					 &rtp_addr);

	return ret;
}
static int process_invite_response(struct sk_buff *skb, unsigned int protoff,
				   unsigned int dataoff,
				   const char **dptr, unsigned int *datalen,
				   unsigned int cseq, unsigned int code)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	struct nf_ct_sip_master *ct_sip_info = nfct_help_data(ct);

	if ((code >= 100 && code <= 199) ||
	    (code >= 200 && code <= 299))
		return process_sdp(skb, protoff, dataoff, dptr, datalen, cseq);
	else if (ct_sip_info->invite_cseq == cseq)
		flush_expectations(ct, true);
	return NF_ACCEPT;
}

static int process_update_response(struct sk_buff *skb, unsigned int protoff,
				   unsigned int dataoff,
				   const char **dptr, unsigned int *datalen,
				   unsigned int cseq, unsigned int code)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	struct nf_ct_sip_master *ct_sip_info = nfct_help_data(ct);

	if ((code >= 100 && code <= 199) ||
	    (code >= 200 && code <= 299))
		return process_sdp(skb, protoff, dataoff, dptr, datalen, cseq);
	else if (ct_sip_info->invite_cseq == cseq)
		flush_expectations(ct, true);
	return NF_ACCEPT;
}

static int process_prack_response(struct sk_buff *skb, unsigned int protoff,
				  unsigned int dataoff,
				  const char **dptr, unsigned int *datalen,
				  unsigned int cseq, unsigned int code)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	struct nf_ct_sip_master *ct_sip_info = nfct_help_data(ct);

	if ((code >= 100 && code <= 199) ||
	    (code >= 200 && code <= 299))
		return process_sdp(skb, protoff, dataoff, dptr, datalen, cseq);
	else if (ct_sip_info->invite_cseq == cseq)
		flush_expectations(ct, true);
	return NF_ACCEPT;
}

static int process_invite_request(struct sk_buff *skb, unsigned int protoff,
				  unsigned int dataoff,
				  const char **dptr, unsigned int *datalen,
				  unsigned int cseq)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	struct nf_ct_sip_master *ct_sip_info = nfct_help_data(ct);
	unsigned int ret;

	flush_expectations(ct, true);
	ret = process_sdp(skb, protoff, dataoff, dptr, datalen, cseq);
	if (ret == NF_ACCEPT)
		ct_sip_info->invite_cseq = cseq;
	return ret;
}

static int process_bye_request(struct sk_buff *skb, unsigned int protoff,
			       unsigned int dataoff,
			       const char **dptr, unsigned int *datalen,
			       unsigned int cseq)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);

	flush_expectations(ct, true);
	return NF_ACCEPT;
}

/* Parse a REGISTER request and create a permanent expectation for incoming
 * signalling connections. The expectation is marked inactive and is activated
 * when receiving a response indicating success from the registrar.
 */
static int process_register_request(struct sk_buff *skb, unsigned int protoff,
				    unsigned int dataoff,
				    const char **dptr, unsigned int *datalen,
				    unsigned int cseq)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	struct nf_ct_sip_master *ct_sip_info = nfct_help_data(ct);
	enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
	unsigned int matchoff, matchlen;
	struct nf_conntrack_expect *exp;
	union nf_inet_addr *saddr, daddr;
	const struct nf_nat_sip_hooks *hooks;
	__be16 port;
	u8 proto;
	unsigned int expires = 0;
	int ret;

	/* Expected connections can not register again. */
	if (ct->status & IPS_EXPECTED)
		return NF_ACCEPT;

	/* We must check the expiration time: a value of zero signals the
	 * registrar to release the binding. We'll remove our expectation
	 * when receiving the new bindings in the response, but we don't
	 * want to create new ones.
	 *
	 * The expiration time may be contained in Expires: header, the
	 * Contact: header parameters or the URI parameters.
	 */
	if (ct_sip_get_header(ct, *dptr, 0, *datalen, SIP_HDR_EXPIRES,
			      &matchoff, &matchlen) > 0)
		expires = simple_strtoul(*dptr + matchoff, NULL, 10);

	ret = ct_sip_parse_header_uri(ct, *dptr, NULL, *datalen,
				      SIP_HDR_CONTACT, NULL,
				      &matchoff, &matchlen, &daddr, &port);
	if (ret < 0) {
		nf_ct_helper_log(skb, ct, "cannot parse contact");
		return NF_DROP;
	} else if (ret == 0)
		return NF_ACCEPT;

	/* We don't support third-party registrations */
	if (!nf_inet_addr_cmp(&ct->tuplehash[dir].tuple.src.u3, &daddr))
		return NF_ACCEPT;

	if (ct_sip_parse_transport(ct, *dptr, matchoff + matchlen, *datalen,
				   &proto) == 0)
		return NF_ACCEPT;

	if (ct_sip_parse_numerical_param(ct, *dptr,
					 matchoff + matchlen, *datalen,
					 "expires=", NULL, NULL, &expires) < 0) {
		nf_ct_helper_log(skb, ct, "cannot parse expires");
		return NF_DROP;
	}

	if (expires == 0) {
		ret = NF_ACCEPT;
		goto store_cseq;
	}

	exp = nf_ct_expect_alloc(ct);
	if (!exp) {
		nf_ct_helper_log(skb, ct, "cannot alloc expectation");
		return NF_DROP;
	}

	saddr = NULL;
	if (sip_direct_signalling)
		saddr = &ct->tuplehash[!dir].tuple.src.u3;

	nf_ct_expect_init(exp, SIP_EXPECT_SIGNALLING, nf_ct_l3num(ct),
			  saddr, &daddr, proto, NULL, &port);
	exp->timeout.expires = sip_timeout * HZ;
	exp->helper = nfct_help(ct)->helper;
	exp->flags = NF_CT_EXPECT_PERMANENT | NF_CT_EXPECT_INACTIVE;

	hooks = rcu_dereference(nf_nat_sip_hooks);
	if (hooks && ct->status & IPS_NAT_MASK)
		ret = hooks->expect(skb, protoff, dataoff, dptr, datalen,
				    exp, matchoff, matchlen);
	else {
		if (nf_ct_expect_related(exp, 0) != 0) {
			nf_ct_helper_log(skb, ct, "cannot add expectation");
			ret = NF_DROP;
		} else
			ret = NF_ACCEPT;
	}
	nf_ct_expect_put(exp);

store_cseq:
	if (ret == NF_ACCEPT)
		ct_sip_info->register_cseq = cseq;
	return ret;
}

static int process_register_response(struct sk_buff *skb, unsigned int protoff,
				     unsigned int dataoff,
				     const char **dptr, unsigned int *datalen,
				     unsigned int cseq, unsigned int code)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	struct nf_ct_sip_master *ct_sip_info = nfct_help_data(ct);
	enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
	union nf_inet_addr addr;
	__be16 port;
	u8 proto;
	unsigned int matchoff, matchlen, coff = 0;
	unsigned int expires = 0;
	int in_contact = 0, ret;

	/* According to RFC 3261, "UAs MUST NOT send a new registration until
	 * they have received a final response from the registrar for the
	 * previous one or the previous REGISTER request has timed out".
	 *
	 * However, some servers fail to detect retransmissions and send late
	 * responses, so we store the sequence number of the last valid
	 * request and compare it here.
	 */
	if (ct_sip_info->register_cseq != cseq)
		return NF_ACCEPT;

	if (code >= 100 && code <= 199)
		return NF_ACCEPT;
	if (code < 200 || code > 299)
		goto flush;

	if (ct_sip_get_header(ct, *dptr, 0, *datalen, SIP_HDR_EXPIRES,
			      &matchoff, &matchlen) > 0)
		expires = simple_strtoul(*dptr + matchoff, NULL, 10);

	while (1) {
		unsigned int c_expires = expires;

		ret = ct_sip_parse_header_uri(ct, *dptr, &coff, *datalen,
					      SIP_HDR_CONTACT, &in_contact,
					      &matchoff, &matchlen,
					      &addr, &port);
		if (ret < 0) {
			nf_ct_helper_log(skb, ct, "cannot parse contact");
			return NF_DROP;
		} else if (ret == 0)
			break;

		/* We don't support third-party registrations */
		if (!nf_inet_addr_cmp(&ct->tuplehash[dir].tuple.dst.u3, &addr))
			continue;

		if (ct_sip_parse_transport(ct, *dptr, matchoff + matchlen,
					   *datalen, &proto) == 0)
			continue;

		ret = ct_sip_parse_numerical_param(ct, *dptr,
						   matchoff + matchlen,
						   *datalen, "expires=",
						   NULL, NULL, &c_expires);
		if (ret < 0) {
			nf_ct_helper_log(skb, ct, "cannot parse expires");
			return NF_DROP;
		}
		if (c_expires == 0)
			break;
		if (refresh_signalling_expectation(ct, &addr, proto, port,
						   c_expires))
			return NF_ACCEPT;
	}

flush:
	flush_expectations(ct, false);
	return NF_ACCEPT;
}

static const struct sip_handler sip_handlers[] = {
	SIP_HANDLER("INVITE", process_invite_request, process_invite_response),
	SIP_HANDLER("UPDATE", process_sdp, process_update_response),
	SIP_HANDLER("ACK", process_sdp, NULL),
	SIP_HANDLER("PRACK", process_sdp, process_prack_response),
	SIP_HANDLER("BYE", process_bye_request, NULL),
	SIP_HANDLER("REGISTER", process_register_request, process_register_response),
};

static int process_sip_response(struct sk_buff *skb, unsigned int protoff,
				unsigned int dataoff,
				const char **dptr, unsigned int *datalen)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	unsigned int matchoff, matchlen, matchend;
	unsigned int code, cseq, i;

	if (*datalen < strlen("SIP/2.0 200"))
		return NF_ACCEPT;
	code = simple_strtoul(*dptr + strlen("SIP/2.0 "), NULL, 10);
	if (!code) {
		nf_ct_helper_log(skb, ct, "cannot get code");
		return NF_DROP;
	}

	if (ct_sip_get_header(ct, *dptr, 0, *datalen, SIP_HDR_CSEQ,
			      &matchoff, &matchlen) <= 0) {
		nf_ct_helper_log(skb, ct, "cannot parse cseq");
		return NF_DROP;
	}
	cseq = simple_strtoul(*dptr + matchoff, NULL, 10);
	if (!cseq && *(*dptr + matchoff) != '0') {
		nf_ct_helper_log(skb, ct, "cannot get cseq");
		return NF_DROP;
	}
	matchend = matchoff + matchlen + 1;

	for (i = 0; i < ARRAY_SIZE(sip_handlers); i++) {
		const struct sip_handler *handler;

		handler = &sip_handlers[i];
		if (handler->response == NULL)
			continue;
		if (*datalen < matchend + handler->len ||
		    strncasecmp(*dptr + matchend, handler->method, handler->len))
			continue;
		return handler->response(skb, protoff, dataoff, dptr, datalen,
					 cseq, code);
	}
	return NF_ACCEPT;
}

static int process_sip_request(struct sk_buff *skb, unsigned int protoff,
			       unsigned int dataoff,
			       const char **dptr, unsigned int *datalen)
{
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
	struct nf_ct_sip_master *ct_sip_info = nfct_help_data(ct);
	enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
	unsigned int matchoff, matchlen;
	unsigned int cseq, i;
	union nf_inet_addr addr;
	__be16 port;

	/* Many Cisco IP phones use a high source port for SIP requests, but
	 * listen for the response on port 5060.  If we are the local
	 * router for one of these phones, save the port number from the
	 * Via: header so that nf_nat_sip can redirect the responses to
	 * the correct port.
	 */
	if (ct_sip_parse_header_uri(ct, *dptr, NULL, *datalen,
				    SIP_HDR_VIA_UDP, NULL, &matchoff,
				    &matchlen, &addr, &port) > 0 &&
	    port != ct->tuplehash[dir].tuple.src.u.udp.port &&
	    nf_inet_addr_cmp(&addr, &ct->tuplehash[dir].tuple.src.u3))
		ct_sip_info->forced_dport = port;

	for (i = 0; i < ARRAY_SIZE(sip_handlers); i++) {
		const struct sip_handler *handler;

		handler = &sip_handlers[i];
		if (handler->request == NULL)
			continue;
		if (*datalen < handler->len + 2 ||
		    strncasecmp(*dptr, handler->method, handler->len))
			continue;
		if ((*dptr)[handler->len] != ' ' ||
		    !isalpha((*dptr)[handler->len+1]))
			continue;

		if (ct_sip_get_header(ct, *dptr, 0, *datalen, SIP_HDR_CSEQ,
				      &matchoff, &matchlen) <= 0) {
			nf_ct_helper_log(skb, ct, "cannot parse cseq");
			return NF_DROP;
		}
		cseq = simple_strtoul(*dptr + matchoff, NULL, 10);
		if (!cseq && *(*dptr + matchoff) != '0') {
			nf_ct_helper_log(skb, ct, "cannot get cseq");
			return NF_DROP;
		}

		return handler->request(skb, protoff, dataoff, dptr, datalen,
					cseq);
	}
	return NF_ACCEPT;
}

static int process_sip_msg(struct sk_buff *skb, struct nf_conn *ct,
			   unsigned int protoff, unsigned int dataoff,
			   const char **dptr, unsigned int *datalen)
{
	const struct nf_nat_sip_hooks *hooks;
	int ret;

	if (strncasecmp(*dptr, "SIP/2.0 ", strlen("SIP/2.0 ")) != 0)
		ret = process_sip_request(skb, protoff, dataoff, dptr, datalen);
	else
		ret = process_sip_response(skb, protoff, dataoff, dptr, datalen);

	if (ret == NF_ACCEPT && ct->status & IPS_NAT_MASK) {
		hooks = rcu_dereference(nf_nat_sip_hooks);
		if (hooks && !hooks->msg(skb, protoff, dataoff,
					 dptr, datalen)) {
			nf_ct_helper_log(skb, ct, "cannot NAT SIP message");
			ret = NF_DROP;
		}
	}

	return ret;
}

static int sip_help_tcp(struct sk_buff *skb, unsigned int protoff,
			struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	struct tcphdr *th, _tcph;
	unsigned int dataoff, datalen;
	unsigned int matchoff, matchlen, clen;
	unsigned int msglen, origlen;
	const char *dptr, *end;
	s16 diff, tdiff = 0;
	int ret = NF_ACCEPT;
	bool term;

	if (ctinfo != IP_CT_ESTABLISHED &&
	    ctinfo != IP_CT_ESTABLISHED_REPLY)
		return NF_ACCEPT;

	/* No Data ? */
	th = skb_header_pointer(skb, protoff, sizeof(_tcph), &_tcph);
	if (th == NULL)
		return NF_ACCEPT;
	dataoff = protoff + th->doff * 4;
	if (dataoff >= skb->len)
		return NF_ACCEPT;

	nf_ct_refresh(ct, skb, sip_timeout * HZ);

	if (unlikely(skb_linearize(skb)))
		return NF_DROP;

	dptr = skb->data + dataoff;
	datalen = skb->len - dataoff;
	if (datalen < strlen("SIP/2.0 200"))
		return NF_ACCEPT;

	while (1) {
		if (ct_sip_get_header(ct, dptr, 0, datalen,
				      SIP_HDR_CONTENT_LENGTH,
				      &matchoff, &matchlen) <= 0)
			break;

		clen = simple_strtoul(dptr + matchoff, (char **)&end, 10);
		if (dptr + matchoff == end)
			break;

		term = false;
		for (; end + strlen("\r\n\r\n") <= dptr + datalen; end++) {
			if (end[0] == '\r' && end[1] == '\n' &&
			    end[2] == '\r' && end[3] == '\n') {
				term = true;
				break;
			}
		}
		if (!term)
			break;
		end += strlen("\r\n\r\n") + clen;

		msglen = origlen = end - dptr;
		if (msglen > datalen)
			return NF_ACCEPT;

		ret = process_sip_msg(skb, ct, protoff, dataoff,
				      &dptr, &msglen);
		/* process_sip_* functions report why this packet is dropped */
		if (ret != NF_ACCEPT)
			break;
		diff     = msglen - origlen;
		tdiff   += diff;

		dataoff += msglen;
		dptr    += msglen;
		datalen  = datalen + diff - msglen;
	}

	if (ret == NF_ACCEPT && ct->status & IPS_NAT_MASK) {
		const struct nf_nat_sip_hooks *hooks;

		hooks = rcu_dereference(nf_nat_sip_hooks);
		if (hooks)
			hooks->seq_adjust(skb, protoff, tdiff);
	}

	return ret;
}

static int sip_help_udp(struct sk_buff *skb, unsigned int protoff,
			struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	unsigned int dataoff, datalen;
	const char *dptr;

	/* No Data ? */
	dataoff = protoff + sizeof(struct udphdr);
	if (dataoff >= skb->len)
		return NF_ACCEPT;

	nf_ct_refresh(ct, skb, sip_timeout * HZ);

	if (unlikely(skb_linearize(skb)))
		return NF_DROP;

	dptr = skb->data + dataoff;
	datalen = skb->len - dataoff;
	if (datalen < strlen("SIP/2.0 200"))
		return NF_ACCEPT;

	return process_sip_msg(skb, ct, protoff, dataoff, &dptr, &datalen);
}

static struct nf_conntrack_helper sip[MAX_PORTS * 4] __read_mostly;

static const struct nf_conntrack_expect_policy sip_exp_policy[SIP_EXPECT_MAX + 1] = {
	[SIP_EXPECT_SIGNALLING] = {
		.name		= "signalling",
		.max_expected	= 1,
		.timeout	= 3 * 60,
	},
	[SIP_EXPECT_AUDIO] = {
		.name		= "audio",
		.max_expected	= 2 * IP_CT_DIR_MAX,
		.timeout	= 3 * 60,
	},
	[SIP_EXPECT_VIDEO] = {
		.name		= "video",
		.max_expected	= 2 * IP_CT_DIR_MAX,
		.timeout	= 3 * 60,
	},
	[SIP_EXPECT_IMAGE] = {
		.name		= "image",
		.max_expected	= IP_CT_DIR_MAX,
		.timeout	= 3 * 60,
	},
};

static void __exit nf_conntrack_sip_fini(void)
{
	nf_conntrack_helpers_unregister(sip, ports_c * 4);
}

static int __init nf_conntrack_sip_init(void)
{
	int i, ret;

	NF_CT_HELPER_BUILD_BUG_ON(sizeof(struct nf_ct_sip_master));

	if (ports_c == 0)
		ports[ports_c++] = SIP_PORT;

	for (i = 0; i < ports_c; i++) {
		nf_ct_helper_init(&sip[4 * i], AF_INET, IPPROTO_UDP,
				  HELPER_NAME, SIP_PORT, ports[i], i,
				  sip_exp_policy, SIP_EXPECT_MAX, sip_help_udp,
				  NULL, THIS_MODULE);
		nf_ct_helper_init(&sip[4 * i + 1], AF_INET, IPPROTO_TCP,
				  HELPER_NAME, SIP_PORT, ports[i], i,
				  sip_exp_policy, SIP_EXPECT_MAX, sip_help_tcp,
				  NULL, THIS_MODULE);
		nf_ct_helper_init(&sip[4 * i + 2], AF_INET6, IPPROTO_UDP,
				  HELPER_NAME, SIP_PORT, ports[i], i,
				  sip_exp_policy, SIP_EXPECT_MAX, sip_help_udp,
				  NULL, THIS_MODULE);
		nf_ct_helper_init(&sip[4 * i + 3], AF_INET6, IPPROTO_TCP,
				  HELPER_NAME, SIP_PORT, ports[i], i,
				  sip_exp_policy, SIP_EXPECT_MAX, sip_help_tcp,
				  NULL, THIS_MODULE);
	}

	ret = nf_conntrack_helpers_register(sip, ports_c * 4);
	if (ret < 0) {
		pr_err("failed to register helpers\n");
		return ret;
	}
	return 0;
}

module_init(nf_conntrack_sip_init);
module_exit(nf_conntrack_sip_fini);
