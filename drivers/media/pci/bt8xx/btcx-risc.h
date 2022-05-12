/* SPDX-License-Identifier: GPL-2.0 */
struct btcx_riscmem {
	unsigned int   size;
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
};

struct btcx_skiplist {
	int start;
	int end;
};

int  btcx_riscmem_alloc(struct pci_dev *pci,
			struct btcx_riscmem *risc,
			unsigned int size);
void btcx_riscmem_free(struct pci_dev *pci,
		       struct btcx_riscmem *risc);

int btcx_screen_clips(int swidth, int sheight, struct v4l2_rect *win,
		      struct v4l2_clip *clips, unsigned int n);
int btcx_align(struct v4l2_rect *win, struct v4l2_clip *clips,
	       unsigned int n, int mask);
void btcx_sort_clips(struct v4l2_clip *clips, unsigned int nclips);
void btcx_calc_skips(int line, int width, int *maxy,
		     struct btcx_skiplist *skips, unsigned int *nskips,
		     const struct v4l2_clip *clips, unsigned int nclips);
