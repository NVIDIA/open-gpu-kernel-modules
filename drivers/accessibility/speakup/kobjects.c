// SPDX-License-Identifier: GPL-2.0
/*
 * Speakup kobject implementation
 *
 * Copyright (C) 2009 William Hubbs
 *
 * This code is based on kobject-example.c, which came with linux 2.6.x.
 *
 * Copyright (C) 2004-2007 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2007 Novell Inc.
 *
 * Released under the GPL version 2 only.
 *
 */
#include <linux/slab.h>		/* For kmalloc. */
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/string_helpers.h>
#include <linux/sysfs.h>
#include <linux/ctype.h>

#include "speakup.h"
#include "spk_priv.h"

/*
 * This is called when a user reads the characters or chartab sys file.
 */
static ssize_t chars_chartab_show(struct kobject *kobj,
				  struct kobj_attribute *attr, char *buf)
{
	int i;
	int len = 0;
	char *cp;
	char *buf_pointer = buf;
	size_t bufsize = PAGE_SIZE;
	unsigned long flags;

	spin_lock_irqsave(&speakup_info.spinlock, flags);
	*buf_pointer = '\0';
	for (i = 0; i < 256; i++) {
		if (bufsize <= 1)
			break;
		if (strcmp("characters", attr->attr.name) == 0) {
			len = scnprintf(buf_pointer, bufsize, "%d\t%s\n",
					i, spk_characters[i]);
		} else {	/* show chartab entry */
			if (IS_TYPE(i, B_CTL))
				cp = "B_CTL";
			else if (IS_TYPE(i, WDLM))
				cp = "WDLM";
			else if (IS_TYPE(i, A_PUNC))
				cp = "A_PUNC";
			else if (IS_TYPE(i, PUNC))
				cp = "PUNC";
			else if (IS_TYPE(i, NUM))
				cp = "NUM";
			else if (IS_TYPE(i, A_CAP))
				cp = "A_CAP";
			else if (IS_TYPE(i, ALPHA))
				cp = "ALPHA";
			else if (IS_TYPE(i, B_CAPSYM))
				cp = "B_CAPSYM";
			else if (IS_TYPE(i, B_SYM))
				cp = "B_SYM";
			else
				cp = "0";
			len =
			    scnprintf(buf_pointer, bufsize, "%d\t%s\n", i, cp);
		}
		bufsize -= len;
		buf_pointer += len;
	}
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return buf_pointer - buf;
}

/*
 * Print informational messages or warnings after updating
 * character descriptions or chartab entries.
 */
static void report_char_chartab_status(int reset, int received, int used,
				       int rejected, int do_characters)
{
	static char const *object_type[] = {
		"character class entries",
		"character descriptions",
	};
	int len;
	char buf[80];

	if (reset) {
		pr_info("%s reset to defaults\n", object_type[do_characters]);
	} else if (received) {
		len = snprintf(buf, sizeof(buf),
			       " updated %d of %d %s\n",
			       used, received, object_type[do_characters]);
		if (rejected)
			snprintf(buf + (len - 1), sizeof(buf) - (len - 1),
				 " with %d reject%s\n",
				 rejected, rejected > 1 ? "s" : "");
		pr_info("%s", buf);
	}
}

/*
 * This is called when a user changes the characters or chartab parameters.
 */
static ssize_t chars_chartab_store(struct kobject *kobj,
				   struct kobj_attribute *attr,
				   const char *buf, size_t count)
{
	char *cp = (char *)buf;
	char *end = cp + count; /* the null at the end of the buffer */
	char *linefeed = NULL;
	char keyword[MAX_DESC_LEN + 1];
	char *outptr = NULL;	/* Will hold keyword or desc. */
	char *temp = NULL;
	char *desc = NULL;
	ssize_t retval = count;
	unsigned long flags;
	unsigned long index = 0;
	int charclass = 0;
	int received = 0;
	int used = 0;
	int rejected = 0;
	int reset = 0;
	int do_characters = !strcmp(attr->attr.name, "characters");
	size_t desc_length = 0;
	int i;

	spin_lock_irqsave(&speakup_info.spinlock, flags);
	while (cp < end) {
		while ((cp < end) && (*cp == ' ' || *cp == '\t'))
			cp++;

		if (cp == end)
			break;
		if ((*cp == '\n') || strchr("dDrR", *cp)) {
			reset = 1;
			break;
		}
		received++;

		linefeed = strchr(cp, '\n');
		if (!linefeed) {
			rejected++;
			break;
		}

		if (!isdigit(*cp)) {
			rejected++;
			cp = linefeed + 1;
			continue;
		}

		/*
		 * Do not replace with kstrtoul:
		 * here we need temp to be updated
		 */
		index = simple_strtoul(cp, &temp, 10);
		if (index > 255) {
			rejected++;
			cp = linefeed + 1;
			continue;
		}

		while ((temp < linefeed) && (*temp == ' ' || *temp == '\t'))
			temp++;

		desc_length = linefeed - temp;
		if (desc_length > MAX_DESC_LEN) {
			rejected++;
			cp = linefeed + 1;
			continue;
		}
		if (do_characters) {
			desc = kmalloc(desc_length + 1, GFP_ATOMIC);
			if (!desc) {
				retval = -ENOMEM;
				reset = 1;	/* just reset on error. */
				break;
			}
			outptr = desc;
		} else {
			outptr = keyword;
		}

		for (i = 0; i < desc_length; i++)
			outptr[i] = temp[i];
		outptr[desc_length] = '\0';

		if (do_characters) {
			if (spk_characters[index] != spk_default_chars[index])
				kfree(spk_characters[index]);
			spk_characters[index] = desc;
			used++;
		} else {
			charclass = spk_chartab_get_value(keyword);
			if (charclass == 0) {
				rejected++;
				cp = linefeed + 1;
				continue;
			}
			if (charclass != spk_chartab[index]) {
				spk_chartab[index] = charclass;
				used++;
			}
		}
		cp = linefeed + 1;
	}

	if (reset) {
		if (do_characters)
			spk_reset_default_chars();
		else
			spk_reset_default_chartab();
	}

	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	report_char_chartab_status(reset, received, used, rejected,
				   do_characters);
	return retval;
}

/*
 * This is called when a user reads the keymap parameter.
 */
static ssize_t keymap_show(struct kobject *kobj, struct kobj_attribute *attr,
			   char *buf)
{
	char *cp = buf;
	int i;
	int n;
	int num_keys;
	int nstates;
	u_char *cp1;
	u_char ch;
	unsigned long flags;

	spin_lock_irqsave(&speakup_info.spinlock, flags);
	cp1 = spk_key_buf + SHIFT_TBL_SIZE;
	num_keys = (int)(*cp1);
	nstates = (int)cp1[1];
	cp += sprintf(cp, "%d, %d, %d,\n", KEY_MAP_VER, num_keys, nstates);
	cp1 += 2; /* now pointing at shift states */
	/* dump num_keys+1 as first row is shift states + flags,
	 * each subsequent row is key + states
	 */
	for (n = 0; n <= num_keys; n++) {
		for (i = 0; i <= nstates; i++) {
			ch = *cp1++;
			cp += sprintf(cp, "%d,", (int)ch);
			*cp++ = (i < nstates) ? SPACE : '\n';
		}
	}
	cp += sprintf(cp, "0, %d\n", KEY_MAP_VER);
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return (int)(cp - buf);
}

/*
 * This is called when a user changes the keymap parameter.
 */
static ssize_t keymap_store(struct kobject *kobj, struct kobj_attribute *attr,
			    const char *buf, size_t count)
{
	int i;
	ssize_t ret = count;
	char *in_buff = NULL;
	char *cp;
	u_char *cp1;
	unsigned long flags;

	spin_lock_irqsave(&speakup_info.spinlock, flags);
	in_buff = kmemdup(buf, count + 1, GFP_ATOMIC);
	if (!in_buff) {
		spin_unlock_irqrestore(&speakup_info.spinlock, flags);
		return -ENOMEM;
	}
	if (strchr("dDrR", *in_buff)) {
		spk_set_key_info(spk_key_defaults, spk_key_buf);
		pr_info("keymap set to default values\n");
		kfree(in_buff);
		spin_unlock_irqrestore(&speakup_info.spinlock, flags);
		return count;
	}
	if (in_buff[count - 1] == '\n')
		in_buff[count - 1] = '\0';
	cp = in_buff;
	cp1 = (u_char *)in_buff;
	for (i = 0; i < 3; i++) {
		cp = spk_s2uchar(cp, cp1);
		cp1++;
	}
	i = (int)cp1[-2] + 1;
	i *= (int)cp1[-1] + 1;
	i += 2; /* 0 and last map ver */
	if (cp1[-3] != KEY_MAP_VER || cp1[-1] > 10 ||
	    i + SHIFT_TBL_SIZE + 4 >= sizeof(spk_key_buf)) {
		pr_warn("i %d %d %d %d\n", i,
			(int)cp1[-3], (int)cp1[-2], (int)cp1[-1]);
		kfree(in_buff);
		spin_unlock_irqrestore(&speakup_info.spinlock, flags);
		return -EINVAL;
	}
	while (--i >= 0) {
		cp = spk_s2uchar(cp, cp1);
		cp1++;
		if (!(*cp))
			break;
	}
	if (i != 0 || cp1[-1] != KEY_MAP_VER || cp1[-2] != 0) {
		ret = -EINVAL;
		pr_warn("end %d %d %d %d\n", i,
			(int)cp1[-3], (int)cp1[-2], (int)cp1[-1]);
	} else {
		if (spk_set_key_info(in_buff, spk_key_buf)) {
			spk_set_key_info(spk_key_defaults, spk_key_buf);
			ret = -EINVAL;
			pr_warn("set key failed\n");
		}
	}
	kfree(in_buff);
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return ret;
}

/*
 * This is called when a user changes the value of the silent parameter.
 */
static ssize_t silent_store(struct kobject *kobj, struct kobj_attribute *attr,
			    const char *buf, size_t count)
{
	int len;
	struct vc_data *vc = vc_cons[fg_console].d;
	char ch = 0;
	char shut;
	unsigned long flags;

	len = strlen(buf);
	if (len > 0 && len < 3) {
		ch = buf[0];
		if (ch == '\n')
			ch = '0';
	}
	if (ch < '0' || ch > '7') {
		pr_warn("silent value '%c' not in range (0,7)\n", ch);
		return -EINVAL;
	}
	spin_lock_irqsave(&speakup_info.spinlock, flags);
	if (ch & 2) {
		shut = 1;
		spk_do_flush();
	} else {
		shut = 0;
	}
	if (ch & 4)
		shut |= 0x40;
	if (ch & 1)
		spk_shut_up |= shut;
	else
		spk_shut_up &= ~shut;
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return count;
}

/*
 * This is called when a user reads the synth setting.
 */
static ssize_t synth_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	int rv;

	if (!synth)
		rv = sprintf(buf, "%s\n", "none");
	else
		rv = sprintf(buf, "%s\n", synth->name);
	return rv;
}

/*
 * This is called when a user requests to change synthesizers.
 */
static ssize_t synth_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t count)
{
	int len;
	char new_synth_name[10];

	len = strlen(buf);
	if (len < 2 || len > 9)
		return -EINVAL;
	memcpy(new_synth_name, buf, len);
	if (new_synth_name[len - 1] == '\n')
		len--;
	new_synth_name[len] = '\0';
	spk_strlwr(new_synth_name);
	if (synth && !strcmp(new_synth_name, synth->name)) {
		pr_warn("%s already in use\n", new_synth_name);
	} else if (synth_init(new_synth_name) != 0) {
		pr_warn("failed to init synth %s\n", new_synth_name);
		return -ENODEV;
	}
	return count;
}

/*
 * This is called when text is sent to the synth via the synth_direct file.
 */
static ssize_t synth_direct_store(struct kobject *kobj,
				  struct kobj_attribute *attr,
				  const char *buf, size_t count)
{
	u_char tmp[256];
	int len;
	int bytes;
	const char *ptr = buf;
	unsigned long flags;

	if (!synth)
		return -EPERM;

	len = strlen(buf);
	spin_lock_irqsave(&speakup_info.spinlock, flags);
	while (len > 0) {
		bytes = min_t(size_t, len, 250);
		strncpy(tmp, ptr, bytes);
		tmp[bytes] = '\0';
		string_unescape_any_inplace(tmp);
		synth_printf("%s", tmp);
		ptr += bytes;
		len -= bytes;
	}
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return count;
}

/*
 * This function is called when a user reads the version.
 */
static ssize_t version_show(struct kobject *kobj, struct kobj_attribute *attr,
			    char *buf)
{
	char *cp;

	cp = buf;
	cp += sprintf(cp, "Speakup version %s\n", SPEAKUP_VERSION);
	if (synth)
		cp += sprintf(cp, "%s synthesizer driver version %s\n",
		synth->name, synth->version);
	return cp - buf;
}

/*
 * This is called when a user reads the punctuation settings.
 */
static ssize_t punc_show(struct kobject *kobj, struct kobj_attribute *attr,
			 char *buf)
{
	int i;
	char *cp = buf;
	struct st_var_header *p_header;
	struct punc_var_t *var;
	struct st_bits_data *pb;
	short mask;
	unsigned long flags;

	p_header = spk_var_header_by_name(attr->attr.name);
	if (!p_header) {
		pr_warn("p_header is null, attr->attr.name is %s\n",
			attr->attr.name);
		return -EINVAL;
	}

	var = spk_get_punc_var(p_header->var_id);
	if (!var) {
		pr_warn("var is null, p_header->var_id is %i\n",
			p_header->var_id);
		return -EINVAL;
	}

	spin_lock_irqsave(&speakup_info.spinlock, flags);
	pb = (struct st_bits_data *)&spk_punc_info[var->value];
	mask = pb->mask;
	for (i = 33; i < 128; i++) {
		if (!(spk_chartab[i] & mask))
			continue;
		*cp++ = (char)i;
	}
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return cp - buf;
}

/*
 * This is called when a user changes the punctuation settings.
 */
static ssize_t punc_store(struct kobject *kobj, struct kobj_attribute *attr,
			  const char *buf, size_t count)
{
	int x;
	struct st_var_header *p_header;
	struct punc_var_t *var;
	char punc_buf[100];
	unsigned long flags;

	x = strlen(buf);
	if (x < 1 || x > 99)
		return -EINVAL;

	p_header = spk_var_header_by_name(attr->attr.name);
	if (!p_header) {
		pr_warn("p_header is null, attr->attr.name is %s\n",
			attr->attr.name);
		return -EINVAL;
	}

	var = spk_get_punc_var(p_header->var_id);
	if (!var) {
		pr_warn("var is null, p_header->var_id is %i\n",
			p_header->var_id);
		return -EINVAL;
	}

	memcpy(punc_buf, buf, x);

	while (x && punc_buf[x - 1] == '\n')
		x--;
	punc_buf[x] = '\0';

	spin_lock_irqsave(&speakup_info.spinlock, flags);

	if (*punc_buf == 'd' || *punc_buf == 'r')
		x = spk_set_mask_bits(NULL, var->value, 3);
	else
		x = spk_set_mask_bits(punc_buf, var->value, 3);

	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return count;
}

/*
 * This function is called when a user reads one of the variable parameters.
 */
ssize_t spk_var_show(struct kobject *kobj, struct kobj_attribute *attr,
		     char *buf)
{
	int rv = 0;
	struct st_var_header *param;
	struct var_t *var;
	char *cp1;
	char *cp;
	char ch;
	unsigned long flags;

	param = spk_var_header_by_name(attr->attr.name);
	if (!param)
		return -EINVAL;

	spin_lock_irqsave(&speakup_info.spinlock, flags);
	var = (struct var_t *)param->data;
	switch (param->var_type) {
	case VAR_NUM:
	case VAR_TIME:
		if (var)
			rv = sprintf(buf, "%i\n", var->u.n.value);
		else
			rv = sprintf(buf, "0\n");
		break;
	case VAR_STRING:
		if (var) {
			cp1 = buf;
			*cp1++ = '"';
			for (cp = (char *)param->p_val; (ch = *cp); cp++) {
				if (ch >= ' ' && ch < '~')
					*cp1++ = ch;
				else
					cp1 += sprintf(cp1, "\\x%02x", ch);
			}
			*cp1++ = '"';
			*cp1++ = '\n';
			*cp1 = '\0';
			rv = cp1 - buf;
		} else {
			rv = sprintf(buf, "\"\"\n");
		}
		break;
	default:
		rv = sprintf(buf, "Bad parameter  %s, type %i\n",
			     param->name, param->var_type);
		break;
	}
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return rv;
}
EXPORT_SYMBOL_GPL(spk_var_show);

/*
 * Used to reset either default_pitch or default_vol.
 */
static inline void spk_reset_default_value(char *header_name,
					   int *synth_default_value, int idx)
{
	struct st_var_header *param;

	if (synth && synth_default_value) {
		param = spk_var_header_by_name(header_name);
		if (param)  {
			spk_set_num_var(synth_default_value[idx],
					param, E_NEW_DEFAULT);
			spk_set_num_var(0, param, E_DEFAULT);
			pr_info("%s reset to default value\n", param->name);
		}
	}
}

/*
 * This function is called when a user echos a value to one of the
 * variable parameters.
 */
ssize_t spk_var_store(struct kobject *kobj, struct kobj_attribute *attr,
		      const char *buf, size_t count)
{
	struct st_var_header *param;
	int ret;
	int len;
	char *cp;
	struct var_t *var_data;
	long value;
	unsigned long flags;

	param = spk_var_header_by_name(attr->attr.name);
	if (!param)
		return -EINVAL;
	if (!param->data)
		return 0;
	ret = 0;
	cp = (char *)buf;
	string_unescape_any_inplace(cp);

	spin_lock_irqsave(&speakup_info.spinlock, flags);
	switch (param->var_type) {
	case VAR_NUM:
	case VAR_TIME:
		if (*cp == 'd' || *cp == 'r' || *cp == '\0')
			len = E_DEFAULT;
		else if (*cp == '+' || *cp == '-')
			len = E_INC;
		else
			len = E_SET;
		if (kstrtol(cp, 10, &value) == 0)
			ret = spk_set_num_var(value, param, len);
		else
			pr_warn("overflow or parsing error has occurred");
		if (ret == -ERANGE) {
			var_data = param->data;
			pr_warn("value for %s out of range, expect %d to %d\n",
				param->name,
				var_data->u.n.low, var_data->u.n.high);
		}

	       /*
		* If voice was just changed, we might need to reset our default
		* pitch and volume.
		*/
		if (param->var_id == VOICE && synth &&
		    (ret == 0 || ret == -ERESTART)) {
			var_data = param->data;
			value = var_data->u.n.value;
			spk_reset_default_value("pitch", synth->default_pitch,
						value);
			spk_reset_default_value("vol", synth->default_vol,
						value);
		}
		break;
	case VAR_STRING:
		len = strlen(cp);
		if ((len >= 1) && (cp[len - 1] == '\n'))
			--len;
		if ((len >= 2) && (cp[0] == '"') && (cp[len - 1] == '"')) {
			++cp;
			len -= 2;
		}
		cp[len] = '\0';
		ret = spk_set_string_var(cp, param, len);
		if (ret == -E2BIG)
			pr_warn("value too long for %s\n",
				param->name);
		break;
	default:
		pr_warn("%s unknown type %d\n",
			param->name, (int)param->var_type);
	break;
	}
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);

	if (ret == -ERESTART)
		pr_info("%s reset to default value\n", param->name);
	return count;
}
EXPORT_SYMBOL_GPL(spk_var_store);

/*
 * Functions for reading and writing lists of i18n messages.  Incomplete.
 */

static ssize_t message_show_helper(char *buf, enum msg_index_t first,
				   enum msg_index_t last)
{
	size_t bufsize = PAGE_SIZE;
	char *buf_pointer = buf;
	int printed;
	enum msg_index_t cursor;
	int index = 0;
	*buf_pointer = '\0'; /* buf_pointer always looking at a NUL byte. */

	for (cursor = first; cursor <= last; cursor++, index++) {
		if (bufsize <= 1)
			break;
		printed = scnprintf(buf_pointer, bufsize, "%d\t%s\n",
				    index, spk_msg_get(cursor));
		buf_pointer += printed;
		bufsize -= printed;
	}

	return buf_pointer - buf;
}

static void report_msg_status(int reset, int received, int used,
			      int rejected, char *groupname)
{
	int len;
	char buf[160];

	if (reset) {
		pr_info("i18n messages from group %s reset to defaults\n",
			groupname);
	} else if (received) {
		len = snprintf(buf, sizeof(buf),
			       " updated %d of %d i18n messages from group %s\n",
				       used, received, groupname);
		if (rejected)
			snprintf(buf + (len - 1), sizeof(buf) - (len - 1),
				 " with %d reject%s\n",
				 rejected, rejected > 1 ? "s" : "");
		pr_info("%s", buf);
	}
}

static ssize_t message_store_helper(const char *buf, size_t count,
				    struct msg_group_t *group)
{
	char *cp = (char *)buf;
	char *end = cp + count;
	char *linefeed = NULL;
	char *temp = NULL;
	ssize_t msg_stored = 0;
	ssize_t retval = count;
	size_t desc_length = 0;
	unsigned long index = 0;
	int received = 0;
	int used = 0;
	int rejected = 0;
	int reset = 0;
	enum msg_index_t firstmessage = group->start;
	enum msg_index_t lastmessage = group->end;
	enum msg_index_t curmessage;

	while (cp < end) {
		while ((cp < end) && (*cp == ' ' || *cp == '\t'))
			cp++;

		if (cp == end)
			break;
		if (strchr("dDrR", *cp)) {
			reset = 1;
			break;
		}
		received++;

		linefeed = strchr(cp, '\n');
		if (!linefeed) {
			rejected++;
			break;
		}

		if (!isdigit(*cp)) {
			rejected++;
			cp = linefeed + 1;
			continue;
		}

		/*
		 * Do not replace with kstrtoul:
		 * here we need temp to be updated
		 */
		index = simple_strtoul(cp, &temp, 10);

		while ((temp < linefeed) && (*temp == ' ' || *temp == '\t'))
			temp++;

		desc_length = linefeed - temp;
		curmessage = firstmessage + index;

		/*
		 * Note the check (curmessage < firstmessage).  It is not
		 * redundant.  Suppose that the user gave us an index
		 * equal to ULONG_MAX - 1.  If firstmessage > 1, then
		 * firstmessage + index < firstmessage!
		 */

		if ((curmessage < firstmessage) || (curmessage > lastmessage)) {
			rejected++;
			cp = linefeed + 1;
			continue;
		}

		msg_stored = spk_msg_set(curmessage, temp, desc_length);
		if (msg_stored < 0) {
			retval = msg_stored;
			if (msg_stored == -ENOMEM)
				reset = 1;
			break;
		}

		used++;

		cp = linefeed + 1;
	}

	if (reset)
		spk_reset_msg_group(group);

	report_msg_status(reset, received, used, rejected, group->name);
	return retval;
}

static ssize_t message_show(struct kobject *kobj,
			    struct kobj_attribute *attr, char *buf)
{
	ssize_t retval = 0;
	struct msg_group_t *group = spk_find_msg_group(attr->attr.name);
	unsigned long flags;

	if (WARN_ON(!group))
		return -EINVAL;

	spin_lock_irqsave(&speakup_info.spinlock, flags);
	retval = message_show_helper(buf, group->start, group->end);
	spin_unlock_irqrestore(&speakup_info.spinlock, flags);
	return retval;
}

static ssize_t message_store(struct kobject *kobj, struct kobj_attribute *attr,
			     const char *buf, size_t count)
{
	struct msg_group_t *group = spk_find_msg_group(attr->attr.name);

	if (WARN_ON(!group))
		return -EINVAL;

	return message_store_helper(buf, count, group);
}

/*
 * Declare the attributes.
 */
static struct kobj_attribute keymap_attribute =
	__ATTR_RW(keymap);
static struct kobj_attribute silent_attribute =
	__ATTR_WO(silent);
static struct kobj_attribute synth_attribute =
	__ATTR_RW(synth);
static struct kobj_attribute synth_direct_attribute =
	__ATTR_WO(synth_direct);
static struct kobj_attribute version_attribute =
	__ATTR_RO(version);

static struct kobj_attribute delimiters_attribute =
	__ATTR(delimiters, 0644, punc_show, punc_store);
static struct kobj_attribute ex_num_attribute =
	__ATTR(ex_num, 0644, punc_show, punc_store);
static struct kobj_attribute punc_all_attribute =
	__ATTR(punc_all, 0644, punc_show, punc_store);
static struct kobj_attribute punc_most_attribute =
	__ATTR(punc_most, 0644, punc_show, punc_store);
static struct kobj_attribute punc_some_attribute =
	__ATTR(punc_some, 0644, punc_show, punc_store);
static struct kobj_attribute repeats_attribute =
	__ATTR(repeats, 0644, punc_show, punc_store);

static struct kobj_attribute attrib_bleep_attribute =
	__ATTR(attrib_bleep, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute bell_pos_attribute =
	__ATTR(bell_pos, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute bleep_time_attribute =
	__ATTR(bleep_time, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute bleeps_attribute =
	__ATTR(bleeps, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute cursor_time_attribute =
	__ATTR(cursor_time, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute key_echo_attribute =
	__ATTR(key_echo, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute no_interrupt_attribute =
	__ATTR(no_interrupt, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute punc_level_attribute =
	__ATTR(punc_level, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute reading_punc_attribute =
	__ATTR(reading_punc, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute say_control_attribute =
	__ATTR(say_control, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute say_word_ctl_attribute =
	__ATTR(say_word_ctl, 0644, spk_var_show, spk_var_store);
static struct kobj_attribute spell_delay_attribute =
	__ATTR(spell_delay, 0644, spk_var_show, spk_var_store);

/*
 * These attributes are i18n related.
 */
static struct kobj_attribute announcements_attribute =
	__ATTR(announcements, 0644, message_show, message_store);
static struct kobj_attribute characters_attribute =
	__ATTR(characters, 0644, chars_chartab_show,
	       chars_chartab_store);
static struct kobj_attribute chartab_attribute =
	__ATTR(chartab, 0644, chars_chartab_show,
	       chars_chartab_store);
static struct kobj_attribute ctl_keys_attribute =
	__ATTR(ctl_keys, 0644, message_show, message_store);
static struct kobj_attribute colors_attribute =
	__ATTR(colors, 0644, message_show, message_store);
static struct kobj_attribute formatted_attribute =
	__ATTR(formatted, 0644, message_show, message_store);
static struct kobj_attribute function_names_attribute =
	__ATTR(function_names, 0644, message_show, message_store);
static struct kobj_attribute key_names_attribute =
	__ATTR(key_names, 0644, message_show, message_store);
static struct kobj_attribute states_attribute =
	__ATTR(states, 0644, message_show, message_store);

/*
 * Create groups of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *main_attrs[] = {
	&keymap_attribute.attr,
	&silent_attribute.attr,
	&synth_attribute.attr,
	&synth_direct_attribute.attr,
	&version_attribute.attr,
	&delimiters_attribute.attr,
	&ex_num_attribute.attr,
	&punc_all_attribute.attr,
	&punc_most_attribute.attr,
	&punc_some_attribute.attr,
	&repeats_attribute.attr,
	&attrib_bleep_attribute.attr,
	&bell_pos_attribute.attr,
	&bleep_time_attribute.attr,
	&bleeps_attribute.attr,
	&cursor_time_attribute.attr,
	&key_echo_attribute.attr,
	&no_interrupt_attribute.attr,
	&punc_level_attribute.attr,
	&reading_punc_attribute.attr,
	&say_control_attribute.attr,
	&say_word_ctl_attribute.attr,
	&spell_delay_attribute.attr,
	NULL,
};

static struct attribute *i18n_attrs[] = {
	&announcements_attribute.attr,
	&characters_attribute.attr,
	&chartab_attribute.attr,
	&ctl_keys_attribute.attr,
	&colors_attribute.attr,
	&formatted_attribute.attr,
	&function_names_attribute.attr,
	&key_names_attribute.attr,
	&states_attribute.attr,
	NULL,
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static const struct attribute_group main_attr_group = {
	.attrs = main_attrs,
};

static const struct attribute_group i18n_attr_group = {
	.attrs = i18n_attrs,
	.name = "i18n",
};

static struct kobject *accessibility_kobj;
struct kobject *speakup_kobj;

int speakup_kobj_init(void)
{
	int retval;

	/*
	 * Create a simple kobject with the name of "accessibility",
	 * located under /sys/
	 *
	 * As this is a simple directory, no uevent will be sent to
	 * userspace.  That is why this function should not be used for
	 * any type of dynamic kobjects, where the name and number are
	 * not known ahead of time.
	 */
	accessibility_kobj = kobject_create_and_add("accessibility", NULL);
	if (!accessibility_kobj) {
		retval = -ENOMEM;
		goto out;
	}

	speakup_kobj = kobject_create_and_add("speakup", accessibility_kobj);
	if (!speakup_kobj) {
		retval = -ENOMEM;
		goto err_acc;
	}

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(speakup_kobj, &main_attr_group);
	if (retval)
		goto err_speakup;

	retval = sysfs_create_group(speakup_kobj, &i18n_attr_group);
	if (retval)
		goto err_group;

	goto out;

err_group:
	sysfs_remove_group(speakup_kobj, &main_attr_group);
err_speakup:
	kobject_put(speakup_kobj);
err_acc:
	kobject_put(accessibility_kobj);
out:
	return retval;
}

void speakup_kobj_exit(void)
{
	sysfs_remove_group(speakup_kobj, &i18n_attr_group);
	sysfs_remove_group(speakup_kobj, &main_attr_group);
	kobject_put(speakup_kobj);
	kobject_put(accessibility_kobj);
}
