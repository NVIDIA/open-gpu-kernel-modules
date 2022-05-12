// SPDX-License-Identifier: GPL-2.0
/*
 * /dev/lcd driver for Apple Network Servers.
 */

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>

#include <linux/uaccess.h>
#include <asm/sections.h>
#include <asm/prom.h>
#include <asm/io.h>

#include "ans-lcd.h"

#define ANSLCD_ADDR		0xf301c000
#define ANSLCD_CTRL_IX 0x00
#define ANSLCD_DATA_IX 0x10

static unsigned long anslcd_short_delay = 80;
static unsigned long anslcd_long_delay = 3280;
static volatile unsigned char __iomem *anslcd_ptr;
static DEFINE_MUTEX(anslcd_mutex);

#undef DEBUG

static void
anslcd_write_byte_ctrl ( unsigned char c )
{
#ifdef DEBUG
	printk(KERN_DEBUG "LCD: CTRL byte: %02x\n",c);
#endif
	out_8(anslcd_ptr + ANSLCD_CTRL_IX, c);
	switch(c) {
		case 1:
		case 2:
		case 3:
			udelay(anslcd_long_delay); break;
		default: udelay(anslcd_short_delay);
	}
}

static void
anslcd_write_byte_data ( unsigned char c )
{
	out_8(anslcd_ptr + ANSLCD_DATA_IX, c);
	udelay(anslcd_short_delay);
}

static ssize_t
anslcd_write( struct file * file, const char __user * buf, 
				size_t count, loff_t *ppos )
{
	const char __user *p = buf;
	int i;

#ifdef DEBUG
	printk(KERN_DEBUG "LCD: write\n");
#endif

	if (!access_ok(buf, count))
		return -EFAULT;

	mutex_lock(&anslcd_mutex);
	for ( i = *ppos; count > 0; ++i, ++p, --count ) 
	{
		char c;
		__get_user(c, p);
		anslcd_write_byte_data( c );
	}
	mutex_unlock(&anslcd_mutex);
	*ppos = i;
	return p - buf;
}

static long
anslcd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	char ch, __user *temp;
	long ret = 0;

#ifdef DEBUG
	printk(KERN_DEBUG "LCD: ioctl(%d,%d)\n",cmd,arg);
#endif

	mutex_lock(&anslcd_mutex);

	switch ( cmd )
	{
	case ANSLCD_CLEAR:
		anslcd_write_byte_ctrl ( 0x38 );
		anslcd_write_byte_ctrl ( 0x0f );
		anslcd_write_byte_ctrl ( 0x06 );
		anslcd_write_byte_ctrl ( 0x01 );
		anslcd_write_byte_ctrl ( 0x02 );
		break;
	case ANSLCD_SENDCTRL:
		temp = (char __user *) arg;
		__get_user(ch, temp);
		for (; ch; temp++) { /* FIXME: This is ugly, but should work, as a \0 byte is not a valid command code */
			anslcd_write_byte_ctrl ( ch );
			__get_user(ch, temp);
		}
		break;
	case ANSLCD_SETSHORTDELAY:
		if (!capable(CAP_SYS_ADMIN))
			ret =-EACCES;
		else
			anslcd_short_delay=arg;
		break;
	case ANSLCD_SETLONGDELAY:
		if (!capable(CAP_SYS_ADMIN))
			ret = -EACCES;
		else
			anslcd_long_delay=arg;
		break;
	default:
		ret = -EINVAL;
	}

	mutex_unlock(&anslcd_mutex);
	return ret;
}

static int
anslcd_open( struct inode * inode, struct file * file )
{
	return 0;
}

const struct file_operations anslcd_fops = {
	.write		= anslcd_write,
	.unlocked_ioctl	= anslcd_ioctl,
	.open		= anslcd_open,
	.llseek		= default_llseek,
};

static struct miscdevice anslcd_dev = {
	LCD_MINOR,
	"anslcd",
	&anslcd_fops
};

static const char anslcd_logo[] __initconst =
				"********************"  /* Line #1 */
				"*      LINUX!      *"  /* Line #3 */
				"*    Welcome to    *"  /* Line #2 */
				"********************"; /* Line #4 */

static int __init
anslcd_init(void)
{
	int a;
	int retval;
	struct device_node* node;

	node = of_find_node_by_name(NULL, "lcd");
	if (!node || !of_node_name_eq(node->parent, "gc")) {
		of_node_put(node);
		return -ENODEV;
	}
	of_node_put(node);

	anslcd_ptr = ioremap(ANSLCD_ADDR, 0x20);
	
	retval = misc_register(&anslcd_dev);
	if(retval < 0){
		printk(KERN_INFO "LCD: misc_register failed\n");
		iounmap(anslcd_ptr);
		return retval;
	}

#ifdef DEBUG
	printk(KERN_DEBUG "LCD: init\n");
#endif

	mutex_lock(&anslcd_mutex);
	anslcd_write_byte_ctrl ( 0x38 );
	anslcd_write_byte_ctrl ( 0x0c );
	anslcd_write_byte_ctrl ( 0x06 );
	anslcd_write_byte_ctrl ( 0x01 );
	anslcd_write_byte_ctrl ( 0x02 );
	for(a=0;a<80;a++) {
		anslcd_write_byte_data(anslcd_logo[a]);
	}
	mutex_unlock(&anslcd_mutex);
	return 0;
}

static void __exit
anslcd_exit(void)
{
	misc_deregister(&anslcd_dev);
	iounmap(anslcd_ptr);
}

module_init(anslcd_init);
module_exit(anslcd_exit);
MODULE_LICENSE("GPL v2");
