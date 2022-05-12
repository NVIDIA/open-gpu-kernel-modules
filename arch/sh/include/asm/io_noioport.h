/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_SH_IO_NOIOPORT_H
#define __ASM_SH_IO_NOIOPORT_H

static inline u8 inb(unsigned long addr)
{
	BUG();
	return -1;
}

static inline u16 inw(unsigned long addr)
{
	BUG();
	return -1;
}

static inline u32 inl(unsigned long addr)
{
	BUG();
	return -1;
}

static inline void outb(unsigned char x, unsigned long port)
{
	BUG();
}

static inline void outw(unsigned short x, unsigned long port)
{
	BUG();
}

static inline void outl(unsigned int x, unsigned long port)
{
	BUG();
}

static inline void __iomem *ioport_map(unsigned long port, unsigned int size)
{
	BUG();
	return NULL;
}

static inline void ioport_unmap(void __iomem *addr)
{
	BUG();
}

#define inb_p(addr)	inb(addr)
#define inw_p(addr)	inw(addr)
#define inl_p(addr)	inl(addr)
#define outb_p(x, addr)	outb((x), (addr))
#define outw_p(x, addr)	outw((x), (addr))
#define outl_p(x, addr)	outl((x), (addr))

static inline void insb(unsigned long port, void *dst, unsigned long count)
{
	BUG();
}

static inline void insw(unsigned long port, void *dst, unsigned long count)
{
	BUG();
}

static inline void insl(unsigned long port, void *dst, unsigned long count)
{
	BUG();
}

static inline void outsb(unsigned long port, const void *src, unsigned long count)
{
	BUG();
}

static inline void outsw(unsigned long port, const void *src, unsigned long count)
{
	BUG();
}

static inline void outsl(unsigned long port, const void *src, unsigned long count)
{
	BUG();
}

#endif /* __ASM_SH_IO_NOIOPORT_H */
