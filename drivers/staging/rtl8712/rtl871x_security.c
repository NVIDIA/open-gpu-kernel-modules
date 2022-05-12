// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 * rtl871x_security.c
 *
 * Copyright(c) 2007 - 2010 Realtek Corporation. All rights reserved.
 * Linux device driver for RTL8192SU
 *
 * Modifications for inclusion into the Linux staging tree are
 * Copyright(c) 2010 Larry Finger. All rights reserved.
 *
 * Contact information:
 * WLAN FAE <wlanfae@realtek.com>
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 ******************************************************************************/

#define  _RTL871X_SECURITY_C_

#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/circ_buf.h>
#include <linux/uaccess.h>
#include <asm/byteorder.h>
#include <linux/atomic.h>
#include <linux/crc32poly.h>
#include <linux/semaphore.h>
#include <linux/ieee80211.h>

#include "osdep_service.h"
#include "drv_types.h"
#include "osdep_intf.h"

/* =====WEP related===== */

struct arc4context {
	u32 x;
	u32 y;
	u8 state[256];
};

static void arcfour_init(struct arc4context *parc4ctx, u8 *key, u32 key_len)
{
	u32	t, u;
	u32	keyindex;
	u32	stateindex;
	u8 *state;
	u32	counter;

	state = parc4ctx->state;
	parc4ctx->x = 0;
	parc4ctx->y = 0;
	for (counter = 0; counter < 256; counter++)
		state[counter] = (u8)counter;
	keyindex = 0;
	stateindex = 0;
	for (counter = 0; counter < 256; counter++) {
		t = state[counter];
		stateindex = (stateindex + key[keyindex] + t) & 0xff;
		u = state[stateindex];
		state[stateindex] = (u8)t;
		state[counter] = (u8)u;
		if (++keyindex >= key_len)
			keyindex = 0;
	}
}

static u32 arcfour_byte(struct arc4context *parc4ctx)
{
	u32 x;
	u32 y;
	u32 sx, sy;
	u8 *state;

	state = parc4ctx->state;
	x = (parc4ctx->x + 1) & 0xff;
	sx = state[x];
	y = (sx + parc4ctx->y) & 0xff;
	sy = state[y];
	parc4ctx->x = x;
	parc4ctx->y = y;
	state[y] = (u8)sx;
	state[x] = (u8)sy;
	return state[(sx + sy) & 0xff];
}

static void arcfour_encrypt(struct arc4context	*parc4ctx,
		     u8 *dest, u8 *src, u32 len)
{
	u32 i;

	for (i = 0; i < len; i++)
		dest[i] = src[i] ^ (unsigned char)arcfour_byte(parc4ctx);
}

static sint bcrc32initialized;
static u32 crc32_table[256];

static u8 crc32_reverseBit(u8 data)
{
	return ((u8)(data << 7) & 0x80) | ((data << 5) & 0x40) | ((data << 3)
		 & 0x20) | ((data << 1) & 0x10) | ((data >> 1) & 0x08) |
		 ((data >> 3) & 0x04) | ((data >> 5) & 0x02) | ((data >> 7) &
		 0x01);
}

static void crc32_init(void)
{
	sint i, j;
	u32 c;
	u8 *p = (u8 *)&c, *p1;
	u8 k;

	if (bcrc32initialized == 1)
		return;

	for (i = 0; i < 256; ++i) {
		k = crc32_reverseBit((u8)i);
		for (c = ((u32)k) << 24, j = 8; j > 0; --j)
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY_BE : (c << 1);
		p1 = (u8 *)&crc32_table[i];
		p1[0] = crc32_reverseBit(p[3]);
		p1[1] = crc32_reverseBit(p[2]);
		p1[2] = crc32_reverseBit(p[1]);
		p1[3] = crc32_reverseBit(p[0]);
	}
	bcrc32initialized = 1;
}

static u32 getcrc32(u8 *buf, u32 len)
{
	u8 *p;
	u32  crc;

	if (!bcrc32initialized)
		crc32_init();
	crc = 0xffffffff; /* preload shift register, per CRC-32 spec */
	for (p = buf; len > 0; ++p, --len)
		crc = crc32_table[(crc ^ *p) & 0xff] ^ (crc >> 8);
	return ~crc;    /* transmit complement, per CRC-32 spec */
}

/*
 * Need to consider the fragment situation
 */
void r8712_wep_encrypt(struct _adapter *padapter, u8 *pxmitframe)
{	/* exclude ICV */
	unsigned char	crc[4];
	struct arc4context  mycontext;
	u32 curfragnum, length, keylength, pki;
	u8 *pframe, *payload, *iv;    /*,*wepkey*/
	u8 wepkey[16];
	struct	pkt_attrib  *pattrib = &((struct xmit_frame *)
				       pxmitframe)->attrib;
	struct	security_priv *psecuritypriv = &padapter->securitypriv;
	struct	xmit_priv *pxmitpriv = &padapter->xmitpriv;

	if (((struct xmit_frame *)pxmitframe)->buf_addr == NULL)
		return;
	pframe = ((struct xmit_frame *)pxmitframe)->buf_addr + TXDESC_OFFSET;
	/*start to encrypt each fragment*/
	if ((pattrib->encrypt == _WEP40_) || (pattrib->encrypt == _WEP104_)) {
		pki = psecuritypriv->PrivacyKeyIndex;
		keylength = psecuritypriv->DefKeylen[pki];
		for (curfragnum = 0; curfragnum < pattrib->nr_frags;
		     curfragnum++) {
			iv = pframe + pattrib->hdrlen;
			memcpy(&wepkey[0], iv, 3);
			memcpy(&wepkey[3], &psecuritypriv->DefKey[
				psecuritypriv->PrivacyKeyIndex].skey[0],
				keylength);
			payload = pframe + pattrib->iv_len + pattrib->hdrlen;
			if ((curfragnum + 1) == pattrib->nr_frags) {
				length = pattrib->last_txcmdsz -
					pattrib->hdrlen -
					pattrib->iv_len -
					pattrib->icv_len;
				*((__le32 *)crc) = cpu_to_le32(getcrc32(
						payload, length));
				arcfour_init(&mycontext, wepkey, 3 + keylength);
				arcfour_encrypt(&mycontext, payload, payload,
						length);
				arcfour_encrypt(&mycontext, payload + length,
						crc, 4);
			} else {
				length = pxmitpriv->frag_len -
					 pattrib->hdrlen - pattrib->iv_len -
					 pattrib->icv_len;
				*((__le32 *)crc) = cpu_to_le32(getcrc32(
						payload, length));
				arcfour_init(&mycontext, wepkey, 3 + keylength);
				arcfour_encrypt(&mycontext, payload, payload,
						length);
				arcfour_encrypt(&mycontext, payload + length,
						crc, 4);
				pframe += pxmitpriv->frag_len;
				pframe = (u8 *)RND4((addr_t)(pframe));
			}
		}
	}
}

void r8712_wep_decrypt(struct _adapter  *padapter, u8 *precvframe)
{
	/* exclude ICV */
	u8 crc[4];
	struct arc4context  mycontext;
	u32 length, keylength;
	u8 *pframe, *payload, *iv, wepkey[16];
	u8  keyindex;
	struct rx_pkt_attrib  *prxattrib = &(((union recv_frame *)
					  precvframe)->u.hdr.attrib);
	struct security_priv *psecuritypriv = &padapter->securitypriv;

	pframe = (unsigned char *)((union recv_frame *)precvframe)->
		  u.hdr.rx_data;
	/* start to decrypt recvframe */
	if ((prxattrib->encrypt == _WEP40_) || (prxattrib->encrypt ==
	     _WEP104_)) {
		iv = pframe + prxattrib->hdrlen;
		keyindex = (iv[3] & 0x3);
		keylength = psecuritypriv->DefKeylen[keyindex];
		memcpy(&wepkey[0], iv, 3);
		memcpy(&wepkey[3], &psecuritypriv->DefKey[
			psecuritypriv->PrivacyKeyIndex].skey[0],
			keylength);
		length = ((union recv_frame *)precvframe)->
			   u.hdr.len - prxattrib->hdrlen - prxattrib->iv_len;
		payload = pframe + prxattrib->iv_len + prxattrib->hdrlen;
		/* decrypt payload include icv */
		arcfour_init(&mycontext, wepkey, 3 + keylength);
		arcfour_encrypt(&mycontext, payload, payload,  length);
		/* calculate icv and compare the icv */
		*((__le32 *)crc) = cpu_to_le32(getcrc32(payload, length - 4));
	}
}

/* 3 =====TKIP related===== */

static u32 secmicgetuint32(u8 *p)
/* Convert from Byte[] to Us4Byte32 in a portable way */
{
	s32 i;
	u32 res = 0;

	for (i = 0; i < 4; i++)
		res |= ((u32)(*p++)) << (8 * i);
	return res;
}

static void secmicputuint32(u8 *p, u32 val)
/* Convert from Us4Byte32 to Byte[] in a portable way */
{
	long i;

	for (i = 0; i < 4; i++) {
		*p++ = (u8)(val & 0xff);
		val >>= 8;
	}
}

static void secmicclear(struct mic_data *pmicdata)
{
/* Reset the state to the empty message. */
	pmicdata->L = pmicdata->K0;
	pmicdata->R = pmicdata->K1;
	pmicdata->nBytesInM = 0;
	pmicdata->M = 0;
}

void r8712_secmicsetkey(struct mic_data *pmicdata, u8 *key)
{
	/* Set the key */
	pmicdata->K0 = secmicgetuint32(key);
	pmicdata->K1 = secmicgetuint32(key + 4);
	/* and reset the message */
	secmicclear(pmicdata);
}

static void secmicappendbyte(struct mic_data *pmicdata, u8 b)
{
	/* Append the byte to our word-sized buffer */
	pmicdata->M |= ((u32)b) << (8 * pmicdata->nBytesInM);
	pmicdata->nBytesInM++;
	/* Process the word if it is full. */
	if (pmicdata->nBytesInM >= 4) {
		pmicdata->L ^= pmicdata->M;
		pmicdata->R ^= ROL32(pmicdata->L, 17);
		pmicdata->L += pmicdata->R;
		pmicdata->R ^= ((pmicdata->L & 0xff00ff00) >> 8) |
			       ((pmicdata->L & 0x00ff00ff) << 8);
		pmicdata->L += pmicdata->R;
		pmicdata->R ^= ROL32(pmicdata->L, 3);
		pmicdata->L += pmicdata->R;
		pmicdata->R ^= ROR32(pmicdata->L, 2);
		pmicdata->L += pmicdata->R;
		/* Clear the buffer */
		pmicdata->M = 0;
		pmicdata->nBytesInM = 0;
	}
}

void r8712_secmicappend(struct mic_data *pmicdata, u8 *src, u32 nbytes)
{
	/* This is simple */
	while (nbytes > 0) {
		secmicappendbyte(pmicdata, *src++);
		nbytes--;
	}
}

void r8712_secgetmic(struct mic_data *pmicdata, u8 *dst)
{
	/* Append the minimum padding */
	secmicappendbyte(pmicdata, 0x5a);
	secmicappendbyte(pmicdata, 0);
	secmicappendbyte(pmicdata, 0);
	secmicappendbyte(pmicdata, 0);
	secmicappendbyte(pmicdata, 0);
	/* and then zeroes until the length is a multiple of 4 */
	while (pmicdata->nBytesInM != 0)
		secmicappendbyte(pmicdata, 0);
	/* The appendByte function has already computed the result. */
	secmicputuint32(dst, pmicdata->L);
	secmicputuint32(dst + 4, pmicdata->R);
	/* Reset to the empty message. */
	secmicclear(pmicdata);
}

void seccalctkipmic(u8 *key, u8 *header, u8 *data, u32 data_len, u8 *mic_code,
		    u8 pri)
{

	struct mic_data	micdata;
	u8 priority[4] = {0x0, 0x0, 0x0, 0x0};

	r8712_secmicsetkey(&micdata, key);
	priority[0] = pri;
	/* Michael MIC pseudo header: DA, SA, 3 x 0, Priority */
	if (header[1] & 1) {   /* ToDS==1 */
		r8712_secmicappend(&micdata, &header[16], 6);  /* DA */
		if (header[1] & 2)  /* From Ds==1 */
			r8712_secmicappend(&micdata, &header[24], 6);
		else
			r8712_secmicappend(&micdata, &header[10], 6);
	} else {	/* ToDS==0 */
		r8712_secmicappend(&micdata, &header[4], 6);   /* DA */
		if (header[1] & 2)  /* From Ds==1 */
			r8712_secmicappend(&micdata, &header[16], 6);
		else
			r8712_secmicappend(&micdata, &header[10], 6);
	}
	r8712_secmicappend(&micdata, &priority[0], 4);
	r8712_secmicappend(&micdata, data, data_len);
	r8712_secgetmic(&micdata, mic_code);
}

/* macros for extraction/creation of unsigned char/unsigned short values  */
#define RotR1(v16)   ((((v16) >> 1) & 0x7FFF) ^ (((v16) & 1) << 15))
#define   Lo8(v16)   ((u8)((v16) & 0x00FF))
#define   Hi8(v16)   ((u8)(((v16) >> 8) & 0x00FF))
#define  Lo16(v32)   ((u16)((v32) & 0xFFFF))
#define  Hi16(v32)   ((u16)(((v32) >> 16) & 0xFFFF))
#define  Mk16(hi, lo) ((lo) ^ (((u16)(hi)) << 8))

/* select the Nth 16-bit word of the temporal key unsigned char array TK[]   */
#define  TK16(N)  Mk16(tk[2 * (N) + 1], tk[2 * (N)])

/* S-box lookup: 16 bits --> 16 bits */
#define _S_(v16)  (Sbox1[0][Lo8(v16)] ^ Sbox1[1][Hi8(v16)])

/* fixed algorithm "parameters" */
#define PHASE1_LOOP_CNT   8    /* this needs to be "big enough"     */
#define TA_SIZE           6    /*  48-bit transmitter address       */
#define TK_SIZE          16    /* 128-bit temporal key              */
#define P1K_SIZE         10    /*  80-bit Phase1 key                */
#define RC4_KEY_SIZE     16    /* 128-bit RC4KEY (104 bits unknown) */


/* 2-unsigned char by 2-unsigned char subset of the full AES S-box table */
static const unsigned short Sbox1[2][256] = {/* Sbox for hash (can be in ROM) */
	{
	0xC6A5, 0xF884, 0xEE99, 0xF68D, 0xFF0D, 0xD6BD, 0xDEB1, 0x9154,
	0x6050, 0x0203, 0xCEA9, 0x567D, 0xE719, 0xB562, 0x4DE6, 0xEC9A,
	0x8F45, 0x1F9D, 0x8940, 0xFA87, 0xEF15, 0xB2EB, 0x8EC9, 0xFB0B,
	0x41EC, 0xB367, 0x5FFD, 0x45EA, 0x23BF, 0x53F7, 0xE496, 0x9B5B,
	0x75C2, 0xE11C, 0x3DAE, 0x4C6A, 0x6C5A, 0x7E41, 0xF502, 0x834F,
	0x685C, 0x51F4, 0xD134, 0xF908, 0xE293, 0xAB73, 0x6253, 0x2A3F,
	0x080C, 0x9552, 0x4665, 0x9D5E, 0x3028, 0x37A1, 0x0A0F, 0x2FB5,
	0x0E09, 0x2436, 0x1B9B, 0xDF3D, 0xCD26, 0x4E69, 0x7FCD, 0xEA9F,
	0x121B, 0x1D9E, 0x5874, 0x342E, 0x362D, 0xDCB2, 0xB4EE, 0x5BFB,
	0xA4F6, 0x764D, 0xB761, 0x7DCE, 0x527B, 0xDD3E, 0x5E71, 0x1397,
	0xA6F5, 0xB968, 0x0000, 0xC12C, 0x4060, 0xE31F, 0x79C8, 0xB6ED,
	0xD4BE, 0x8D46, 0x67D9, 0x724B, 0x94DE, 0x98D4, 0xB0E8, 0x854A,
	0xBB6B, 0xC52A, 0x4FE5, 0xED16, 0x86C5, 0x9AD7, 0x6655, 0x1194,
	0x8ACF, 0xE910, 0x0406, 0xFE81, 0xA0F0, 0x7844, 0x25BA, 0x4BE3,
	0xA2F3, 0x5DFE, 0x80C0, 0x058A, 0x3FAD, 0x21BC, 0x7048, 0xF104,
	0x63DF, 0x77C1, 0xAF75, 0x4263, 0x2030, 0xE51A, 0xFD0E, 0xBF6D,
	0x814C, 0x1814, 0x2635, 0xC32F, 0xBEE1, 0x35A2, 0x88CC, 0x2E39,
	0x9357, 0x55F2, 0xFC82, 0x7A47, 0xC8AC, 0xBAE7, 0x322B, 0xE695,
	0xC0A0, 0x1998, 0x9ED1, 0xA37F, 0x4466, 0x547E, 0x3BAB, 0x0B83,
	0x8CCA, 0xC729, 0x6BD3, 0x283C, 0xA779, 0xBCE2, 0x161D, 0xAD76,
	0xDB3B, 0x6456, 0x744E, 0x141E, 0x92DB, 0x0C0A, 0x486C, 0xB8E4,
	0x9F5D, 0xBD6E, 0x43EF, 0xC4A6, 0x39A8, 0x31A4, 0xD337, 0xF28B,
	0xD532, 0x8B43, 0x6E59, 0xDAB7, 0x018C, 0xB164, 0x9CD2, 0x49E0,
	0xD8B4, 0xACFA, 0xF307, 0xCF25, 0xCAAF, 0xF48E, 0x47E9, 0x1018,
	0x6FD5, 0xF088, 0x4A6F, 0x5C72, 0x3824, 0x57F1, 0x73C7, 0x9751,
	0xCB23, 0xA17C, 0xE89C, 0x3E21, 0x96DD, 0x61DC, 0x0D86, 0x0F85,
	0xE090, 0x7C42, 0x71C4, 0xCCAA, 0x90D8, 0x0605, 0xF701, 0x1C12,
	0xC2A3, 0x6A5F, 0xAEF9, 0x69D0, 0x1791, 0x9958, 0x3A27, 0x27B9,
	0xD938, 0xEB13, 0x2BB3, 0x2233, 0xD2BB, 0xA970, 0x0789, 0x33A7,
	0x2DB6, 0x3C22, 0x1592, 0xC920, 0x8749, 0xAAFF, 0x5078, 0xA57A,
	0x038F, 0x59F8, 0x0980, 0x1A17, 0x65DA, 0xD731, 0x84C6, 0xD0B8,
	0x82C3, 0x29B0, 0x5A77, 0x1E11, 0x7BCB, 0xA8FC, 0x6DD6, 0x2C3A,
	},
	{  /* second half is unsigned char-reversed version of first! */
	0xA5C6, 0x84F8, 0x99EE, 0x8DF6, 0x0DFF, 0xBDD6, 0xB1DE, 0x5491,
	0x5060, 0x0302, 0xA9CE, 0x7D56, 0x19E7, 0x62B5, 0xE64D, 0x9AEC,
	0x458F, 0x9D1F, 0x4089, 0x87FA, 0x15EF, 0xEBB2, 0xC98E, 0x0BFB,
	0xEC41, 0x67B3, 0xFD5F, 0xEA45, 0xBF23, 0xF753, 0x96E4, 0x5B9B,
	0xC275, 0x1CE1, 0xAE3D, 0x6A4C, 0x5A6C, 0x417E, 0x02F5, 0x4F83,
	0x5C68, 0xF451, 0x34D1, 0x08F9, 0x93E2, 0x73AB, 0x5362, 0x3F2A,
	0x0C08, 0x5295, 0x6546, 0x5E9D, 0x2830, 0xA137, 0x0F0A, 0xB52F,
	0x090E, 0x3624, 0x9B1B, 0x3DDF, 0x26CD, 0x694E, 0xCD7F, 0x9FEA,
	0x1B12, 0x9E1D, 0x7458, 0x2E34, 0x2D36, 0xB2DC, 0xEEB4, 0xFB5B,
	0xF6A4, 0x4D76, 0x61B7, 0xCE7D, 0x7B52, 0x3EDD, 0x715E, 0x9713,
	0xF5A6, 0x68B9, 0x0000, 0x2CC1, 0x6040, 0x1FE3, 0xC879, 0xEDB6,
	0xBED4, 0x468D, 0xD967, 0x4B72, 0xDE94, 0xD498, 0xE8B0, 0x4A85,
	0x6BBB, 0x2AC5, 0xE54F, 0x16ED, 0xC586, 0xD79A, 0x5566, 0x9411,
	0xCF8A, 0x10E9, 0x0604, 0x81FE, 0xF0A0, 0x4478, 0xBA25, 0xE34B,
	0xF3A2, 0xFE5D, 0xC080, 0x8A05, 0xAD3F, 0xBC21, 0x4870, 0x04F1,
	0xDF63, 0xC177, 0x75AF, 0x6342, 0x3020, 0x1AE5, 0x0EFD, 0x6DBF,
	0x4C81, 0x1418, 0x3526, 0x2FC3, 0xE1BE, 0xA235, 0xCC88, 0x392E,
	0x5793, 0xF255, 0x82FC, 0x477A, 0xACC8, 0xE7BA, 0x2B32, 0x95E6,
	0xA0C0, 0x9819, 0xD19E, 0x7FA3, 0x6644, 0x7E54, 0xAB3B, 0x830B,
	0xCA8C, 0x29C7, 0xD36B, 0x3C28, 0x79A7, 0xE2BC, 0x1D16, 0x76AD,
	0x3BDB, 0x5664, 0x4E74, 0x1E14, 0xDB92, 0x0A0C, 0x6C48, 0xE4B8,
	0x5D9F, 0x6EBD, 0xEF43, 0xA6C4, 0xA839, 0xA431, 0x37D3, 0x8BF2,
	0x32D5, 0x438B, 0x596E, 0xB7DA, 0x8C01, 0x64B1, 0xD29C, 0xE049,
	0xB4D8, 0xFAAC, 0x07F3, 0x25CF, 0xAFCA, 0x8EF4, 0xE947, 0x1810,
	0xD56F, 0x88F0, 0x6F4A, 0x725C, 0x2438, 0xF157, 0xC773, 0x5197,
	0x23CB, 0x7CA1, 0x9CE8, 0x213E, 0xDD96, 0xDC61, 0x860D, 0x850F,
	0x90E0, 0x427C, 0xC471, 0xAACC, 0xD890, 0x0506, 0x01F7, 0x121C,
	0xA3C2, 0x5F6A, 0xF9AE, 0xD069, 0x9117, 0x5899, 0x273A, 0xB927,
	0x38D9, 0x13EB, 0xB32B, 0x3322, 0xBBD2, 0x70A9, 0x8907, 0xA733,
	0xB62D, 0x223C, 0x9215, 0x20C9, 0x4987, 0xFFAA, 0x7850, 0x7AA5,
	0x8F03, 0xF859, 0x8009, 0x171A, 0xDA65, 0x31D7, 0xC684, 0xB8D0,
	0xC382, 0xB029, 0x775A, 0x111E, 0xCB7B, 0xFCA8, 0xD66D, 0x3A2C,
	}
};

/*
 **********************************************************************
 * Routine: Phase 1 -- generate P1K, given TA, TK, IV32
 *
 * Inputs:
 *     tk[]      = temporal key                         [128 bits]
 *     ta[]      = transmitter's MAC address            [ 48 bits]
 *     iv32      = upper 32 bits of IV                  [ 32 bits]
 * Output:
 *     p1k[]     = Phase 1 key                          [ 80 bits]
 *
 * Note:
 *     This function only needs to be called every 2**16 packets,
 *     although in theory it could be called every packet.
 *
 **********************************************************************
 */
static void phase1(u16 *p1k, const u8 *tk, const u8 *ta, u32 iv32)
{
	sint  i;

	/* Initialize the 80 bits of P1K[] from IV32 and TA[0..5]     */
	p1k[0] = Lo16(iv32);
	p1k[1] = Hi16(iv32);
	p1k[2] = Mk16(ta[1], ta[0]); /* use TA[] as little-endian */
	p1k[3] = Mk16(ta[3], ta[2]);
	p1k[4] = Mk16(ta[5], ta[4]);
	/* Now compute an unbalanced Feistel cipher with 80-bit block */
	/* size on the 80-bit block P1K[], using the 128-bit key TK[] */
	for (i = 0; i < PHASE1_LOOP_CNT; i++) {  /* Each add is mod 2**16 */
		p1k[0] += _S_(p1k[4] ^ TK16((i & 1) + 0));
		p1k[1] += _S_(p1k[0] ^ TK16((i & 1) + 2));
		p1k[2] += _S_(p1k[1] ^ TK16((i & 1) + 4));
		p1k[3] += _S_(p1k[2] ^ TK16((i & 1) + 6));
		p1k[4] += _S_(p1k[3] ^ TK16((i & 1) + 0));
		p1k[4] +=  (unsigned short)i;	/* avoid "slide attacks" */
	}
}

/*
 **********************************************************************
 * Routine: Phase 2 -- generate RC4KEY, given TK, P1K, IV16
 *
 * Inputs:
 *     tk[]      = Temporal key                         [128 bits]
 *     p1k[]     = Phase 1 output key                   [ 80 bits]
 *     iv16      = low 16 bits of IV counter            [ 16 bits]
 * Output:
 *     rc4key[]  = the key used to encrypt the packet   [128 bits]
 *
 * Note:
 *     The value {TA,IV32,IV16} for Phase1/Phase2 must be unique
 *     across all packets using the same key TK value. Then, for a
 *     given value of TK[], this TKIP48 construction guarantees that
 *     the final RC4KEY value is unique across all packets.
 *
 * Suggested implementation optimization: if PPK[] is "overlaid"
 *     appropriately on RC4KEY[], there is no need for the final
 *     for loop below that copies the PPK[] result into RC4KEY[].
 *
 **********************************************************************
 */
static void phase2(u8 *rc4key, const u8 *tk, const u16 *p1k, u16 iv16)
{
	sint  i;
	u16 PPK[6];			/* temporary key for mixing    */

	/* Note: all adds in the PPK[] equations below are mod 2**16 */
	for (i = 0; i < 5; i++)
		PPK[i] = p1k[i]; /* first, copy P1K to PPK */
	PPK[5]  =  p1k[4] + iv16; /* next,  add in IV16 */
	/* Bijective non-linear mixing of the 96 bits of PPK[0..5] */
	PPK[0] += _S_(PPK[5] ^ TK16(0));   /* Mix key in each "round" */
	PPK[1] += _S_(PPK[0] ^ TK16(1));
	PPK[2] += _S_(PPK[1] ^ TK16(2));
	PPK[3] += _S_(PPK[2] ^ TK16(3));
	PPK[4] += _S_(PPK[3] ^ TK16(4));
	PPK[5] += _S_(PPK[4] ^ TK16(5));   /* Total # S-box lookups == 6  */
	/* Final sweep: bijective, "linear". Rotates kill LSB correlations   */
	PPK[0] +=  RotR1(PPK[5] ^ TK16(6));
	PPK[1] +=  RotR1(PPK[0] ^ TK16(7));   /* Use all of TK[] in Phase2   */
	PPK[2] +=  RotR1(PPK[1]);
	PPK[3] +=  RotR1(PPK[2]);
	PPK[4] +=  RotR1(PPK[3]);
	PPK[5] +=  RotR1(PPK[4]);
	/* Note: At this point, for a given key TK[0..15], the 96-bit output */
	/* value PPK[0..5] is guaranteed to be unique, as a function   */
	/* of the 96-bit "input" value   {TA,IV32,IV16}. That is, P1K  */
	/* is now a keyed permutation of {TA,IV32,IV16}. */
	/* Set RC4KEY[0..3], which includes "cleartext" portion of RC4 key   */
	rc4key[0] = Hi8(iv16); /* RC4KEY[0..2] is the WEP IV  */
	rc4key[1] = (Hi8(iv16) | 0x20) & 0x7F; /* Help avoid weak (FMS) keys  */
	rc4key[2] = Lo8(iv16);
	rc4key[3] = Lo8((PPK[5] ^ TK16(0)) >> 1);
	/* Copy 96 bits of PPK[0..5] to RC4KEY[4..15]  (little-endian) */
	for (i = 0; i < 6; i++) {
		rc4key[4 + 2 * i] = Lo8(PPK[i]);
		rc4key[5 + 2 * i] = Hi8(PPK[i]);
	}
}

/*The hlen isn't include the IV*/
u32 r8712_tkip_encrypt(struct _adapter *padapter, u8 *pxmitframe)
{	/*  exclude ICV */
	u16 pnl;
	u32 pnh;
	u8 rc4key[16];
	u8 ttkey[16];
	u8 crc[4];
	struct arc4context mycontext;
	u32 curfragnum, length;

	u8 *pframe, *payload, *iv, *prwskey;
	union pn48 txpn;
	struct sta_info *stainfo;
	struct pkt_attrib *pattrib = &((struct xmit_frame *)pxmitframe)->attrib;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	u32 res = _SUCCESS;

	if (((struct xmit_frame *)pxmitframe)->buf_addr == NULL)
		return _FAIL;

	pframe = ((struct xmit_frame *)pxmitframe)->buf_addr + TXDESC_OFFSET;
	/* 4 start to encrypt each fragment */
	if (pattrib->encrypt == _TKIP_) {
		if (pattrib->psta)
			stainfo = pattrib->psta;
		else
			stainfo = r8712_get_stainfo(&padapter->stapriv,
				  &pattrib->ra[0]);
		if (stainfo) {
			prwskey = &stainfo->x_UncstKey.skey[0];
			for (curfragnum = 0; curfragnum < pattrib->nr_frags;
			     curfragnum++) {
				iv = pframe + pattrib->hdrlen;
				payload = pframe + pattrib->iv_len +
					  pattrib->hdrlen;
				GET_TKIP_PN(iv, txpn);
				pnl = (u16)(txpn.val);
				pnh = (u32)(txpn.val >> 16);
				phase1((u16 *)&ttkey[0], prwskey,
				       &pattrib->ta[0], pnh);
				phase2(&rc4key[0], prwskey, (u16 *)&ttkey[0],
				       pnl);
				if ((curfragnum + 1) == pattrib->nr_frags) {
					/* 4 the last fragment */
					length = pattrib->last_txcmdsz -
					     pattrib->hdrlen -
					     pattrib->iv_len -
					     pattrib->icv_len;
					*((__le32 *)crc) = cpu_to_le32(
						getcrc32(payload, length));
					arcfour_init(&mycontext, rc4key, 16);
					arcfour_encrypt(&mycontext, payload,
							payload, length);
					arcfour_encrypt(&mycontext, payload +
							length, crc, 4);
				} else {
					length = pxmitpriv->frag_len -
						 pattrib->hdrlen -
						 pattrib->iv_len -
						 pattrib->icv_len;
					*((__le32 *)crc) = cpu_to_le32(getcrc32(
							payload, length));
					arcfour_init(&mycontext, rc4key, 16);
					arcfour_encrypt(&mycontext, payload,
							 payload, length);
					arcfour_encrypt(&mycontext,
							payload + length, crc,
							4);
					pframe += pxmitpriv->frag_len;
					pframe = (u8 *)RND4((addr_t)(pframe));
				}
			}
		} else {
			res = _FAIL;
		}
	}
	return res;
}

/* The hlen doesn't include the IV */
void r8712_tkip_decrypt(struct _adapter *padapter, u8 *precvframe)
{	/* exclude ICV */
	u16 pnl;
	u32 pnh;
	u8 rc4key[16];
	u8 ttkey[16];
	u8 crc[4];
	struct arc4context mycontext;
	u32 length;
	u8 *pframe, *payload, *iv, *prwskey, idx = 0;
	union pn48 txpn;
	struct	sta_info *stainfo;
	struct	rx_pkt_attrib *prxattrib = &((union recv_frame *)
					   precvframe)->u.hdr.attrib;
	struct	security_priv	*psecuritypriv = &padapter->securitypriv;

	pframe = (unsigned char *)((union recv_frame *)
				   precvframe)->u.hdr.rx_data;
	/* 4 start to decrypt recvframe */
	if (prxattrib->encrypt == _TKIP_) {
		stainfo = r8712_get_stainfo(&padapter->stapriv,
					    &prxattrib->ta[0]);
		if (stainfo) {
			iv = pframe + prxattrib->hdrlen;
			payload = pframe + prxattrib->iv_len +
				  prxattrib->hdrlen;
			length = ((union recv_frame *)precvframe)->
				 u.hdr.len - prxattrib->hdrlen -
				 prxattrib->iv_len;
			if (is_multicast_ether_addr(prxattrib->ra)) {
				idx = iv[3];
				prwskey = &psecuritypriv->XGrpKey[
					 ((idx >> 6) & 0x3) - 1].skey[0];
				if (!psecuritypriv->binstallGrpkey)
					return;
			} else {
				prwskey = &stainfo->x_UncstKey.skey[0];
			}
			GET_TKIP_PN(iv, txpn);
			pnl = (u16)(txpn.val);
			pnh = (u32)(txpn.val >> 16);
			phase1((u16 *)&ttkey[0], prwskey, &prxattrib->ta[0],
				pnh);
			phase2(&rc4key[0], prwskey, (unsigned short *)
			       &ttkey[0], pnl);
			/* 4 decrypt payload include icv */
			arcfour_init(&mycontext, rc4key, 16);
			arcfour_encrypt(&mycontext, payload, payload, length);
			*((__le32 *)crc) = cpu_to_le32(getcrc32(payload,
					length - 4));
		}
	}
}

/* 3 =====AES related===== */

#define MAX_MSG_SIZE	2048
/*****************************/
/******** SBOX Table *********/
/*****************************/

static const u8 sbox_table[256] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/****************************************/
/* aes128k128d()                        */
/* Performs a 128 bit AES encrypt with  */
/* 128 bit data.                        */
/****************************************/
static void xor_128(u8 *a, u8 *b, u8 *out)
{
	sint i;

	for (i = 0; i < 16; i++)
		out[i] = a[i] ^ b[i];
}

static void xor_32(u8 *a, u8 *b, u8 *out)
{
	sint i;

	for (i = 0; i < 4; i++)
		out[i] = a[i] ^ b[i];
}

static u8 sbox(u8 a)
{
	return sbox_table[(sint)a];
}

static void next_key(u8 *key, sint round)
{
	u8 rcon;
	u8 sbox_key[4];
	static const u8 rcon_table[12] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
		0x1b, 0x36, 0x36, 0x36
	};

	sbox_key[0] = sbox(key[13]);
	sbox_key[1] = sbox(key[14]);
	sbox_key[2] = sbox(key[15]);
	sbox_key[3] = sbox(key[12]);
	rcon = rcon_table[round];
	xor_32(&key[0], sbox_key, &key[0]);
	key[0] = key[0] ^ rcon;
	xor_32(&key[4], &key[0], &key[4]);
	xor_32(&key[8], &key[4], &key[8]);
	xor_32(&key[12], &key[8], &key[12]);
}

static void byte_sub(u8 *in, u8 *out)
{
	sint i;

	for (i = 0; i < 16; i++)
		out[i] = sbox(in[i]);
}

static void shift_row(u8 *in, u8 *out)
{
	out[0] =  in[0];
	out[1] =  in[5];
	out[2] =  in[10];
	out[3] =  in[15];
	out[4] =  in[4];
	out[5] =  in[9];
	out[6] =  in[14];
	out[7] =  in[3];
	out[8] =  in[8];
	out[9] =  in[13];
	out[10] = in[2];
	out[11] = in[7];
	out[12] = in[12];
	out[13] = in[1];
	out[14] = in[6];
	out[15] = in[11];
}

static void mix_column(u8 *in, u8 *out)
{
	sint i;
	u8 add1b[4];
	u8 add1bf7[4];
	u8 rotl[4];
	u8 swap_halves[4];
	u8 andf7[4];
	u8 rotr[4];
	u8 temp[4];
	u8 tempb[4];

	for (i = 0; i < 4; i++) {
		if ((in[i] & 0x80) == 0x80)
			add1b[i] = 0x1b;
		else
			add1b[i] = 0x00;
	}
	swap_halves[0] = in[2];    /* Swap halves */
	swap_halves[1] = in[3];
	swap_halves[2] = in[0];
	swap_halves[3] = in[1];
	rotl[0] = in[3];        /* Rotate left 8 bits */
	rotl[1] = in[0];
	rotl[2] = in[1];
	rotl[3] = in[2];
	andf7[0] = in[0] & 0x7f;
	andf7[1] = in[1] & 0x7f;
	andf7[2] = in[2] & 0x7f;
	andf7[3] = in[3] & 0x7f;
	for (i = 3; i > 0; i--) {   /* logical shift left 1 bit */
		andf7[i] = andf7[i] << 1;
		if ((andf7[i - 1] & 0x80) == 0x80)
			andf7[i] = (andf7[i] | 0x01);
	}
	andf7[0] = andf7[0] << 1;
	andf7[0] = andf7[0] & 0xfe;
	xor_32(add1b, andf7, add1bf7);
	xor_32(in, add1bf7, rotr);
	temp[0] = rotr[0];         /* Rotate right 8 bits */
	rotr[0] = rotr[1];
	rotr[1] = rotr[2];
	rotr[2] = rotr[3];
	rotr[3] = temp[0];
	xor_32(add1bf7, rotr, temp);
	xor_32(swap_halves, rotl, tempb);
	xor_32(temp, tempb, out);
}

static void aes128k128d(u8 *key, u8 *data, u8 *ciphertext)
{
	sint round;
	sint i;
	u8 intermediatea[16];
	u8 intermediateb[16];
	u8 round_key[16];

	for (i = 0; i < 16; i++)
		round_key[i] = key[i];
	for (round = 0; round < 11; round++) {
		if (round == 0) {
			xor_128(round_key, data, ciphertext);
			next_key(round_key, round);
		} else if (round == 10) {
			byte_sub(ciphertext, intermediatea);
			shift_row(intermediatea, intermediateb);
			xor_128(intermediateb, round_key, ciphertext);
		} else {   /* 1 - 9 */
			byte_sub(ciphertext, intermediatea);
			shift_row(intermediatea, intermediateb);
			mix_column(&intermediateb[0], &intermediatea[0]);
			mix_column(&intermediateb[4], &intermediatea[4]);
			mix_column(&intermediateb[8], &intermediatea[8]);
			mix_column(&intermediateb[12], &intermediatea[12]);
			xor_128(intermediatea, round_key, ciphertext);
			next_key(round_key, round);
		}
	}
}

/************************************************/
/* construct_mic_iv()                           */
/* Builds the MIC IV from header fields and PN  */
/************************************************/
static void construct_mic_iv(u8 *mic_iv, sint qc_exists, sint a4_exists,
			     u8 *mpdu, uint payload_length, u8 *pn_vector)
{
	sint i;

	mic_iv[0] = 0x59;
	if (qc_exists && a4_exists)
		mic_iv[1] = mpdu[30] & 0x0f;    /* QoS_TC           */
	if (qc_exists && !a4_exists)
		mic_iv[1] = mpdu[24] & 0x0f;   /* mute bits 7-4    */
	if (!qc_exists)
		mic_iv[1] = 0x00;
	for (i = 2; i < 8; i++)
		mic_iv[i] = mpdu[i + 8];
	for (i = 8; i < 14; i++)
		mic_iv[i] = pn_vector[13 - i]; /* mic_iv[8:13] = PN[5:0] */
	mic_iv[14] = (unsigned char)(payload_length / 256);
	mic_iv[15] = (unsigned char)(payload_length % 256);
}

/************************************************/
/* construct_mic_header1()                      */
/* Builds the first MIC header block from       */
/* header fields.                               */
/************************************************/
static void construct_mic_header1(u8 *mic_header1, sint header_length, u8 *mpdu)
{
	mic_header1[0] = (u8)((header_length - 2) / 256);
	mic_header1[1] = (u8)((header_length - 2) % 256);
	mic_header1[2] = mpdu[0] & 0xcf;    /* Mute CF poll & CF ack bits */
	/* Mute retry, more data and pwr mgt bits */
	mic_header1[3] = mpdu[1] & 0xc7;
	mic_header1[4] = mpdu[4];       /* A1 */
	mic_header1[5] = mpdu[5];
	mic_header1[6] = mpdu[6];
	mic_header1[7] = mpdu[7];
	mic_header1[8] = mpdu[8];
	mic_header1[9] = mpdu[9];
	mic_header1[10] = mpdu[10];     /* A2 */
	mic_header1[11] = mpdu[11];
	mic_header1[12] = mpdu[12];
	mic_header1[13] = mpdu[13];
	mic_header1[14] = mpdu[14];
	mic_header1[15] = mpdu[15];
}

/************************************************/
/* construct_mic_header2()                      */
/* Builds the last MIC header block from        */
/* header fields.                               */
/************************************************/
static void construct_mic_header2(u8 *mic_header2, u8 *mpdu, sint a4_exists,
			   sint qc_exists)
{
	sint i;

	for (i = 0; i < 16; i++)
		mic_header2[i] = 0x00;
	mic_header2[0] = mpdu[16];    /* A3 */
	mic_header2[1] = mpdu[17];
	mic_header2[2] = mpdu[18];
	mic_header2[3] = mpdu[19];
	mic_header2[4] = mpdu[20];
	mic_header2[5] = mpdu[21];
	mic_header2[6] = 0x00;
	mic_header2[7] = 0x00; /* mpdu[23]; */
	if (!qc_exists && a4_exists)
		for (i = 0; i < 6; i++)
			mic_header2[8 + i] = mpdu[24 + i];   /* A4 */
	if (qc_exists && !a4_exists) {
		mic_header2[8] = mpdu[24] & 0x0f; /* mute bits 15 - 4 */
		mic_header2[9] = mpdu[25] & 0x00;
	}
	if (qc_exists && a4_exists) {
		for (i = 0; i < 6; i++)
			mic_header2[8 + i] = mpdu[24 + i];   /* A4 */
		mic_header2[14] = mpdu[30] & 0x0f;
		mic_header2[15] = mpdu[31] & 0x00;
	}
}

/************************************************/
/* construct_mic_header2()                      */
/* Builds the last MIC header block from        */
/* header fields.                               */
/************************************************/
static void construct_ctr_preload(u8 *ctr_preload,
				  sint a4_exists, sint qc_exists,
				  u8 *mpdu, u8 *pn_vector, sint c)
{
	sint i;

	for (i = 0; i < 16; i++)
		ctr_preload[i] = 0x00;
	i = 0;
	ctr_preload[0] = 0x01;    /* flag */
	if (qc_exists && a4_exists)
		ctr_preload[1] = mpdu[30] & 0x0f;
	if (qc_exists && !a4_exists)
		ctr_preload[1] = mpdu[24] & 0x0f;
	for (i = 2; i < 8; i++)
		ctr_preload[i] = mpdu[i + 8];
	for (i = 8; i < 14; i++)
		ctr_preload[i] = pn_vector[13 - i];
	ctr_preload[14] = (unsigned char)(c / 256); /* Ctr */
	ctr_preload[15] = (unsigned char)(c % 256);
}

/************************************/
/* bitwise_xor()                    */
/* A 128 bit, bitwise exclusive or  */
/************************************/
static void bitwise_xor(u8 *ina, u8 *inb, u8 *out)
{
	sint i;

	for (i = 0; i < 16; i++)
		out[i] = ina[i] ^ inb[i];
}

static void aes_cipher(u8 *key, uint hdrlen,
		       u8 *pframe, uint plen)
{
	uint qc_exists, a4_exists, i, j, payload_remainder;
	uint num_blocks, payload_index;

	u8 pn_vector[6];
	u8 mic_iv[16];
	u8 mic_header1[16];
	u8 mic_header2[16];
	u8 ctr_preload[16];

	/* Intermediate Buffers */
	u8 chain_buffer[16];
	u8 aes_out[16];
	u8 padded_buffer[16];
	u8 mic[8];
	u16 frtype  = GetFrameType(pframe);
	u16 frsubtype  = GetFrameSubType(pframe);

	frsubtype >>= 4;
	memset((void *)mic_iv, 0, 16);
	memset((void *)mic_header1, 0, 16);
	memset((void *)mic_header2, 0, 16);
	memset((void *)ctr_preload, 0, 16);
	memset((void *)chain_buffer, 0, 16);
	memset((void *)aes_out, 0, 16);
	memset((void *)padded_buffer, 0, 16);

	if ((hdrlen == WLAN_HDR_A3_LEN) || (hdrlen ==  WLAN_HDR_A3_QOS_LEN))
		a4_exists = 0;
	else
		a4_exists = 1;

	if ((frtype == IEEE80211_STYPE_DATA_CFACK) ||
	    (frtype == IEEE80211_STYPE_DATA_CFPOLL) ||
	    (frtype == IEEE80211_STYPE_DATA_CFACKPOLL)) {
		qc_exists = 1;
		if (hdrlen !=  WLAN_HDR_A3_QOS_LEN)
			hdrlen += 2;
	} else if ((frsubtype == 0x08) ||
		   (frsubtype == 0x09) ||
		   (frsubtype == 0x0a) ||
		   (frsubtype == 0x0b)) {
		if (hdrlen !=  WLAN_HDR_A3_QOS_LEN)
			hdrlen += 2;
		qc_exists = 1;
	} else {
		qc_exists = 0;
	}
	pn_vector[0] = pframe[hdrlen];
	pn_vector[1] = pframe[hdrlen + 1];
	pn_vector[2] = pframe[hdrlen + 4];
	pn_vector[3] = pframe[hdrlen + 5];
	pn_vector[4] = pframe[hdrlen + 6];
	pn_vector[5] = pframe[hdrlen + 7];
	construct_mic_iv(mic_iv, qc_exists, a4_exists, pframe, plen, pn_vector);
	construct_mic_header1(mic_header1, hdrlen, pframe);
	construct_mic_header2(mic_header2, pframe, a4_exists, qc_exists);
	payload_remainder = plen % 16;
	num_blocks = plen / 16;
	/* Find start of payload */
	payload_index = hdrlen + 8;
	/* Calculate MIC */
	aes128k128d(key, mic_iv, aes_out);
	bitwise_xor(aes_out, mic_header1, chain_buffer);
	aes128k128d(key, chain_buffer, aes_out);
	bitwise_xor(aes_out, mic_header2, chain_buffer);
	aes128k128d(key, chain_buffer, aes_out);
	for (i = 0; i < num_blocks; i++) {
		bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);
		payload_index += 16;
		aes128k128d(key, chain_buffer, aes_out);
	}
	/* Add on the final payload block if it needs padding */
	if (payload_remainder > 0) {
		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++)
			padded_buffer[j] = pframe[payload_index++];
		bitwise_xor(aes_out, padded_buffer, chain_buffer);
		aes128k128d(key, chain_buffer, aes_out);
	}
	for (j = 0; j < 8; j++)
		mic[j] = aes_out[j];
	/* Insert MIC into payload */
	for (j = 0; j < 8; j++)
		pframe[payload_index + j] = mic[j];
	payload_index = hdrlen + 8;
	for (i = 0; i < num_blocks; i++) {
		construct_ctr_preload(ctr_preload, a4_exists, qc_exists,
				      pframe, pn_vector, i + 1);
		aes128k128d(key, ctr_preload, aes_out);
		bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);
		for (j = 0; j < 16; j++)
			pframe[payload_index++] = chain_buffer[j];
	}
	if (payload_remainder > 0) {  /* If short final block, then pad it,*/
				      /* encrypt and copy unpadded part back */
		construct_ctr_preload(ctr_preload, a4_exists, qc_exists,
				      pframe, pn_vector, num_blocks + 1);
		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++)
			padded_buffer[j] = pframe[payload_index + j];
		aes128k128d(key, ctr_preload, aes_out);
		bitwise_xor(aes_out, padded_buffer, chain_buffer);
		for (j = 0; j < payload_remainder; j++)
			pframe[payload_index++] = chain_buffer[j];
	}
	/* Encrypt the MIC */
	construct_ctr_preload(ctr_preload, a4_exists, qc_exists,
			      pframe, pn_vector, 0);
	for (j = 0; j < 16; j++)
		padded_buffer[j] = 0x00;
	for (j = 0; j < 8; j++)
		padded_buffer[j] = pframe[j + hdrlen + 8 + plen];
	aes128k128d(key, ctr_preload, aes_out);
	bitwise_xor(aes_out, padded_buffer, chain_buffer);
	for (j = 0; j < 8; j++)
		pframe[payload_index++] = chain_buffer[j];
}

u32 r8712_aes_encrypt(struct _adapter *padapter, u8 *pxmitframe)
{	/* exclude ICV */
	/* Intermediate Buffers */
	sint	curfragnum, length;
	u8	*pframe, *prwskey;
	struct	sta_info *stainfo;
	struct	pkt_attrib  *pattrib = &((struct xmit_frame *)
				       pxmitframe)->attrib;
	struct	xmit_priv *pxmitpriv = &padapter->xmitpriv;
	u32 res = _SUCCESS;

	if (((struct xmit_frame *)pxmitframe)->buf_addr == NULL)
		return _FAIL;
	pframe = ((struct xmit_frame *)pxmitframe)->buf_addr + TXDESC_OFFSET;
	/* 4 start to encrypt each fragment */
	if (pattrib->encrypt == _AES_) {
		if (pattrib->psta)
			stainfo = pattrib->psta;
		else
			stainfo = r8712_get_stainfo(&padapter->stapriv,
				  &pattrib->ra[0]);
		if (stainfo) {
			prwskey = &stainfo->x_UncstKey.skey[0];
			for (curfragnum = 0; curfragnum < pattrib->nr_frags;
			     curfragnum++) {
				if ((curfragnum + 1) == pattrib->nr_frags) {
					length = pattrib->last_txcmdsz -
						 pattrib->hdrlen -
						 pattrib->iv_len -
						 pattrib->icv_len;
					aes_cipher(prwskey, pattrib->hdrlen,
						   pframe, length);
				} else {
					length = pxmitpriv->frag_len -
						 pattrib->hdrlen -
						 pattrib->iv_len -
						 pattrib->icv_len;
					aes_cipher(prwskey, pattrib->hdrlen,
						   pframe, length);
					pframe += pxmitpriv->frag_len;
					pframe = (u8 *)RND4((addr_t)(pframe));
				}
			}
		} else {
			res = _FAIL;
		}
	}
	return res;
}

static void aes_decipher(u8 *key, uint hdrlen,
			 u8 *pframe, uint plen)
{
	static u8 message[MAX_MSG_SIZE];
	uint qc_exists, a4_exists, i, j, payload_remainder;
	uint num_blocks, payload_index;
	u8 pn_vector[6];
	u8 mic_iv[16];
	u8 mic_header1[16];
	u8 mic_header2[16];
	u8 ctr_preload[16];
	/* Intermediate Buffers */
	u8 chain_buffer[16];
	u8 aes_out[16];
	u8 padded_buffer[16];
	u8 mic[8];
	uint frtype  = GetFrameType(pframe);
	uint frsubtype  = GetFrameSubType(pframe);

	frsubtype >>= 4;
	memset((void *)mic_iv, 0, 16);
	memset((void *)mic_header1, 0, 16);
	memset((void *)mic_header2, 0, 16);
	memset((void *)ctr_preload, 0, 16);
	memset((void *)chain_buffer, 0, 16);
	memset((void *)aes_out, 0, 16);
	memset((void *)padded_buffer, 0, 16);
	/* start to decrypt the payload */
	/*(plen including llc, payload and mic) */
	num_blocks = (plen - 8) / 16;
	payload_remainder = (plen - 8) % 16;
	pn_vector[0] = pframe[hdrlen];
	pn_vector[1] = pframe[hdrlen + 1];
	pn_vector[2] = pframe[hdrlen + 4];
	pn_vector[3] = pframe[hdrlen + 5];
	pn_vector[4] = pframe[hdrlen + 6];
	pn_vector[5] = pframe[hdrlen + 7];
	if ((hdrlen == WLAN_HDR_A3_LEN) || (hdrlen ==  WLAN_HDR_A3_QOS_LEN))
		a4_exists = 0;
	else
		a4_exists = 1;
	if ((frtype == IEEE80211_STYPE_DATA_CFACK) ||
	    (frtype == IEEE80211_STYPE_DATA_CFPOLL) ||
	    (frtype == IEEE80211_STYPE_DATA_CFACKPOLL)) {
		qc_exists = 1;
		if (hdrlen != WLAN_HDR_A3_QOS_LEN)
			hdrlen += 2;
	} else if ((frsubtype == 0x08) ||
		   (frsubtype == 0x09) ||
		   (frsubtype == 0x0a) ||
		   (frsubtype == 0x0b)) {
		if (hdrlen != WLAN_HDR_A3_QOS_LEN)
			hdrlen += 2;
		qc_exists = 1;
	} else {
		qc_exists = 0;
	}
	/* now, decrypt pframe with hdrlen offset and plen long */
	payload_index = hdrlen + 8; /* 8 is for extiv */
	for (i = 0; i < num_blocks; i++) {
		construct_ctr_preload(ctr_preload, a4_exists, qc_exists,
				      pframe, pn_vector, i + 1);
		aes128k128d(key, ctr_preload, aes_out);
		bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);
		for (j = 0; j < 16; j++)
			pframe[payload_index++] = chain_buffer[j];
	}
	if (payload_remainder > 0) {  /* If short final block, pad it,*/
		/* encrypt it and copy the unpadded part back   */
		construct_ctr_preload(ctr_preload, a4_exists, qc_exists,
				      pframe, pn_vector, num_blocks + 1);
		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++)
			padded_buffer[j] = pframe[payload_index + j];
		aes128k128d(key, ctr_preload, aes_out);
		bitwise_xor(aes_out, padded_buffer, chain_buffer);
		for (j = 0; j < payload_remainder; j++)
			pframe[payload_index++] = chain_buffer[j];
	}
	/* start to calculate the mic */
	memcpy((void *)message, pframe, (hdrlen + plen + 8));
	pn_vector[0] = pframe[hdrlen];
	pn_vector[1] = pframe[hdrlen + 1];
	pn_vector[2] = pframe[hdrlen + 4];
	pn_vector[3] = pframe[hdrlen + 5];
	pn_vector[4] = pframe[hdrlen + 6];
	pn_vector[5] = pframe[hdrlen + 7];
	construct_mic_iv(mic_iv, qc_exists, a4_exists, message, plen - 8,
			 pn_vector);
	construct_mic_header1(mic_header1, hdrlen, message);
	construct_mic_header2(mic_header2, message, a4_exists, qc_exists);
	payload_remainder = (plen - 8) % 16;
	num_blocks = (plen - 8) / 16;
	/* Find start of payload */
	payload_index = hdrlen + 8;
	/* Calculate MIC */
	aes128k128d(key, mic_iv, aes_out);
	bitwise_xor(aes_out, mic_header1, chain_buffer);
	aes128k128d(key, chain_buffer, aes_out);
	bitwise_xor(aes_out, mic_header2, chain_buffer);
	aes128k128d(key, chain_buffer, aes_out);
	for (i = 0; i < num_blocks; i++) {
		bitwise_xor(aes_out, &message[payload_index], chain_buffer);
		payload_index += 16;
		aes128k128d(key, chain_buffer, aes_out);
	}
	/* Add on the final payload block if it needs padding */
	if (payload_remainder > 0) {
		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++)
			padded_buffer[j] = message[payload_index++];
		bitwise_xor(aes_out, padded_buffer, chain_buffer);
		aes128k128d(key, chain_buffer, aes_out);
	}
	for (j = 0; j < 8; j++)
		mic[j] = aes_out[j];
	/* Insert MIC into payload */
	for (j = 0; j < 8; j++)
		message[payload_index + j] = mic[j];
	payload_index = hdrlen + 8;
	for (i = 0; i < num_blocks; i++) {
		construct_ctr_preload(ctr_preload, a4_exists, qc_exists,
				      message, pn_vector, i + 1);
		aes128k128d(key, ctr_preload, aes_out);
		bitwise_xor(aes_out, &message[payload_index], chain_buffer);
		for (j = 0; j < 16; j++)
			message[payload_index++] = chain_buffer[j];
	}
	if (payload_remainder > 0) { /* If short final block, pad it,*/
				     /* encrypt and copy unpadded part back */
		construct_ctr_preload(ctr_preload, a4_exists, qc_exists,
				      message, pn_vector, num_blocks + 1);
		for (j = 0; j < 16; j++)
			padded_buffer[j] = 0x00;
		for (j = 0; j < payload_remainder; j++)
			padded_buffer[j] = message[payload_index + j];
		aes128k128d(key, ctr_preload, aes_out);
		bitwise_xor(aes_out, padded_buffer, chain_buffer);
		for (j = 0; j < payload_remainder; j++)
			message[payload_index++] = chain_buffer[j];
	}
	/* Encrypt the MIC */
	construct_ctr_preload(ctr_preload, a4_exists, qc_exists, message,
			      pn_vector, 0);
	for (j = 0; j < 16; j++)
		padded_buffer[j] = 0x00;
	for (j = 0; j < 8; j++)
		padded_buffer[j] = message[j + hdrlen + plen];
	aes128k128d(key, ctr_preload, aes_out);
	bitwise_xor(aes_out, padded_buffer, chain_buffer);
	for (j = 0; j < 8; j++)
		message[payload_index++] = chain_buffer[j];
	/* compare the mic */
}

void r8712_aes_decrypt(struct _adapter *padapter, u8 *precvframe)
{	/* exclude ICV */
	/* Intermediate Buffers */
	sint		length;
	u8	*pframe, *prwskey, *iv, idx;
	struct	sta_info *stainfo;
	struct	rx_pkt_attrib *prxattrib = &((union recv_frame *)
					   precvframe)->u.hdr.attrib;
	struct	security_priv *psecuritypriv = &padapter->securitypriv;

	pframe = (unsigned char *)((union recv_frame *)precvframe)->
		 u.hdr.rx_data;
	/* 4 start to encrypt each fragment */
	if (prxattrib->encrypt == _AES_) {
		stainfo = r8712_get_stainfo(&padapter->stapriv,
					    &prxattrib->ta[0]);
		if (stainfo) {
			if (is_multicast_ether_addr(prxattrib->ra)) {
				iv = pframe + prxattrib->hdrlen;
				idx = iv[3];
				prwskey = &psecuritypriv->XGrpKey[
					  ((idx >> 6) & 0x3) - 1].skey[0];
				if (!psecuritypriv->binstallGrpkey)
					return;

			} else {
				prwskey = &stainfo->x_UncstKey.skey[0];
			}
			length = ((union recv_frame *)precvframe)->
				 u.hdr.len - prxattrib->hdrlen -
				 prxattrib->iv_len;
			aes_decipher(prwskey, prxattrib->hdrlen, pframe,
				     length);
		}
	}
}

void r8712_use_tkipkey_handler(struct timer_list *t)
{
	struct _adapter *padapter =
		from_timer(padapter, t, securitypriv.tkip_timer);

	padapter->securitypriv.busetkipkey = true;
}
