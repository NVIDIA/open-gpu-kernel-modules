// SPDX-License-Identifier: GPL-2.0
#include <linux/compiler.h>
#include <linux/string.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <jvmti.h>
#ifdef HAVE_JVMTI_CMLR
#include <jvmticmlr.h>
#endif
#include <limits.h>

#include "jvmti_agent.h"

static int has_line_numbers;
void *jvmti_agent;

static void print_error(jvmtiEnv *jvmti, const char *msg, jvmtiError ret)
{
	char *err_msg = NULL;
	jvmtiError err;
	err = (*jvmti)->GetErrorName(jvmti, ret, &err_msg);
	if (err == JVMTI_ERROR_NONE) {
		warnx("%s failed with %s", msg, err_msg);
		(*jvmti)->Deallocate(jvmti, (unsigned char *)err_msg);
	} else {
		warnx("%s failed with an unknown error %d", msg, ret);
	}
}

#ifdef HAVE_JVMTI_CMLR
static jvmtiError
do_get_line_number(jvmtiEnv *jvmti, void *pc, jmethodID m, jint bci,
		   jvmti_line_info_t *tab)
{
	jint i, nr_lines = 0;
	jvmtiLineNumberEntry *loc_tab = NULL;
	jvmtiError ret;
	jint src_line = -1;

	ret = (*jvmti)->GetLineNumberTable(jvmti, m, &nr_lines, &loc_tab);
	if (ret == JVMTI_ERROR_ABSENT_INFORMATION || ret == JVMTI_ERROR_NATIVE_METHOD) {
		/* No debug information for this method */
		return ret;
	} else if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "GetLineNumberTable", ret);
		return ret;
	}

	for (i = 0; i < nr_lines && loc_tab[i].start_location <= bci; i++) {
		src_line = i;
	}

	if (src_line != -1) {
		tab->pc = (unsigned long)pc;
		tab->line_number = loc_tab[src_line].line_number;
		tab->discrim = 0; /* not yet used */
		tab->methodID = m;

		ret = JVMTI_ERROR_NONE;
	} else {
		ret = JVMTI_ERROR_ABSENT_INFORMATION;
	}

	(*jvmti)->Deallocate(jvmti, (unsigned char *)loc_tab);

	return ret;
}

static jvmtiError
get_line_numbers(jvmtiEnv *jvmti, const void *compile_info, jvmti_line_info_t **tab, int *nr_lines)
{
	const jvmtiCompiledMethodLoadRecordHeader *hdr;
	jvmtiCompiledMethodLoadInlineRecord *rec;
	PCStackInfo *c;
	jint ret;
	int nr_total = 0;
	int i, lines_total = 0;

	if (!(tab && nr_lines))
		return JVMTI_ERROR_NULL_POINTER;

	/*
	 * Phase 1 -- get the number of lines necessary
	 */
	for (hdr = compile_info; hdr != NULL; hdr = hdr->next) {
		if (hdr->kind == JVMTI_CMLR_INLINE_INFO) {
			rec = (jvmtiCompiledMethodLoadInlineRecord *)hdr;
			nr_total += rec->numpcs;
		}
	}

	if (nr_total == 0)
		return JVMTI_ERROR_NOT_FOUND;

	/*
	 * Phase 2 -- allocate big enough line table
	 */
	*tab = malloc(nr_total * sizeof(**tab));
	if (!*tab)
		return JVMTI_ERROR_OUT_OF_MEMORY;

	for (hdr = compile_info; hdr != NULL; hdr = hdr->next) {
		if (hdr->kind == JVMTI_CMLR_INLINE_INFO) {
			rec = (jvmtiCompiledMethodLoadInlineRecord *)hdr;
			for (i = 0; i < rec->numpcs; i++) {
				c = rec->pcinfo + i;
                                /*
                                 * c->methods is the stack of inlined method calls
                                 * at c->pc. [0] is the leaf method. Caller frames
                                 * are ignored at the moment.
                                 */
				ret = do_get_line_number(jvmti, c->pc,
							 c->methods[0],
							 c->bcis[0],
							 *tab + lines_total);
				if (ret == JVMTI_ERROR_NONE)
					lines_total++;
			}
		}
	}
	*nr_lines = lines_total;
	return JVMTI_ERROR_NONE;
}
#else /* HAVE_JVMTI_CMLR */

static jvmtiError
get_line_numbers(jvmtiEnv *jvmti __maybe_unused, const void *compile_info __maybe_unused,
		 jvmti_line_info_t **tab __maybe_unused, int *nr_lines __maybe_unused)
{
	return JVMTI_ERROR_NONE;
}
#endif /* HAVE_JVMTI_CMLR */

static void
copy_class_filename(const char * class_sign, const char * file_name, char * result, size_t max_length)
{
	/*
	* Assume path name is class hierarchy, this is a common practice with Java programs
	*/
	if (*class_sign == 'L') {
		int j, i = 0;
		char *p = strrchr(class_sign, '/');
		if (p) {
			/* drop the 'L' prefix and copy up to the final '/' */
			for (i = 0; i < (p - class_sign); i++)
				result[i] = class_sign[i+1];
		}
		/*
		* append file name, we use loops and not string ops to avoid modifying
		* class_sign which is used later for the symbol name
		*/
		for (j = 0; i < (max_length - 1) && file_name && j < strlen(file_name); j++, i++)
			result[i] = file_name[j];

		result[i] = '\0';
	} else {
		/* fallback case */
		strlcpy(result, file_name, max_length);
	}
}

static jvmtiError
get_source_filename(jvmtiEnv *jvmti, jmethodID methodID, char ** buffer)
{
	jvmtiError ret;
	jclass decl_class;
	char *file_name = NULL;
	char *class_sign = NULL;
	char fn[PATH_MAX];
	size_t len;

	ret = (*jvmti)->GetMethodDeclaringClass(jvmti, methodID, &decl_class);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "GetMethodDeclaringClass", ret);
		return ret;
	}

	ret = (*jvmti)->GetSourceFileName(jvmti, decl_class, &file_name);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "GetSourceFileName", ret);
		return ret;
	}

	ret = (*jvmti)->GetClassSignature(jvmti, decl_class, &class_sign, NULL);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "GetClassSignature", ret);
		goto free_file_name_error;
	}

	copy_class_filename(class_sign, file_name, fn, PATH_MAX);
	len = strlen(fn);
	*buffer = malloc((len + 1) * sizeof(char));
	if (!*buffer) {
		print_error(jvmti, "GetClassSignature", ret);
		ret = JVMTI_ERROR_OUT_OF_MEMORY;
		goto free_class_sign_error;
	}
	strcpy(*buffer, fn);
	ret = JVMTI_ERROR_NONE;

free_class_sign_error:
	(*jvmti)->Deallocate(jvmti, (unsigned char *)class_sign);
free_file_name_error:
	(*jvmti)->Deallocate(jvmti, (unsigned char *)file_name);

	return ret;
}

static jvmtiError
fill_source_filenames(jvmtiEnv *jvmti, int nr_lines,
		      const jvmti_line_info_t * line_tab,
		      char ** file_names)
{
	int index;
	jvmtiError ret;

	for (index = 0; index < nr_lines; ++index) {
		ret = get_source_filename(jvmti, line_tab[index].methodID, &(file_names[index]));
		if (ret != JVMTI_ERROR_NONE)
			return ret;
	}

	return JVMTI_ERROR_NONE;
}

static void JNICALL
compiled_method_load_cb(jvmtiEnv *jvmti,
			jmethodID method,
			jint code_size,
			void const *code_addr,
			jint map_length,
			jvmtiAddrLocationMap const *map,
			const void *compile_info)
{
	jvmti_line_info_t *line_tab = NULL;
	char ** line_file_names = NULL;
	jclass decl_class;
	char *class_sign = NULL;
	char *func_name = NULL;
	char *func_sign = NULL;
	uint64_t addr = (uint64_t)(uintptr_t)code_addr;
	jvmtiError ret;
	int nr_lines = 0; /* in line_tab[] */
	size_t len;
	int output_debug_info = 0;

	ret = (*jvmti)->GetMethodDeclaringClass(jvmti, method,
						&decl_class);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "GetMethodDeclaringClass", ret);
		return;
	}

	if (has_line_numbers && map && map_length) {
		ret = get_line_numbers(jvmti, compile_info, &line_tab, &nr_lines);
		if (ret != JVMTI_ERROR_NONE) {
			if (ret != JVMTI_ERROR_NOT_FOUND) {
				warnx("jvmti: cannot get line table for method");
			}
			nr_lines = 0;
		} else if (nr_lines > 0) {
			line_file_names = malloc(sizeof(char*) * nr_lines);
			if (!line_file_names) {
				warnx("jvmti: cannot allocate space for line table method names");
			} else {
				memset(line_file_names, 0, sizeof(char*) * nr_lines);
				ret = fill_source_filenames(jvmti, nr_lines, line_tab, line_file_names);
				if (ret != JVMTI_ERROR_NONE) {
					warnx("jvmti: fill_source_filenames failed");
				} else {
					output_debug_info = 1;
				}
			}
		}
	}

	ret = (*jvmti)->GetClassSignature(jvmti, decl_class,
					  &class_sign, NULL);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "GetClassSignature", ret);
		goto error;
	}

	ret = (*jvmti)->GetMethodName(jvmti, method, &func_name,
				      &func_sign, NULL);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "GetMethodName", ret);
		goto error;
	}

	/*
	 * write source line info record if we have it
	 */
	if (output_debug_info)
		if (jvmti_write_debug_info(jvmti_agent, addr, nr_lines, line_tab, (const char * const *) line_file_names))
			warnx("jvmti: write_debug_info() failed");

	len = strlen(func_name) + strlen(class_sign) + strlen(func_sign) + 2;
	{
		char str[len];
		snprintf(str, len, "%s%s%s", class_sign, func_name, func_sign);

		if (jvmti_write_code(jvmti_agent, str, addr, code_addr, code_size))
			warnx("jvmti: write_code() failed");
	}
error:
	(*jvmti)->Deallocate(jvmti, (unsigned char *)func_name);
	(*jvmti)->Deallocate(jvmti, (unsigned char *)func_sign);
	(*jvmti)->Deallocate(jvmti, (unsigned char *)class_sign);
	free(line_tab);
	while (line_file_names && (nr_lines > 0)) {
	    if (line_file_names[nr_lines - 1]) {
	        free(line_file_names[nr_lines - 1]);
	    }
	    nr_lines -= 1;
	}
	free(line_file_names);
}

static void JNICALL
code_generated_cb(jvmtiEnv *jvmti,
		  char const *name,
		  void const *code_addr,
		  jint code_size)
{
	uint64_t addr = (uint64_t)(unsigned long)code_addr;
	int ret;

	ret = jvmti_write_code(jvmti_agent, name, addr, code_addr, code_size);
	if (ret)
		warnx("jvmti: write_code() failed for code_generated");
}

JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *jvm, char *options, void *reserved __maybe_unused)
{
	jvmtiEventCallbacks cb;
	jvmtiCapabilities caps1;
	jvmtiJlocationFormat format;
	jvmtiEnv *jvmti = NULL;
	jint ret;

	jvmti_agent = jvmti_open();
	if (!jvmti_agent) {
		warnx("jvmti: open_agent failed");
		return -1;
	}

	/*
	 * Request a JVMTI interface version 1 environment
	 */
	ret = (*jvm)->GetEnv(jvm, (void *)&jvmti, JVMTI_VERSION_1);
	if (ret != JNI_OK) {
		warnx("jvmti: jvmti version 1 not supported");
		return -1;
	}

	/*
	 * acquire method_load capability, we require it
	 * request line numbers (optional)
	 */
	memset(&caps1, 0, sizeof(caps1));
	caps1.can_generate_compiled_method_load_events = 1;

	ret = (*jvmti)->AddCapabilities(jvmti, &caps1);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "AddCapabilities", ret);
		return -1;
	}
	ret = (*jvmti)->GetJLocationFormat(jvmti, &format);
        if (ret == JVMTI_ERROR_NONE && format == JVMTI_JLOCATION_JVMBCI) {
                memset(&caps1, 0, sizeof(caps1));
                caps1.can_get_line_numbers = 1;
                caps1.can_get_source_file_name = 1;
		ret = (*jvmti)->AddCapabilities(jvmti, &caps1);
                if (ret == JVMTI_ERROR_NONE)
                        has_line_numbers = 1;
        } else if (ret != JVMTI_ERROR_NONE)
		print_error(jvmti, "GetJLocationFormat", ret);


	memset(&cb, 0, sizeof(cb));

	cb.CompiledMethodLoad   = compiled_method_load_cb;
	cb.DynamicCodeGenerated = code_generated_cb;

	ret = (*jvmti)->SetEventCallbacks(jvmti, &cb, sizeof(cb));
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "SetEventCallbacks", ret);
		return -1;
	}

	ret = (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,
			JVMTI_EVENT_COMPILED_METHOD_LOAD, NULL);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "SetEventNotificationMode(METHOD_LOAD)", ret);
		return -1;
	}

	ret = (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,
			JVMTI_EVENT_DYNAMIC_CODE_GENERATED, NULL);
	if (ret != JVMTI_ERROR_NONE) {
		print_error(jvmti, "SetEventNotificationMode(CODE_GENERATED)", ret);
		return -1;
	}
	return 0;
}

JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM *jvm __maybe_unused)
{
	int ret;

	ret = jvmti_close(jvmti_agent);
	if (ret)
		errx(1, "Error: op_close_agent()");
}
