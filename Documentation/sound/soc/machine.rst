===================
ASoC Machine Driver
===================

The ASoC machine (or board) driver is the code that glues together all the
component drivers (e.g. codecs, platforms and DAIs). It also describes the
relationships between each component which include audio paths, GPIOs,
interrupts, clocking, jacks and voltage regulators.

The machine driver can contain codec and platform specific code. It registers
the audio subsystem with the kernel as a platform device and is represented by
the following struct:-
::

  /* SoC machine */
  struct snd_soc_card {
	char *name;

	...

	int (*probe)(struct platform_device *pdev);
	int (*remove)(struct platform_device *pdev);

	/* the pre and post PM functions are used to do any PM work before and
	 * after the codec and DAIs do any PM work. */
	int (*suspend_pre)(struct platform_device *pdev, pm_message_t state);
	int (*suspend_post)(struct platform_device *pdev, pm_message_t state);
	int (*resume_pre)(struct platform_device *pdev);
	int (*resume_post)(struct platform_device *pdev);

	...

	/* CPU <--> Codec DAI links  */
	struct snd_soc_dai_link *dai_link;
	int num_links;

	...
  };

probe()/remove()
----------------
probe/remove are optional. Do any machine specific probe here.


suspend()/resume()
------------------
The machine driver has pre and post versions of suspend and resume to take care
of any machine audio tasks that have to be done before or after the codec, DAIs
and DMA is suspended and resumed. Optional.


Machine DAI Configuration
-------------------------
The machine DAI configuration glues all the codec and CPU DAIs together. It can
also be used to set up the DAI system clock and for any machine related DAI
initialisation e.g. the machine audio map can be connected to the codec audio
map, unconnected codec pins can be set as such.

struct snd_soc_dai_link is used to set up each DAI in your machine. e.g.
::

  /* corgi digital audio interface glue - connects codec <--> CPU */
  static struct snd_soc_dai_link corgi_dai = {
	.name = "WM8731",
	.stream_name = "WM8731",
	.cpu_dai_name = "pxa-is2-dai",
	.codec_dai_name = "wm8731-hifi",
	.platform_name = "pxa-pcm-audio",
	.codec_name = "wm8713-codec.0-001a",
	.init = corgi_wm8731_init,
	.ops = &corgi_ops,
  };

struct snd_soc_card then sets up the machine with its DAIs. e.g.
::

  /* corgi audio machine driver */
  static struct snd_soc_card snd_soc_corgi = {
	.name = "Corgi",
	.dai_link = &corgi_dai,
	.num_links = 1,
  };


Machine Power Map
-----------------

The machine driver can optionally extend the codec power map and to become an
audio power map of the audio subsystem. This allows for automatic power up/down
of speaker/HP amplifiers, etc. Codec pins can be connected to the machines jack
sockets in the machine init function.


Machine Controls
----------------

Machine specific audio mixer controls can be added in the DAI init function.
