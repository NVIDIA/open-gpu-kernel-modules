#!/bin/sh

PATH="${PATH}:/bin:/sbin:/usr/bin"

# make sure we are in the directory containing this script
SCRIPTDIR=`dirname $0`
cd $SCRIPTDIR

CC="$1"
ARCH=$2
ISYSTEM=`$CC -print-file-name=include 2> /dev/null`
SOURCES=$3
HEADERS=$SOURCES/include
OUTPUT=$4
XEN_PRESENT=1
PREEMPT_RT_PRESENT=0
KERNEL_ARCH="$ARCH"

if [ "$ARCH" = "i386" -o "$ARCH" = "x86_64" ]; then
    if [ -d "$SOURCES/arch/x86" ]; then
        KERNEL_ARCH="x86"
    fi
fi

# VGX_BUILD parameter defined only for VGX builds (vGPU Host driver)
# VGX_KVM_BUILD parameter defined only vGPU builds on KVM hypervisor
# GRID_BUILD parameter defined only for GRID builds (GRID Guest driver)
# GRID_BUILD_CSP parameter defined only for GRID CSP builds (GRID Guest driver for CSPs)

test_xen() {
    #
    # Determine if the target kernel is a Xen kernel. It used to be
    # sufficient to check for CONFIG_XEN, but the introduction of
    # modular para-virtualization (CONFIG_PARAVIRT, etc.) and
    # Xen guest support, it is no longer possible to determine the
    # target environment at build time. Therefore, if both
    # CONFIG_XEN and CONFIG_PARAVIRT are present, text_xen() treats
    # the kernel as a stand-alone kernel.
    #
    if ! test_configuration_option CONFIG_XEN ||
         test_configuration_option CONFIG_PARAVIRT; then
        XEN_PRESENT=0
    fi
}

append_conftest() {
    #
    # Echo data from stdin: this is a transitional function to make it easier
    # to port conftests from drivers with parallel conftest generation to
    # older driver versions
    #

    while read LINE; do
        echo ${LINE}
    done
}

translate_and_preprocess_header_files() {
    # Inputs:
    #   $1: list of relative file paths
    #
    # This routine creates an upper case, underscore version of each of the
    # relative file paths, and uses that as the token to either define or
    # undefine in a C header file. For example, linux/fence.h becomes
    # NV_LINUX_FENCE_H_PRESENT, and that is either defined or undefined, in the
    # output (which goes to stdout, just like the rest of this file).

    # -MG or -MD can interfere with the use of -M and -M -MG for testing file
    # existence; filter out any occurrences from CFLAGS. CFLAGS is intentionally
    # wrapped with whitespace in the input to sed(1) so the regex can match zero
    # or more occurrences of "-MD" or "-MG", surrounded by whitespace to avoid
    # accidental matches with tokens that happen to contain either of those
    # strings, without special handling of the beginning or the end of the line.
    TEST_CFLAGS=`echo "-E -M $CFLAGS " | sed -e 's/\( -M[DG]\)* / /g'`

    for file in "$@"; do
        file_define=NV_`echo $file | tr '/.' '_' | tr '-' '_' | tr 'a-z' 'A-Z'`_PRESENT

        CODE="#include <$file>"

        if echo "$CODE" | $CC $TEST_CFLAGS - > /dev/null 2>&1; then
            echo "#define $file_define"
        else
            # If preprocessing failed, it could have been because the header
            # file under test is not present, or because it is present but
            # depends upon the inclusion of other header files. Attempting
            # preprocessing again with -MG will ignore a missing header file
            # but will still fail if the header file is present.
            if echo "$CODE" | $CC $TEST_CFLAGS -MG - > /dev/null 2>&1; then
                echo "#undef $file_define"
            else
                echo "#define $file_define"
            fi
        fi
    done
}

test_headers() {
    #
    # Determine which header files (of a set that may or may not be
    # present) are provided by the target kernel.
    #
    FILES="asm/system.h"
    FILES="$FILES drm/drmP.h"
    FILES="$FILES drm/drm_auth.h"
    FILES="$FILES drm/drm_gem.h"
    FILES="$FILES drm/drm_crtc.h"
    FILES="$FILES drm/drm_atomic.h"
    FILES="$FILES drm/drm_atomic_helper.h"
    FILES="$FILES drm/drm_encoder.h"
    FILES="$FILES drm/drm_atomic_uapi.h"
    FILES="$FILES drm/drm_drv.h"
    FILES="$FILES drm/drm_framebuffer.h"
    FILES="$FILES drm/drm_connector.h"
    FILES="$FILES drm/drm_probe_helper.h"
    FILES="$FILES drm/drm_blend.h"
    FILES="$FILES drm/drm_fourcc.h"
    FILES="$FILES drm/drm_prime.h"
    FILES="$FILES drm/drm_plane.h"
    FILES="$FILES drm/drm_vblank.h"
    FILES="$FILES drm/drm_file.h"
    FILES="$FILES drm/drm_ioctl.h"
    FILES="$FILES drm/drm_device.h"
    FILES="$FILES drm/drm_mode_config.h"
    FILES="$FILES dt-bindings/interconnect/tegra_icc_id.h"
    FILES="$FILES generated/autoconf.h"
    FILES="$FILES generated/compile.h"
    FILES="$FILES generated/utsrelease.h"
    FILES="$FILES linux/efi.h"
    FILES="$FILES linux/kconfig.h"
    FILES="$FILES linux/platform/tegra/mc_utils.h"
    FILES="$FILES linux/semaphore.h"
    FILES="$FILES linux/printk.h"
    FILES="$FILES linux/ratelimit.h"
    FILES="$FILES linux/prio_tree.h"
    FILES="$FILES linux/log2.h"
    FILES="$FILES linux/of.h"
    FILES="$FILES linux/bug.h"
    FILES="$FILES linux/sched/signal.h"
    FILES="$FILES linux/sched/task.h"
    FILES="$FILES linux/sched/task_stack.h"
    FILES="$FILES xen/ioemu.h"
    FILES="$FILES linux/fence.h"
    FILES="$FILES linux/dma-resv.h"
    FILES="$FILES soc/tegra/chip-id.h"
    FILES="$FILES soc/tegra/fuse.h"
    FILES="$FILES soc/tegra/tegra_bpmp.h"
    FILES="$FILES video/nv_internal.h"
    FILES="$FILES linux/platform/tegra/dce/dce-client-ipc.h"
    FILES="$FILES linux/nvhost.h"
    FILES="$FILES linux/nvhost_t194.h"
    FILES="$FILES asm/book3s/64/hash-64k.h"
    FILES="$FILES asm/set_memory.h"
    FILES="$FILES asm/prom.h"
    FILES="$FILES asm/powernv.h"
    FILES="$FILES linux/atomic.h"
    FILES="$FILES asm/barrier.h"
    FILES="$FILES asm/opal-api.h"
    FILES="$FILES sound/hdaudio.h"
    FILES="$FILES asm/pgtable_types.h"
    FILES="$FILES linux/stringhash.h"
    FILES="$FILES linux/dma-map-ops.h"
    FILES="$FILES rdma/peer_mem.h"
    FILES="$FILES sound/hda_codec.h"
    FILES="$FILES linux/dma-buf.h"
    FILES="$FILES linux/time.h"
    FILES="$FILES linux/platform_device.h"
    FILES="$FILES linux/mutex.h"
    FILES="$FILES linux/reset.h"
    FILES="$FILES linux/of_platform.h"
    FILES="$FILES linux/of_device.h"
    FILES="$FILES linux/of_gpio.h"
    FILES="$FILES linux/gpio.h"
    FILES="$FILES linux/gpio/consumer.h"
    FILES="$FILES linux/interconnect.h"
    FILES="$FILES linux/pm_runtime.h"
    FILES="$FILES linux/clk.h"
    FILES="$FILES linux/clk-provider.h"
    FILES="$FILES linux/ioasid.h"
    FILES="$FILES linux/stdarg.h"
    FILES="$FILES linux/iosys-map.h"
    FILES="$FILES asm/coco.h"

    translate_and_preprocess_header_files $FILES
}

build_cflags() {
    BASE_CFLAGS="-O2 -D__KERNEL__ \
-DKBUILD_BASENAME=\"#conftest$$\" -DKBUILD_MODNAME=\"#conftest$$\" \
-nostdinc -isystem $ISYSTEM"

    if [ "$OUTPUT" != "$SOURCES" ]; then
        OUTPUT_CFLAGS="-I$OUTPUT/include2 -I$OUTPUT/include"
        if [ -f "$OUTPUT/include/generated/autoconf.h" ]; then
            AUTOCONF_FILE="$OUTPUT/include/generated/autoconf.h"
        else
            AUTOCONF_FILE="$OUTPUT/include/linux/autoconf.h"
        fi
    else
        if [ -f "$HEADERS/generated/autoconf.h" ]; then
            AUTOCONF_FILE="$HEADERS/generated/autoconf.h"
        else
            AUTOCONF_FILE="$HEADERS/linux/autoconf.h"
        fi
    fi

    test_xen

    if [ "$XEN_PRESENT" != "0" ]; then
        MACH_CFLAGS="-I$HEADERS/asm/mach-xen"
    fi

    SOURCE_HEADERS="$HEADERS"
    SOURCE_ARCH_HEADERS="$SOURCES/arch/$KERNEL_ARCH/include"
    OUTPUT_HEADERS="$OUTPUT/include"
    OUTPUT_ARCH_HEADERS="$OUTPUT/arch/$KERNEL_ARCH/include"

    # Look for mach- directories on this arch, and add it to the list of
    # includes if that platform is enabled in the configuration file, which
    # may have a definition like this:
    #   #define CONFIG_ARCH_<MACHUPPERCASE> 1
    for _mach_dir in `ls -1d $SOURCES/arch/$KERNEL_ARCH/mach-* 2>/dev/null`; do
        _mach=`echo $_mach_dir | \
            sed -e "s,$SOURCES/arch/$KERNEL_ARCH/mach-,," | \
            tr 'a-z' 'A-Z'`
        grep "CONFIG_ARCH_$_mach \+1" $AUTOCONF_FILE > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            MACH_CFLAGS="$MACH_CFLAGS -I$_mach_dir/include"
        fi
    done

    if [ "$ARCH" = "arm" ]; then
        MACH_CFLAGS="$MACH_CFLAGS -D__LINUX_ARM_ARCH__=7"
    fi

    # Add the mach-default includes (only found on x86/older kernels)
    MACH_CFLAGS="$MACH_CFLAGS -I$SOURCE_HEADERS/asm-$KERNEL_ARCH/mach-default"
    MACH_CFLAGS="$MACH_CFLAGS -I$SOURCE_ARCH_HEADERS/asm/mach-default"

    CFLAGS="$BASE_CFLAGS $MACH_CFLAGS $OUTPUT_CFLAGS -include $AUTOCONF_FILE"
    CFLAGS="$CFLAGS -I$SOURCE_HEADERS"
    CFLAGS="$CFLAGS -I$SOURCE_HEADERS/uapi"
    CFLAGS="$CFLAGS -I$SOURCE_HEADERS/xen"
    CFLAGS="$CFLAGS -I$OUTPUT_HEADERS/generated/uapi"
    CFLAGS="$CFLAGS -I$SOURCE_ARCH_HEADERS"
    CFLAGS="$CFLAGS -I$SOURCE_ARCH_HEADERS/uapi"
    CFLAGS="$CFLAGS -I$OUTPUT_ARCH_HEADERS/generated"
    CFLAGS="$CFLAGS -I$OUTPUT_ARCH_HEADERS/generated/uapi"

    if [ -n "$BUILD_PARAMS" ]; then
        CFLAGS="$CFLAGS -D$BUILD_PARAMS"
    fi

    # Check if gcc supports asm goto and set CC_HAVE_ASM_GOTO if it does.
    # Older kernels perform this check and set this flag in Kbuild, and since
    # conftest.sh runs outside of Kbuild it ends up building without this flag.
    # Starting with commit e9666d10a5677a494260d60d1fa0b73cc7646eb3 this test
    # is done within Kconfig, and the preprocessor flag is no longer needed.

    GCC_GOTO_SH="$SOURCES/build/gcc-goto.sh"

    if [ -f "$GCC_GOTO_SH" ]; then
        # Newer versions of gcc-goto.sh don't print anything on success, but
        # this is okay, since it's no longer necessary to set CC_HAVE_ASM_GOTO
        # based on the output of those versions of gcc-goto.sh.
        if [ `/bin/sh "$GCC_GOTO_SH" "$CC"` = "y" ]; then
            CFLAGS="$CFLAGS -DCC_HAVE_ASM_GOTO"
        fi
    fi

    #
    # If CONFIG_HAVE_FENTRY is enabled and gcc supports -mfentry flags then set
    # CC_USING_FENTRY and add -mfentry into cflags.
    #
    # linux/ftrace.h file indirectly gets included into the conftest source and
    # fails to get compiled, because conftest.sh runs outside of Kbuild it ends
    # up building without -mfentry and CC_USING_FENTRY flags.
    #
    grep "CONFIG_HAVE_FENTRY \+1" $AUTOCONF_FILE > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "" > conftest$$.c

        $CC -mfentry -c -x c conftest$$.c > /dev/null 2>&1
        rm -f conftest$$.c

        if [ -f conftest$$.o ]; then
            rm -f conftest$$.o

            CFLAGS="$CFLAGS -mfentry -DCC_USING_FENTRY"
        fi
    fi
}

CONFTEST_PREAMBLE="#include \"conftest/headers.h\"
    #if defined(NV_LINUX_KCONFIG_H_PRESENT)
    #include <linux/kconfig.h>
    #endif
    #if defined(NV_GENERATED_AUTOCONF_H_PRESENT)
    #include <generated/autoconf.h>
    #else
    #include <linux/autoconf.h>
    #endif
    #if defined(CONFIG_XEN) && \
        defined(CONFIG_XEN_INTERFACE_VERSION) &&  !defined(__XEN_INTERFACE_VERSION__)
    #define __XEN_INTERFACE_VERSION__ CONFIG_XEN_INTERFACE_VERSION
    #endif
    #if defined(CONFIG_KASAN) && defined(CONFIG_ARM64)
    #if defined(CONFIG_KASAN_SW_TAGS)
    #define KASAN_SHADOW_SCALE_SHIFT 4
    #else
    #define KASAN_SHADOW_SCALE_SHIFT 3
    #endif
    #endif"

test_configuration_option() {
    #
    # Check to see if the given configuration option is defined
    #

    get_configuration_option $1 >/dev/null 2>&1

    return $?

}

set_configuration() {
    #
    # Set a specific configuration option.  This function is called to always
    # enable a configuration, in order to verify whether the test code for that
    # configuration is no longer required and the corresponding
    # conditionally-compiled code in the driver can be removed.
    #
    DEF="$1"

    if [ "$3" = "" ]
    then
        VAL=""
        CAT="$2"
    else
        VAL="$2"
        CAT="$3"
    fi

    echo "#define ${DEF} ${VAL}" | append_conftest "${CAT}"
}

unset_configuration() {
    #
    # Un-set a specific configuration option.  This function is called to
    # always disable a configuration, in order to verify whether the test
    # code for that configuration is no longer required and the corresponding
    # conditionally-compiled code in the driver can be removed.
    #
    DEF="$1"
    CAT="$2"

    echo "#undef ${DEF}" | append_conftest "${CAT}"
}

compile_check_conftest() {
    #
    # Compile the current conftest C file and check+output the result
    #
    CODE="$1"
    DEF="$2"
    VAL="$3"
    CAT="$4"

    echo "$CONFTEST_PREAMBLE
    $CODE" > conftest$$.c

    $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
    rm -f conftest$$.c

    if [ -f conftest$$.o ]; then
        rm -f conftest$$.o
        if [ "${CAT}" = "functions" ]; then
            #
            # The logic for "functions" compilation tests is inverted compared to
            # other compilation steps: if the function is present, the code
            # snippet will fail to compile because the function call won't match
            # the prototype. If the function is not present, the code snippet
            # will produce an object file with the function as an unresolved
            # symbol.
            #
            echo "#undef ${DEF}" | append_conftest "${CAT}"
        else
            echo "#define ${DEF} ${VAL}" | append_conftest "${CAT}"
        fi
        return
    else
        if [ "${CAT}" = "functions" ]; then
            echo "#define ${DEF} ${VAL}" | append_conftest "${CAT}"
        else
            echo "#undef ${DEF}" | append_conftest "${CAT}"
        fi
        return
    fi
}

export_symbol_present_conftest() {
    #
    # Check Module.symvers to see whether the given symbol is present.
    #

    SYMBOL="$1"
    TAB='	'

    if grep -e "${TAB}${SYMBOL}${TAB}.*${TAB}EXPORT_SYMBOL.*\$" \
               "$OUTPUT/Module.symvers" >/dev/null 2>&1; then
        echo "#define NV_IS_EXPORT_SYMBOL_PRESENT_$SYMBOL 1" |
            append_conftest "symbols"
    else
        # May be a false negative if Module.symvers is absent or incomplete,
        # or if the Module.symvers format changes.
        echo "#define NV_IS_EXPORT_SYMBOL_PRESENT_$SYMBOL 0" |
            append_conftest "symbols"
    fi
}

export_symbol_gpl_conftest() {
    #
    # Check Module.symvers to see whether the given symbol is present and its
    # export type is GPL-only (including deprecated GPL-only symbols).
    #

    SYMBOL="$1"
    TAB='	'

    if grep -e "${TAB}${SYMBOL}${TAB}.*${TAB}EXPORT_\(UNUSED_\)*SYMBOL_GPL\$" \
               "$OUTPUT/Module.symvers" >/dev/null 2>&1; then
        echo "#define NV_IS_EXPORT_SYMBOL_GPL_$SYMBOL 1" |
            append_conftest "symbols"
    else
        # May be a false negative if Module.symvers is absent or incomplete,
        # or if the Module.symvers format changes.
        echo "#define NV_IS_EXPORT_SYMBOL_GPL_$SYMBOL 0" |
            append_conftest "symbols"
    fi
}

get_configuration_option() {
    #
    # Print the value of given configuration option, if defined
    #
    RET=1
    OPTION=$1

    OLD_FILE="linux/autoconf.h"
    NEW_FILE="generated/autoconf.h"
    FILE=""

    if [ -f $HEADERS/$NEW_FILE -o -f $OUTPUT/include/$NEW_FILE ]; then
        FILE=$NEW_FILE
    elif [ -f $HEADERS/$OLD_FILE -o -f $OUTPUT/include/$OLD_FILE ]; then
        FILE=$OLD_FILE
    fi

    if [ -n "$FILE" ]; then
        #
        # We are looking at a configured source tree; verify
        # that its configuration includes the given option
        # via a compile check, and print the option's value.
        #

        if [ -f $HEADERS/$FILE ]; then
            INCLUDE_DIRECTORY=$HEADERS
        elif [ -f $OUTPUT/include/$FILE ]; then
            INCLUDE_DIRECTORY=$OUTPUT/include
        else
            return 1
        fi

        echo "#include <$FILE>
        #ifndef $OPTION
        #error $OPTION not defined!
        #endif

        $OPTION
        " > conftest$$.c

        $CC -E -P -I$INCLUDE_DIRECTORY -o conftest$$ conftest$$.c > /dev/null 2>&1

        if [ -e conftest$$ ]; then
            tr -d '\r\n\t ' < conftest$$
            RET=$?
        fi

        rm -f conftest$$.c conftest$$
    else
        CONFIG=$OUTPUT/.config
        if [ -f $CONFIG ] && grep "^$OPTION=" $CONFIG; then
            grep "^$OPTION=" $CONFIG | cut -f 2- -d "="
            RET=$?
        fi
    fi

    return $RET

}

check_for_ib_peer_memory_symbols() {
    kernel_dir="$1"
    module_symvers="${kernel_dir}/Module.symvers"

    sym_ib_register="ib_register_peer_memory_client"
    sym_ib_unregister="ib_unregister_peer_memory_client"
    tab='	'

    # Return 0 for true(no errors), 1 for false
    if [ ! -f "${module_symvers}" ]; then
        return 1
    fi

    if grep -e "${tab}${sym_ib_register}${tab}.*${tab}EXPORT_SYMBOL.*\$"    \
               "${module_symvers}" > /dev/null 2>&1 &&
       grep -e "${tab}${sym_ib_unregister}${tab}.*${tab}EXPORT_SYMBOL.*\$"  \
               "${module_symvers}" > /dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

compile_test() {
    case "$1" in
        set_memory_uc)
            #
            # Determine if the set_memory_uc() function is present.
            # It does not exist on all architectures.
            #
            CODE="
            #include <linux/types.h>
            #if defined(NV_ASM_SET_MEMORY_H_PRESENT)
            #if defined(NV_ASM_PGTABLE_TYPES_H_PRESENT)
            #include <asm/pgtable_types.h>
            #endif
            #include <asm/set_memory.h>
            #else
            #include <asm/cacheflush.h>
            #endif
            void conftest_set_memory_uc(void) {
                set_memory_uc();
            }"

            compile_check_conftest "$CODE" "NV_SET_MEMORY_UC_PRESENT" "" "functions"
        ;;

        set_memory_array_uc)
            #
            # Determine if the set_memory_array_uc() function is present.
            # It does not exist on all architectures.
            #
            CODE="
            #include <linux/types.h>
            #if defined(NV_ASM_SET_MEMORY_H_PRESENT)
            #if defined(NV_ASM_PGTABLE_TYPES_H_PRESENT)
            #include <asm/pgtable_types.h>
            #endif
            #include <asm/set_memory.h>
            #else
            #include <asm/cacheflush.h>
            #endif
            void conftest_set_memory_array_uc(void) {
                set_memory_array_uc();
            }"

            compile_check_conftest "$CODE" "NV_SET_MEMORY_ARRAY_UC_PRESENT" "" "functions"
        ;;

        sysfs_slab_unlink)
            #
            # Determine if the sysfs_slab_unlink() function is present.
            #
            # This test is useful to check for the presence a fix for the deferred
            # kmem_cache destroy feature (see nvbug: 2543505).
            #
            # Added by commit d50d82faa0c9 ("slub: fix failure when we delete and
            # create a slab cache") in 4.18 (2018-06-27).
            #
            CODE="
            #include <linux/slab.h>
            void conftest_sysfs_slab_unlink(void) {
                sysfs_slab_unlink();
            }"

            compile_check_conftest "$CODE" "NV_SYSFS_SLAB_UNLINK_PRESENT" "" "functions"
        ;;

        list_is_first)
            #
            # Determine if the list_is_first() function is present.
            #
            # Added by commit 70b44595eafe ("mm, compaction: use free lists
            # to quickly locate a migration source") in 5.1 (2019-03-05)
            #
            CODE="
            #include <linux/list.h>
            void conftest_list_is_first(void) {
                list_is_first();
            }"

            compile_check_conftest "$CODE" "NV_LIST_IS_FIRST_PRESENT" "" "functions"
        ;;

        set_pages_uc)
            #
            # Determine if the set_pages_uc() function is present.
            # It does not exist on all architectures.
            #
            CODE="
            #include <linux/types.h>
            #if defined(NV_ASM_SET_MEMORY_H_PRESENT)
            #if defined(NV_ASM_PGTABLE_TYPES_H_PRESENT)
            #include <asm/pgtable_types.h>
            #endif
            #include <asm/set_memory.h>
            #else
            #include <asm/cacheflush.h>
            #endif
            void conftest_set_pages_uc(void) {
                set_pages_uc();
            }"

            compile_check_conftest "$CODE" "NV_SET_PAGES_UC_PRESENT" "" "functions"
        ;;

        set_pages_array_uc)
            #
            # Determine if the set_pages_array_uc() function is present.
            # It does not exist on all architectures.
            #
            # set_pages_array_uc() was added by commit
            # 0f3507555f6fa4acbc85a646d6e8766230db38fc ("x86, CPA: Add
            # set_pages_arrayuc and set_pages_array_wb") in v2.6.30-rc1 (Thu Mar
            # 19 14:51:15 2009)
            #
            CODE="
            #include <linux/types.h>
            #if defined(NV_ASM_SET_MEMORY_H_PRESENT)
            #if defined(NV_ASM_PGTABLE_TYPES_H_PRESENT)
            #include <asm/pgtable_types.h>
            #endif
            #include <asm/set_memory.h>
            #else
            #include <asm/cacheflush.h>
            #endif
            void conftest_set_pages_array_uc(void) {
                set_pages_array_uc();
            }"

            compile_check_conftest "$CODE" "NV_SET_PAGES_ARRAY_UC_PRESENT" "" "functions"
        ;;

        flush_cache_all)
            #
            # Determine if flush_cache_all() function is present
            #
            # flush_cache_all() was removed by commit id
            # 68234df4ea79 ("arm64: kill flush_cache_all()") in 4.2 (2015-04-20)
            # for aarch64
            #
            CODE="
            #include <asm/cacheflush.h>
            int conftest_flush_cache_all(void) {
                return flush_cache_all();
            }"
            compile_check_conftest "$CODE" "NV_FLUSH_CACHE_ALL_PRESENT" "" "functions"
        ;;

        pci_get_domain_bus_and_slot)
            #
            # Determine if the pci_get_domain_bus_and_slot() function
            # is present.
            #
            # Added by commit 3c299dc22635 ("PCI: add
            # pci_get_domain_bus_and_slot function") in 2.6.33.
            #
            CODE="
            #include <linux/pci.h>
            void conftest_pci_get_domain_bus_and_slot(void) {
                pci_get_domain_bus_and_slot();
            }"

            compile_check_conftest "$CODE" "NV_PCI_GET_DOMAIN_BUS_AND_SLOT_PRESENT" "" "functions"
        ;;

        pci_bus_address)
            #
            # Determine if the pci_bus_address() function is
            # present.
            #
            # Added by commit 06cf56e497c8 ("PCI: Add pci_bus_address() to
            # get bus address of a BAR") in v3.14
            #
            CODE="
            #include <linux/pci.h>
            void conftest_pci_bus_address(void) {
                pci_bus_address();
            }"

            compile_check_conftest "$CODE" "NV_PCI_BUS_ADDRESS_PRESENT" "" "functions"
        ;;

        hash__remap_4k_pfn)
            #
            # Determine if the hash__remap_4k_pfn() function is
            # present.
            #
            # Added by commit 6cc1a0ee4ce2 ("powerpc/mm/radix: Add radix
            # callback for pmd accessors") in v4.7 (committed 2016-04-29).
            # Present only in arch/powerpc
            #
            CODE="
            #if defined(NV_ASM_BOOK3S_64_HASH_64K_H_PRESENT)
            #include <linux/mm.h>
            #include <asm/book3s/64/hash-64k.h>
            #endif
            void conftest_hash__remap_4k_pfn(void) {
                hash__remap_4k_pfn();
            }"

            compile_check_conftest "$CODE" "NV_HASH__REMAP_4K_PFN_PRESENT" "" "functions"
        ;;

        register_cpu_notifier)
            #
            # Determine if register_cpu_notifier() is present
            #
            # Removed by commit 530e9b76ae8f ("cpu/hotplug: Remove obsolete
            # cpu hotplug register/unregister functions") in v4.10
            # (2016-12-21)
            #
            CODE="
            #include <linux/cpu.h>
            void conftest_register_cpu_notifier(void) {
                register_cpu_notifier();
            }" > conftest$$.c
            compile_check_conftest "$CODE" "NV_REGISTER_CPU_NOTIFIER_PRESENT" "" "functions"
        ;;

        cpuhp_setup_state)
            #
            # Determine if cpuhp_setup_state() is present
            #
            # Added by commit 5b7aa87e0482 ("cpu/hotplug: Implement
            # setup/removal interface") in v4.6 (commited 2016-02-26)
            #
            # It is used as a replacement for register_cpu_notifier
            CODE="
            #include <linux/cpu.h>
            void conftest_cpuhp_setup_state(void) {
                cpuhp_setup_state();
            }" > conftest$$.c
            compile_check_conftest "$CODE" "NV_CPUHP_SETUP_STATE_PRESENT" "" "functions"
        ;;

        ioremap_cache)
            #
            # Determine if the ioremap_cache() function is present.
            # It does not exist on all architectures.
            #
            CODE="
            #include <asm/io.h>
            void conftest_ioremap_cache(void) {
                ioremap_cache();
            }"

            compile_check_conftest "$CODE" "NV_IOREMAP_CACHE_PRESENT" "" "functions"
        ;;

        ioremap_wc)
            #
            # Determine if the ioremap_wc() function is present.
            # It does not exist on all architectures.
            #
            CODE="
            #include <asm/io.h>
            void conftest_ioremap_wc(void) {
                ioremap_wc();
            }"

            compile_check_conftest "$CODE" "NV_IOREMAP_WC_PRESENT" "" "functions"
        ;;

        file_operations)
            # 'ioctl' field removed by commit b19dd42faf41
            # ("bkl: Remove locked .ioctl file operation") in v2.6.36
            CODE="
            #include <linux/fs.h>
            int conftest_file_operations(void) {
                return offsetof(struct file_operations, ioctl);
            }"

            compile_check_conftest "$CODE" "NV_FILE_OPERATIONS_HAS_IOCTL" "" "types"
        ;;

        sg_alloc_table)
            #
            # sg_alloc_table_from_pages added by commit efc42bc98058
            # ("scatterlist: add sg_alloc_table_from_pages function") in v3.6
            #
            CODE="
            #include <linux/scatterlist.h>
            void conftest_sg_alloc_table_from_pages(void) {
                sg_alloc_table_from_pages();
            }"

            compile_check_conftest "$CODE" "NV_SG_ALLOC_TABLE_FROM_PAGES_PRESENT" "" "functions"
        ;;

        efi_enabled)
            #
            # Added in 2.6.12 as a variable
            #
            # Determine if the efi_enabled symbol is present (as a variable),
            # or if the efi_enabled() function is present and how many
            # arguments it takes.
            #
            # Converted from a variable to a function by commit 83e68189745a
            # ("efi: Make 'efi_enabled' a function to query EFI facilities")
            # in v3.8
            #
            echo "$CONFTEST_PREAMBLE
            #if defined(NV_LINUX_EFI_H_PRESENT)
            #include <linux/efi.h>
            #endif
            int conftest_efi_enabled(void) {
                return efi_enabled(0);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_EFI_ENABLED_PRESENT" | append_conftest "functions"
                echo "#define NV_EFI_ENABLED_ARGUMENT_COUNT 1" | append_conftest "functions"
                rm -f conftest$$.o
                return
            else
                echo "#define NV_EFI_ENABLED_PRESENT" | append_conftest "symbols"
                return
            fi
        ;;

        dom0_kernel_present)
            # Add config parameter if running on DOM0.
            if [ -n "$VGX_BUILD" ]; then
                echo "#define NV_DOM0_KERNEL_PRESENT" | append_conftest "generic"
            else
                echo "#undef NV_DOM0_KERNEL_PRESENT" | append_conftest "generic"
            fi
            return
        ;;

        nvidia_vgpu_kvm_build)
           # Add config parameter if running on KVM host.
           if [ -n "$VGX_KVM_BUILD" ]; then
                echo "#define NV_VGPU_KVM_BUILD" | append_conftest "generic"
            else
                echo "#undef NV_VGPU_KVM_BUILD" | append_conftest "generic"
            fi
            return
        ;;

        vfio_register_notifier)
            #
            # Check number of arguments required.
            #
            # New parameters added by commit 22195cbd3451 ("vfio:
            # vfio_register_notifier: classify iommu notifier") in v4.10
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/vfio.h>
            int conftest_vfio_register_notifier(void) {
                return vfio_register_notifier((struct device *) NULL, (struct notifier_block *) NULL);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_VFIO_NOTIFIER_ARGUMENT_COUNT 2" | append_conftest "functions"
                rm -f conftest$$.o
                return
            else
                echo "#define NV_VFIO_NOTIFIER_ARGUMENT_COUNT 4" | append_conftest "functions"
                return
            fi
        ;;

        vfio_info_add_capability_has_cap_type_id_arg)
            #
            # Check if vfio_info_add_capability() has cap_type_id parameter.
            #
            # Removed by commit dda01f787df9 ("vfio: Simplify capability
            # helper") in v4.16 (2017-12-12)
            #
            CODE="
            #include <linux/vfio.h>
            int vfio_info_add_capability(struct vfio_info_cap *caps,
                                         int cap_type_id,
                                         void *cap_type) {
                return 0;
            }"

            compile_check_conftest "$CODE" "NV_VFIO_INFO_ADD_CAPABILITY_HAS_CAP_TYPE_ID_ARGS" "" "types"
        ;;

        vmbus_channel_has_ringbuffer_page)
            #
            # Check if ringbuffer_page field exist in vmbus_channel structure
            #
            # Changed in commit 52a42c2a90226dc61c99bbd0cb096deeb52c334b
            # ("vmbus: keep pointer to ring buffer page") in v5.0 (2018-09-14)
            #

            CODE="
            #include <linux/hyperv.h>

            int conftest_vmbus_channel_has_ringbuffer_page(void) {
                    return offsetof(struct vmbus_channel, ringbuffer_page);
            }"

            compile_check_conftest "$CODE" "NV_VMBUS_CHANNEL_HAS_RING_BUFFER_PAGE" "" "types"
        ;;

        nvidia_grid_build)
            if [ -n "$GRID_BUILD" ]; then
                echo "#define NV_GRID_BUILD" | append_conftest "generic"
            else
                echo "#undef NV_GRID_BUILD" | append_conftest "generic"
            fi
            return
        ;;

        nvidia_grid_csp_build)
            if [ -n "$GRID_BUILD_CSP" ]; then
                echo "#define NV_GRID_BUILD_CSP $GRID_BUILD_CSP" | append_conftest "generic"
            else
                echo "#undef NV_GRID_BUILD_CSP" | append_conftest "generic"
            fi
            return
        ;;

        vm_fault_has_address)
            #
            # Determine if the 'vm_fault' structure has an 'address', or a
            # 'virtual_address' field. The .virtual_address field was
            # effectively renamed to .address:
            #
            # 'address' added by commit 82b0f8c39a38 ("mm: join
            # struct fault_env and vm_fault") in v4.10 (2016-12-14)
            #
            # 'virtual_address' removed by commit 1a29d85eb0f1 ("mm: use
            # vmf->address instead of of vmf->virtual_address") in v4.10
            # (2016-12-14)
            #
            CODE="
            #include <linux/mm.h>
            int conftest_vm_fault_has_address(void) {
                return offsetof(struct vm_fault, address);
            }"

            compile_check_conftest "$CODE" "NV_VM_FAULT_HAS_ADDRESS" "" "types"
        ;;

        kmem_cache_has_kobj_remove_work)
            #
            # Determine if the 'kmem_cache' structure has 'kobj_remove_work'.
            #
            # 'kobj_remove_work' was added by commit 3b7b314053d02 ("slub: make
            # sysfs file removal asynchronous") in v4.12 (2017-06-23). This
            # commit introduced a race between kmem_cache destroy and create
            # which we need to workaround in our driver (see nvbug: 2543505).
            # Also see comment for sysfs_slab_unlink conftest.
            #
            CODE="
            #include <linux/mm.h>
            #include <linux/slab.h>
            #include <linux/slub_def.h>
            int conftest_kmem_cache_has_kobj_remove_work(void) {
                return offsetof(struct kmem_cache, kobj_remove_work);
            }"

            compile_check_conftest "$CODE" "NV_KMEM_CACHE_HAS_KOBJ_REMOVE_WORK" "" "types"
        ;;

        mdev_uuid)
            #
            # Determine if mdev_uuid() function is present or not
            #
            # Added by commit 99e3123e3d72 ("vfio-mdev: Make mdev_device
            # private and abstract interfaces") in v4.10
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            void conftest_mdev_uuid() {
                mdev_uuid();
            }"

            compile_check_conftest "$CODE" "NV_MDEV_UUID_PRESENT" "" "functions"

            #
            # Determine if mdev_uuid() returns 'const guid_t *'.
            #
            # mdev_uuid() function prototype updated to return 'const guid_t *'
            # by commit 278bca7f318e ("vfio-mdev: Switch to use new generic UUID
            # API") in v5.1 (2019-01-10).
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            const guid_t *conftest_mdev_uuid_return_guid_ptr(struct mdev_device *mdev) {
                return mdev_uuid(mdev);
            }"

            compile_check_conftest "$CODE" "NV_MDEV_UUID_RETURN_GUID_PTR" "" "types"
        ;;

        mdev_dev)
            #
            # Determine if mdev_dev() function is present or not
            #
            # Added by commit 99e3123e3d72 ("vfio-mdev: Make mdev_device
            # private and abstract interfaces") in v4.10
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            void conftest_mdev_dev() {
                mdev_dev();
            }"

            compile_check_conftest "$CODE" "NV_MDEV_DEV_PRESENT" "" "functions"
        ;;

        mdev_get_type_group_id)
            #
            # Determine if mdev_get_type_group_id() function is present or not
            #
            # Added by commit 15fcc44be0c7a ("vfio/mdev: Add
            # mdev/mtype_get_type_group_id()") in v5.13
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            void conftest_mdev_get_type_group_id() {
                mdev_get_type_group_id();
            }"

            compile_check_conftest "$CODE" "NV_MDEV_GET_TYPE_GROUP_ID_PRESENT" "" "functions"
        ;;

        mdev_parent)
            #
            # Determine if the struct mdev_parent type is present.
            #
            # Added by commit 42930553a7c1 ("vfio-mdev: de-polute the
            # namespace, rename parent_device & parent_ops") in v4.10
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            struct mdev_parent_ops conftest_mdev_parent;
            "

            compile_check_conftest "$CODE" "NV_MDEV_PARENT_OPS_STRUCT_PRESENT" "" "types"
        ;;

        mdev_parent_dev)
            #
            # Determine if mdev_parent_dev() function is present or not
            #
            # Added by commit 9372e6feaafb ("vfio-mdev: Make mdev_parent
            # private") in v4.10
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            void conftest_mdev_parent_dev() {
                mdev_parent_dev();
            }"

            compile_check_conftest "$CODE" "NV_MDEV_PARENT_DEV_PRESENT" "" "functions"
        ;;

        mdev_from_dev)
            #
            # Determine if mdev_from_dev() function is present or not.
            #
            # Added by commit 99e3123e3d72 ("vfio-mdev: Make mdev_device
            # private and abstract interfaces") in v4.10 (2016-12-30)
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            void conftest_mdev_from_dev() {
                mdev_from_dev();
            }"

            compile_check_conftest "$CODE" "NV_MDEV_FROM_DEV_PRESENT" "" "functions"
        ;;

        mdev_set_iommu_device)
            #
            # Determine if mdev_set_iommu_device() function is present or not.
            #
            # Added by commit 8ac13175cbe9 ("vfio/mdev: Add iommu related member
            # in mdev_device) in v5.1 (2019-04-12)
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            void conftest_mdev_set_iommu_device() {
                mdev_set_iommu_device();
            }"

            compile_check_conftest "$CODE" "NV_MDEV_SET_IOMMU_DEVICE_PRESENT" "" "functions"
        ;;

        mdev_parent_ops_has_open_device)
            # Determine if 'mdev_parent_ops' structure has a 'open_device'
            # field.
            #
            # Added by commit 2fd585f4ed9d ("vfio: Provide better generic support
            # for open/release vfio_device_ops") in 5.15 (2021-08-05)
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/mdev.h>
            int conftest_mdev_parent_ops_has_open_device(void) {
                return offsetof(struct mdev_parent_ops, open_device);
            }"

            compile_check_conftest "$CODE" "NV_MDEV_PARENT_OPS_HAS_OPEN_DEVICE" "" "types"
        ;;

        pci_irq_vector_helpers)
            #
            # Determine if pci_alloc_irq_vectors(), pci_free_irq_vectors()
            # functions are present or not.
            #
            # Added by commit aff171641d181ea573 (PCI: Provide sensible IRQ
            # vector alloc/free routines) (2016-07-12)
            #
            CODE="
            #include <linux/pci.h>
            #include <linux/msi.h>
            void conftest_pci_irq_vector_helpers() {
                pci_alloc_irq_vectors();
                pci_free_irq_vectors ();
            }"

            compile_check_conftest "$CODE" "NV_PCI_IRQ_VECTOR_HELPERS_PRESENT" "" "functions"
        ;;


        vfio_device_gfx_plane_info)
            #
            # determine if the 'struct vfio_device_gfx_plane_info' type is present.
            #
            # Added by commit e20eaa2382e7 ("vfio: ABI for mdev display
            # dma-buf operation") in v4.16 (2017-11-23)
            #
            CODE="
            #include <linux/vfio.h>
            struct vfio_device_gfx_plane_info info;"

            compile_check_conftest "$CODE" "NV_VFIO_DEVICE_GFX_PLANE_INFO_PRESENT" "" "types"
        ;;

        vfio_device_migration_has_start_pfn)
            #
            # Determine if the 'vfio_device_migration_info' structure has
            # a 'start_pfn' field.
            #
            # This member was present in proposed interface for vGPU Migration
            # ("[PATCH v3 0/5] Add migration support for VFIO device ")
            # https://lists.gnu.org/archive/html/qemu-devel/2019-02/msg05176.html
            # which is not present in upstreamed commit a8a24f3f6e38 (vfio: UAPI
            # for migration interface for device state) in v5.8 (2020-05-29)
            #
            CODE="
            #include <linux/vfio.h>
            int conftest_vfio_device_migration_has_start_pfn(void) {
                return offsetof(struct vfio_device_migration_info, start_pfn);
            }"

            compile_check_conftest "$CODE" "NV_VFIO_DEVICE_MIGRATION_HAS_START_PFN" "" "types"
        ;;

        drm_available)
            # Determine if the DRM subsystem is usable
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DRV_H_PRESENT)
            #include <drm/drm_drv.h>
            #endif

            #if !defined(CONFIG_DRM) && !defined(CONFIG_DRM_MODULE)
            #error DRM not enabled
            #endif

            void conftest_drm_available(void) {
                struct drm_driver drv;

                /* 2013-10-02 1bb72532ac260a2d3982b40bdd4c936d779d0d16 */
                (void)drm_dev_alloc;

                /* 2013-10-02 c22f0ace1926da399d9a16dfaf09174c1b03594c */
                (void)drm_dev_register;

                /* 2013-10-02 c3a49737ef7db0bdd4fcf6cf0b7140a883e32b2a */
                (void)drm_dev_unregister;
            }"

            compile_check_conftest "$CODE" "NV_DRM_AVAILABLE" "" "generic"
        ;;

        drm_dev_unref)
            #
            # Determine if drm_dev_unref() is present.
            # If it isn't, we use drm_dev_free() instead.
            #
            # drm_dev_free was added by commit 0dc8fe5985e0 ("drm: introduce
            # drm_dev_free() to fix error paths") in v3.13 (2013-10-02)
            #
            # Renamed to drm_dev_unref by commit 099d1c290e2e
            # ("drm: provide device-refcount") in v3.15 (2014-01-29)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif
            void conftest_drm_dev_unref(void) {
                drm_dev_unref();
            }"

            compile_check_conftest "$CODE" "NV_DRM_DEV_UNREF_PRESENT" "" "functions"
        ;;

        pde_data)
            #
            # Determine if the pde_data() function is present.
            #
            # The commit c28198889c15 removed the function
            # 'PDE_DATA()', and replaced it with 'pde_data()'
            # ("proc: remove PDE_DATA() completely") in v5.17-rc1.
            #
            CODE="
            #include <linux/proc_fs.h>
            void conftest_pde_data(void) {
                pde_data();
            }"

            compile_check_conftest "$CODE" "NV_PDE_DATA_LOWER_CASE_PRESENT" "" "functions"
        ;;

        get_num_physpages)
            #
            # Determine if the get_num_physpages() function is
            # present.
            #
            # Added by commit 7ee3d4e8cd56 ("mm: introduce helper function
            # mem_init_print_info() to simplify mem_init()") in v3.11
            #
            CODE="
            #include <linux/mm.h>
            void conftest_get_num_physpages(void) {
                get_num_physpages(NULL);
            }"

            compile_check_conftest "$CODE" "NV_GET_NUM_PHYSPAGES_PRESENT" "" "functions"
        ;;

        proc_remove)
            #
            # Determine if the proc_remove() function is present.
            #
            # Added by commit a8ca16ea7b0a ("proc: Supply a function to
            # remove a proc entry by PDE") in v3.10
            #
            CODE="
            #include <linux/proc_fs.h>
            void conftest_proc_remove(void) {
                proc_remove();
            }"

            compile_check_conftest "$CODE" "NV_PROC_REMOVE_PRESENT" "" "functions"
        ;;

        backing_dev_info)
            #
            # Determine if the 'address_space' structure has
            # a 'backing_dev_info' field.
            #
            # Removed by commit b83ae6d42143 ("fs: remove
            # mapping->backing_dev_info") in v4.0
            #
            CODE="
            #include <linux/fs.h>
            int conftest_backing_dev_info(void) {
                return offsetof(struct address_space, backing_dev_info);
            }"

            compile_check_conftest "$CODE" "NV_ADDRESS_SPACE_HAS_BACKING_DEV_INFO" "" "types"
        ;;

        address_space)
            #
            # Determine if the 'address_space' structure has
            # a 'tree_lock' field of type rwlock_t.
            #
            # 'tree_lock' was changed to spinlock_t by commit 19fd6231279b
            # ("mm: spinlock tree_lock") in v2.6.27
            #
            # It was removed altogether by commit b93b016313b3 ("page cache:
            # use xa_lock") in v4.17
            #
            CODE="
            #include <linux/fs.h>
            int conftest_address_space(void) {
                struct address_space as;
                rwlock_init(&as.tree_lock);
                return offsetof(struct address_space, tree_lock);
            }"

            compile_check_conftest "$CODE" "NV_ADDRESS_SPACE_HAS_RWLOCK_TREE_LOCK" "" "types"
        ;;

        address_space_init_once)
            #
            # Determine if address_space_init_once is present.
            #
            # Added by commit 2aa15890f3c1 ("mm: prevent concurrent
            # unmap_mapping_range() on the same inode") in v2.6.38
            #
            # If not present, it will be defined in uvm-linux.h.
            #
            CODE="
            #include <linux/fs.h>
            void conftest_address_space_init_once(void) {
                address_space_init_once();
            }"

            compile_check_conftest "$CODE" "NV_ADDRESS_SPACE_INIT_ONCE_PRESENT" "" "functions"
        ;;

        kuid_t)
            #
            # Determine if the 'kuid_t' type is present.
            #
            # Added by commit 7a4e7408c5ca ("userns: Add kuid_t and kgid_t
            # and associated infrastructure in uidgid.h") in v3.5
            #
            CODE="
            #include <linux/sched.h>
            kuid_t conftest_kuid_t;
            "

            compile_check_conftest "$CODE" "NV_KUID_T_PRESENT" "" "types"
        ;;

        pm_vt_switch_required)
            #
            # Determine if the pm_vt_switch_required() function is present.
            #
            # Added by commit f43f627d2f17 ("PM: make VT switching to the
            # suspend console optional v3") in v3.10
            #
            CODE="
            #include <linux/pm.h>
            void conftest_pm_vt_switch_required(void) {
                pm_vt_switch_required();
            }"

            compile_check_conftest "$CODE" "NV_PM_VT_SWITCH_REQUIRED_PRESENT" "" "functions"
        ;;

        xen_ioemu_inject_msi)
            # Determine if the xen_ioemu_inject_msi() function is present.
            CODE="
            #if defined(NV_XEN_IOEMU_H_PRESENT)
            #include <linux/kernel.h>
            #include <xen/interface/xen.h>
            #include <xen/hvm.h>
            #include <xen/ioemu.h>
            #endif
            void conftest_xen_ioemu_inject_msi(void) {
                xen_ioemu_inject_msi();
            }"

            compile_check_conftest "$CODE" "NV_XEN_IOEMU_INJECT_MSI" "" "functions"
        ;;

        phys_to_dma)
            #
            # Determine if the phys_to_dma function is present.
            # It does not exist on all architectures.
            #
            CODE="
            #include <linux/dma-mapping.h>
            void conftest_phys_to_dma(void) {
                phys_to_dma();
            }"

            compile_check_conftest "$CODE" "NV_PHYS_TO_DMA_PRESENT" "" "functions"
        ;;


        dma_attr_macros)
           #
           # Determine if the NV_DMA_ATTR_SKIP_CPU_SYNC_PRESENT macro present.
           # It does not exist on all architectures.
           #
           CODE="
           #include <linux/dma-mapping.h>
           void conftest_dma_attr_macros(void) {
               int ret;
               ret = DMA_ATTR_SKIP_CPU_SYNC();
           }"
           compile_check_conftest "$CODE" "NV_DMA_ATTR_SKIP_CPU_SYNC_PRESENT" "" "functions"
        ;;

       dma_map_page_attrs)
           #
           # Determine if the dma_map_page_attrs function is present.
           # It does not exist on all architectures.
           #
           CODE="
           #include <linux/dma-mapping.h>
           void conftest_dma_map_page_attrs(void) {
               dma_map_page_attrs();
           }"

           compile_check_conftest "$CODE" "NV_DMA_MAP_PAGE_ATTRS_PRESENT" "" "functions"
        ;;

        dma_ops)
            #
            # Determine if the 'dma_ops' structure is present.
            # It does not exist on all architectures.
            #
            CODE="
            #include <linux/dma-mapping.h>
            void conftest_dma_ops(void) {
                (void)dma_ops;
            }"

            compile_check_conftest "$CODE" "NV_DMA_OPS_PRESENT" "" "symbols"
        ;;

        swiotlb_dma_ops)
            #
            # Determine if the 'swiotlb_dma_ops' structure is present.
            # It does not exist on all architectures.
            #
            CODE="
            #include <linux/dma-mapping.h>
            void conftest_dma_ops(void) {
                (void)swiotlb_dma_ops;
            }"

            compile_check_conftest "$CODE" "NV_SWIOTLB_DMA_OPS_PRESENT" "" "symbols"
        ;;

        get_dma_ops)
            #
            # Determine if the get_dma_ops() function is present.
            #
            # The structure was made available to all architectures by commit
            # e1c7e324539a ("dma-mapping: always provide the dma_map_ops
            # based implementation") in v4.5
            #
            # Commit 0a0f0d8be76d ("dma-mapping: split <linux/dma-mapping.h>")
            # in v5.10-rc1 (2020-09-22), moved get_dma_ops() function
            # prototype from <linux/dma-mapping.h> to <linux/dma-map-ops.h>.
            #
            CODE="
            #if defined(NV_LINUX_DMA_MAP_OPS_H_PRESENT)
            #include <linux/dma-map-ops.h>
            #else
            #include <linux/dma-mapping.h>
            #endif
            void conftest_get_dma_ops(void) {
                get_dma_ops();
            }"

            compile_check_conftest "$CODE" "NV_GET_DMA_OPS_PRESENT" "" "functions"
        ;;

        noncoherent_swiotlb_dma_ops)
            #
            # Determine if the 'noncoherent_swiotlb_dma_ops' symbol is present.
            # This API only exists on ARM64.
            #
            # Added by commit 7363590d2c46 ("arm64: Implement coherent DMA API
            # based on swiotlb") in v3.15
            #
            # Removed by commit 9d3bfbb4df58 ("arm64: Combine coherent and
            # non-coherent swiotlb dma_ops") in v4.0
            #
            CODE="
            #include <linux/dma-mapping.h>
            void conftest_noncoherent_swiotlb_dma_ops(void) {
                (void)noncoherent_swiotlb_dma_ops;
            }"

            compile_check_conftest "$CODE" "NV_NONCOHERENT_SWIOTLB_DMA_OPS_PRESENT" "" "symbols"
        ;;

        dma_map_resource)
            #
            # Determine if the dma_map_resource() function is present.
            #
            # Added by commit 6f3d87968f9c ("dma-mapping: add
            # dma_{map,unmap}_resource") in v4.9 (2016-08-10)
            #
            CODE="
            #include <linux/dma-mapping.h>
            void conftest_dma_map_resource(void) {
                dma_map_resource();
            }"

            compile_check_conftest "$CODE" "NV_DMA_MAP_RESOURCE_PRESENT" "" "functions"
        ;;

        write_cr4)
            #
            # Determine if the write_cr4() function is present.
            #
            CODE="
            #include <asm/processor.h>
            void conftest_write_cr4(void) {
                write_cr4();
            }"

            compile_check_conftest "$CODE" "NV_WRITE_CR4_PRESENT" "" "functions"
        ;;

       nvhost_dma_fence_unpack)
           #
           # Determine if the nvhost_dma_fence_unpack function is present.
           # This is only present in NVIDIA Tegra downstream kernels.
           #
           CODE="
           #if defined(NV_LINUX_NVHOST_H_PRESENT)
           #include <linux/nvhost.h>
           #endif
           void conftest_nvhost_dma_fence_unpack(void) {
               nvhost_dma_fence_unpack();
           }"

           compile_check_conftest "$CODE" "NV_NVHOST_DMA_FENCE_UNPACK_PRESENT" "" "functions"
        ;;

        of_get_property)
            #
            # Determine if the of_get_property function is present.
            #
            # Support for kernels without CONFIG_OF defined added by commit
            # 89272b8c0d42 ("dt: add empty of_get_property for non-dt") in v3.1
            #
            # Test if linux/of.h header file inclusion is successful or not and
            # define/undefine NV_LINUX_OF_H_USABLE depending upon status of inclusion
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/of.h>
            " > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_LINUX_OF_H_USABLE" | append_conftest "generic"
                CODE="
                #include <linux/of.h>
                void conftest_of_get_property() {
                    of_get_property();
                }"

                compile_check_conftest "$CODE" "NV_OF_GET_PROPERTY_PRESENT" "" "functions"
            else
                echo "#undef NV_LINUX_OF_H_USABLE" | append_conftest "generic"
                echo "#undef NV_OF_GET_PROPERTY_PRESENT" | append_conftest "functions"
            fi
        ;;

        of_find_node_by_phandle)
            #
            # Determine if the of_find_node_by_phandle function is present.
            #
            # Support for kernels without CONFIG_OF defined added by commit
            # ce16b9d23561 ("of: define of_find_node_by_phandle for
            # !CONFIG_OF") in v4.2
            #
            # Test if linux/of.h header file inclusion is successful or not and
            # define/undefine NV_LINUX_OF_H_USABLE depending upon status of inclusion.
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/of.h>
            " > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_LINUX_OF_H_USABLE" | append_conftest "generic"
                CODE="
                #include <linux/of.h>
                void conftest_of_find_node_by_phandle() {
                    of_find_node_by_phandle();
                }"

                compile_check_conftest "$CODE" "NV_OF_FIND_NODE_BY_PHANDLE_PRESENT" "" "functions"
            else
                echo "#undef NV_LINUX_OF_H_USABLE" | append_conftest "generic"
                echo "#undef NV_OF_FIND_NODE_BY_PHANDLE_PRESENT" | append_conftest "functions"
            fi
        ;;

        of_node_to_nid)
            #
            # Determine if of_node_to_nid is present
            #
            # Dummy implementation added by commit 559e2b7ee7a1
            # ("of: Provide default of_node_to_nid() implementation.") in v2.6.36
            #
            # Real implementation added by commit 298535c00a2c
            # ("of, numa: Add NUMA of binding implementation.") in v4.7
            #
            # Test if linux/of.h header file inclusion is successful or not and
            # define/undefine NV_LINUX_OF_H_USABLE depending upon status of inclusion.
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/of.h>
            " > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_LINUX_OF_H_USABLE" | append_conftest "generic"
                CODE="
                #include <linux/version.h>
                #include <linux/utsname.h>
                #include <linux/of.h>
                void conftest_of_node_to_nid() {
                    of_node_to_nid();
                }"

                compile_check_conftest "$CODE" "NV_OF_NODE_TO_NID_PRESENT" "" "functions"
            else
                echo "#undef NV_LINUX_OF_H_USABLE" | append_conftest "generic"
                echo "#undef NV_OF_NODE_TO_NID_PRESENT" | append_conftest "functions"
            fi
        ;;

        pnv_pci_get_npu_dev)
            #
            # Determine if the pnv_pci_get_npu_dev function is present.
            #
            # Added by commit 5d2aa710e697 ("powerpc/powernv: Add support
            # for Nvlink NPUs") in v4.5
            #
            CODE="
            #include <linux/pci.h>
            void conftest_pnv_pci_get_npu_dev() {
                pnv_pci_get_npu_dev();
            }"

            compile_check_conftest "$CODE" "NV_PNV_PCI_GET_NPU_DEV_PRESENT" "" "functions"
        ;;

        kernel_write)
            #
            # Determine if the function kernel_write() is present.
            #
            # First exported by commit 7bb307e894d5 ("export kernel_write(),
            # convert open-coded instances") in v3.9
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/fs.h>
            void conftest_kernel_write(void) {
                kernel_write();
            }" > conftest$$.c;

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#undef NV_KERNEL_WRITE_PRESENT" | append_conftest "function"
                rm -f conftest$$.o
            else
                echo "#define NV_KERNEL_WRITE_PRESENT" | append_conftest "function"

                #
                # Determine the pos argument type, which was changed by
                # commit e13ec939e96b1 (fs: fix kernel_write prototype) on
                # 9/1/2017.
                #
                echo "$CONFTEST_PREAMBLE
                #include <linux/fs.h>
                ssize_t kernel_write(struct file *file, const void *buf,
                                     size_t count, loff_t *pos)
                {
                    return 0;
                }" > conftest$$.c;

                $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
                rm -f conftest$$.c

                if [ -f conftest$$.o ]; then
                    echo "#define NV_KERNEL_WRITE_HAS_POINTER_POS_ARG" | append_conftest "function"
                    rm -f conftest$$.o
                else
                    echo "#undef NV_KERNEL_WRITE_HAS_POINTER_POS_ARG" | append_conftest "function"
                fi
            fi
        ;;

        kernel_read_has_pointer_pos_arg)
            #
            # Determine the pos argument type, which was changed by
            # commit bdd1d2d3d251c (fs: fix kernel_read prototype) on
            # 9/1/2017.
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/fs.h>
            ssize_t kernel_read(struct file *file, void *buf, size_t count,
                                loff_t *pos)
            {
                return 0;
            }" > conftest$$.c;

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_KERNEL_READ_HAS_POINTER_POS_ARG" | append_conftest "function"
                rm -f conftest$$.o
            else
                echo "#undef NV_KERNEL_READ_HAS_POINTER_POS_ARG" | append_conftest "function"
            fi
        ;;

        vm_insert_pfn_prot)
            #
            # Determine if vm_insert_pfn_prot function is present
            #
            # Added by commit 1745cbc5d0de ("mm: Add vm_insert_pfn_prot()") in
            # v3.16.59
            #
            # Removed by commit f5e6d1d5f8f3 ("mm: introduce
            # vmf_insert_pfn_prot()") in v4.20.
            #
            CODE="
            #include <linux/mm.h>
            void conftest_vm_insert_pfn_prot() {
                vm_insert_pfn_prot();
            }"

            compile_check_conftest "$CODE" "NV_VM_INSERT_PFN_PROT_PRESENT" "" "functions"
        ;;

        vmf_insert_pfn_prot)
            #
            # Determine if vmf_insert_pfn_prot function is present
            #
            # Added by commit f5e6d1d5f8f3 ("mm: introduce
            # vmf_insert_pfn_prot()") in v4.20.
            #
            CODE="
            #include <linux/mm.h>
            void conftest_vmf_insert_pfn_prot() {
                vmf_insert_pfn_prot();
            }"

            compile_check_conftest "$CODE" "NV_VMF_INSERT_PFN_PROT_PRESENT" "" "functions"
        ;;

        drm_atomic_available)
            #
            # Determine if the DRM atomic modesetting subsystem is usable
            #
            # Added by commit 036ef5733ba4
            # ("drm/atomic: Allow drivers to subclass drm_atomic_state, v3") in
            # v4.2 (2018-05-18).
            #
            # Make conftest more robust by adding test for
            # drm_atomic_set_mode_prop_for_crtc(), this function added by
            # commit 955f3c334f0f ("drm/atomic: Add MODE_ID property") in v4.2
            # (2015-05-25). If the DRM atomic modesetting subsystem is
            # back ported to Linux kernel older than v4.2, then commit
            # 955f3c334f0f must be back ported in order to get NVIDIA-DRM KMS
            # support.
            # Commit 72fdb40c1a4b ("drm: extract drm_atomic_uapi.c") in v4.20
            # (2018-09-05), moved drm_atomic_set_mode_prop_for_crtc() function
            # prototype from drm/drm_atomic.h to drm/drm_atomic_uapi.h.
            #
            echo "$CONFTEST_PREAMBLE
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif
            #include <drm/drm_atomic.h>
            #if !defined(CONFIG_DRM) && !defined(CONFIG_DRM_MODULE)
            #error DRM not enabled
            #endif
            void conftest_drm_atomic_modeset_available(void) {
                size_t a;

                a = offsetof(struct drm_mode_config_funcs, atomic_state_alloc);
            }" > conftest$$.c;

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o

                echo "$CONFTEST_PREAMBLE
                #if defined(NV_DRM_DRMP_H_PRESENT)
                #include <drm/drmP.h>
                #endif
                #include <drm/drm_atomic.h>
                #if defined(NV_DRM_DRM_ATOMIC_UAPI_H_PRESENT)
                #include <drm/drm_atomic_uapi.h>
                #endif
                void conftest_drm_atomic_set_mode_prop_for_crtc(void) {
                    drm_atomic_set_mode_prop_for_crtc();
                }" > conftest$$.c;

                $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
                rm -f conftest$$.c

                if [ -f conftest$$.o ]; then
                    rm -f conftest$$.o
                    echo "#undef NV_DRM_ATOMIC_MODESET_AVAILABLE" | append_conftest "generic"
                else
                    echo "#define NV_DRM_ATOMIC_MODESET_AVAILABLE" | append_conftest "generic"
                fi
            else
                echo "#undef NV_DRM_ATOMIC_MODESET_AVAILABLE" | append_conftest "generic"
            fi
        ;;

        drm_bus_present)
            #
            # Determine if the 'struct drm_bus' type is present.
            #
            # Added by commit 8410ea3b95d1 ("drm: rework PCI/platform driver
            # interface.") in v2.6.39 (2010-12-15)
            #
            # Removed by commit c5786fe5f1c5 ("drm: Goody bye, drm_bus!")
            # in v3.18 (2014-08-29)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            void conftest_drm_bus_present(void) {
                struct drm_bus bus;
            }"

            compile_check_conftest "$CODE" "NV_DRM_BUS_PRESENT" "" "types"
        ;;

        drm_bus_has_bus_type)
            #
            # Determine if the 'drm_bus' structure has a 'bus_type' field.
            #
            # Added by commit 8410ea3b95d1 ("drm: rework PCI/platform driver
            # interface.") in v2.6.39 (2010-12-15)
            #
            # Removed by commit 42b21049fc26 ("drm: kill drm_bus->bus_type")
            # in v3.16 (2013-11-03)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            int conftest_drm_bus_has_bus_type(void) {
                return offsetof(struct drm_bus, bus_type);
            }"

            compile_check_conftest "$CODE" "NV_DRM_BUS_HAS_BUS_TYPE" "" "types"
        ;;

        drm_bus_has_get_irq)
            #
            # Determine if the 'drm_bus' structure has a 'get_irq' field.
            #
            # Added by commit 8410ea3b95d1 ("drm: rework PCI/platform
            # driver interface.") in v2.6.39 (2010-12-15)
            #
            # Removed by commit b2a21aa25a39 ("drm: remove bus->get_irq
            # implementations") in v3.16 (2013-11-03)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            int conftest_drm_bus_has_get_irq(void) {
                return offsetof(struct drm_bus, get_irq);
            }"

            compile_check_conftest "$CODE" "NV_DRM_BUS_HAS_GET_IRQ" "" "types"
        ;;

        drm_bus_has_get_name)
            #
            # Determine if the 'drm_bus' structure has a 'get_name' field.
            #
            # Added by commit 8410ea3b95d1 ("drm: rework PCI/platform driver
            # interface.") in v2.6.39 (2010-12-15)
            #
            # removed by commit 9de1b51f1fae ("drm: remove drm_bus->get_name")
            # in v3.16 (2013-11-03)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            int conftest_drm_bus_has_get_name(void) {
                return offsetof(struct drm_bus, get_name);
            }"

            compile_check_conftest "$CODE" "NV_DRM_BUS_HAS_GET_NAME" "" "types"
        ;;

        drm_driver_has_device_list)
            #
            # Determine if the 'drm_driver' structure has a 'device_list' field.
            #
            # Renamed from device_list to legacy_device_list by commit
            # b3f2333de8e8 ("drm: restrict the device list for shadow
            # attached drivers") in v3.14 (2013-12-11)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DRV_H_PRESENT)
            #include <drm/drm_drv.h>
            #endif

            int conftest_drm_driver_has_device_list(void) {
                return offsetof(struct drm_driver, device_list);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_HAS_DEVICE_LIST" "" "types"
        ;;


        drm_driver_has_legacy_dev_list)
            #
            # Determine if the 'drm_driver' structure has a 'legacy_dev_list' field.
            #
            # Renamed from device_list to legacy_device_list by commit
            # b3f2333de8e8 ("drm: restrict the device list for shadow
            # attached drivers") in v3.14 (2013-12-11)
            #
            # The commit 57bb1ee60340 ("drm: Compile out legacy chunks from
            # struct drm_device") compiles out the legacy chunks like
            # drm_driver::legacy_dev_list.
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DRV_H_PRESENT)
            #include <drm/drm_drv.h>
            #endif

            int conftest_drm_driver_has_legacy_dev_list(void) {
                return offsetof(struct drm_driver, legacy_dev_list);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_HAS_LEGACY_DEV_LIST" "" "types"
        ;;

        jiffies_to_timespec)
            #
            # Determine if jiffies_to_timespec() is present
            #
            # removed by commit 751addac78b6
            # ("y2038: remove obsolete jiffies conversion functions")
            # in v5.6-rc1 (2019-12-13).
        CODE="
        #include <linux/jiffies.h>
        void conftest_jiffies_to_timespec(void){
            jiffies_to_timespec();
        }"
            compile_check_conftest "$CODE" "NV_JIFFIES_TO_TIMESPEC_PRESENT" "" "functions"
        ;;

        drm_init_function_args)
            #
            # Determine if these functions:
            #   drm_universal_plane_init()
            #   drm_crtc_init_with_planes()
            #   drm_encoder_init()
            # have a 'name' argument, which was added by these commits:
            #   drm_universal_plane_init:   2015-12-09  b0b3b7951114315d65398c27648705ca1c322faa
            #   drm_crtc_init_with_planes:  2015-12-09  f98828769c8838f526703ef180b3088a714af2f9
            #   drm_encoder_init:           2015-12-09  13a3d91f17a5f7ed2acd275d18b6acfdb131fb15
            #
            # Additionally determine whether drm_universal_plane_init() has a
            # 'format_modifiers' argument, which was added by:
            #   2017-07-23  e6fc3b68558e4c6d8d160b5daf2511b99afa8814
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_CRTC_H_PRESENT)
            #include <drm/drm_crtc.h>
            #endif

            int conftest_drm_crtc_init_with_planes_has_name_arg(void) {
                return
                    drm_crtc_init_with_planes(
                            NULL,  /* struct drm_device *dev */
                            NULL,  /* struct drm_crtc *crtc */
                            NULL,  /* struct drm_plane *primary */
                            NULL,  /* struct drm_plane *cursor */
                            NULL,  /* const struct drm_crtc_funcs *funcs */
                            NULL);  /* const char *name */
            }"

            compile_check_conftest "$CODE" "NV_DRM_CRTC_INIT_WITH_PLANES_HAS_NAME_ARG" "" "types"

            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_ENCODER_H_PRESENT)
            #include <drm/drm_encoder.h>
            #endif

            int conftest_drm_encoder_init_has_name_arg(void) {
                return
                    drm_encoder_init(
                            NULL,  /* struct drm_device *dev */
                            NULL,  /* struct drm_encoder *encoder */
                            NULL,  /* const struct drm_encoder_funcs *funcs */
                            DRM_MODE_ENCODER_NONE, /* int encoder_type */
                            NULL); /* const char *name */
            }"

            compile_check_conftest "$CODE" "NV_DRM_ENCODER_INIT_HAS_NAME_ARG" "" "types"

            echo "$CONFTEST_PREAMBLE
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_PLANE_H_PRESENT)
            #include <drm/drm_plane.h>
            #endif

            int conftest_drm_universal_plane_init_has_format_modifiers_arg(void) {
                return
                    drm_universal_plane_init(
                            NULL,  /* struct drm_device *dev */
                            NULL,  /* struct drm_plane *plane */
                            0,     /* unsigned long possible_crtcs */
                            NULL,  /* const struct drm_plane_funcs *funcs */
                            NULL,  /* const uint32_t *formats */
                            0,     /* unsigned int format_count */
                            NULL,  /* const uint64_t *format_modifiers */
                            DRM_PLANE_TYPE_PRIMARY,
                            NULL);  /* const char *name */
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o

                echo "#define NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG" | append_conftest "types"
                echo "#define NV_DRM_UNIVERSAL_PLANE_INIT_HAS_NAME_ARG" | append_conftest "types"
            else
                echo "#undef NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG" | append_conftest "types"

                echo "$CONFTEST_PREAMBLE
                #if defined(NV_DRM_DRMP_H_PRESENT)
                #include <drm/drmP.h>
                #endif

                #if defined(NV_DRM_DRM_PLANE_H_PRESENT)
                #include <drm/drm_plane.h>
                #endif

                int conftest_drm_universal_plane_init_has_name_arg(void) {
                    return
                        drm_universal_plane_init(
                                NULL,  /* struct drm_device *dev */
                                NULL,  /* struct drm_plane *plane */
                                0,     /* unsigned long possible_crtcs */
                                NULL,  /* const struct drm_plane_funcs *funcs */
                                NULL,  /* const uint32_t *formats */
                                0,     /* unsigned int format_count */
                                DRM_PLANE_TYPE_PRIMARY,
                                NULL);  /* const char *name */
                }" > conftest$$.c

                $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1

                if [ -f conftest$$.o ]; then
                    rm -f conftest$$.o

                    echo "#define NV_DRM_UNIVERSAL_PLANE_INIT_HAS_NAME_ARG" | append_conftest "types"
                else
                    echo "#undef NV_DRM_UNIVERSAL_PLANE_INIT_HAS_NAME_ARG" | append_conftest "types"
                fi
            fi

        ;;

        vzalloc)
            #
            # Determine if the vzalloc function is present
            #
            # Added by commit e1ca7788dec6 ("mm: add vzalloc() and
            # vzalloc_node() helpers") in v2.6.37 (2010-10-26)
            #
            CODE="
            #include <linux/vmalloc.h>
            void conftest_vzalloc() {
                vzalloc();
            }"

            compile_check_conftest "$CODE" "NV_VZALLOC_PRESENT" "" "functions"
        ;;

        drm_driver_has_set_busid)
            #
            # Determine if the drm_driver structure has a 'set_busid' callback
            # field.
            #
            # Added by commit 915b4d11b8b9 ("drm: add driver->set_busid()
            # callback") in v3.18 (2014-08-29)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            int conftest_drm_driver_has_set_busid(void) {
                return offsetof(struct drm_driver, set_busid);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_HAS_SET_BUSID" "" "types"
        ;;

        drm_driver_has_gem_prime_res_obj)
            #
            # Determine if the drm_driver structure has a 'gem_prime_res_obj'
            # callback field.
            #
            # Added by commit 3aac4502fd3f ("dma-buf: use reservation
            # objects") in v3.17 (2014-07-01).
            #
            # Removed by commit 51c98747113e (drm/prime: Ditch
            # gem_prime_res_obj hook) in v5.4.
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            int conftest_drm_driver_has_gem_prime_res_obj(void) {
                return offsetof(struct drm_driver, gem_prime_res_obj);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_HAS_GEM_PRIME_RES_OBJ" "" "types"
        ;;

        drm_crtc_state_has_connectors_changed)
            #
            # Determine if the crtc_state has a 'connectors_changed' field.
            #
            # Added by commit fc596660dd4e ("drm/atomic: add
            # connectors_changed to separate it from mode_changed, v2")
            # in v4.3 (2015-07-21)
            #
            CODE="
            #include <drm/drm_crtc.h>
            void conftest_drm_crtc_state_has_connectors_changed(void) {
                struct drm_crtc_state foo;
                (void)foo.connectors_changed;
            }"

            compile_check_conftest "$CODE" "NV_DRM_CRTC_STATE_HAS_CONNECTORS_CHANGED" "" "types"
        ;;

        drm_reinit_primary_mode_group)
            #
            # Determine if the function drm_reinit_primary_mode_group() is
            # present.
            #
            # Added by commit 2390cd11bfbe ("drm/crtc: add interface to
            # reinitialise the legacy mode group") in v3.17 (2014-06-05)
            #
            # Removed by commit 3fdefa399e46 ("drm: gc now dead
            # mode_group code") in v4.3 (2015-07-09)
            #
            CODE="
            #if defined(NV_DRM_DRM_CRTC_H_PRESENT)
            #include <drm/drm_crtc.h>
            #endif
            void conftest_drm_reinit_primary_mode_group(void) {
                drm_reinit_primary_mode_group();
            }"

            compile_check_conftest "$CODE" "NV_DRM_REINIT_PRIMARY_MODE_GROUP_PRESENT" "" "functions"
        ;;

        wait_on_bit_lock_argument_count)
            #
            # Determine how many arguments wait_on_bit_lock takes.
            #
            # Changed by commit 743162013d40 ("sched: Remove proliferation
            # of wait_on_bit() action functions") in v3.17 (2014-07-07)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/wait.h>
            void conftest_wait_on_bit_lock(void) {
                wait_on_bit_lock(NULL, 0, 0);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_WAIT_ON_BIT_LOCK_ARGUMENT_COUNT 3" | append_conftest "functions"
                return
            fi

            echo "$CONFTEST_PREAMBLE
            #include <linux/wait.h>
            void conftest_wait_on_bit_lock(void) {
                wait_on_bit_lock(NULL, 0, NULL, 0);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_WAIT_ON_BIT_LOCK_ARGUMENT_COUNT 4" | append_conftest "functions"
                return
            fi
            echo "#error wait_on_bit_lock() conftest failed!" | append_conftest "functions"
        ;;

        bitmap_clear)
            #
            # Determine if the bitmap_clear function is present
            #
            # Added by commit c1a2a962a2ad ("bitmap: introduce bitmap_set,
            # bitmap_clear, bitmap_find_next_zero_area") in v2.6.33
            # (2009-12-15)
            #
            CODE="
            #include <linux/bitmap.h>
            void conftest_bitmap_clear() {
                bitmap_clear();
            }"

            compile_check_conftest "$CODE" "NV_BITMAP_CLEAR_PRESENT" "" "functions"
        ;;

        pci_stop_and_remove_bus_device)
            #
            # Determine if the pci_stop_and_remove_bus_device() function is present.
            #
            # Added by commit 210647af897a ("PCI: Rename pci_remove_bus_device
            # to pci_stop_and_remove_bus_device") in v3.4 (2012-02-25)
            #
            CODE="
            #include <linux/types.h>
            #include <linux/pci.h>
            void conftest_pci_stop_and_remove_bus_device() {
                pci_stop_and_remove_bus_device();
            }"

            compile_check_conftest "$CODE" "NV_PCI_STOP_AND_REMOVE_BUS_DEVICE_PRESENT" "" "functions"
        ;;

        pci_remove_bus_device)
            #
            # Determine if the pci_remove_bus_device() function is present.
            # Added before Linux-2.6.12-rc2 2005-04-16
            # Because we support builds on non-PCI platforms, we still need
            # to check for this function's presence.
            #
            CODE="
            #include <linux/types.h>
            #include <linux/pci.h>
            void conftest_pci_remove_bus_device() {
                pci_remove_bus_device();
            }"

            compile_check_conftest "$CODE" "NV_PCI_REMOVE_BUS_DEVICE_PRESENT" "" "functions"
        ;;

        drm_helper_mode_fill_fb_struct | drm_helper_mode_fill_fb_struct_has_const_mode_cmd_arg)
            #
            # Determine if the drm_helper_mode_fill_fb_struct function takes
            # 'dev' argument.
            #
            # The drm_helper_mode_fill_fb_struct() has been updated to
            # take 'dev' parameter by commit a3f913ca9892 ("drm: Pass 'dev'
            # to drm_helper_mode_fill_fb_struct()") in v4.11 (2016-12-14)
            #
            echo "$CONFTEST_PREAMBLE
            #include <drm/drm_crtc_helper.h>
            void drm_helper_mode_fill_fb_struct(struct drm_device *dev,
                                                struct drm_framebuffer *fb,
                                                const struct drm_mode_fb_cmd2 *mode_cmd)
            {
                return;
            }" > conftest$$.c;

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DRM_HELPER_MODE_FILL_FB_STRUCT_HAS_DEV_ARG" | append_conftest "function"
                echo "#define NV_DRM_HELPER_MODE_FILL_FB_STRUCT_HAS_CONST_MODE_CMD_ARG" | append_conftest "function"
                rm -f conftest$$.o
            else
                echo "#undef NV_DRM_HELPER_MODE_FILL_FB_STRUCT_HAS_DEV_ARG" | append_conftest "function"

                #
                # Determine if the drm_mode_fb_cmd2 pointer argument is const in
                # drm_mode_config_funcs::fb_create and drm_helper_mode_fill_fb_struct().
                #
                # The drm_mode_fb_cmd2 pointer through this call chain was made
                # const by commit 1eb83451ba55 ("drm: Pass the user drm_mode_fb_cmd2
                # as const to .fb_create()") in v4.5 (2015-11-11)
                #
                echo "$CONFTEST_PREAMBLE
                #include <drm/drm_crtc_helper.h>
                void drm_helper_mode_fill_fb_struct(struct drm_framebuffer *fb,
                                                    const struct drm_mode_fb_cmd2 *mode_cmd)
                {
                    return;
                }" > conftest$$.c;

                $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
                rm -f conftest$$.c

                if [ -f conftest$$.o ]; then
                    echo "#define NV_DRM_HELPER_MODE_FILL_FB_STRUCT_HAS_CONST_MODE_CMD_ARG" | append_conftest "function"
                    rm -f conftest$$.o
                else
                    echo "#undef NV_DRM_HELPER_MODE_FILL_FB_STRUCT_HAS_CONST_MODE_CMD_ARG" | append_conftest "function"
                fi
            fi
        ;;

        mm_context_t)
            #
            # Determine if the 'mm_context_t' data type is present
            # and if it has an 'id' member.
            # It does not exist on all architectures.
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/mm.h>
            int conftest_mm_context_t(void) {
                return offsetof(mm_context_t, id);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_MM_CONTEXT_T_HAS_ID" | append_conftest "types"
                rm -f conftest$$.o
                return
            else
                echo "#undef NV_MM_CONTEXT_T_HAS_ID" | append_conftest "types"
                return
            fi
        ;;

        pci_dev_has_ats_enabled)
            #
            # Determine if the 'pci_dev' data type has a 'ats_enabled' member.
            #
            # Added by commit d544d75ac96aa ("PCI: Embed ATS info directly
            # into struct pci_dev") in v4.3-rc1 (2015-08-14)
            #
            CODE="
            #include <linux/pci.h>
            int conftest_pci_dev_ats_enabled_t(void) {
                return ((struct pci_dev *)0)->ats_enabled;
            }"

            compile_check_conftest "$CODE" "NV_PCI_DEV_HAS_ATS_ENABLED" "" "types"
        ;;

        mt_device_gre)
            #
            # Determine if MT_DEVICE_GRE flag is present.
            #
            # MT_DEVICE_GRE flag is removed by commit 58cc6b72a21274
            # ("arm64: mm: Remove unused support for Device-GRE memory type") in v5.14-rc1
            # (2021-06-01).
            #
            CODE="
            #include <asm/memory.h>
            unsigned int conftest_mt_device_gre(void) {
                return MT_DEVICE_GRE;
            }"

            compile_check_conftest "$CODE" "NV_MT_DEVICE_GRE_PRESENT" "" "types"
        ;;

        get_user_pages)
            #
            # Conftest for get_user_pages()
            #
            # Use long type for get_user_pages and unsigned long for nr_pages
            # by commit 28a35716d317 ("mm: use long type for page counts
            # in mm_populate() and get_user_pages()") in v3.9 (2013-02-22)
            #
            # Removed struct task_struct *tsk & struct mm_struct *mm from
            # get_user_pages by commit cde70140fed8 ("mm/gup: Overload
            # get_user_pages() functions") in v4.6 (2016-02-12)
            #
            # Replaced get_user_pages6 with get_user_pages by commit
            # c12d2da56d0e ("mm/gup: Remove the macro overload API migration
            # helpers from the get_user*() APIs") in v4.6 (2016-04-04)
            #
            # Replaced write and force parameters with gup_flags by
            # commit 768ae309a961 ("mm: replace get_user_pages() write/force
            # parameters with gup_flags") in v4.9 (2016-10-13)
            #
            # linux-4.4.168 cherry-picked commit 768ae309a961 without
            # c12d2da56d0e which is covered in Conftest #3.
            #
            # Conftest #1: Check if get_user_pages accepts 6 arguments.
            # Return if true.
            # Fall through to conftest #2 on failure.
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/mm.h>
            long get_user_pages(unsigned long start,
                                unsigned long nr_pages,
                                int write,
                                int force,
                                struct page **pages,
                                struct vm_area_struct **vmas) {
                return 0;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c
            if [ -f conftest$$.o ]; then
                echo "#define NV_GET_USER_PAGES_HAS_WRITE_AND_FORCE_ARGS" | append_conftest "functions"
                echo "#undef NV_GET_USER_PAGES_HAS_TASK_STRUCT" | append_conftest "functions"
                rm -f conftest$$.o
                return
            fi

            # Conftest #2: Check if get_user_pages has gup_flags instead of
            # write and force parameters. And that gup doesn't accept a
            # task_struct and mm_struct as its first arguments.
            # Return if available.
            # Fall through to conftest #3 on failure.

            echo "$CONFTEST_PREAMBLE
            #include <linux/mm.h>
            long get_user_pages(unsigned long start,
                                unsigned long nr_pages,
                                unsigned int gup_flags,
                                struct page **pages,
                                struct vm_area_struct **vmas) {
                return 0;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#undef NV_GET_USER_PAGES_HAS_WRITE_AND_FORCE_ARGS" | append_conftest "functions"
                echo "#undef NV_GET_USER_PAGES_HAS_TASK_STRUCT" | append_conftest "functions"
                rm -f conftest$$.o
                return
            fi

            # Conftest #3: Check if get_user_pages has gup_flags instead of
            # write and force parameters AND that gup has task_struct and
            # mm_struct as its first arguments.
            # Return if available.
            # Fall through to default case if absent.

            echo "$CONFTEST_PREAMBLE
            #include <linux/mm.h>
            long get_user_pages(struct task_struct *tsk,
                                struct mm_struct *mm,
                                unsigned long start,
                                unsigned long nr_pages,
                                unsigned int gup_flags,
                                struct page **pages,
                                struct vm_area_struct **vmas) {
                return 0;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#undef NV_GET_USER_PAGES_HAS_WRITE_AND_FORCE_ARGS" | append_conftest "functions"
                echo "#define NV_GET_USER_PAGES_HAS_TASK_STRUCT" | append_conftest "functions"
                rm -f conftest$$.o
                return
            fi

            echo "#define NV_GET_USER_PAGES_HAS_WRITE_AND_FORCE_ARGS" | append_conftest "functions"
            echo "#define NV_GET_USER_PAGES_HAS_TASK_STRUCT" | append_conftest "functions"

            return
        ;;

        get_user_pages_remote)
            #
            # Determine if the function get_user_pages_remote() is
            # present and has write/force/locked/tsk parameters.
            #
            # get_user_pages_remote() was added by commit 1e9877902dc7
            # ("mm/gup: Introduce get_user_pages_remote()") in v4.6 (2016-02-12)
            #
            # get_user_pages[_remote]() write/force parameters
            # replaced with gup_flags by commits 768ae309a961 ("mm: replace
            # get_user_pages() write/force parameters with gup_flags") and
            # commit 9beae1ea8930 ("mm: replace get_user_pages_remote()
            # write/force parameters with gup_flags") in v4.9 (2016-10-13)
            #
            # get_user_pages_remote() added 'locked' parameter by
            # commit 5b56d49fc31d ("mm: add locked parameter to
            # get_user_pages_remote()") in v4.10 (2016-12-14)
            #
            # get_user_pages_remote() removed 'tsk' parameter by
            # commit 64019a2e467a ("mm/gup: remove task_struct pointer for
            # all gup code") in v5.9-rc1 (2020-08-11).
            #
            # conftest #1: check if get_user_pages_remote() is available
            # return if not available.
            # Fall through to conftest #2 if it is present
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/mm.h>
            void conftest_get_user_pages_remote(void) {
                get_user_pages_remote();
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#undef NV_GET_USER_PAGES_REMOTE_PRESENT" | append_conftest "functions"
                echo "#undef NV_GET_USER_PAGES_REMOTE_HAS_TSK_ARG" | append_conftest "functions"
                echo "#undef NV_GET_USER_PAGES_REMOTE_HAS_WRITE_AND_FORCE_ARGS" | append_conftest "functions"
                echo "#undef NV_GET_USER_PAGES_REMOTE_HAS_LOCKED_ARG" | append_conftest "functions"
                rm -f conftest$$.o
                return
            fi

            #
            # conftest #2: check if get_user_pages_remote() has write and
            # force arguments. Return if these arguments are present
            # Fall through to conftest #3 if these args are absent.
            #
            echo "#define NV_GET_USER_PAGES_REMOTE_PRESENT" | append_conftest "functions"
            echo "$CONFTEST_PREAMBLE
            #include <linux/mm.h>
            long get_user_pages_remote(struct task_struct *tsk,
                                       struct mm_struct *mm,
                                       unsigned long start,
                                       unsigned long nr_pages,
                                       int write,
                                       int force,
                                       struct page **pages,
                                       struct vm_area_struct **vmas) {
                return 0;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_GET_USER_PAGES_REMOTE_HAS_TSK_ARG" | append_conftest "functions"
                echo "#define NV_GET_USER_PAGES_REMOTE_HAS_WRITE_AND_FORCE_ARGS" | append_conftest "functions"
                echo "#undef NV_GET_USER_PAGES_REMOTE_HAS_LOCKED_ARG" | append_conftest "functions"
                rm -f conftest$$.o
                return
            fi

            echo "#undef NV_GET_USER_PAGES_REMOTE_HAS_WRITE_AND_FORCE_ARGS" | append_conftest "functions"

            #
            # conftest #3: check if get_user_pages_remote() has locked argument
            # Return if these arguments are present. Fall through to conftest #4
            # if these args are absent.
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/mm.h>
            long get_user_pages_remote(struct task_struct *tsk,
                                       struct mm_struct *mm,
                                       unsigned long start,
                                       unsigned long nr_pages,
                                       unsigned int gup_flags,
                                       struct page **pages,
                                       struct vm_area_struct **vmas,
                                       int *locked) {
                return 0;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_GET_USER_PAGES_REMOTE_HAS_TSK_ARG" | append_conftest "functions"
                echo "#define NV_GET_USER_PAGES_REMOTE_HAS_LOCKED_ARG" | append_conftest "functions"
                rm -f conftest$$.o
                return
            fi

            #
            # conftest #4: check if get_user_pages_remote() does not take
            # tsk argument.
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/mm.h>
            long get_user_pages_remote(struct mm_struct *mm,
                                       unsigned long start,
                                       unsigned long nr_pages,
                                       unsigned int gup_flags,
                                       struct page **pages,
                                       struct vm_area_struct **vmas,
                                       int *locked) {
                return 0;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#undef NV_GET_USER_PAGES_REMOTE_HAS_TSK_ARG" | append_conftest "functions"
                echo "#define NV_GET_USER_PAGES_REMOTE_HAS_LOCKED_ARG" | append_conftest "functions"
                rm -f conftest$$.o
            else

                echo "#define NV_GET_USER_PAGES_REMOTE_HAS_TSK_ARG" | append_conftest "functions"
                echo "#undef NV_GET_USER_PAGES_REMOTE_HAS_LOCKED_ARG" | append_conftest "functions"
            fi
        ;;

        usleep_range)
            #
            # Determine if the function usleep_range() is present.
            #
            # Added by commit 5e7f5a178bba ("timer: Added usleep_range timer")
            # in v2.6.36 (2010-08-04)
            #
            CODE="
            #include <linux/delay.h>
            void conftest_usleep_range(void) {
                usleep_range();
            }"

            compile_check_conftest "$CODE" "NV_USLEEP_RANGE_PRESENT" "" "functions"
        ;;

        radix_tree_empty)
            #
            # Determine if the function radix_tree_empty() is present.
            #
            # Added by commit e9256efcc8e3 ("radix-tree: introduce
            # radix_tree_empty") in v4.7 (2016-05-20)
            #
            CODE="
            #include <linux/radix-tree.h>
            int conftest_radix_tree_empty(void) {
                radix_tree_empty();
            }"

            compile_check_conftest "$CODE" "NV_RADIX_TREE_EMPTY_PRESENT" "" "functions"
        ;;

        drm_gem_object_lookup)
            #
            # Determine the number of arguments of drm_gem_object_lookup().
            #
            # First argument of type drm_device removed by commit
            # a8ad0bd84f98 ("drm: Remove unused drm_device from
            # drm_gem_object_lookup()") in v4.7 (2016-05-09)
            #
            echo "$CONFTEST_PREAMBLE
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif
            #if defined(NV_DRM_DRM_GEM_H_PRESENT)
            #include <drm/drm_gem.h>
            #endif
            void conftest_drm_gem_object_lookup(void) {
                drm_gem_object_lookup(NULL, NULL, 0);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DRM_GEM_OBJECT_LOOKUP_ARGUMENT_COUNT 3" | append_conftest "functions"
                rm -f conftest$$.o
                return
            else
                echo "#define NV_DRM_GEM_OBJECT_LOOKUP_ARGUMENT_COUNT 2" | append_conftest "functions"
            fi
        ;;

        drm_master_drop_has_from_release_arg)
            #
            # Determine if drm_driver::master_drop() has 'from_release' argument.
            #
            # Last argument 'bool from_release' has been removed by commit
            # d6ed682eba54 ("drm: Refactor drop/set master code a bit")
            # in v4.8 (2016-06-21)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            void conftest_drm_master_drop_has_from_release_arg(struct drm_driver *drv) {
                drv->master_drop(NULL, NULL, false);
            }"

            compile_check_conftest "$CODE" "NV_DRM_MASTER_DROP_HAS_FROM_RELEASE_ARG" "" "types"
        ;;

        drm_atomic_state_ref_counting)
            #
            # Determine if functions drm_atomic_state_get/put() are
            # present.
            #
            # Added by commit 0853695c3ba4 ("drm: Add reference counting to
            # drm_atomic_state") in v4.10 (2016-10-14)
            #
            CODE="
            #if defined(NV_DRM_DRM_ATOMIC_H_PRESENT)
            #include <drm/drm_atomic.h>
            #endif
            void conftest_drm_atomic_state_get(void) {
                drm_atomic_state_get();
            }"

            compile_check_conftest "$CODE" "NV_DRM_ATOMIC_STATE_REF_COUNTING_PRESENT" "" "functions"
        ;;

        vm_ops_fault_removed_vma_arg)
            #
            # Determine if vma.vm_ops.fault takes (vma, vmf), or just (vmf)
            # args. Acronym key:
            #   vma: struct vm_area_struct
            #   vm_ops: struct vm_operations_struct
            #   vmf: struct vm_fault
            #
            # The redundant vma arg was removed from BOTH vma.vm_ops.fault and
            # vma.vm_ops.page_mkwrite by commit 11bac8000449 ("mm, fs: reduce
            # fault, page_mkwrite, and pfn_mkwrite to take only vmf") in
            # v4.11 (2017-02-24)
            #
            CODE="
            #include <linux/mm.h>
            void conftest_vm_ops_fault_removed_vma_arg(void) {
                struct vm_operations_struct vm_ops;
                struct vm_fault *vmf;
                (void)vm_ops.fault(vmf);
            }"

            compile_check_conftest "$CODE" "NV_VM_OPS_FAULT_REMOVED_VMA_ARG" "" "types"
        ;;

        pnv_npu2_init_context)
            #
            # Determine if the pnv_npu2_init_context() function is
            # present and the signature of its callback.
            #
            # Added by commit 1ab66d1fbada ("powerpc/powernv: Introduce
            # address translation services for Nvlink2") in v4.12
            # (2017-04-03).
            #
            echo "$CONFTEST_PREAMBLE
            #if defined(NV_ASM_POWERNV_H_PRESENT)
            #include <linux/pci.h>
            #include <asm/powernv.h>
            #endif
            void conftest_pnv_npu2_init_context(void) {
                pnv_npu2_init_context();
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c
            if [ -f conftest$$.o ]; then
                echo "#undef NV_PNV_NPU2_INIT_CONTEXT_PRESENT" | append_conftest "functions"
                echo "#undef NV_PNV_NPU2_INIT_CONTEXT_CALLBACK_RETURNS_VOID" | append_conftest "functions"
                rm -f conftest$$.o
                return
            fi

            echo "#define NV_PNV_NPU2_INIT_CONTEXT_PRESENT" | append_conftest "functions"

            # Check the callback signature
            echo "$CONFTEST_PREAMBLE
            #if defined(NV_ASM_POWERNV_H_PRESENT)
            #include <linux/pci.h>
            #include <asm/powernv.h>
            #endif

            struct npu_context *pnv_npu2_init_context(struct pci_dev *gpdev,
                unsigned long flags,
                void (*cb)(struct npu_context *, void *),
                void *priv) {
                return NULL;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c
            if [ -f conftest$$.o ]; then
                echo "#define NV_PNV_NPU2_INIT_CONTEXT_CALLBACK_RETURNS_VOID" | append_conftest "functions"
                rm -f conftest$$.o
                return
            fi

            echo "#undef NV_PNV_NPU2_INIT_CONTEXT_CALLBACK_RETURNS_VOID" | append_conftest "functions"
        ;;

        of_get_ibm_chip_id)
            #
            # Determine if the of_get_ibm_chip_id() function is present.
            #
            # Added by commit b130e7c04f11 ("powerpc: export
            # of_get_ibm_chip_id function") in v4.2 (2015-05-07)
            #
            CODE="
            #include <linux/version.h>
            #if defined(NV_ASM_PROM_H_PRESENT)
            #include <asm/prom.h>
            #endif
            void conftest_of_get_ibm_chip_id(void) {
                #if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 2, 0)
                of_get_ibm_chip_id();
                #endif
            }"

            compile_check_conftest "$CODE" "NV_OF_GET_IBM_CHIP_ID_PRESENT" "" "functions"
        ;;

        drm_driver_unload_has_int_return_type)
            #
            # Determine if drm_driver::unload() returns integer value
            #
            # Changed to void by commit 11b3c20bdd15 ("drm: Change the return
            # type of the unload hook to void") in v4.11 (2017-01-06)
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            int conftest_drm_driver_unload_has_int_return_type(struct drm_driver *drv) {
                return drv->unload(NULL /* dev */);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_UNLOAD_HAS_INT_RETURN_TYPE" "" "types"
        ;;

        is_export_symbol_present_*)
            export_symbol_present_conftest $(echo $1 | cut -f5- -d_)
        ;;

        is_export_symbol_gpl_*)
            export_symbol_gpl_conftest $(echo $1 | cut -f5- -d_)
        ;;

        drm_atomic_helper_crtc_destroy_state_has_crtc_arg)
            #
            # Determine if __drm_atomic_helper_crtc_destroy_state() has 'crtc'
            # argument.
            #
            # 'crtc' argument removed by commit ec2dc6a0fe38 ("drm: Drop crtc
            # argument from __drm_atomic_helper_crtc_destroy_state") in v4.7
            # (2016-05-09)
            #
            CODE="
            #if defined(NV_DRM_DRM_ATOMIC_HELPER_H_PRESENT)
            #include <drm/drm_atomic_helper.h>
            #endif
            void conftest_drm_atomic_helper_crtc_destroy_state_has_crtc_arg(void) {
                __drm_atomic_helper_crtc_destroy_state(NULL, NULL);
            }"

            compile_check_conftest "$CODE" "NV_DRM_ATOMIC_HELPER_CRTC_DESTROY_STATE_HAS_CRTC_ARG" "" "types"
        ;;

        drm_atomic_helper_plane_destroy_state_has_plane_arg)
            #
            # Determine if __drm_atomic_helper_plane_destroy_state has
            # 'plane' argument.
            #
            # 'plane' argument removed by commit 2f701695fd3a (drm: Drop plane
            # argument from __drm_atomic_helper_plane_destroy_state") in v4.7
            # (2016-05-09)
            #
            CODE="
            #if defined(NV_DRM_DRM_ATOMIC_HELPER_H_PRESENT)
            #include <drm/drm_atomic_helper.h>
            #endif
            void conftest_drm_atomic_helper_plane_destroy_state_has_plane_arg(void) {
                __drm_atomic_helper_plane_destroy_state(NULL, NULL);
            }"

            compile_check_conftest "$CODE" "NV_DRM_ATOMIC_HELPER_PLANE_DESTROY_STATE_HAS_PLANE_ARG" "" "types"
        ;;

        drm_atomic_helper_connector_dpms)
            #
            # Determine if the function drm_atomic_helper_connector_dpms() is present.
            #
            # Removed by commit 7d902c05b480 ("drm: Nuke
            # drm_atomic_helper_connector_dpms") in v4.14 (2017-07-25)
            #
            CODE="
            #if defined(NV_DRM_DRM_ATOMIC_HELPER_H_PRESENT)
            #include <drm/drm_atomic_helper.h>
            #endif
            void conftest_drm_atomic_helper_connector_dpms(void) {
                drm_atomic_helper_connector_dpms();
            }"

            compile_check_conftest "$CODE" "NV_DRM_ATOMIC_HELPER_CONNECTOR_DPMS_PRESENT" "" "functions"
        ;;

        get_backlight_device_by_name)
            #
            # Determine if the get_backlight_device_by_name() function is present
            #
            CODE="
            #include <linux/backlight.h>
            int conftest_get_backlight_device_by_name(void) {
                return get_backlight_device_by_name();
            }"
            compile_check_conftest "$CODE" "NV_GET_BACKLIGHT_DEVICE_BY_NAME_PRESENT" "" "functions"
        ;;

        timer_setup)
            #
            # Determine if the function timer_setup() is present.
            #
            # Added by commit 686fef928bba ("timer: Prepare to change timer
            # callback argument type") in v4.14 (2017-09-28)
            #
            CODE="
            #include <linux/timer.h>
            int conftest_timer_setup(void) {
                return timer_setup();
            }"
            compile_check_conftest "$CODE" "NV_TIMER_SETUP_PRESENT" "" "functions"
        ;;

        radix_tree_replace_slot)
            #
            # Determine if the radix_tree_replace_slot() function is
            # present and how many arguments it takes.
            #
            # root parameter added to radix_tree_replace_slot (but the symbol
            # was not exported) by commit 6d75f366b924 ("lib: radix-tree:
            # check accounting of existing slot replacement users") in v4.10
            # (2016-12-12)
            #
            # radix_tree_replace_slot symbol export added by commit
            # 10257d719686 ("EXPORT_SYMBOL radix_tree_replace_slot") in v4.11
            # (2017-01-11)
            #
            CODE="
            #include <linux/radix-tree.h>
            #include <linux/version.h>
            void conftest_radix_tree_replace_slot(void) {
            #if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0)) || (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0))
                radix_tree_replace_slot();
            #endif
            }"
            compile_check_conftest "$CODE" "NV_RADIX_TREE_REPLACE_SLOT_PRESENT" "" "functions"

            echo "$CONFTEST_PREAMBLE
            #include <linux/radix-tree.h>
            void conftest_radix_tree_replace_slot(void) {
                radix_tree_replace_slot(NULL, NULL);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_RADIX_TREE_REPLACE_SLOT_ARGUMENT_COUNT 2" | append_conftest "functions"
                return
            fi

            echo "$CONFTEST_PREAMBLE
            #include <linux/radix-tree.h>
            void conftest_radix_tree_replace_slot(void) {
                radix_tree_replace_slot(NULL, NULL, NULL);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_RADIX_TREE_REPLACE_SLOT_ARGUMENT_COUNT 3" | append_conftest "functions"
                return
            else
                echo "#error radix_tree_replace_slot() conftest failed!" | append_conftest "functions"
            fi
        ;;

        kthread_create_on_node)
            #
            # Determine if kthread_create_on_node is available
            #
            # kthread_create_on_node was added in by commit 207205a2ba26
            # ("kthread: NUMA aware kthread_create_on_node()") in v2.6.39
            # (2011-03-22).
            #
            CODE="
            #include <linux/kthread.h>
            void kthread_create_on_node_conftest(void) {
                 (void)kthread_create_on_node();
            }"

            compile_check_conftest "$CODE" "NV_KTHREAD_CREATE_ON_NODE_PRESENT" "" "functions"
        ;;

        cpumask_of_node)
            #
            # Determine whether cpumask_of_node is available.
            #
            # ARM support for cpumask_of_node() lagged until commit 1a2db300348b
            # ("arm64, numa: Add NUMA support for arm64 platforms.") in v4.7
            # (2016-04-08)
            #
            CODE="
            #include    <asm/topology.h>
            void conftest_cpumask_of_node(void) {
            (void)cpumask_of_node();
            }"

            compile_check_conftest "$CODE" "NV_CPUMASK_OF_NODE_PRESENT" "" "functions"
        ;;

        drm_mode_object_find_has_file_priv_arg)
            #
            # Determine if drm_mode_object_find() has 'file_priv' arguments.
            #
            # Updated to take 'file_priv' argument by commit 418da17214ac
            # ("drm: Pass struct drm_file * to __drm_mode_object_find [v2]")
            # in v4.15 (2017-03-14)
            #
            CODE="
            #include <drm/drm_mode_object.h>
            void conftest_drm_mode_object_find_has_file_priv_arg(
                    struct drm_device *dev,
                    struct drm_file *file_priv,
                    uint32_t id,
                    uint32_t type) {
                (void)drm_mode_object_find(dev, file_priv, id, type);
            }"

            compile_check_conftest "$CODE" "NV_DRM_MODE_OBJECT_FIND_HAS_FILE_PRIV_ARG" | append_conftest "types"
        ;;

        pci_enable_msix_range)
            #
            # Determine if the pci_enable_msix_range() function is present.
            #
            # Added by commit 302a2523c277 ("PCI/MSI: Add
            # pci_enable_msi_range() and pci_enable_msix_range()") in v3.14
            # (2013-12-30)
            #
            CODE="
            #include <linux/pci.h>
            void conftest_pci_enable_msix_range(void) {
                pci_enable_msix_range();
            }"

            compile_check_conftest "$CODE" "NV_PCI_ENABLE_MSIX_RANGE_PRESENT" "" "functions"
        ;;

        dma_buf_owner)
            #
            # Determine if the dma_buf struct has an owner member.
            #
            # Added by commit 9abdffe286c1 ("dma-buf: add ref counting for
            # module as exporter") in v4.2 (2015-05-05)
            #
            CODE="
            #include <linux/dma-buf.h>
            int conftest_dma_buf_owner(void) {
                return offsetof(struct dma_buf, owner);
            }"

            compile_check_conftest "$CODE" "NV_DMA_BUF_OWNER_PRESENT" "" "types"
        ;;

        dma_buf_export_args)
            #
            # Determine argument count for dma_buf_export().
            #
            # 4 arguments added by commit d15bd7ee445d
            # ("dma-buf: Introduce dma buffer sharing mechanism")
            # in v3.3 (2011-12-26)
            #
            # Additional argument added by commit 3aac4502fd3f
            # ("dma-buf: use reservation objects") in v3.17 (2014-07-01).
            #
            # Parameters wrapped in a single struct dma_buf_export_info by commit:
            # d8fbe341beb6("dma-buf: cleanup dma_buf_export() to make it easily extensible")
            # in v4.1 (2015-01-23).
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            struct dma_buf* conftest_dma_buf_export(void) {
                return dma_buf_export(NULL);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_DMA_BUF_EXPORT_ARGUMENT_COUNT 1" | append_conftest "functions"
                return
            fi

            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            struct dma_buf* conftest_dma_buf_export(void) {
                return dma_buf_export(NULL, NULL, 0, 0);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_DMA_BUF_EXPORT_ARGUMENT_COUNT 4" | append_conftest "functions"
                return
            fi

            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            struct dma_buf* conftest_dma_buf_export(void) {
                return dma_buf_export(NULL, NULL, 0, 0, NULL);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_DMA_BUF_EXPORT_ARGUMENT_COUNT 5" | append_conftest "functions"
                return
            fi
            echo "#error dma_buf_export() conftest failed!" | append_conftest "functions"
        ;;

        dma_buf_ops_has_kmap)
            #
            # Determine if .kmap exists in dma_buf_ops.
            # In some kernels, this is a mandatory callback.
            #
            # Added by commit fc13020e086b
            # ("dma-buf: add support for kernel cpu access") in v3.4 (2012-03-20)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            int conftest_dma_buf_ops_has_kmap(void) {
                return offsetof(struct dma_buf_ops, kmap);
            }
            int conftest_dma_buf_ops_has_kunmap(void) {
                return offsetof(struct dma_buf_ops, kunmap);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DMA_BUF_OPS_HAS_KMAP" | append_conftest "types"
                rm -f conftest$$.o
                return
            else
                echo "#undef NV_DMA_BUF_OPS_HAS_KMAP" | append_conftest "types"
                return
            fi
        ;;

        dma_buf_ops_has_kmap_atomic)
            #
            # Determine if .kmap_atomic exists in dma_buf_ops.
            # In some kernels, this is a mandatory callback.
            #
            # Added by commit fc13020e086b
            # ("dma-buf: add support for kernel cpu access")in v3.4 (2012-03-20)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            int conftest_dma_buf_ops_has_kmap_atomic(void) {
                return offsetof(struct dma_buf_ops, kmap_atomic);
            }
            int conftest_dma_buf_ops_has_kunmap_atomic(void) {
                return offsetof(struct dma_buf_ops, kunmap_atomic);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DMA_BUF_OPS_HAS_KMAP_ATOMIC" | append_conftest "types"
                rm -f conftest$$.o
                return
            else
                echo "#undef NV_DMA_BUF_OPS_HAS_KMAP_ATOMIC" | append_conftest "types"
                return
            fi
        ;;

        dma_buf_ops_has_map)
            #
            # Determine if .map exists in dma_buf_ops.
            # In some kernels, this is a mandatory callback.
            #
            # Added by commit f9b67f0014cb
            # ("dma-buf: Rename dma-ops to prevent conflict with kunmap_atomic macro")
            # in v4.12 (2017-04-19)
            #
            # Removed as a mandatory callback by commit f82aab2d521e
            # ("dma-buf: Remove requirement for ops->map() from dma_buf_export")
            # in v4.20 (2018-08-07)
            #
            # Completely removed from dma-buf by commit 4337ebbbbda3
            # ("dma-buf: Remove kernel map/unmap hooks") in v5.6 (2019-11-18)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            int conftest_dma_buf_ops_has_map(void) {
                return offsetof(struct dma_buf_ops, map);
            }
            int conftest_dma_buf_ops_has_unmap(void) {
                return offsetof(struct dma_buf_ops, unmap);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DMA_BUF_OPS_HAS_MAP" | append_conftest "types"
                rm -f conftest$$.o
                return
            else
                echo "#undef NV_DMA_BUF_OPS_HAS_MAP" | append_conftest "types"
                return
            fi
        ;;

        dma_buf_ops_has_map_atomic)
            #
            # Determine if map_atomic/unmap_atomic exists in dma_buf_ops.
            # In some kernels, this is a mandatory callback.
            #
            # Added by commit f9b67f0014cb
            # ("dma-buf: Rename dma-ops to prevent conflict with kunmap_atomic macro")
            # in v4.12 (2017-04-19)
            #
            # Removed by commit f664a5269542
            # ("dma-buf: remove kmap_atomic interface") in v4.19 (2018-05-28)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            int conftest_dma_buf_ops_has_map_atomic(void) {
                return offsetof(struct dma_buf_ops, map_atomic);
            }
            int conftest_dma_buf_ops_has_unmap_atomic(void) {
                return offsetof(struct dma_buf_ops, unmap_atomic);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DMA_BUF_OPS_HAS_MAP_ATOMIC" | append_conftest "types"
                rm -f conftest$$.o
                return
            else
                echo "#undef NV_DMA_BUF_OPS_HAS_MAP_ATOMIC" | append_conftest "types"
                return
            fi
        ;;

        dma_buf_has_dynamic_attachment)
            #
            # Determine if the function dma_buf_attachment_is_dynamic()
            # is present.
            #
            # Added by commit: 15fd552d186c
            # ("dma-buf: change DMA-buf locking convention v3") in v5.5 (2018-07-03)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            bool conftest_dma_buf_attachment_is_dynamic(void) {
                return dma_buf_attachment_is_dynamic(NULL);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DMA_BUF_HAS_DYNAMIC_ATTACHMENT" | append_conftest "functions"
                rm -f conftest$$.o
                return
            else
                echo "#undef NV_DMA_BUF_HAS_DYNAMIC_ATTACHMENT" | append_conftest "functions"
                return
            fi
        ;;

        dma_buf_attachment_has_peer2peer)
            #
            # Determine if peer2peer is present in struct dma_buf_attachment.
            # peer2peer being true indicates that a dma-buf importer is able
            # to handle peer resources not backed by struct page.
            #
            # Added by commit: 09606b5446c2
            # ("dma-buf: add peer2peer flag") in v5.8 (2018-03-22)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/dma-buf.h>
            int conftest_dma_buf_peer2peer(void) {
                return offsetof(struct dma_buf_attachment, peer2peer);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DMA_BUF_ATTACHMENT_HAS_PEER2PEER" | append_conftest "types"
                rm -f conftest$$.o
                return
            else
                echo "#undef NV_DMA_BUF_ATTACHMENT_HAS_PEER2PEER" | append_conftest "types"
                return
            fi
        ;;

        drm_connector_funcs_have_mode_in_name)
            #
            # Determine if _mode_ is present in connector function names.  We
            # only test drm_mode_connector_attach_encoder() and assume the
            # other functions are changed in sync.
            #
            # drm_mode_connector_attach_encoder() was renamed to
            # drm_connector_attach_encoder() by commit cde4c44d8769 ("drm:
            # drop _mode_ from drm_mode_connector_attach_encoder") in v4.19
            # (2018-07-09)
            #
            # drm_mode_connector_update_edid_property() was renamed by commit
            # c555f02371c3 ("drm: drop _mode_ from update_edit_property()")
            # in v4.19 (2018-07-09).
            #
            # The other DRM functions were renamed by commit 97e14fbeb53f
            # ("drm: drop _mode_ from remaining connector functions") in v4.19
            # (2018-07-09)
            #
            # Note that drm_connector.h by introduced by commit 522171951761
            # ("drm: Extract drm_connector.[hc]") in v4.9 (2016-08-12)
            #
            CODE="
            #include <drm/drm_connector.h>
            void conftest_drm_connector_funcs_have_mode_in_name(void) {
                drm_mode_connector_attach_encoder();
            }"

            compile_check_conftest "$CODE" "NV_DRM_CONNECTOR_FUNCS_HAVE_MODE_IN_NAME" "" "functions"
        ;;


        node_states_n_memory)
            #
            # Determine if the N_MEMORY constant exists.
            #
            # Added by commit 8219fc48adb3 ("mm: node_states: introduce
            # N_MEMORY") in v3.8 (2012-12-12).
            #
            CODE="
            #include <linux/nodemask.h>
            int conftest_node_states_n_memory(void) {
                return N_MEMORY;
            }"

            compile_check_conftest "$CODE" "NV_NODE_STATES_N_MEMORY_PRESENT" "" "types"
        ;;

        vm_fault_t)
            #
            # Determine if vm_fault_t is present
            #
            # Added by commit 1c8f422059ae5da07db7406ab916203f9417e396 ("mm:
            # change return type to vm_fault_t") in v4.17 (2018-04-05)
            #
            CODE="
            #include <linux/mm.h>
            vm_fault_t conftest_vm_fault_t;
            "
            compile_check_conftest "$CODE" "NV_VM_FAULT_T_IS_PRESENT" "" "types"
        ;;

        vmf_insert_pfn)
            #
            # Determine if the function vmf_insert_pfn() is
            # present.
            #
            # Added by commit 1c8f422059ae5da07db7406ab916203f9417e396 ("mm:
            # change return type to vm_fault_t") in v4.17 (2018-04-05)
            #
            CODE="
            #include <linux/mm.h>
            void conftest_vmf_insert_pfn(void) {
                vmf_insert_pfn();
            }"

            compile_check_conftest "$CODE" "NV_VMF_INSERT_PFN_PRESENT" "" "functions"
        ;;

        drm_framebuffer_get)
            #
            # Determine if the function drm_framebuffer_get() is present.
            #
            # Added by commit a4a69da06bc1 ("drm: Introduce
            # drm_framebuffer_{get,put}()") in v4.12 (2017-02-28).
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_FRAMEBUFFER_H_PRESENT)
            #include <drm/drm_framebuffer.h>
            #endif

            void conftest_drm_framebuffer_get(void) {
                drm_framebuffer_get();
            }"

            compile_check_conftest "$CODE" "NV_DRM_FRAMEBUFFER_GET_PRESENT" "" "functions"
        ;;

        drm_gem_object_get)
            #
            # Determine if the function drm_gem_object_get() is present.
            #
            # Added by commit e6b62714e87c ("drm: Introduce
            # drm_gem_object_{get,put}()") in v4.12 (2017-02-28).
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_GEM_H_PRESENT)
            #include <drm/drm_gem.h>
            #endif
            void conftest_drm_gem_object_get(void) {
                drm_gem_object_get();
            }"

            compile_check_conftest "$CODE" "NV_DRM_GEM_OBJECT_GET_PRESENT" "" "functions"
        ;;

        drm_dev_put)
            #
            # Determine if the function drm_dev_put() is present.
            #
            # Added by commit 9a96f55034e4 ("drm: introduce drm_dev_{get/put}
            # functions") in v4.15 (2017-09-26).
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DRV_H_PRESENT)
            #include <drm/drm_drv.h>
            #endif
            void conftest_drm_dev_put(void) {
                drm_dev_put();
            }"

            compile_check_conftest "$CODE" "NV_DRM_DEV_PUT_PRESENT" "" "functions"
        ;;

        drm_connector_list_iter)
            #
            # Determine if the drm_connector_list_iter struct is present.
            #
            # Added by commit 613051dac40da1751ab269572766d3348d45a197 ("drm:
            # locking&new iterators for connector_list") in v4.11 (2016-12-14).
            #
            CODE="
            #include <drm/drm_connector.h>
            int conftest_drm_connector_list_iter(void) {
                struct drm_connector_list_iter conn_iter;
            }"

            compile_check_conftest "$CODE" "NV_DRM_CONNECTOR_LIST_ITER_PRESENT" "" "types"

            #
            # Determine if the function drm_connector_list_iter_get() is
            # renamed to drm_connector_list_iter_begin().
            #
            # Renamed by b982dab1e66d2b998e80a97acb6eaf56518988d3 (drm: Rename
            # connector list iterator API) in v4.12 (2017-02-28).
            #
            CODE="
            #if defined(NV_DRM_DRM_CONNECTOR_H_PRESENT)
            #include <drm/drm_connector.h>
            #endif
            void conftest_drm_connector_list_iter_begin(void) {
                drm_connector_list_iter_begin();
            }"

            compile_check_conftest "$CODE" "NV_DRM_CONNECTOR_LIST_ITER_BEGIN_PRESENT" "" "functions"
        ;;

        drm_atomic_helper_swap_state_has_stall_arg)
            #
            # Determine if drm_atomic_helper_swap_state() has 'stall' argument.
            #
            # drm_atomic_helper_swap_state() function prototype updated to take
            # 'state' and 'stall' arguments by commit
            # 5e84c2690b805caeff3b4c6c9564c7b8de54742d (drm/atomic-helper:
            # Massage swap_state signature somewhat)
            # in v4.8 (2016-06-10).
            #
            CODE="
            #include <drm/drm_atomic_helper.h>
            void conftest_drm_atomic_helper_swap_state_has_stall_arg(
                    struct drm_atomic_state *state,
                    bool stall) {
                (void)drm_atomic_helper_swap_state(state, stall);
            }"

            compile_check_conftest "$CODE" "NV_DRM_ATOMIC_HELPER_SWAP_STATE_HAS_STALL_ARG" | append_conftest "types"

            #
            # Determine if drm_atomic_helper_swap_state() returns int.
            #
            # drm_atomic_helper_swap_state() function prototype
            # updated to return int by commit
            # c066d2310ae9bbc695c06e9237f6ea741ec35e43 (drm/atomic: Change
            # drm_atomic_helper_swap_state to return an error.) in v4.14
            # (2017-07-11).
            #
            CODE="
            #include <drm/drm_atomic_helper.h>
            int conftest_drm_atomic_helper_swap_state_return_int(
                    struct drm_atomic_state *state,
                    bool stall) {
                return drm_atomic_helper_swap_state(state, stall);
            }"

            compile_check_conftest "$CODE" "NV_DRM_ATOMIC_HELPER_SWAP_STATE_RETURN_INT" | append_conftest "types"
        ;;

        pm_runtime_available)
            #
            # Determine if struct dev_pm_info has the 'usage_count' field.
            #
            # This was added to the kernel in commit 5e928f77a09a0 in v2.6.32
            # (2008-08-18), but originally were dependent on CONFIG_PM_RUNTIME,
            # which was folded into the more generic CONFIG_PM in commit
            # d30d819dc8310 in v3.19 (2014-11-27).
            # Rather than attempt to select the appropriate CONFIG option,
            # simply check if this member is present.
            #
            CODE="
            #include <linux/pm.h>
            void pm_runtime_conftest(void) {
                struct dev_pm_info dpmi;
                atomic_set(&dpmi.usage_count, 1);
            }"

            compile_check_conftest "$CODE" "NV_PM_RUNTIME_AVAILABLE" "" "generic"
        ;;

        device_driver_of_match_table)
            #
            # Determine if the device_driver struct has an of_match_table member.
            #
            # of_match_table was added by commit 597b9d1e44e9 ("drivercore:
            # Add of_match_table to the common device drivers") in v2.6.35
            # (2010-04-13).
            #
            CODE="
            #include <linux/device.h>
            int conftest_device_driver_of_match_table(void) {
                return offsetof(struct device_driver, of_match_table);
            }"

            compile_check_conftest "$CODE" "NV_DEVICE_DRIVER_OF_MATCH_TABLE_PRESENT" "" "types"
        ;;

        device_of_node)
            #
            # Determine if the device struct has an of_node member.
            #
            # of_node member was added by commit d706c1b05027 ("driver-core:
            # Add device node pointer to struct device") in v2.6.35
            # (2010-04-13).
            #
            CODE="
            #include <linux/device.h>
            int conftest_device_of_node(void) {
                return offsetof(struct device, of_node);
            }"

            compile_check_conftest "$CODE" "NV_DEVICE_OF_NODE_PRESENT" "" "types"
        ;;

        dev_is_pci)
            #
            # Determine if the dev_is_pci() macro is present.
            #
            # dev_is_pci() macro was added by commit fb8a0d9d1bfd ("pci: Add
            # SR-IOV convenience functions and macros") in v2.6.34
            # (2010-02-10).
            #
            CODE="
            #include <linux/pci.h>
            void conftest_dev_is_pci(void) {
                if(dev_is_pci()) {}
            }
            "

            compile_check_conftest "$CODE" "NV_DEV_IS_PCI_PRESENT" "" "functions"
        ;;

        of_find_matching_node)
            #
            # Determine if the of_find_matching_node() function is present.
            #
            # Test if linux/of.h header file inclusion is successful or not and
            # define/undefine NV_LINUX_OF_H_USABLE depending upon status of inclusion.
            #
            # of_find_matching_node was added by commit 283029d16a88
            # ("[POWERPC] Add of_find_matching_node() helper function") in
            # v2.6.25 (2008-01-09).
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/of.h>
            " > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_LINUX_OF_H_USABLE" | append_conftest "generic"
                CODE="
                #include <linux/of.h>
                void conftest_of_find_matching_node() {
                    of_find_matching_node();
                }"

                compile_check_conftest "$CODE" "NV_OF_FIND_MATCHING_NODE_PRESENT" "" "functions"
            else
                echo "#undef NV_LINUX_OF_H_USABLE" | append_conftest "generic"
                echo "#undef NV_OF_FIND_MATCHING_NODE_PRESENT" | append_conftest "functions"
            fi
        ;;

        dma_direct_map_resource)
            #
            # Determine whether dma_is_direct() exists.
            #
            # dma_is_direct() was added by commit 356da6d0cde3 ("dma-mapping:
            # bypass indirect calls for dma-direct") in 5.1 (2018-12-06).
            #
            # If dma_is_direct() does exist, then we assume that
            # dma_direct_map_resource() exists.  Both functions were added
            # as part of the same patchset.
            #
            # The presence of dma_is_direct() and dma_direct_map_resource()
            # means that dma_direct can perform DMA mappings itself.
            #
            CODE="
            #include <linux/dma-mapping.h>
            void conftest_dma_is_direct(void) {
                dma_is_direct();
            }"

            compile_check_conftest "$CODE" "NV_DMA_IS_DIRECT_PRESENT" "" "functions"
        ;;

        tegra_get_platform)
            #
            # Determine if tegra_get_platform() function is present
            #
            CODE="
            #if defined NV_SOC_TEGRA_CHIP_ID_H_PRESENT
            #include <soc/tegra/chip-id.h>
            #elif defined(NV_SOC_TEGRA_FUSE_H_PRESENT)
            #include <soc/tegra/fuse.h>
            #endif
            void conftest_tegra_get_platform(void) {
                tegra_get_platform(0);
            }
            "

            compile_check_conftest "$CODE" "NV_TEGRA_GET_PLATFORM_PRESENT" "" "functions"
        ;;

        tegra_bpmp_send_receive)
            #
            # Determine if tegra_bpmp_send_receive() function is present
            #
            CODE="
            #if defined NV_SOC_TEGRA_TEGRA_BPMP_H_PRESENT
            #include <soc/tegra/tegra_bpmp.h>
            #endif
            int conftest_tegra_bpmp_send_receive(
                    int mrq,
                    void *ob_data,
                    int ob_sz,
                    void *ib_data,
                    int ib_sz) {
                return tegra_bpmp_send_receive(mrq, ob_data, ob_sz, ib_data, ib_sz);
            }
            "

            compile_check_conftest "$CODE" "NV_TEGRA_BPMP_SEND_RECEIVE" "" "functions"
        ;;

        drm_alpha_blending_available)
            #
            # Determine if the DRM subsystem supports alpha blending
            #
            # This conftest using "generic" rather than "functions" because
            # with the logic of "functions" the presence of
            # *either*_alpha_property or _blend_mode_property would be enough
            # to cause NV_DRM_ALPHA_BLENDING_AVAILABLE to be defined.
            #
            CODE="
            #if defined(NV_DRM_DRM_BLEND_H_PRESENT)
            #include <drm/drm_blend.h>
            #endif
            void conftest_drm_alpha_blending_available(void) {
                /* 2018-04-11 ae0e28265e216dad11d4cbde42fc15e92919af78 */
                (void)drm_plane_create_alpha_property;

                /* 2018-08-23 a5ec8332d4280500544e316f76c04a7adc02ce03 */
                (void)drm_plane_create_blend_mode_property;
            }"

            compile_check_conftest "$CODE" "NV_DRM_ALPHA_BLENDING_AVAILABLE" "" "generic"
        ;;

        drm_rotation_available)
            #
            # Determine if the DRM subsystem supports rotation.
            #
            # drm_plane_create_rotation_property() was added on 2016-09-26 by
            # d138dd3c0c70979215f3184cf36f95875e37932e (drm: Add support for
            # optional per-plane rotation property) in linux kernel. Presence
            # of it is sufficient to say that DRM subsystem support rotation.
            #
            CODE="
            #if defined(NV_DRM_DRM_BLEND_H_PRESENT)
            #include <drm/drm_blend.h>
            #endif
            void conftest_drm_rotation_available(void) {
                drm_plane_create_rotation_property();
            }"

            compile_check_conftest "$CODE" "NV_DRM_ROTATION_AVAILABLE" "" "functions"
            ;;

        drm_driver_prime_flag_present)
            #
            # Determine whether driver feature flag DRIVER_PRIME is present.
            #
            # The DRIVER_PRIME flag was added by commit 3248877ea179 (drm:
            # base prime/dma-buf support (v5)) in v3.4 (2011-11-25) and is
            # removed by commit 0424fdaf883a (drm/prime: Actually remove
            # DRIVER_PRIME everywhere) on 2019-06-17.
            #
            # DRIVER_PRIME definition moved from drmP.h to drm_drv.h by
            # commit 85e634bce01a (drm: Extract drm_drv.h) in v4.10
            # (2016-11-14).
            #
            # DRIVER_PRIME define is changed to enum value by commit
            # 0e2a933b02c9 (drm: Switch DRIVER_ flags to an enum) in v5.1
            # (2019-01-29).
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DRV_H_PRESENT)
            #include <drm/drm_drv.h>
            #endif

            unsigned int drm_driver_prime_flag_present_conftest(void) {
                return DRIVER_PRIME;
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_PRIME_FLAG_PRESENT" "" "types"
        ;;

        drm_connector_for_each_possible_encoder)
            #
            # Determine the number of arguments of the
            # drm_connector_for_each_possible_encoder() macro.
            #
            # drm_connector_for_each_possible_encoder() is added by commit
            # 83aefbb887b5 (drm: Add drm_connector_for_each_possible_encoder())
            # in v4.19. The definition and prorotype is changed to take only
            # two arguments connector and encoder, by commit 62afb4ad425a
            # (drm/connector: Allow max possible encoders to attach to a
            # connector) in v5.5rc1.
            #
            echo "$CONFTEST_PREAMBLE
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_CONNECTOR_H_PRESENT)
            #include <drm/drm_connector.h>
            #endif

            void conftest_drm_connector_for_each_possible_encoder(
                struct drm_connector *connector,
                struct drm_encoder *encoder,
                int i) {

                drm_connector_for_each_possible_encoder(connector, encoder, i) {
                }
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                echo "#define NV_DRM_CONNECTOR_FOR_EACH_POSSIBLE_ENCODER_ARGUMENT_COUNT 3" | append_conftest "functions"
                rm -f conftest$$.o
                return
            else
                echo "#define NV_DRM_CONNECTOR_FOR_EACH_POSSIBLE_ENCODER_ARGUMENT_COUNT 2" | append_conftest "functions"
            fi
        ;;

        mmu_notifier_ops_invalidate_range)
            #
            # Determine if the mmu_notifier_ops struct has the
            # 'invalidate_range' member.
            #
            # struct mmu_notifier_ops.invalidate_range was added by commit
            # 0f0a327fa12cd55de5e7f8c05a70ac3d047f405e ("mmu_notifier: add the
            # callback for mmu_notifier_invalidate_range()") in v3.19
            # (2014-11-13).
            CODE="
            #include <linux/mmu_notifier.h>
            int conftest_mmu_notifier_ops_invalidate_range(void) {
                return offsetof(struct mmu_notifier_ops, invalidate_range);
            }"

            compile_check_conftest "$CODE" "NV_MMU_NOTIFIER_OPS_HAS_INVALIDATE_RANGE" "" "types"
        ;;

        drm_format_num_planes)
            #
            # Determine if drm_format_num_planes() function is present.
            #
            # The drm_format_num_planes() function was added by commit
            # d0d110e09629 drm: Add drm_format_num_planes() utility function in
            # v3.3 (2011-12-20). Prototype was moved from drm_crtc.h to
            # drm_fourcc.h by commit ae4df11a0f53 (drm: Move format-related
            # helpers to drm_fourcc.c) in v4.8 (2016-06-09).
            # drm_format_num_planes() has been removed by commit 05c452c115bf
            # (drm: Remove users of drm_format_num_planes) removed v5.3
            # (2019-05-16).
            #
            CODE="

            #if defined(NV_DRM_DRM_CRTC_H_PRESENT)
            #include <drm/drm_crtc.h>
            #endif

            #if defined(NV_DRM_DRM_FOURCC_H_PRESENT)
            #include <drm/drm_fourcc.h>
            #endif

            void conftest_drm_format_num_planes(void) {
                drm_format_num_planes();
            }
            "

            compile_check_conftest "$CODE" "NV_DRM_FORMAT_NUM_PLANES_PRESENT" "" "functions"
        ;;

        drm_gem_object_has_resv)
            #
            # Determine if the 'drm_gem_object' structure has a 'resv' field.
            #
            # A 'resv' filed in the 'drm_gem_object' structure, is added by
            # commit 1ba627148ef5 (drm: Add reservation_object to
            # drm_gem_object) in v5.2.
            #
            CODE="$CONFTEST_PREAMBLE
            #if defined(NV_DRM_DRM_GEM_H_PRESENT)
            #include <drm/drm_gem.h>
            #endif

            int conftest_drm_gem_object_has_resv(void) {
                return offsetof(struct drm_gem_object, resv);
            }"

            compile_check_conftest "$CODE" "NV_DRM_GEM_OBJECT_HAS_RESV" "" "types"
        ;;

        proc_ops)
            #
            # Determine if the 'struct proc_ops' type is present.
            #
            # Added by commit d56c0d45f0e2 ("proc: decouple proc from VFS with
            # "struct proc_ops"") in 5.6-rc1
            #
            CODE="
            #include <linux/proc_fs.h>

            struct proc_ops p_ops;
            "

            compile_check_conftest "$CODE" "NV_PROC_OPS_PRESENT" "" "types"
        ;;

        drm_crtc_state_has_async_flip)
            #
            # Determine if the 'drm_crtc_state' structure has a 'async_flip'
            # field.
            #
            # Commit 4d85f45c73a2 (drm/atomic: Rename crtc_state->pageflip_flags
            # to async_flip) replaced 'pageflip_flags' by 'async_flip' in v5.4.
            #
            CODE="
            #if defined(NV_DRM_DRM_CRTC_H_PRESENT)
            #include <drm/drm_crtc.h>
            #endif

            int conftest_drm_crtc_state_has_async_flip(void) {
                return offsetof(struct drm_crtc_state, async_flip);
            }"

            compile_check_conftest "$CODE" "NV_DRM_CRTC_STATE_HAS_ASYNC_FLIP" "" "types"
        ;;

        drm_crtc_state_has_pageflip_flags)
            #
            # Determine if the 'drm_crtc_state' structure has a
            # 'pageflip_flags' field.
            #
            # 'pageflip_flags' added by commit 6cbe5c466d73 (drm/atomic: Save
            # flip flags in drm_crtc_state) in v4.12. Commit 4d85f45c73a2
            # (drm/atomic: Rename crtc_state->pageflip_flags to async_flip)
            # replaced 'pageflip_flags' by 'async_flip' in v5.4.
            #
            CODE="
            #if defined(NV_DRM_DRM_CRTC_H_PRESENT)
            #include <drm/drm_crtc.h>
            #endif

            int conftest_drm_crtc_state_has_pageflip_flags(void) {
                return offsetof(struct drm_crtc_state, pageflip_flags);
            }"

            compile_check_conftest "$CODE" "NV_DRM_CRTC_STATE_HAS_PAGEFLIP_FLAGS" "" "types"
        ;;

        ktime_get_raw_ts64)
            #
            # Determine if ktime_get_raw_ts64() is present
            #
            # Added by commit fb7fcc96a86cf ("timekeeping: Standardize on
            # ktime_get_*() naming") in 4.18 (2018-04-27)
            #
        CODE="
        #include <linux/ktime.h>
        void conftest_ktime_get_raw_ts64(void){
            ktime_get_raw_ts64();
        }"
            compile_check_conftest "$CODE" "NV_KTIME_GET_RAW_TS64_PRESENT" "" "functions"
        ;;

        ktime_get_real_ts64)
            #
            # Determine if ktime_get_real_ts64() is present
            #
            # Added by commit d6d29896c665d ("timekeeping: Provide timespec64
            # based interfaces") in 3.17 (2014-07-16)
            #
        CODE="
        #include <linux/ktime.h>
        void conftest_ktime_get_real_ts64(void){
            ktime_get_real_ts64();
        }"
            compile_check_conftest "$CODE" "NV_KTIME_GET_REAL_TS64_PRESENT" "" "functions"
        ;;

        drm_format_modifiers_present)
            #
            # Determine whether the base DRM format modifier support is present.
            #
            # This will show up in a few places:
            #
            # -Definition of the format modifier constructor macro, which
            #  we can use to reconstruct our bleeding-edge format modifiers
            #  when the local kernel headers don't include them.
            #
            # -The first set of format modifier vendor macros, including the
            #  poorly named "NV" vendor, which was later renamed "NVIDIA".
            #
            # -the "modifier[]" member of the AddFB2 ioctl's parameter
            #  structure.
            #
            # All these were added by commit e3eb3250d84e (drm: add support for
            # tiled/compressed/etc modifier in addfb2) in 4.1-rc1 (2015-02-05).
            CODE="
            #include <drm/drm_mode.h>
            #include <drm/drm_fourcc.h>
            int conftest_fourcc_fb_modifiers(void) {
                u64 my_fake_mod = fourcc_mod_code(INTEL, 0);
                (void)my_fake_mod;
                return offsetof(struct drm_mode_fb_cmd2, modifier);
            }"

            compile_check_conftest "$CODE" "NV_DRM_FORMAT_MODIFIERS_PRESENT" "" "types"

        ;;

        timespec64)
            #
            # Determine if struct timespec64 is present
            # Added by commit 361a3bf00582 ("time64: Add time64.h header and
            # define struct timespec64") in 3.17 (2014-07-16)
            #
        CODE="
        #include <linux/time.h>

        struct timespec64 ts64;
        "
            compile_check_conftest "$CODE" "NV_TIMESPEC64_PRESENT" "" "types"

        ;;

        vmalloc_has_pgprot_t_arg)
            #
            # Determine if __vmalloc has the 'pgprot' argument.
            #
            # The third argument to __vmalloc, page protection
            # 'pgprot_t prot', was removed by commit 88dca4ca5a93
            # (mm: remove the pgprot argument to __vmalloc)
            # in v5.8-rc1 (2020-06-01).
        CODE="
        #include <linux/vmalloc.h>

        void conftest_vmalloc_has_pgprot_t_arg(void) {
            pgprot_t prot;
            (void)__vmalloc(0, 0, prot);
        }"

            compile_check_conftest "$CODE" "NV_VMALLOC_HAS_PGPROT_T_ARG" "" "types"

        ;;

        mm_has_mmap_lock)
            #
            # Determine if the 'mm_struct' structure has a 'mmap_lock' field.
            #
            # Kernel commit da1c55f1b272 ("mmap locking API: rename mmap_sem
            # to mmap_lock") replaced the field 'mmap_sem' by 'mmap_lock'
            # in v5.8-rc1 (2020-06-08).
            CODE="
            #include <linux/mm_types.h>

            int conftest_mm_has_mmap_lock(void) {
                return offsetof(struct mm_struct, mmap_lock);
            }"

            compile_check_conftest "$CODE" "NV_MM_HAS_MMAP_LOCK" "" "types"
        ;;

        full_name_hash)
            #
            # Determine how many arguments full_name_hash takes.
            #
            # Changed by commit 8387ff2577e ("vfs: make the string hashes salt
            # the hash") in v4.8 (2016-06-10)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/stringhash.h>
            void conftest_full_name_hash(void) {
                full_name_hash(NULL, NULL, 0);
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_FULL_NAME_HASH_ARGUMENT_COUNT 3" | append_conftest "functions"
            else
                echo "#define NV_FULL_NAME_HASH_ARGUMENT_COUNT 2" | append_conftest "functions"
            fi
        ;;

        hlist_for_each_entry)
            #
            # Determine how many arguments hlist_for_each_entry takes.
            #
            # Changed by commit b67bfe0d42c ("hlist: drop the node parameter
            # from iterators") in v3.9 (2013-02-28)
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/list.h>
            void conftest_hlist_for_each_entry(void) {
                struct hlist_head *head;
                struct dummy
                {
                    struct hlist_node hlist;
                };
                struct dummy *pos;
                hlist_for_each_entry(pos, head, hlist) {}
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_HLIST_FOR_EACH_ENTRY_ARGUMENT_COUNT 3" | append_conftest "functions"
            else
                echo "#define NV_HLIST_FOR_EACH_ENTRY_ARGUMENT_COUNT 4" | append_conftest "functions"
            fi
        ;;

        drm_vma_offset_exact_lookup_locked)
            #
            # Determine if the drm_vma_offset_exact_lookup_locked() function
            # is present.
            #
            # Added by commit 2225cfe46bcc ("drm/gem: Use kref_get_unless_zero
            # for the weak mmap references") in v4.4
            #
            CODE="
            #include <drm/drm_vma_manager.h>
            void conftest_drm_vma_offset_exact_lookup_locked(void) {
                drm_vma_offset_exact_lookup_locked();
            }"

            compile_check_conftest "$CODE" "NV_DRM_VMA_OFFSET_EXACT_LOOKUP_LOCKED_PRESENT" "" "functions"
        ;;

        drm_vma_node_is_allowed_has_tag_arg)
            #
            # Determine if drm_vma_node_is_allowed() has 'tag' arguments of
            # 'struct drm_file *' type.
            #
            # Updated to take 'tag' argument by commit d9a1f0b4eb60 ("drm: use
            # drm_file to tag vm-bos") in v4.9
            #
            CODE="
            #include <drm/drm_vma_manager.h>
            bool drm_vma_node_is_allowed(struct drm_vma_offset_node *node,
                                         struct drm_file *tag) {
                return true;
            }"

            compile_check_conftest "$CODE" "NV_DRM_VMA_NODE_IS_ALLOWED_HAS_TAG_ARG" | append_conftest "types"
        ;;

        drm_vma_offset_node_has_readonly)
            #
            # Determine if the 'drm_vma_offset_node' structure has a 'readonly'
            # field.
            #
            # Added by commit 3e977ac6179b ("drm/i915: Prevent writing into a
            # read-only object via a GGTT mmap") in v4.19.
            #
            CODE="
            #include <drm/drm_vma_manager.h>

            int conftest_drm_vma_offset_node_has_readonly(void) {
                return offsetof(struct drm_vma_offset_node, readonly);
            }"

            compile_check_conftest "$CODE" "NV_DRM_VMA_OFFSET_NODE_HAS_READONLY" "" "types"

        ;;

        pci_enable_atomic_ops_to_root)
            # pci_enable_atomic_ops_to_root was added by
            # commit 430a23689dea ("PCI: Add pci_enable_atomic_ops_to_root()")
            # in v4.16-rc1 (2018-01-05)
            #
            CODE="
            #include <linux/pci.h>
            void conftest_pci_enable_atomic_ops_to_root(void) {
                pci_enable_atomic_ops_to_root();
            }"
            compile_check_conftest "$CODE" "NV_PCI_ENABLE_ATOMIC_OPS_TO_ROOT_PRESENT" "" "functions"
        ;;

        kvmalloc)
            #
            # Determine if kvmalloc() is present
            #
            # Added by commit a7c3e901a46ff54c016d040847eda598a9e3e653 ("mm:
            # introduce kv[mz]alloc helpers") in v4.12 (2017-05-08).
            #
        CODE="
        #include <linux/mm.h>
        void conftest_kvmalloc(void){
            kvmalloc();
        }"
            compile_check_conftest "$CODE" "NV_KVMALLOC_PRESENT" "" "functions"

        ;;

        drm_gem_object_put_unlocked)
            #
            # Determine if the function drm_gem_object_put_unlocked() is present.
            #
            # In v5.9-rc1, commit 2f4dd13d4bb8 ("drm/gem: add
            # drm_gem_object_put helper") removes drm_gem_object_put_unlocked()
            # function and replace its definition by transient macro. Commit
            # ab15d56e27be ("drm: remove transient
            # drm_gem_object_put_unlocked()") finally removes
            # drm_gem_object_put_unlocked() macro.
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_GEM_H_PRESENT)
            #include <drm/drm_gem.h>
            #endif
            void conftest_drm_gem_object_put_unlocked(void) {
                drm_gem_object_put_unlocked();
            }"

            compile_check_conftest "$CODE" "NV_DRM_GEM_OBJECT_PUT_UNLOCK_PRESENT" "" "functions"
        ;;

        drm_display_mode_has_vrefresh)
            #
            # Determine if the 'drm_display_mode' structure has a 'vrefresh'
            # field.
            #
            # Removed by commit 0425662fdf05 ("drm: Nuke mode->vrefresh") in
            # v5.9-rc1.
            #
            CODE="
            #include <drm/drm_modes.h>

            int conftest_drm_display_mode_has_vrefresh(void) {
                return offsetof(struct drm_display_mode, vrefresh);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DISPLAY_MODE_HAS_VREFRESH" "types"

        ;;

        drm_driver_master_set_has_int_return_type)
            #
            # Determine if drm_driver::master_set() returns integer value
            #
            # Changed to void by commit 907f53200f98 ("drm: vmwgfx: remove
            # drm_driver::master_set() return type") in v5.9-rc1.
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DRV_H_PRESENT)
            #include <drm/drm_drv.h>
            #endif

            int conftest_drm_driver_master_set_has_int_return_type(struct drm_driver *drv,
                struct drm_device *dev, struct drm_file *file_priv, bool from_open) {

                return drv->master_set(dev, file_priv, from_open);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_SET_MASTER_HAS_INT_RETURN_TYPE" "" "types"
        ;;

        drm_driver_has_gem_free_object)
            #
            # Determine if the 'drm_driver' structure has a 'gem_free_object'
            # function pointer.
            #
            # drm_driver::gem_free_object is removed by commit 1a9458aeb8eb
            # ("drm: remove drm_driver::gem_free_object") in v5.9-rc1.
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DRV_H_PRESENT)
            #include <drm/drm_drv.h>
            #endif

            int conftest_drm_driver_has_gem_free_object(void) {
                return offsetof(struct drm_driver, gem_free_object);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_HAS_GEM_FREE_OBJECT" "" "types"
        ;;

        vga_tryget)
            #
            # Determine if vga_tryget() is present
            #
            # vga_tryget() was removed by commit f369bc3f9096 ("vgaarb: mark
            # vga_tryget static") in v5.9-rc1 (2020-08-01).
            #
            CODE="
            #include <linux/vgaarb.h>
            void conftest_vga_tryget(void) {
                vga_tryget();
            }"

            compile_check_conftest "$CODE" "NV_VGA_TRYGET_PRESENT" "" "functions"
        ;;

        pci_channel_state)
            #
            # Determine if pci_channel_state enum type is present.
            #
            # pci_channel_state was removed by commit 16d79cd4e23b ("PCI: Use
            # 'pci_channel_state_t' instead of 'enum pci_channel_state'") in
            # v5.9-rc1 (2020-07-02).
            #
            CODE="
            #include <linux/pci.h>

            enum pci_channel_state state;
            "

            compile_check_conftest "$CODE" "NV_PCI_CHANNEL_STATE_PRESENT" "" "types"
        ;;

        pgprot_decrypted)
            #
            # Determine if the macro 'pgprot_decrypted()' is present.
            #
            # Added by commit 21729f81ce8a ("x86/mm: Provide general kernel
            # support for memory encryption") in v4.14 (2017-07-18)
            CODE="
            #include <asm/pgtable.h>

            void conftest_pgprot_decrypted(void)
                if(pgprot_decrypted()) {}
            }"

            compile_check_conftest "$CODE" "NV_PGPROT_DECRYPTED_PRESENT" "" "functions"

        ;;

        cc_mkdec)
            #
            # Determine if cc_mkdec() is present.
            #
            # cc_mkdec() by commit b577f542f93c ("x86/coco: Add API to handle
            # encryption mask) in v5.18-rc1 (2022-02-22).
            #
            CODE="
            #if defined(NV_ASM_COCO_H_PRESENT)
            #include <asm/coco.h>
            #endif

            void conftest_cc_mkdec(void) {
                cc_mkdec();
            }"

            compile_check_conftest "$CODE" "NV_CC_MKDEC_PRESENT" "" "functions"
        ;;

        drm_prime_pages_to_sg_has_drm_device_arg)
            #
            # Determine if drm_prime_pages_to_sg() has 'dev' argument.
            #
            # drm_prime_pages_to_sg() is updated to take 'dev' argument by commit
            # 707d561f77b5 ("drm: allow limiting the scatter list size.").
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif
            #if defined(NV_DRM_DRM_PRIME_H_PRESENT)
            #include <drm/drm_prime.h>
            #endif

            struct sg_table *drm_prime_pages_to_sg(struct drm_device *dev,
                                                   struct page **pages,
                                                   unsigned int nr_pages) {
                return 0;
            }"

            compile_check_conftest "$CODE" "NV_DRM_PRIME_PAGES_TO_SG_HAS_DRM_DEVICE_ARG" "" "types"
        ;;

        drm_driver_has_gem_prime_callbacks)
            #
            # Determine if drm_driver structure has the GEM and PRIME callback
            # function pointers.
            #
            # The GEM and PRIME callback are removed from drm_driver
            # structure, by commit d693def4fd1c ("drm: Remove obsolete GEM and
            # PRIME callbacks from struct drm_driver").
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DRV_H_PRESENT)
            #include <drm/drm_drv.h>
            #endif

            void conftest_drm_driver_has_gem_and_prime_callbacks(void) {
                struct drm_driver drv;

                drv.gem_prime_pin = 0;
                drv.gem_prime_get_sg_table = 0;
                drv.gem_prime_vmap = 0;
                drv.gem_prime_vunmap = 0;
                drv.gem_vm_ops = 0;
            }"

            compile_check_conftest "$CODE" "NV_DRM_DRIVER_HAS_GEM_PRIME_CALLBACKS" "" "types"
        ;;

        drm_crtc_atomic_check_has_atomic_state_arg)
            #
            # Determine if drm_crtc_helper_funcs::atomic_check takes 'state'
            # argument of 'struct drm_atomic_state' type.
            #
            # The commit 29b77ad7b9ca ("drm/atomic: Pass the full state to CRTC
            # atomic_check") passed the full atomic state to
            # drm_crtc_helper_funcs::atomic_check()
            #
            # To test the signature of drm_crtc_helper_funcs::atomic_check(),
            # declare a function prototype with typeof ::atomic_check(), and then
            # define the corresponding function implementation with the expected
            # signature.  Successful compilation indicates that ::atomic_check()
            # has the expected signature.
            #
            echo "$CONFTEST_PREAMBLE
            #include <drm/drm_modeset_helper_vtables.h>

            static const struct drm_crtc_helper_funcs *funcs;
            typeof(*funcs->atomic_check) conftest_drm_crtc_atomic_check_has_atomic_state_arg;

            int conftest_drm_crtc_atomic_check_has_atomic_state_arg(
                    struct drm_crtc *crtc, struct drm_atomic_state *state) {
                return 0;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_DRM_CRTC_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG" | append_conftest "types"
            else
                echo "#undef NV_DRM_CRTC_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG" | append_conftest "types"
            fi
        ;;

        drm_gem_object_vmap_has_map_arg)
            #
            # Determine if drm_gem_object_funcs::vmap takes 'map'
            # argument of 'struct dma_buf_map' type.
            #
            # The commit 49a3f51dfeee ("drm/gem: Use struct dma_buf_map in GEM
            # vmap ops and convert GEM backends") update
            # drm_gem_object_funcs::vmap to take 'map' argument.
            #
            CODE="
            #include <drm/drm_gem.h>
            int conftest_drm_gem_object_vmap_has_map_arg(
                    struct drm_gem_object *obj, struct dma_buf_map *map) {
                return obj->funcs->vmap(obj, map);
            }"

            compile_check_conftest "$CODE" "NV_DRM_GEM_OBJECT_VMAP_HAS_MAP_ARG" "" "types"
        ;;

        set_close_on_exec)
            #
            # __set_close_on_exec(() was added by
            # commit 1dce27c5aa67 ("Wrap accesses to the fd_sets")
            # in v3.4-rc1 (2012-02-19)
            #
            CODE="
            #include <linux/types.h>
            #include <linux/fdtable.h>
            void conftest_set_close_on_exec(void) {
                __set_close_on_exec();
            }"

            compile_check_conftest "$CODE" "NV_SET_CLOSE_ON_EXEC_PRESENT" "" "functions"
        ;;

        iterate_fd)
            #
            # iterate_fd() was added by
            # commit c3c073f808b2 ("new helper: iterate_fd()")
            # in v3.7-rc1 (2012-09-26)
            #
            CODE="
            #include <linux/types.h>
            #include <linux/fdtable.h>
            void conftest_iterate_fd(void) {
                iterate_fd();
            }"

            compile_check_conftest "$CODE" "NV_ITERATE_FD_PRESENT" "" "functions"
        ;;

        seq_read_iter)
            #
            # Determine if seq_read_iter() is present
            #
            # seq_read_iter() was added by commit d4d50710a8b4 ("seq_file:
            # add seq_read_iter") in v5.10-rc1 (2020-11-04).
            #
            CODE="
            #include <linux/seq_file.h>
            void conftest_seq_read_iter(void) {
                seq_read_iter();
            }"

            compile_check_conftest "$CODE" "NV_SEQ_READ_ITER_PRESENT" "" "functions"
        ;;

        pci_class_multimedia_hd_audio)
            #
            # Determine if 'PCI_CLASS_MULTIMEDIA_HD_AUDIO' macro is present
            # in <linux/pci_ids.h>.
            #
            # The commit 07f4f97d7b4b ("vga_switcheroo: Use device link for HDA
            # controller") has moved 'PCI_CLASS_MULTIMEDIA_HD_AUDIO' macro from
            # <sound/hdaudio.h> to <linux/pci_ids.h> in v4.17-rc1 (2018-03-03).
            #
            CODE="
            #include <linux/pci_ids.h>
            unsigned int conftest_pci_class_multimedia_hd_audio(void) {
                return PCI_CLASS_MULTIMEDIA_HD_AUDIO;
            }"

            compile_check_conftest "$CODE" "NV_PCI_CLASS_MULTIMEDIA_HD_AUDIO_PRESENT" "" "generic"
        ;;

        sg_page_iter_page)
            #
            # Determine if sg_page_iter_page() is present
            #
            # sg_page_iter_page() was added by commit 2db76d7c3c6db
            # ("lib/scatterlist: sg_page_iter: support sg lists w/o backing
            # pages") in v3.10-rc1 (2013-05-11).
            #
            CODE="
            #include <linux/scatterlist.h>
            void conftest_sg_page_iter_page(void) {
                sg_page_iter_page();
            }"

            compile_check_conftest "$CODE" "NV_SG_PAGE_ITER_PAGE_PRESENT" "" "functions"
        ;;

        unsafe_follow_pfn)
            #
            # Determine if unsafe_follow_pfn() is present.
            #
            # unsafe_follow_pfn() was added by commit 69bacee7f9ad
            # ("mm: Add unsafe_follow_pfn") in v5.13-rc1.
            #
            CODE="
            #include <linux/mm.h>
            void conftest_unsafe_follow_pfn(void) {
                unsafe_follow_pfn();
            }"

            compile_check_conftest "$CODE" "NV_UNSAFE_FOLLOW_PFN_PRESENT" "" "functions"
        ;;

        drm_plane_atomic_check_has_atomic_state_arg)
            #
            # Determine if drm_plane_helper_funcs::atomic_check takes 'state'
            # argument of 'struct drm_atomic_state' type.
            #
            # The commit 7c11b99a8e58 ("drm/atomic: Pass the full state to
            # planes atomic_check") passed the full atomic state to
            # drm_plane_helper_funcs::atomic_check()
            #
            # To test the signature of drm_plane_helper_funcs::atomic_check(),
            # declare a function prototype with typeof ::atomic_check(), and then
            # define the corresponding function implementation with the expected
            # signature.  Successful compilation indicates that ::atomic_check()
            # has the expected signature.
            #
            echo "$CONFTEST_PREAMBLE
            #include <drm/drm_modeset_helper_vtables.h>

            static const struct drm_plane_helper_funcs *funcs;
            typeof(*funcs->atomic_check) conftest_drm_plane_atomic_check_has_atomic_state_arg;

            int conftest_drm_plane_atomic_check_has_atomic_state_arg(
                    struct drm_plane *plane, struct drm_atomic_state *state) {
                return 0;
            }" > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                echo "#define NV_DRM_PLANE_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG" | append_conftest "types"
            else
                echo "#undef NV_DRM_PLANE_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG" | append_conftest "types"
            fi
        ;;

        ib_peer_memory_symbols)
            #
            # Determine if the following symbols exist in Module.symvers:
            # 1. ib_register_peer_memory_client
            # 2. ib_unregister_peer_memory_client
            # The conftest first checks in the kernel's own Module.symvers in
            # the regular path. If the symbols are not found there, it's possible
            # that MOFED is installed and check for these symbols in MOFED's
            # Module.symvers whose path is different from the kernel's symvers.
            #
            # Note: KERNELRELEASE and ARCH are defined by Kbuild and automatically
            # passed down to conftest.sh as env vars.

            MLNX_OFED_KERNEL_DIR=/usr/src/ofa_kernel
            VAR_DKMS_SOURCES_DIR=$(test -d /var/lib/dkms/mlnx-ofed-kernel &&
                                   ls -d /var/lib/dkms/mlnx-ofed-kernel/*/build 2>/dev/null)

            if check_for_ib_peer_memory_symbols "$OUTPUT" || \
               check_for_ib_peer_memory_symbols "$MLNX_OFED_KERNEL_DIR/$ARCH/$KERNELRELEASE" || \
               check_for_ib_peer_memory_symbols "$MLNX_OFED_KERNEL_DIR/$KERNELRELEASE" || \
               check_for_ib_peer_memory_symbols "$MLNX_OFED_KERNEL_DIR/default" || \
               check_for_ib_peer_memory_symbols "$VAR_DKMS_SOURCES_DIR"; then
                echo "#define NV_MLNX_IB_PEER_MEM_SYMBOLS_PRESENT" | append_conftest "symbols"
            else
                echo "#undef NV_MLNX_IB_PEER_MEM_SYMBOLS_PRESENT" | append_conftest "symbols"
            fi
        ;;

        add_memory_driver_managed)
            #
            # Determine if the add_memory_driver_managed function is present
            #
            # Added by commit 7b7b27214bba ("mm/memory_hotplug: introduce
            # add_memory_driver_managed()") in v5.8-rc1 (2020-06-05)
            #
            CODE="
            #include <linux/memory_hotplug.h>
            void conftest_add_memory_driver_managed() {
                add_memory_driver_managed();
            }"

            compile_check_conftest "$CODE" "NV_ADD_MEMORY_DRIVER_MANAGED_PRESENT" "" "functions"
        ;;

        add_memory_driver_managed_has_mhp_flags_arg)
            #
            # Check if add_memory_driver_managed() has mhp_flags arg.
            #
            # Added by commit b6117199787c ("mm/memory_hotplug: prepare passing flags to
            # add_memory() and friends") in v5.10-rc1 (2020-10-16)
            #
            CODE="
            #include <linux/memory_hotplug.h>
            int add_memory_driver_managed(int nid, u64 start, u64 size,
                                          const char *resource_name,
                                          mhp_t mhp_flags) {
                return 0;
            }"

            compile_check_conftest "$CODE" "NV_ADD_MEMORY_DRIVER_MANAGED_HAS_MHP_FLAGS_ARG" "" "types"
        ;;

        remove_memory_has_nid_arg)
            #
            # Check if remove_memory() has nid parameter.
            #
            # Removed by commit e1c158e4956612e7 ("mm/memory_hotplug: remove nid
            # parameter from remove_memory() and friends") in v5.15-rc1 (2021-09-09)
            #
            CODE="
            #include <linux/memory_hotplug.h>
            int remove_memory(int nid, u64 start, u64 size) {
                return 0;
            }"

            compile_check_conftest "$CODE" "NV_REMOVE_MEMORY_HAS_NID_ARG" "" "types"
        ;;


        device_property_read_u64)
            #
            # Determine if the device_property_read_u64 function is present
            #
            # Added by commit b31384fa5de37a1 ("Driver core: Unified device
            # properties interface for platform firmware") in v3.19-rc1 (2014-11-05)
            #
            CODE="
            #include <linux/acpi.h>
            void conftest_device_property_read_u64() {
                device_property_read_u64();
            }"

            compile_check_conftest "$CODE" "NV_DEVICE_PROPERTY_READ_U64_PRESENT" "" "functions"
        ;;

        of_property_count_elems_of_size)
            #
            # Determine if of_property_count_elems_of_size is present
            #
            # Added by commit 1df09bcof (" Move OF property and graph API from
            # base.c to property.c"
            #
            # Test if linux/of.h header file inclusion is successful or not,
            # depending on that check, for of_property_count_elems_of_size
            # presence
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/of.h>
            " > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                CODE="
                #include <linux/of.h>
                void conftest_of_property_count_elems_of_size() {
                    of_property_count_elems_of_size();
                }"

                compile_check_conftest "$CODE" "NV_OF_PROPERTY_COUNT_ELEMS_OF_SIZE_PRESENT" "" "functions"
            else
                echo "#undef NV_OF_PROPERTY_COUNT_ELEMS_OF_SIZE_PRESENT" | append_conftest "functions"
            fi
        ;;

        of_property_read_variable_u8_array)
            #
            # Determine if of_property_read_variable_u8_array is present
            #
            # Added by commit 1df09bcof (" Move OF property and graph API from
            # base.c to property.c"
            #
            # Test if linux/of.h header file inclusion is successful or not,
            # depending on that, check for of_property_read_variable_u8_array
            # presence
            #
            echo "$CONFTEST_PREAMBLE
            #include <linux/of.h>
            " > conftest$$.c

            $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
            rm -f conftest$$.c

            if [ -f conftest$$.o ]; then
                rm -f conftest$$.o
                CODE="
                #include <linux/of.h>
                void conftest_of_property_read_variable_u8_array() {
                    of_property_read_variable_u8_array();
                }"

                compile_check_conftest "$CODE" "NV_OF_PROPERTY_READ_VARIABLE_U8_ARRAY_PRESENT" "" "functions"
            else
                echo "#undef NV_OF_PROPERTY_READ_VARIABLE_U8_ARRAY_PRESENT" | append_conftest "functions"
            fi
        ;;

        devm_of_platform_populate)
            #
            # Determine if devm_of_platform_populate() function is present
            #
            # Added by commit 38b0b21of (add devm_ functions for populate and
            # depopulate")
            #
            CODE="
            #if defined(NV_LINUX_OF_PLATFORM_H_PRESENT)
            #include <linux/of_platform.h>
            #endif
            void conftest_devm_of_platform_populate(void)
            {
                devm_of_platform_populate(NULL, NULL);
            }
            "
            compile_check_conftest "$CODE" "NV_DEVM_OF_PLATFORM_POPULATE_PRESENT" "" "functions"
        ;;

        of_dma_configure)
            #
            # Determine if of_dma_configure() function is present
            #
            # Added by commit 591c1eeof ("configure the platform device
            # dma parameters")
            #
            CODE="
            #if defined(NV_LINUX_OF_DEVICE_H_PRESENT)
            #include <linux/of_device.h>
            #endif
            void conftest_of_dma_configure(void)
            {
                of_dma_configure();
            }
            "

            compile_check_conftest "$CODE" "NV_OF_DMA_CONFIGURE_PRESENT" "" "functions"
        ;;

        icc_get)
            #
            # Determine if icc_get() function is present
            #
            # Added by commit 11f1cec ("interconnect: Add generic on-chip
            # interconnect API")
            #
            CODE="
            #if defined(NV_LINUX_INTERCONNECT_H_PRESENT)
            #include <linux/interconnect.h>
            #endif
            void conftest_icc_get(void)
            {
                icc_get();
            }
            "

            compile_check_conftest "$CODE" "NV_ICC_GET_PRESENT" "" "functions"
        ;;

        icc_set_bw)
            #
            # Determine if icc_set_bw() function is present
            #
            # Added by commit 11f1cec ("interconnect: Add generic on-chip
            # interconnect API")
            #
            CODE="
            #if defined(NV_LINUX_INTERCONNECT_H_PRESENT)
            #include <linux/interconnect.h>
            #endif
            void conftest_icc_set_bw(void)
            {
                icc_set_bw();
            }
            "

            compile_check_conftest "$CODE" "NV_ICC_SET_BW_PRESENT" "" "functions"
        ;;

        icc_put)
            #
            # Determine if icc_put() function is present
            #
            # Added by commit 11f1cec ("interconnect: Add generic on-chip
            # interconnect API")
            #
            CODE="
            #if defined(NV_LINUX_INTERCONNECT_H_PRESENT)
            #include <linux/interconnect.h>
            #endif
            void conftest_icc_put(void)
            {
                icc_put();
            }
            "

            compile_check_conftest "$CODE" "NV_ICC_PUT_PRESENT" "" "functions"
        ;;

        i2c_new_client_device)
            #
            # Determine if i2c_new_client_device() function is present
            #
            # Added by commit 390fd04i2c ("remove deprecated i2c_new_device API")
            #
            CODE="
            #include <linux/i2c.h>
            void conftest_i2c_new_client_device(void)
            {
                i2c_new_client_device();
            }
            "

            compile_check_conftest "$CODE" "NV_I2C_NEW_CLIENT_DEVICE_PRESENT" "" "functions"
        ;;

        i2c_unregister_device)
            #
            # Determine if i2c_unregister_device() function is present
            #
            # Added by commit 9c1600ei2c ("Add i2c_board_info and i2c_new_device()")
            #
            CODE="
            #include <linux/i2c.h>
            void conftest_i2c_unregister_device(void)
            {
                i2c_unregister_device();
            }
            "

            compile_check_conftest "$CODE" "NV_I2C_UNREGISTER_DEVICE_PRESENT" "" "functions"
        ;;

        of_get_named_gpio)
            #
            # Determine if of_get_named_gpio() function is present
            #
            # Added by commit a6b0919 ("of/gpio: Add new method for getting gpios
            # under different property names")
            #
            CODE="
            #if defined(NV_LINUX_OF_GPIO_H_PRESENT)
            #include <linux/of_gpio.h>
            #endif
            void conftest_of_get_named_gpio(void)
            {
                of_get_named_gpio();
            }
            "

            compile_check_conftest "$CODE" "NV_OF_GET_NAME_GPIO_PRESENT" "" "functions"
        ;;

        devm_gpio_request_one)
            #
            # Determine if devm_gpio_request_one() function is present
            #
            # Added by commit 09d71ff (gpiolib: Implement devm_gpio_request_one()")
            #
            CODE="
            #if defined(NV_LINUX_GPIO_H_PRESENT)
            #include <linux/gpio.h>
            #endif
            void conftest_devm_gpio_request_one(void)
            {
                devm_gpio_request_one();
            }
            "

            compile_check_conftest "$CODE" "NV_DEVM_GPIO_REQUEST_ONE_PRESENT" "" "functions"
        ;;

        gpio_direction_input)
            #
            # Determine if gpio_direction_input() function is present
            #
            # Added by commit c7caf86 (gpio: remove gpio_ensure_requested()")
            #
            CODE="
            #if defined(NV_LINUX_GPIO_H_PRESENT)
            #include <linux/gpio.h>
            #endif
            void conftest_gpio_direction_input(void)
            {
                gpio_direction_input();
            }
            "

            compile_check_conftest "$CODE" "NV_GPIO_DIRECTION_INPUT_PRESENT" "" "functions"
        ;;

        gpio_direction_output)
            #
            # Determine if gpio_direction_output() function is present
            #
            # Added by commit c7caf86 (gpio: remove gpio_ensure_requested()")
            #
            CODE="
            #if defined(NV_LINUX_GPIO_H_PRESENT)
            #include <linux/gpio.h>
            #endif
            void conftest_gpio_direction_output(void)
            {
                gpio_direction_output();
            }
            "

            compile_check_conftest "$CODE" "NV_GPIO_DIRECTION_OUTPUT_PRESENT" "" "functions"
        ;;

        gpio_get_value)
            #
            # Determine if gpio_get_value() function is present
            #
            # Added by commit 7563bbf ("gpiolib/arches: Centralise bolierplate
            # asm/gpio.h")
            #
            CODE="
            #if defined(NV_LINUX_GPIO_H_PRESENT)
            #include <linux/gpio.h>
            #endif
            void conftest_gpio_get_value(void)
            {
                gpio_get_value();
            }
            "

            compile_check_conftest "$CODE" "NV_GPIO_GET_VALUE_PRESENT" "" "functions"
        ;;

        gpio_set_value)
            #
            # Determine if gpio_set_value() function is present
            #
            # Added by commit 7563bbf ("gpiolib/arches: Centralise bolierplate
            # asm/gpio.h")
            #
            CODE="
            #if defined(NV_LINUX_GPIO_H_PRESENT)
            #include <linux/gpio.h>
            #endif
            void conftest_gpio_set_value(void)
            {
                gpio_set_value();
            }
            "

            compile_check_conftest "$CODE" "NV_GPIO_SET_VALUE_PRESENT" "" "functions"
        ;;

        gpio_to_irq)
            #
            # Determine if gpio_to_irq() function is present
            #
            # Added by commit 7563bbf ("gpiolib/arches: Centralise bolierplate
            # asm/gpio.h")
            #
            CODE="
            #if defined(NV_LINUX_GPIO_H_PRESENT)
            #include <linux/gpio.h>
            #endif
            void conftest_gpio_to_irq(void)
            {
                gpio_to_irq();
            }
            "

            compile_check_conftest "$CODE" "NV_GPIO_TO_IRQ_PRESENT" "" "functions"
        ;;

        migrate_vma_setup)
            #
            # Determine if migrate_vma_setup() function is present
            #
            # migrate_vma_setup() function was added by commit
            # a7d1f22bb74f32cf3cd93f52776007e161f1a738 ("mm: turn migrate_vma
            # upside down) in v5.4.
            # (2019-08-20).
            CODE="
            #include <linux/migrate.h>
            int conftest_migrate_vma_setup(void) {
                migrate_vma_setup();
            }"

            compile_check_conftest "$CODE" "NV_MIGRATE_VMA_SETUP_PRESENT" "" "functions"
        ;;

        migrate_vma_added_flags)
            #
            # Determine if migrate_vma structure has flags
            #
            # flags were added to struct migrate_vma by commit
            # 5143192cd410c4fc83be09a2e73423765aee072b ("mm/migrate: add a flags
            # parameter to_migrate_vma) in v5.9.
            # (2020-07-28).
            CODE="
            #include <linux/migrate.h>
            int conftest_migrate_vma_added_flags(void) {
                return offsetof(struct migrate_vma, flags);
            }"

            compile_check_conftest "$CODE" "NV_MIGRATE_VMA_FLAGS_PRESENT" "" "types"
        ;;

        drm_device_has_pdev)
            #
            # Determine if the 'drm_device' structure has a 'pdev' field.
            #
            # Removed by commit b347e04452ff ("drm: Remove pdev field from
            # struct drm_device") in v5.14-rc1.
            #
            CODE="
            #if defined(NV_DRM_DRMP_H_PRESENT)
            #include <drm/drmP.h>
            #endif

            #if defined(NV_DRM_DRM_DEVICE_H_PRESENT)
            #include <drm/drm_device.h>
            #endif

            int conftest_drm_device_has_pdev(void) {
                return offsetof(struct drm_device, pdev);
            }"

            compile_check_conftest "$CODE" "NV_DRM_DEVICE_HAS_PDEV" "" "types"
        ;;

        make_device_exclusive_range)
            #
            # Determine if the make_device_exclusive_range() function is present
            #
            # make_device_exclusive_range() function was added by commit
            # b756a3b5e7ead ("mm: device exclusive memory access") in v5.14
            # (2021-06-30).
            CODE="
            #include <linux/rmap.h>
            int conftest_make_device_exclusive_range(void) {
                make_device_exclusive_range();
            }"

            compile_check_conftest "$CODE" "NV_MAKE_DEVICE_EXCLUSIVE_RANGE_PRESENT" "" "functions"
        ;;

        ioasid_get)
            #
            # Determine if ioasid_get() function is present
            #
            # ioasid_get() function was added by commit
            # cb4789b0d19ff231ce9f73376a023341300aed96 (iommu/ioasid: Add ioasidreferences) in v5.11.
            # (2020-11-23).
            CODE="
            #if defined(NV_LINUX_IOASID_H_PRESENT)
            #include <linux/ioasid.h>
            #endif
            void conftest_ioasid_get(void) {
                ioasid_get();
            }"

            compile_check_conftest "$CODE" "NV_IOASID_GET_PRESENT" "" "functions"
        ;;

        drm_crtc_state_has_no_vblank)
            #
            # Determine if the 'drm_crtc_state' structure has 'no_vblank'.
            #
            # drm_crtc_state::no_vblank was added by commit b25c60af7a877
            # ("drm/crtc: Add a generic infrastructure to fake VBLANK events")
            # in 4.18.0-rc3 (2018-07-03).
            #
            CODE="
            #include <drm/drm_crtc.h>
            void conftest_drm_crtc_state_has_no_vblank(void) {
                struct drm_crtc_state foo;
                (void)foo.no_vblank;
            }"

            compile_check_conftest "$CODE" "NV_DRM_CRTC_STATE_HAS_NO_VBLANK" "" "types"
        ;;

        drm_mode_config_has_allow_fb_modifiers)
            #
            # Determine if the 'drm_mode_config' structure has
            # an 'allow_fb_modifiers' field.
            #
            # an 'allow_fb_modifiers' field in the 'drm_mode_config' structure,
            # is added by commit e3eb3250d84e ("drm: add support for
            # tiled/compressed/etc modifier in addfb2") in v4.1, and removed by
            # commit 3d082157a242 ("drm: remove allow_fb_modifiers") in v5.18-rc1.
            #
            # The 'struct drm_mode_config' definition, is moved to
            # drm_mode_config.h file by commit 28575f165d36 ("drm: Extract
            # drm_mode_config.[hc]") in v4.10.
            #
            CODE="$CONFTEST_PREAMBLE
            #if defined(NV_DRM_DRM_MODE_CONFIG_H_PRESENT)
            #include <drm/drm_mode_config.h>
            #else
            #include <drm/drm_crtc.h>
            #endif
            int conftest_drm_mode_config_has_allow_fb_modifiers(void) {
                return offsetof(struct drm_mode_config, allow_fb_modifiers);
            }"

            compile_check_conftest "$CODE" "NV_DRM_MODE_CONFIG_HAS_ALLOW_FB_MODIFIERS" "" "types"
        ;;

        dma_set_mask_and_coherent)
            #
            # Determine if dma_set_mask_and_coherent function is present.
            # Added by commit 4aa806b771d1 ("DMA-API: provide a helper to set both DMA
            # and coherent DMA masks") in v3.13 (2013-06-26).
            #
            CODE="
            #include <linux/dma-mapping.h>
            void conftest_dma_set_mask_and_coherent(void) {
                dma_set_mask_and_coherent();
            }"

            compile_check_conftest "$CODE" "NV_DMA_SET_MASK_AND_COHERENT_PRESENT" "" "functions"
        ;;

        drm_has_hdr_output_metadata)
            #
            # Determine if drm_mode.h has 'hdr_output_metadata' structure.
            #
            # struct hdr_output_metadata was added by commit fbb5d0353c62d
            # ("drm: Add HDR source metadata property") in 5.1.0-rc5
            # (2019-05-16)
            #
            CODE="
            #include <drm/drm_mode.h>
            void conftest_drm_has_hdr_output_metadata(void) {
                struct hdr_output_metadata foo;
                (void)foo;
            }"

            compile_check_conftest "$CODE" "NV_DRM_HAS_HDR_OUTPUT_METADATA" "" "types"
        ;;

        uts_release)
            #
            # print the kernel's UTS_RELEASE string.
            #
            echo "#include <generated/utsrelease.h>
            UTS_RELEASE" > conftest$$.c

            $CC $CFLAGS -E -P conftest$$.c
            rm -f conftest$$.c
        ;;

        acpi_bus_get_device)
            #
            # Determine if the acpi_bus_get_device() function is present
            #
            # acpi_bus_get_device() was removed by commit ac2a3feefad5
            # ("ACPI: bus: Eliminate acpi_bus_get_device()") in
            # v5.18-rc2 (2022-04-05).
            #
            CODE="
            #include <linux/acpi.h>
            int conftest_acpi_bus_get_device(void) {
                return acpi_bus_get_device();
            }"
            compile_check_conftest "$CODE" "NV_ACPI_BUS_GET_DEVICE_PRESENT" "" "functions"
        ;;

        dma_resv_add_fence)
            #
            # Determine if the dma_resv_add_fence() function is present.
            #
            # dma_resv_add_excl_fence() and dma_resv_add_shared_fence() were
            # removed and replaced with dma_resv_add_fence() by commit
            # 73511edf8b19 ("dma-buf: specify usage while adding fences to
            # dma_resv obj v7") in linux-next, expected in v5.19-rc1.
            #
            CODE="
            #if defined(NV_LINUX_DMA_RESV_H_PRESENT)
            #include <linux/dma-resv.h>
            #endif
            void conftest_dma_resv_add_fence(void) {
                dma_resv_add_fence();
            }"

            compile_check_conftest "$CODE" "NV_DMA_RESV_ADD_FENCE_PRESENT" "" "functions"
        ;;

        dma_resv_reserve_fences)
            #
            # Determine if the dma_resv_reserve_fences() function is present.
            #
            # dma_resv_reserve_shared() was removed and replaced with
            # dma_resv_reserve_fences() by commit c8d4c18bfbc4
            # ("dma-buf/drivers: make reserving a shared slot mandatory v4") in
            # linux-next, expected in v5.19-rc1.
            #
            CODE="
            #if defined(NV_LINUX_DMA_RESV_H_PRESENT)
            #include <linux/dma-resv.h>
            #endif
            void conftest_dma_resv_reserve_fences(void) {
                dma_resv_reserve_fences();
            }"

            compile_check_conftest "$CODE" "NV_DMA_RESV_RESERVE_FENCES_PRESENT" "" "functions"
        ;;

        reservation_object_reserve_shared_has_num_fences_arg)
            #
            # Determine if reservation_object_reserve_shared() has 'num_fences'
            # argument.
            #
            # reservation_object_reserve_shared() function prototype was updated
            # to take 'num_fences' argument by commit ca05359f1e64 ("dma-buf:
            # allow reserving more than one shared fence slot") in v4.21-rc1
            # (2018-12-14).
            #
            CODE="
            #include <linux/reservation.h>
            void conftest_reservation_object_reserve_shared_has_num_fences_arg(
                    struct reservation_object *obj,
                    unsigned int num_fences) {
                (void) reservation_object_reserve_shared(obj, num_fences);
            }"

            compile_check_conftest "$CODE" "NV_RESERVATION_OBJECT_RESERVE_SHARED_HAS_NUM_FENCES_ARG" "" "types"
        ;;

        # When adding a new conftest entry, please use the correct format for
        # specifying the relevant upstream Linux kernel commit.
        #
        # <function> was added|removed|etc by commit <sha> ("<commit message")
        # in <kernel-version> (<commit date>).

        *)
            # Unknown test name given
            echo "Error: unknown conftest '$1' requested" >&2
            exit 1
        ;;
    esac
}

case "$5" in
    cc_sanity_check)
        #
        # Check if the selected compiler can create object files
        # in the current environment.
        #
        VERBOSE=$6

        echo "int cc_sanity_check(void) {
            return 0;
        }" > conftest$$.c

        $CC -c conftest$$.c > /dev/null 2>&1
        rm -f conftest$$.c

        if [ ! -f conftest$$.o ]; then
            if [ "$VERBOSE" = "full_output" ]; then
                echo "";
            fi
            if [ "$CC" != "cc" ]; then
                echo "The C compiler '$CC' does not appear to be able to"
                echo "create object files.  Please make sure you have "
                echo "your Linux distribution's libc development package"
                echo "installed and that '$CC' is a valid C compiler";
                echo "name."
            else
                echo "The C compiler '$CC' does not appear to be able to"
                echo "create executables.  Please make sure you have "
                echo "your Linux distribution's gcc and libc development"
                echo "packages installed."
            fi
            if [ "$VERBOSE" = "full_output" ]; then
                echo "";
                echo "*** Failed CC sanity check. Bailing out! ***";
                echo "";
            fi
            exit 1
        else
            rm -f conftest$$.o
            exit 0
        fi
    ;;

    cc_version_check)
        #
        # Verify that the same compiler major and minor version is
        # used for the kernel and kernel module. A mismatch condition is
        # not considered fatal, so this conftest returns a success status
        # code, even if it fails. Failure of the test can be distinguished
        # by testing for empty (success) versus non-empty (failure) output.
        #
        # Some gcc version strings that have proven problematic for parsing
        # in the past:
        #
        #  gcc.real (GCC) 3.3 (Debian)
        #  gcc-Version 3.3 (Debian)
        #  gcc (GCC) 3.1.1 20020606 (Debian prerelease)
        #  version gcc 3.2.3
        #
        #  As of this writing, GCC uses a version number as x.y.z and below
        #  are the typical version strings seen with various distributions.
        #  gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-23)
        #  gcc version 4.8.5 20150623 (Red Hat 4.8.5-39) (GCC)
        #  gcc (GCC) 8.3.1 20190507 (Red Hat 8.3.1-4)
        #  gcc (GCC) 10.2.1 20200723 (Red Hat 10.2.1-1)
        #  gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
        #  gcc (Ubuntu 7.5.0-3ubuntu1~16.04) 7.5.0
        #  gcc (Debian 8.3.0-6) 8.3.0
        #  aarch64-linux-gcc.br_real (Buildroot 2020.08-14-ge5a2a90) 9.3.0, GNU ld (GNU Binutils) 2.33.1
        #
        #  In order to extract GCC version correctly for version strings
        #  like the last one above, we first check for x.y.z and if that
        #  fails, we fallback to x.y format.
        VERBOSE=$6

        kernel_compile_h=$OUTPUT/include/generated/compile.h

        if [ ! -f ${kernel_compile_h} ]; then
            # The kernel's compile.h file is not present, so there
            # isn't a convenient way to identify the compiler version
            # used to build the kernel.
            IGNORE_CC_MISMATCH=1
        fi

        if [ -n "$IGNORE_CC_MISMATCH" ]; then
            exit 0
        fi

        kernel_cc_string=`cat ${kernel_compile_h} | \
            grep LINUX_COMPILER | cut -f 2 -d '"'`

        kernel_cc_version=`echo ${kernel_cc_string} | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -n 1`
        if [ -z "${kernel_cc_version}" ]; then
            kernel_cc_version=`echo ${kernel_cc_string} | grep -o '[0-9]\+\.[0-9]\+' | head -n 1`
        fi
        kernel_cc_major=`echo ${kernel_cc_version} | cut -d '.' -f 1`
        kernel_cc_minor=`echo ${kernel_cc_version} | cut -d '.' -f 2`

        echo "
        #if (__GNUC__ != ${kernel_cc_major}) || (__GNUC_MINOR__ != ${kernel_cc_minor})
        #error \"cc version mismatch\"
        #endif
        " > conftest$$.c

        $CC $CFLAGS -c conftest$$.c > /dev/null 2>&1
        rm -f conftest$$.c

        if [ -f conftest$$.o ]; then
            rm -f conftest$$.o
            exit 0;
        else
            #
            # The gcc version check failed
            #

            if [ "$VERBOSE" = "full_output" ]; then
                echo "";
                echo "Warning: Compiler version check failed:";
                echo "";
                echo "The major and minor number of the compiler used to";
                echo "compile the kernel:";
                echo "";
                echo "${kernel_cc_string}";
                echo "";
                echo "does not match the compiler used here:";
                echo "";
                $CC --version
                echo "";
                echo "It is recommended to set the CC environment variable";
                echo "to the compiler that was used to compile the kernel.";
                echo ""
                echo "To skip the test and silence this warning message, set";
                echo "the IGNORE_CC_MISMATCH environment variable to \"1\".";
                echo "However, mixing compiler versions between the kernel";
                echo "and kernel modules can result in subtle bugs that are";
                echo "difficult to diagnose.";
                echo "";
                echo "*** Failed CC version check. ***";
                echo "";
            elif [ "$VERBOSE" = "just_msg" ]; then
                echo "Warning: The kernel was built with ${kernel_cc_string}, but the" \
                     "current compiler version is `$CC --version | head -n 1`.";
            fi
            exit 0;
        fi
    ;;

    xen_sanity_check)
        #
        # Check if the target kernel is a Xen kernel. If so, exit, since
        # the RM doesn't currently support Xen.
        #
        VERBOSE=$6

        if [ -n "$IGNORE_XEN_PRESENCE" -o -n "$VGX_BUILD" ]; then
            exit 0
        fi

        test_xen

        if [ "$XEN_PRESENT" != "0" ]; then
            echo "The kernel you are installing for is a Xen kernel!";
            echo "";
            echo "The NVIDIA driver does not currently support Xen kernels. If ";
            echo "you are using a stock distribution kernel, please install ";
            echo "a variant of this kernel without Xen support; if this is a ";
            echo "custom kernel, please install a standard Linux kernel.  Then ";
            echo "try installing the NVIDIA kernel module again.";
            echo "";
            if [ "$VERBOSE" = "full_output" ]; then
                echo "*** Failed Xen sanity check. Bailing out! ***";
                echo "";
            fi
            exit 1
        else
            exit 0
        fi
    ;;

    preempt_rt_sanity_check)
        #
        # Check if the target kernel has the PREEMPT_RT patch set applied. If
        # so, exit, since the RM doesn't support this configuration.
        #
        VERBOSE=$6

        if [ -n "$IGNORE_PREEMPT_RT_PRESENCE" ]; then
            exit 0
        fi

        if test_configuration_option CONFIG_PREEMPT_RT; then
            PREEMPT_RT_PRESENT=1
        elif test_configuration_option CONFIG_PREEMPT_RT_FULL; then
            PREEMPT_RT_PRESENT=1
        fi

        if [ "$PREEMPT_RT_PRESENT" != "0" ]; then
            echo "The kernel you are installing for is a PREEMPT_RT kernel!";
            echo "";
            echo "The NVIDIA driver does not support real-time kernels. If you ";
            echo "are using a stock distribution kernel, please install ";
            echo "a variant of this kernel that does not have the PREEMPT_RT ";
            echo "patch set applied; if this is a custom kernel, please ";
            echo "install a standard Linux kernel.  Then try installing the ";
            echo "NVIDIA kernel module again.";
            echo "";
            if [ "$VERBOSE" = "full_output" ]; then
                echo "*** Failed PREEMPT_RT sanity check. Bailing out! ***";
                echo "";
            fi
            exit 1
        else
            exit 0
        fi
    ;;

    patch_check)
        #
        # Check for any "official" patches that may have been applied and
        # construct a description table for reporting purposes.
        #
        PATCHES=""

        for PATCH in patch-*.h; do
            if [ -f $PATCH ]; then
                echo "#include \"$PATCH\""
                PATCHES="$PATCHES "`echo $PATCH | sed -s 's/patch-\(.*\)\.h/\1/'`
            fi
        done

        echo "static struct {
                const char *short_description;
                const char *description;
              } __nv_patches[] = {"
            for i in $PATCHES; do
                echo "{ \"$i\", NV_PATCH_${i}_DESCRIPTION },"
            done
        echo "{ NULL, NULL } };"

        exit 0
    ;;

    compile_tests)
        #
        # Run a series of compile tests to determine the set of interfaces
        # and features available in the target kernel.
        #
        shift 5

        CFLAGS=$1
        shift

        for i in $*; do compile_test $i; done

        for file in conftest*.d; do
            rm -f $file > /dev/null 2>&1
        done

        exit 0
    ;;

    dom0_sanity_check)
        #
        # Determine whether running in DOM0.
        #
        VERBOSE=$6

        if [ -n "$VGX_BUILD" ]; then
            if [ -f /proc/xen/capabilities ]; then
                if [ "`cat /proc/xen/capabilities`" == "control_d" ]; then
                    exit 0
                fi
            else
                echo "The kernel is not running in DOM0.";
                echo "";
                if [ "$VERBOSE" = "full_output" ]; then
                    echo "*** Failed DOM0 sanity check. Bailing out! ***";
                    echo "";
                fi
            fi
            exit 1
        fi
    ;;
    vgpu_kvm_sanity_check)
        #
        # Determine whether we are running a vGPU on KVM host.
        #
        VERBOSE=$6
        iommu=CONFIG_VFIO_IOMMU_TYPE1
        mdev=CONFIG_VFIO_MDEV
        kvm=CONFIG_KVM_VFIO
        VFIO_IOMMU_PRESENT=0
        VFIO_MDEV_PRESENT=0
        KVM_PRESENT=0

        if [ -n "$VGX_KVM_BUILD" ]; then
            if (test_configuration_option ${iommu} || test_configuration_option ${iommu}_MODULE); then
                VFIO_IOMMU_PRESENT=1
            fi

            if (test_configuration_option ${mdev} || test_configuration_option ${mdev}_MODULE); then
                VFIO_MDEV_PRESENT=1
            fi

            if (test_configuration_option ${kvm} || test_configuration_option ${kvm}_MODULE); then
                KVM_PRESENT=1
            fi

            if [ "$VFIO_IOMMU_PRESENT" != "0" ] &&
               [ "$VFIO_MDEV_PRESENT" != "0" ] &&
               [ "$KVM_PRESENT" != "0" ] ; then
                    exit 0
            else
                echo "Below CONFIG options are missing on the kernel for installing";
                echo "NVIDIA vGPU driver on KVM host";
                if [ "$VFIO_IOMMU_PRESENT" = "0" ]; then
                    echo "CONFIG_VFIO_IOMMU_TYPE1";
                fi

                if [ "$VFIO_MDEV_PRESENT" = "0" ]; then
                    echo "CONFIG_VFIO_MDEV";
                fi

                if [ "$KVM_PRESENT" = "0" ]; then
                    echo "CONFIG_KVM";
                fi
                echo "Please install the kernel with above CONFIG options set, then";
                echo "try installing again";
                echo "";

                if [ "$VERBOSE" = "full_output" ]; then
                    echo "*** Failed vGPU on KVM sanity check. Bailing out! ***";
                    echo "";
                fi
            fi
            exit 1
        else
            exit 0
        fi
    ;;
    test_configuration_option)
        #
        # Check to see if the given config option is set.
        #
        OPTION=$6

        test_configuration_option $OPTION
        exit $?
    ;;

    get_configuration_option)
        #
        # Get the value of the given config option.
        #
        OPTION=$6

        get_configuration_option $OPTION
        exit $?
    ;;


    guess_module_signing_hash)
        #
        # Determine the best cryptographic hash to use for module signing,
        # to the extent that is possible.
        #

        HASH=$(get_configuration_option CONFIG_MODULE_SIG_HASH)

        if [ $? -eq 0 ] && [ -n "$HASH" ]; then
            echo $HASH
            exit 0
        else
            for SHA in 512 384 256 224 1; do
                if test_configuration_option CONFIG_MODULE_SIG_SHA$SHA; then
                    echo sha$SHA
                    exit 0
                fi
            done
        fi
        exit 1
    ;;


    test_kernel_headers)
        #
        # Check for the availability of certain kernel headers
        #

        CFLAGS=$6

        test_headers

        for file in conftest*.d; do
            rm -f $file > /dev/null 2>&1
        done

        exit $?
    ;;


    build_cflags)
        #
        # Generate CFLAGS for use in the compile tests
        #

        build_cflags
        echo $CFLAGS
        exit 0
    ;;

    module_symvers_sanity_check)
        #
        # Check whether Module.symvers exists and contains at least one
        # EXPORT_SYMBOL* symbol from vmlinux
        #

        if [ -n "$IGNORE_MISSING_MODULE_SYMVERS" ]; then
            exit 0
        fi

        TAB='	'

        if [ -f "$OUTPUT/Module.symvers" ] && \
             grep -e "^[^${TAB}]*${TAB}[^${TAB}]*${TAB}\+vmlinux" \
                     "$OUTPUT/Module.symvers" >/dev/null 2>&1; then
            exit 0
        fi

        echo "The Module.symvers file is missing, or does not contain any"
        echo "symbols exported from the kernel. This could cause the NVIDIA"
        echo "kernel modules to be built against a configuration that does"
        echo "not accurately reflect the actual target kernel."
        echo "The Module.symvers file check can be disabled by setting the"
        echo "environment variable IGNORE_MISSING_MODULE_SYMVERS to 1."

        exit 1
    ;;
esac
