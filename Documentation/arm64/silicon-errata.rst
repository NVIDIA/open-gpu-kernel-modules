=======================================
Silicon Errata and Software Workarounds
=======================================

Author: Will Deacon <will.deacon@arm.com>

Date  : 27 November 2015

It is an unfortunate fact of life that hardware is often produced with
so-called "errata", which can cause it to deviate from the architecture
under specific circumstances.  For hardware produced by ARM, these
errata are broadly classified into the following categories:

  ==========  ========================================================
  Category A  A critical error without a viable workaround.
  Category B  A significant or critical error with an acceptable
              workaround.
  Category C  A minor error that is not expected to occur under normal
              operation.
  ==========  ========================================================

For more information, consult one of the "Software Developers Errata
Notice" documents available on infocenter.arm.com (registration
required).

As far as Linux is concerned, Category B errata may require some special
treatment in the operating system. For example, avoiding a particular
sequence of code, or configuring the processor in a particular way. A
less common situation may require similar actions in order to declassify
a Category A erratum into a Category C erratum. These are collectively
known as "software workarounds" and are only required in the minority of
cases (e.g. those cases that both require a non-secure workaround *and*
can be triggered by Linux).

For software workarounds that may adversely impact systems unaffected by
the erratum in question, a Kconfig entry is added under "Kernel
Features" -> "ARM errata workarounds via the alternatives framework".
These are enabled by default and patched in at runtime when an affected
CPU is detected. For less-intrusive workarounds, a Kconfig option is not
available and the code is structured (preferably with a comment) in such
a way that the erratum will not be hit.

This approach can make it slightly onerous to determine exactly which
errata are worked around in an arbitrary kernel source tree, so this
file acts as a registry of software workarounds in the Linux Kernel and
will be updated when new workarounds are committed and backported to
stable kernels.

+----------------+-----------------+-----------------+-----------------------------+
| Implementor    | Component       | Erratum ID      | Kconfig                     |
+================+=================+=================+=============================+
| Allwinner      | A64/R18         | UNKNOWN1        | SUN50I_ERRATUM_UNKNOWN1     |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A53      | #826319         | ARM64_ERRATUM_826319        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A53      | #827319         | ARM64_ERRATUM_827319        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A53      | #824069         | ARM64_ERRATUM_824069        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A53      | #819472         | ARM64_ERRATUM_819472        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A53      | #845719         | ARM64_ERRATUM_845719        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A53      | #843419         | ARM64_ERRATUM_843419        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A55      | #1024718        | ARM64_ERRATUM_1024718       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A55      | #1530923        | ARM64_ERRATUM_1530923       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A57      | #832075         | ARM64_ERRATUM_832075        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A57      | #852523         | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A57      | #834220         | ARM64_ERRATUM_834220        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A57      | #1319537        | ARM64_ERRATUM_1319367       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A72      | #853709         | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A72      | #1319367        | ARM64_ERRATUM_1319367       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A73      | #858921         | ARM64_ERRATUM_858921        |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A76      | #1188873,1418040| ARM64_ERRATUM_1418040       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A76      | #1165522        | ARM64_ERRATUM_1165522       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A76      | #1286807        | ARM64_ERRATUM_1286807       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A76      | #1463225        | ARM64_ERRATUM_1463225       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Cortex-A77      | #1508412        | ARM64_ERRATUM_1508412       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Neoverse-N1     | #1188873,1418040| ARM64_ERRATUM_1418040       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Neoverse-N1     | #1349291        | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | Neoverse-N1     | #1542419        | ARM64_ERRATUM_1542419       |
+----------------+-----------------+-----------------+-----------------------------+
| ARM            | MMU-500         | #841119,826419  | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| Broadcom       | Brahma-B53      | N/A             | ARM64_ERRATUM_845719        |
+----------------+-----------------+-----------------+-----------------------------+
| Broadcom       | Brahma-B53      | N/A             | ARM64_ERRATUM_843419        |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX ITS    | #22375,24313    | CAVIUM_ERRATUM_22375        |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX ITS    | #23144          | CAVIUM_ERRATUM_23144        |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX GICv3  | #23154          | CAVIUM_ERRATUM_23154        |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX GICv3  | #38539          | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX Core   | #27456          | CAVIUM_ERRATUM_27456        |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX Core   | #30115          | CAVIUM_ERRATUM_30115        |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX SMMUv2 | #27704          | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX2 SMMUv3| #74             | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX2 SMMUv3| #126            | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| Cavium         | ThunderX2 Core  | #219            | CAVIUM_TX2_ERRATUM_219      |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| Marvell        | ARM-MMU-500     | #582743         | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| NVIDIA         | Carmel Core     | N/A             | NVIDIA_CARMEL_CNP_ERRATUM   |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| Freescale/NXP  | LS2080A/LS1043A | A-008585        | FSL_ERRATUM_A008585         |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| Hisilicon      | Hip0{5,6,7}     | #161010101      | HISILICON_ERRATUM_161010101 |
+----------------+-----------------+-----------------+-----------------------------+
| Hisilicon      | Hip0{6,7}       | #161010701      | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| Hisilicon      | Hip0{6,7}       | #161010803      | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
| Hisilicon      | Hip07           | #161600802      | HISILICON_ERRATUM_161600802 |
+----------------+-----------------+-----------------+-----------------------------+
| Hisilicon      | Hip08 SMMU PMCG | #162001800      | N/A                         |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| Qualcomm Tech. | Kryo/Falkor v1  | E1003           | QCOM_FALKOR_ERRATUM_1003    |
+----------------+-----------------+-----------------+-----------------------------+
| Qualcomm Tech. | Kryo/Falkor v1  | E1009           | QCOM_FALKOR_ERRATUM_1009    |
+----------------+-----------------+-----------------+-----------------------------+
| Qualcomm Tech. | QDF2400 ITS     | E0065           | QCOM_QDF2400_ERRATUM_0065   |
+----------------+-----------------+-----------------+-----------------------------+
| Qualcomm Tech. | Falkor v{1,2}   | E1041           | QCOM_FALKOR_ERRATUM_1041    |
+----------------+-----------------+-----------------+-----------------------------+
| Qualcomm Tech. | Kryo4xx Gold    | N/A             | ARM64_ERRATUM_1463225       |
+----------------+-----------------+-----------------+-----------------------------+
| Qualcomm Tech. | Kryo4xx Gold    | N/A             | ARM64_ERRATUM_1418040       |
+----------------+-----------------+-----------------+-----------------------------+
| Qualcomm Tech. | Kryo4xx Silver  | N/A             | ARM64_ERRATUM_1530923       |
+----------------+-----------------+-----------------+-----------------------------+
| Qualcomm Tech. | Kryo4xx Silver  | N/A             | ARM64_ERRATUM_1024718       |
+----------------+-----------------+-----------------+-----------------------------+
+----------------+-----------------+-----------------+-----------------------------+
| Fujitsu        | A64FX           | E#010001        | FUJITSU_ERRATUM_010001      |
+----------------+-----------------+-----------------+-----------------------------+
