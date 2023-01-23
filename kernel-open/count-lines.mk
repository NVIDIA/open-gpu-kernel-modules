count:
	@echo "conftests:$(words $(ALL_CONFTESTS))" \
		  "objects:$(words $(NV_OBJECTS_DEPEND_ON_CONFTEST))" \
		  "modules:$(words $(NV_KERNEL_MODULES))"

.PHONY: count

# Include the top-level makefile to get $(NV_KERNEL_MODULES)
include Makefile

# Set $(src) for the to-be-included nvidia*.Kbuild files
src := $(CURDIR)

# Include nvidia*.Kbuild and append the nvidia*-y objects to ALL_OBJECTS
$(foreach _module, $(NV_KERNEL_MODULES),          \
     $(eval include $(_module)/$(_module).Kbuild) \
 )

# Concatenate all of the conftest lists; use $(sort ) to remove duplicates
ALL_CONFTESTS := $(sort $(NV_CONFTEST_FUNCTION_COMPILE_TESTS) \
                        $(NV_CONFTEST_GENERIC_COMPILE_TESTS)  \
                        $(NV_CONFTEST_MACRO_COMPILE_TESTS)    \
                        $(NV_CONFTEST_SYMBOL_COMPILE_TESTS)   \
                        $(NV_CONFTEST_TYPE_COMPILE_TESTS)     \
                  )
