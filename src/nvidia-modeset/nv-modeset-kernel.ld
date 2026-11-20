/*
 * nv-modeset-kernel.o linker script
 *
 * g++ puts inline functions, vtables, template functions, etc, in separate
 * ".gnu.linkonce.*" / ".text.*" /... sections even without '-ffunction-sections
 * -fdata-sections'.  Duplicates will be collapsed at link time via
 * '--force-group-allocation'.  However, the weird section names will still be
 * preserved which just increases module load time.

 * Merge these sections with this little linker script.
 *
 * C++ symbols/sections get filtered by their common '_Z' prefix, demanded by
 * the Itanium ABI.
 */

SECTIONS {

  .text   : { *(.text) *(.text._Z*) *(.gnu.linkonce.t._Z*) }

  .rodata : { *(.rodata) *(.rodata._Z*) *(.gnu.linkonce.r._Z*) }

  .data   : { *(.data) *(.data._Z*) }

  .bss    : { *(.bss) *(.bss._Z*) }

  /* The rest of the sections ("orphaned sections") will just be copied from
     the input to the output */
}
