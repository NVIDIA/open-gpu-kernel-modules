/*
 * Auxiliary resman linker script, for use with NV_KERNEL_TRACING=1 on aarch64.
 *
 * When profiling/tracing is enabled at compile time with gcc's
 * "-fpatchable-function-entry=4,2" option, each function entry is surrounded
 * by four NOP instructions (two before, two after).  The compiler also
 * generates a special ELF section, "__patchable_function_entries", which is
 * a table of pointers to each patchable entry point.  This is used by the
 * kernel's ftrace to patch in and out tracing calls at runtime.
 *
 * Nothing in nv-kernel.o references these sections, so --gc-sections
 * deadcode elimination would normally discard them.
 *
 * Augment our linker script with a directive to concatenate all the
 * __patchable_function_entries sections, and keep them regardless of
 * deadcode elimination.
 */
SECTIONS {
    __patchable_function_entries : { KEEP(*(__patchable_function_entries)) }
}
