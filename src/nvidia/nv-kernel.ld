/*
 * resman linker script
 *
 * Linking nv-kernel.o has several problems:
 *
 * (1) We build with '-ffunction-sections -fdata-sections' to put each
 * function and data into separate ELF sections, so that the linker
 * can distinguish separate functions and garbage collect dead code
 * ('--gc-sections').  The linker is supposed to then merge sections
 * together (e.g., all the ".text.*" into ".text", all the ".data.*"
 * sections into ".data").  The linker doesn't seem to do this when
 * linking a relocatable object file.
 *
 * (2) g++ puts inline functions, vtables, template functions, etc, in
 * separate ".gnu.linkonce.*" sections.  Duplicates are supposed to get
 * collapsed at link time.  The linker doesn't seem to do this when
 * linking a relocatable object file.
 *
 * Resolve both of these problems by defining our own naive linker
 * script to do the merging described above.
 */

SECTIONS {

  .text   : { *(.text) *(.text.*) *(.gnu.linkonce.t.*) }

  .data   : { *(.data) *(.data.*) }

  .rodata : { *(.rodata) *(.rodata.*) *(.gnu.linkonce.r.*) }

  .bss    : { *(.bss) *(.bss.*) }

  /* The rest of the sections ("orphaned sections") will just be copied from
     the input to the output */
}
