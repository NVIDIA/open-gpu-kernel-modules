// SPDX-License-Identifier: GPL-2.0
/*
 * Extra Boot Config
 * Masami Hiramatsu <mhiramat@kernel.org>
 */

#define pr_fmt(fmt)    "bootconfig: " fmt

#include <linux/bootconfig.h>
#include <linux/bug.h>
#include <linux/ctype.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/memblock.h>
#include <linux/printk.h>
#include <linux/string.h>

/*
 * Extra Boot Config (XBC) is given as tree-structured ascii text of
 * key-value pairs on memory.
 * xbc_parse() parses the text to build a simple tree. Each tree node is
 * simply a key word or a value. A key node may have a next key node or/and
 * a child node (both key and value). A value node may have a next value
 * node (for array).
 */

static struct xbc_node *xbc_nodes __initdata;
static int xbc_node_num __initdata;
static char *xbc_data __initdata;
static size_t xbc_data_size __initdata;
static struct xbc_node *last_parent __initdata;
static const char *xbc_err_msg __initdata;
static int xbc_err_pos __initdata;
static int open_brace[XBC_DEPTH_MAX] __initdata;
static int brace_index __initdata;

static int __init xbc_parse_error(const char *msg, const char *p)
{
	xbc_err_msg = msg;
	xbc_err_pos = (int)(p - xbc_data);

	return -EINVAL;
}

/**
 * xbc_root_node() - Get the root node of extended boot config
 *
 * Return the address of root node of extended boot config. If the
 * extended boot config is not initiized, return NULL.
 */
struct xbc_node * __init xbc_root_node(void)
{
	if (unlikely(!xbc_data))
		return NULL;

	return xbc_nodes;
}

/**
 * xbc_node_index() - Get the index of XBC node
 * @node: A target node of getting index.
 *
 * Return the index number of @node in XBC node list.
 */
int __init xbc_node_index(struct xbc_node *node)
{
	return node - &xbc_nodes[0];
}

/**
 * xbc_node_get_parent() - Get the parent XBC node
 * @node: An XBC node.
 *
 * Return the parent node of @node. If the node is top node of the tree,
 * return NULL.
 */
struct xbc_node * __init xbc_node_get_parent(struct xbc_node *node)
{
	return node->parent == XBC_NODE_MAX ? NULL : &xbc_nodes[node->parent];
}

/**
 * xbc_node_get_child() - Get the child XBC node
 * @node: An XBC node.
 *
 * Return the first child node of @node. If the node has no child, return
 * NULL.
 */
struct xbc_node * __init xbc_node_get_child(struct xbc_node *node)
{
	return node->child ? &xbc_nodes[node->child] : NULL;
}

/**
 * xbc_node_get_next() - Get the next sibling XBC node
 * @node: An XBC node.
 *
 * Return the NEXT sibling node of @node. If the node has no next sibling,
 * return NULL. Note that even if this returns NULL, it doesn't mean @node
 * has no siblings. (You also has to check whether the parent's child node
 * is @node or not.)
 */
struct xbc_node * __init xbc_node_get_next(struct xbc_node *node)
{
	return node->next ? &xbc_nodes[node->next] : NULL;
}

/**
 * xbc_node_get_data() - Get the data of XBC node
 * @node: An XBC node.
 *
 * Return the data (which is always a null terminated string) of @node.
 * If the node has invalid data, warn and return NULL.
 */
const char * __init xbc_node_get_data(struct xbc_node *node)
{
	int offset = node->data & ~XBC_VALUE;

	if (WARN_ON(offset >= xbc_data_size))
		return NULL;

	return xbc_data + offset;
}

static bool __init
xbc_node_match_prefix(struct xbc_node *node, const char **prefix)
{
	const char *p = xbc_node_get_data(node);
	int len = strlen(p);

	if (strncmp(*prefix, p, len))
		return false;

	p = *prefix + len;
	if (*p == '.')
		p++;
	else if (*p != '\0')
		return false;
	*prefix = p;

	return true;
}

/**
 * xbc_node_find_child() - Find a child node which matches given key
 * @parent: An XBC node.
 * @key: A key string.
 *
 * Search a node under @parent which matches @key. The @key can contain
 * several words jointed with '.'. If @parent is NULL, this searches the
 * node from whole tree. Return NULL if no node is matched.
 */
struct xbc_node * __init
xbc_node_find_child(struct xbc_node *parent, const char *key)
{
	struct xbc_node *node;

	if (parent)
		node = xbc_node_get_child(parent);
	else
		node = xbc_root_node();

	while (node && xbc_node_is_key(node)) {
		if (!xbc_node_match_prefix(node, &key))
			node = xbc_node_get_next(node);
		else if (*key != '\0')
			node = xbc_node_get_child(node);
		else
			break;
	}

	return node;
}

/**
 * xbc_node_find_value() - Find a value node which matches given key
 * @parent: An XBC node.
 * @key: A key string.
 * @vnode: A container pointer of found XBC node.
 *
 * Search a value node under @parent whose (parent) key node matches @key,
 * store it in *@vnode, and returns the value string.
 * The @key can contain several words jointed with '.'. If @parent is NULL,
 * this searches the node from whole tree. Return the value string if a
 * matched key found, return NULL if no node is matched.
 * Note that this returns 0-length string and stores NULL in *@vnode if the
 * key has no value. And also it will return the value of the first entry if
 * the value is an array.
 */
const char * __init
xbc_node_find_value(struct xbc_node *parent, const char *key,
		    struct xbc_node **vnode)
{
	struct xbc_node *node = xbc_node_find_child(parent, key);

	if (!node || !xbc_node_is_key(node))
		return NULL;

	node = xbc_node_get_child(node);
	if (node && !xbc_node_is_value(node))
		return NULL;

	if (vnode)
		*vnode = node;

	return node ? xbc_node_get_data(node) : "";
}

/**
 * xbc_node_compose_key_after() - Compose partial key string of the XBC node
 * @root: Root XBC node
 * @node: Target XBC node.
 * @buf: A buffer to store the key.
 * @size: The size of the @buf.
 *
 * Compose the partial key of the @node into @buf, which is starting right
 * after @root (@root is not included.) If @root is NULL, this returns full
 * key words of @node.
 * Returns the total length of the key stored in @buf. Returns -EINVAL
 * if @node is NULL or @root is not the ancestor of @node or @root is @node,
 * or returns -ERANGE if the key depth is deeper than max depth.
 * This is expected to be used with xbc_find_node() to list up all (child)
 * keys under given key.
 */
int __init xbc_node_compose_key_after(struct xbc_node *root,
				      struct xbc_node *node,
				      char *buf, size_t size)
{
	u16 keys[XBC_DEPTH_MAX];
	int depth = 0, ret = 0, total = 0;

	if (!node || node == root)
		return -EINVAL;

	if (xbc_node_is_value(node))
		node = xbc_node_get_parent(node);

	while (node && node != root) {
		keys[depth++] = xbc_node_index(node);
		if (depth == XBC_DEPTH_MAX)
			return -ERANGE;
		node = xbc_node_get_parent(node);
	}
	if (!node && root)
		return -EINVAL;

	while (--depth >= 0) {
		node = xbc_nodes + keys[depth];
		ret = snprintf(buf, size, "%s%s", xbc_node_get_data(node),
			       depth ? "." : "");
		if (ret < 0)
			return ret;
		if (ret > size) {
			size = 0;
		} else {
			size -= ret;
			buf += ret;
		}
		total += ret;
	}

	return total;
}

/**
 * xbc_node_find_next_leaf() - Find the next leaf node under given node
 * @root: An XBC root node
 * @node: An XBC node which starts from.
 *
 * Search the next leaf node (which means the terminal key node) of @node
 * under @root node (including @root node itself).
 * Return the next node or NULL if next leaf node is not found.
 */
struct xbc_node * __init xbc_node_find_next_leaf(struct xbc_node *root,
						 struct xbc_node *node)
{
	if (unlikely(!xbc_data))
		return NULL;

	if (!node) {	/* First try */
		node = root;
		if (!node)
			node = xbc_nodes;
	} else {
		if (node == root)	/* @root was a leaf, no child node. */
			return NULL;

		while (!node->next) {
			node = xbc_node_get_parent(node);
			if (node == root)
				return NULL;
			/* User passed a node which is not uder parent */
			if (WARN_ON(!node))
				return NULL;
		}
		node = xbc_node_get_next(node);
	}

	while (node && !xbc_node_is_leaf(node))
		node = xbc_node_get_child(node);

	return node;
}

/**
 * xbc_node_find_next_key_value() - Find the next key-value pair nodes
 * @root: An XBC root node
 * @leaf: A container pointer of XBC node which starts from.
 *
 * Search the next leaf node (which means the terminal key node) of *@leaf
 * under @root node. Returns the value and update *@leaf if next leaf node
 * is found, or NULL if no next leaf node is found.
 * Note that this returns 0-length string if the key has no value, or
 * the value of the first entry if the value is an array.
 */
const char * __init xbc_node_find_next_key_value(struct xbc_node *root,
						 struct xbc_node **leaf)
{
	/* tip must be passed */
	if (WARN_ON(!leaf))
		return NULL;

	*leaf = xbc_node_find_next_leaf(root, *leaf);
	if (!*leaf)
		return NULL;
	if ((*leaf)->child)
		return xbc_node_get_data(xbc_node_get_child(*leaf));
	else
		return "";	/* No value key */
}

/* XBC parse and tree build */

static int __init xbc_init_node(struct xbc_node *node, char *data, u32 flag)
{
	unsigned long offset = data - xbc_data;

	if (WARN_ON(offset >= XBC_DATA_MAX))
		return -EINVAL;

	node->data = (u16)offset | flag;
	node->child = 0;
	node->next = 0;

	return 0;
}

static struct xbc_node * __init xbc_add_node(char *data, u32 flag)
{
	struct xbc_node *node;

	if (xbc_node_num == XBC_NODE_MAX)
		return NULL;

	node = &xbc_nodes[xbc_node_num++];
	if (xbc_init_node(node, data, flag) < 0)
		return NULL;

	return node;
}

static inline __init struct xbc_node *xbc_last_sibling(struct xbc_node *node)
{
	while (node->next)
		node = xbc_node_get_next(node);

	return node;
}

static struct xbc_node * __init xbc_add_sibling(char *data, u32 flag)
{
	struct xbc_node *sib, *node = xbc_add_node(data, flag);

	if (node) {
		if (!last_parent) {
			node->parent = XBC_NODE_MAX;
			sib = xbc_last_sibling(xbc_nodes);
			sib->next = xbc_node_index(node);
		} else {
			node->parent = xbc_node_index(last_parent);
			if (!last_parent->child) {
				last_parent->child = xbc_node_index(node);
			} else {
				sib = xbc_node_get_child(last_parent);
				sib = xbc_last_sibling(sib);
				sib->next = xbc_node_index(node);
			}
		}
	} else
		xbc_parse_error("Too many nodes", data);

	return node;
}

static inline __init struct xbc_node *xbc_add_child(char *data, u32 flag)
{
	struct xbc_node *node = xbc_add_sibling(data, flag);

	if (node)
		last_parent = node;

	return node;
}

static inline __init bool xbc_valid_keyword(char *key)
{
	if (key[0] == '\0')
		return false;

	while (isalnum(*key) || *key == '-' || *key == '_')
		key++;

	return *key == '\0';
}

static char *skip_comment(char *p)
{
	char *ret;

	ret = strchr(p, '\n');
	if (!ret)
		ret = p + strlen(p);
	else
		ret++;

	return ret;
}

static char *skip_spaces_until_newline(char *p)
{
	while (isspace(*p) && *p != '\n')
		p++;
	return p;
}

static int __init __xbc_open_brace(char *p)
{
	/* Push the last key as open brace */
	open_brace[brace_index++] = xbc_node_index(last_parent);
	if (brace_index >= XBC_DEPTH_MAX)
		return xbc_parse_error("Exceed max depth of braces", p);

	return 0;
}

static int __init __xbc_close_brace(char *p)
{
	brace_index--;
	if (!last_parent || brace_index < 0 ||
	    (open_brace[brace_index] != xbc_node_index(last_parent)))
		return xbc_parse_error("Unexpected closing brace", p);

	if (brace_index == 0)
		last_parent = NULL;
	else
		last_parent = &xbc_nodes[open_brace[brace_index - 1]];

	return 0;
}

/*
 * Return delimiter or error, no node added. As same as lib/cmdline.c,
 * you can use " around spaces, but can't escape " for value.
 */
static int __init __xbc_parse_value(char **__v, char **__n)
{
	char *p, *v = *__v;
	int c, quotes = 0;

	v = skip_spaces(v);
	while (*v == '#') {
		v = skip_comment(v);
		v = skip_spaces(v);
	}
	if (*v == '"' || *v == '\'') {
		quotes = *v;
		v++;
	}
	p = v - 1;
	while ((c = *++p)) {
		if (!isprint(c) && !isspace(c))
			return xbc_parse_error("Non printable value", p);
		if (quotes) {
			if (c != quotes)
				continue;
			quotes = 0;
			*p++ = '\0';
			p = skip_spaces_until_newline(p);
			c = *p;
			if (c && !strchr(",;\n#}", c))
				return xbc_parse_error("No value delimiter", p);
			if (*p)
				p++;
			break;
		}
		if (strchr(",;\n#}", c)) {
			*p++ = '\0';
			v = strim(v);
			break;
		}
	}
	if (quotes)
		return xbc_parse_error("No closing quotes", p);
	if (c == '#') {
		p = skip_comment(p);
		c = '\n';	/* A comment must be treated as a newline */
	}
	*__n = p;
	*__v = v;

	return c;
}

static int __init xbc_parse_array(char **__v)
{
	struct xbc_node *node;
	char *next;
	int c = 0;

	do {
		c = __xbc_parse_value(__v, &next);
		if (c < 0)
			return c;

		node = xbc_add_sibling(*__v, XBC_VALUE);
		if (!node)
			return -ENOMEM;
		*__v = next;
	} while (c == ',');
	node->next = 0;

	return c;
}

static inline __init
struct xbc_node *find_match_node(struct xbc_node *node, char *k)
{
	while (node) {
		if (!strcmp(xbc_node_get_data(node), k))
			break;
		node = xbc_node_get_next(node);
	}
	return node;
}

static int __init __xbc_add_key(char *k)
{
	struct xbc_node *node, *child;

	if (!xbc_valid_keyword(k))
		return xbc_parse_error("Invalid keyword", k);

	if (unlikely(xbc_node_num == 0))
		goto add_node;

	if (!last_parent)	/* the first level */
		node = find_match_node(xbc_nodes, k);
	else {
		child = xbc_node_get_child(last_parent);
		if (child && xbc_node_is_value(child))
			return xbc_parse_error("Subkey is mixed with value", k);
		node = find_match_node(child, k);
	}

	if (node)
		last_parent = node;
	else {
add_node:
		node = xbc_add_child(k, XBC_KEY);
		if (!node)
			return -ENOMEM;
	}
	return 0;
}

static int __init __xbc_parse_keys(char *k)
{
	char *p;
	int ret;

	k = strim(k);
	while ((p = strchr(k, '.'))) {
		*p++ = '\0';
		ret = __xbc_add_key(k);
		if (ret)
			return ret;
		k = p;
	}

	return __xbc_add_key(k);
}

static int __init xbc_parse_kv(char **k, char *v, int op)
{
	struct xbc_node *prev_parent = last_parent;
	struct xbc_node *child;
	char *next;
	int c, ret;

	ret = __xbc_parse_keys(*k);
	if (ret)
		return ret;

	child = xbc_node_get_child(last_parent);
	if (child) {
		if (xbc_node_is_key(child))
			return xbc_parse_error("Value is mixed with subkey", v);
		else if (op == '=')
			return xbc_parse_error("Value is redefined", v);
	}

	c = __xbc_parse_value(&v, &next);
	if (c < 0)
		return c;

	if (op == ':' && child) {
		xbc_init_node(child, v, XBC_VALUE);
	} else if (!xbc_add_sibling(v, XBC_VALUE))
		return -ENOMEM;

	if (c == ',') {	/* Array */
		c = xbc_parse_array(&next);
		if (c < 0)
			return c;
	}

	last_parent = prev_parent;

	if (c == '}') {
		ret = __xbc_close_brace(next - 1);
		if (ret < 0)
			return ret;
	}

	*k = next;

	return 0;
}

static int __init xbc_parse_key(char **k, char *n)
{
	struct xbc_node *prev_parent = last_parent;
	int ret;

	*k = strim(*k);
	if (**k != '\0') {
		ret = __xbc_parse_keys(*k);
		if (ret)
			return ret;
		last_parent = prev_parent;
	}
	*k = n;

	return 0;
}

static int __init xbc_open_brace(char **k, char *n)
{
	int ret;

	ret = __xbc_parse_keys(*k);
	if (ret)
		return ret;
	*k = n;

	return __xbc_open_brace(n - 1);
}

static int __init xbc_close_brace(char **k, char *n)
{
	int ret;

	ret = xbc_parse_key(k, n);
	if (ret)
		return ret;
	/* k is updated in xbc_parse_key() */

	return __xbc_close_brace(n - 1);
}

static int __init xbc_verify_tree(void)
{
	int i, depth, len, wlen;
	struct xbc_node *n, *m;

	/* Brace closing */
	if (brace_index) {
		n = &xbc_nodes[open_brace[brace_index]];
		return xbc_parse_error("Brace is not closed",
					xbc_node_get_data(n));
	}

	/* Empty tree */
	if (xbc_node_num == 0) {
		xbc_parse_error("Empty config", xbc_data);
		return -ENOENT;
	}

	for (i = 0; i < xbc_node_num; i++) {
		if (xbc_nodes[i].next > xbc_node_num) {
			return xbc_parse_error("No closing brace",
				xbc_node_get_data(xbc_nodes + i));
		}
	}

	/* Key tree limitation check */
	n = &xbc_nodes[0];
	depth = 1;
	len = 0;

	while (n) {
		wlen = strlen(xbc_node_get_data(n)) + 1;
		len += wlen;
		if (len > XBC_KEYLEN_MAX)
			return xbc_parse_error("Too long key length",
				xbc_node_get_data(n));

		m = xbc_node_get_child(n);
		if (m && xbc_node_is_key(m)) {
			n = m;
			depth++;
			if (depth > XBC_DEPTH_MAX)
				return xbc_parse_error("Too many key words",
						xbc_node_get_data(n));
			continue;
		}
		len -= wlen;
		m = xbc_node_get_next(n);
		while (!m) {
			n = xbc_node_get_parent(n);
			if (!n)
				break;
			len -= strlen(xbc_node_get_data(n)) + 1;
			depth--;
			m = xbc_node_get_next(n);
		}
		n = m;
	}

	return 0;
}

/**
 * xbc_destroy_all() - Clean up all parsed bootconfig
 *
 * This clears all data structures of parsed bootconfig on memory.
 * If you need to reuse xbc_init() with new boot config, you can
 * use this.
 */
void __init xbc_destroy_all(void)
{
	xbc_data = NULL;
	xbc_data_size = 0;
	xbc_node_num = 0;
	memblock_free(__pa(xbc_nodes), sizeof(struct xbc_node) * XBC_NODE_MAX);
	xbc_nodes = NULL;
	brace_index = 0;
}

/**
 * xbc_init() - Parse given XBC file and build XBC internal tree
 * @buf: boot config text
 * @emsg: A pointer of const char * to store the error message
 * @epos: A pointer of int to store the error position
 *
 * This parses the boot config text in @buf. @buf must be a
 * null terminated string and smaller than XBC_DATA_MAX.
 * Return the number of stored nodes (>0) if succeeded, or -errno
 * if there is any error.
 * In error cases, @emsg will be updated with an error message and
 * @epos will be updated with the error position which is the byte offset
 * of @buf. If the error is not a parser error, @epos will be -1.
 */
int __init xbc_init(char *buf, const char **emsg, int *epos)
{
	char *p, *q;
	int ret, c;

	if (epos)
		*epos = -1;

	if (xbc_data) {
		if (emsg)
			*emsg = "Bootconfig is already initialized";
		return -EBUSY;
	}

	ret = strlen(buf);
	if (ret > XBC_DATA_MAX - 1 || ret == 0) {
		if (emsg)
			*emsg = ret ? "Config data is too big" :
				"Config data is empty";
		return -ERANGE;
	}

	xbc_nodes = memblock_alloc(sizeof(struct xbc_node) * XBC_NODE_MAX,
				   SMP_CACHE_BYTES);
	if (!xbc_nodes) {
		if (emsg)
			*emsg = "Failed to allocate bootconfig nodes";
		return -ENOMEM;
	}
	memset(xbc_nodes, 0, sizeof(struct xbc_node) * XBC_NODE_MAX);
	xbc_data = buf;
	xbc_data_size = ret + 1;
	last_parent = NULL;

	p = buf;
	do {
		q = strpbrk(p, "{}=+;:\n#");
		if (!q) {
			p = skip_spaces(p);
			if (*p != '\0')
				ret = xbc_parse_error("No delimiter", p);
			break;
		}

		c = *q;
		*q++ = '\0';
		switch (c) {
		case ':':
		case '+':
			if (*q++ != '=') {
				ret = xbc_parse_error(c == '+' ?
						"Wrong '+' operator" :
						"Wrong ':' operator",
							q - 2);
				break;
			}
			fallthrough;
		case '=':
			ret = xbc_parse_kv(&p, q, c);
			break;
		case '{':
			ret = xbc_open_brace(&p, q);
			break;
		case '#':
			q = skip_comment(q);
			fallthrough;
		case ';':
		case '\n':
			ret = xbc_parse_key(&p, q);
			break;
		case '}':
			ret = xbc_close_brace(&p, q);
			break;
		}
	} while (!ret);

	if (!ret)
		ret = xbc_verify_tree();

	if (ret < 0) {
		if (epos)
			*epos = xbc_err_pos;
		if (emsg)
			*emsg = xbc_err_msg;
		xbc_destroy_all();
	} else
		ret = xbc_node_num;

	return ret;
}

/**
 * xbc_debug_dump() - Dump current XBC node list
 *
 * Dump the current XBC node list on printk buffer for debug.
 */
void __init xbc_debug_dump(void)
{
	int i;

	for (i = 0; i < xbc_node_num; i++) {
		pr_debug("[%d] %s (%s) .next=%d, .child=%d .parent=%d\n", i,
			xbc_node_get_data(xbc_nodes + i),
			xbc_node_is_value(xbc_nodes + i) ? "value" : "key",
			xbc_nodes[i].next, xbc_nodes[i].child,
			xbc_nodes[i].parent);
	}
}
