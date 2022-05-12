/* SPDX-License-Identifier: GPL-2.0 */
#ifndef PXA2XX_LIB_H
#define PXA2XX_LIB_H

#include <uapi/sound/asound.h>
#include <linux/platform_device.h>

/* PCM */
struct snd_pcm_substream;
struct snd_pcm_hw_params;
struct snd_soc_pcm_runtime;
struct snd_pcm;
struct snd_soc_component;

extern int pxa2xx_pcm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params);
extern int pxa2xx_pcm_hw_free(struct snd_pcm_substream *substream);
extern int pxa2xx_pcm_trigger(struct snd_pcm_substream *substream, int cmd);
extern snd_pcm_uframes_t pxa2xx_pcm_pointer(struct snd_pcm_substream *substream);
extern int pxa2xx_pcm_prepare(struct snd_pcm_substream *substream);
extern int pxa2xx_pcm_open(struct snd_pcm_substream *substream);
extern int pxa2xx_pcm_close(struct snd_pcm_substream *substream);
extern int pxa2xx_pcm_mmap(struct snd_pcm_substream *substream,
	struct vm_area_struct *vma);
extern int pxa2xx_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream);
extern void pxa2xx_pcm_free_dma_buffers(struct snd_pcm *pcm);
extern void pxa2xx_soc_pcm_free(struct snd_soc_component *component,
				struct snd_pcm *pcm);
extern int pxa2xx_soc_pcm_new(struct snd_soc_component *component,
			      struct snd_soc_pcm_runtime *rtd);
extern int pxa2xx_soc_pcm_open(struct snd_soc_component *component,
			       struct snd_pcm_substream *substream);
extern int pxa2xx_soc_pcm_close(struct snd_soc_component *component,
				struct snd_pcm_substream *substream);
extern int pxa2xx_soc_pcm_hw_params(struct snd_soc_component *component,
				    struct snd_pcm_substream *substream,
				    struct snd_pcm_hw_params *params);
extern int pxa2xx_soc_pcm_hw_free(struct snd_soc_component *component,
				  struct snd_pcm_substream *substream);
extern int pxa2xx_soc_pcm_prepare(struct snd_soc_component *component,
				  struct snd_pcm_substream *substream);
extern int pxa2xx_soc_pcm_trigger(struct snd_soc_component *component,
				  struct snd_pcm_substream *substream, int cmd);
extern snd_pcm_uframes_t
pxa2xx_soc_pcm_pointer(struct snd_soc_component *component,
		       struct snd_pcm_substream *substream);
extern int pxa2xx_soc_pcm_mmap(struct snd_soc_component *component,
			       struct snd_pcm_substream *substream,
			       struct vm_area_struct *vma);

/* AC97 */

extern int pxa2xx_ac97_read(int slot, unsigned short reg);
extern int pxa2xx_ac97_write(int slot, unsigned short reg, unsigned short val);

extern bool pxa2xx_ac97_try_warm_reset(void);
extern bool pxa2xx_ac97_try_cold_reset(void);
extern void pxa2xx_ac97_finish_reset(void);

extern int pxa2xx_ac97_hw_suspend(void);
extern int pxa2xx_ac97_hw_resume(void);

extern int pxa2xx_ac97_hw_probe(struct platform_device *dev);
extern void pxa2xx_ac97_hw_remove(struct platform_device *dev);

#endif
