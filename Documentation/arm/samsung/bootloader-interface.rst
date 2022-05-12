==========================================================
Interface between kernel and boot loaders on Exynos boards
==========================================================

Author: Krzysztof Kozlowski

Date  : 6 June 2015

The document tries to describe currently used interface between Linux kernel
and boot loaders on Samsung Exynos based boards. This is not a definition
of interface but rather a description of existing state, a reference
for information purpose only.

In the document "boot loader" means any of following: U-boot, proprietary
SBOOT or any other firmware for ARMv7 and ARMv8 initializing the board before
executing kernel.


1. Non-Secure mode

Address:      sysram_ns_base_addr

============= ============================================ ==================
Offset        Value                                        Purpose
============= ============================================ ==================
0x08          exynos_cpu_resume_ns, mcpm_entry_point       System suspend
0x0c          0x00000bad (Magic cookie)                    System suspend
0x1c          exynos4_secondary_startup                    Secondary CPU boot
0x1c + 4*cpu  exynos4_secondary_startup (Exynos4412)       Secondary CPU boot
0x20          0xfcba0d10 (Magic cookie)                    AFTR
0x24          exynos_cpu_resume_ns                         AFTR
0x28 + 4*cpu  0x8 (Magic cookie, Exynos3250)               AFTR
0x28          0x0 or last value during resume (Exynos542x) System suspend
============= ============================================ ==================


2. Secure mode

Address:      sysram_base_addr

============= ============================================ ==================
Offset        Value                                        Purpose
============= ============================================ ==================
0x00          exynos4_secondary_startup                    Secondary CPU boot
0x04          exynos4_secondary_startup (Exynos542x)       Secondary CPU boot
4*cpu         exynos4_secondary_startup (Exynos4412)       Secondary CPU boot
0x20          exynos_cpu_resume (Exynos4210 r1.0)          AFTR
0x24          0xfcba0d10 (Magic cookie, Exynos4210 r1.0)   AFTR
============= ============================================ ==================

Address:      pmu_base_addr

============= ============================================ ==================
Offset        Value                                        Purpose
============= ============================================ ==================
0x0800        exynos_cpu_resume                            AFTR, suspend
0x0800        mcpm_entry_point (Exynos542x with MCPM)      AFTR, suspend
0x0804        0xfcba0d10 (Magic cookie)                    AFTR
0x0804        0x00000bad (Magic cookie)                    System suspend
0x0814        exynos4_secondary_startup (Exynos4210 r1.1)  Secondary CPU boot
0x0818        0xfcba0d10 (Magic cookie, Exynos4210 r1.1)   AFTR
0x081C        exynos_cpu_resume (Exynos4210 r1.1)          AFTR
============= ============================================ ==================

3. Other (regardless of secure/non-secure mode)

Address:      pmu_base_addr

============= =============================== ===============================
Offset        Value                           Purpose
============= =============================== ===============================
0x0908        Non-zero                        Secondary CPU boot up indicator
                                              on Exynos3250 and Exynos542x
============= =============================== ===============================


4. Glossary

AFTR - ARM Off Top Running, a low power mode, Cortex cores and many other
modules are power gated, except the TOP modules
MCPM - Multi-Cluster Power Management
