// SPDX-License-Identifier: GPL-2.0
/*
 * It tests the mlock/mlock2() when they are invoked
 * on randomly memory region.
 */
#include <unistd.h>
#include <sys/resource.h>
#include <sys/capability.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "mlock2.h"

#define CHUNK_UNIT (128 * 1024)
#define MLOCK_RLIMIT_SIZE (CHUNK_UNIT * 2)
#define MLOCK_WITHIN_LIMIT_SIZE CHUNK_UNIT
#define MLOCK_OUTOF_LIMIT_SIZE (CHUNK_UNIT * 3)

#define TEST_LOOP 100
#define PAGE_ALIGN(size, ps) (((size) + ((ps) - 1)) & ~((ps) - 1))

int set_cap_limits(rlim_t max)
{
	struct rlimit new;
	cap_t cap = cap_init();

	new.rlim_cur = max;
	new.rlim_max = max;
	if (setrlimit(RLIMIT_MEMLOCK, &new)) {
		perror("setrlimit() returns error\n");
		return -1;
	}

	/* drop capabilities including CAP_IPC_LOCK */
	if (cap_set_proc(cap)) {
		perror("cap_set_proc() returns error\n");
		return -2;
	}

	return 0;
}

int get_proc_locked_vm_size(void)
{
	FILE *f;
	int ret = -1;
	char line[1024] = {0};
	unsigned long lock_size = 0;

	f = fopen("/proc/self/status", "r");
	if (!f) {
		perror("fopen");
		return -1;
	}

	while (fgets(line, 1024, f)) {
		if (strstr(line, "VmLck")) {
			ret = sscanf(line, "VmLck:\t%8lu kB", &lock_size);
			if (ret <= 0) {
				printf("sscanf() on VmLck error: %s: %d\n",
						line, ret);
				fclose(f);
				return -1;
			}
			fclose(f);
			return (int)(lock_size << 10);
		}
	}

	perror("cann't parse VmLck in /proc/self/status\n");
	fclose(f);
	return -1;
}

/*
 * Get the MMUPageSize of the memory region including input
 * address from proc file.
 *
 * return value: on error case, 0 will be returned.
 * Otherwise the page size(in bytes) is returned.
 */
int get_proc_page_size(unsigned long addr)
{
	FILE *smaps;
	char *line;
	unsigned long mmupage_size = 0;
	size_t size;

	smaps = seek_to_smaps_entry(addr);
	if (!smaps) {
		printf("Unable to parse /proc/self/smaps\n");
		return 0;
	}

	while (getline(&line, &size, smaps) > 0) {
		if (!strstr(line, "MMUPageSize")) {
			free(line);
			line = NULL;
			size = 0;
			continue;
		}

		/* found the MMUPageSize of this section */
		if (sscanf(line, "MMUPageSize:    %8lu kB",
					&mmupage_size) < 1) {
			printf("Unable to parse smaps entry for Size:%s\n",
					line);
			break;
		}

	}
	free(line);
	if (smaps)
		fclose(smaps);
	return mmupage_size << 10;
}

/*
 * Test mlock/mlock2() on provided memory chunk.
 * It expects the mlock/mlock2() to be successful (within rlimit)
 *
 * With allocated memory chunk [p, p + alloc_size), this
 * test will choose start/len randomly to perform mlock/mlock2
 * [start, start +  len] memory range. The range is within range
 * of the allocated chunk.
 *
 * The memory region size alloc_size is within the rlimit.
 * So we always expect a success of mlock/mlock2.
 *
 * VmLck is assumed to be 0 before this test.
 *
 *    return value: 0 - success
 *    else: failure
 */
int test_mlock_within_limit(char *p, int alloc_size)
{
	int i;
	int ret = 0;
	int locked_vm_size = 0;
	struct rlimit cur;
	int page_size = 0;

	getrlimit(RLIMIT_MEMLOCK, &cur);
	if (cur.rlim_cur < alloc_size) {
		printf("alloc_size[%d] < %u rlimit,lead to mlock failure\n",
				alloc_size, (unsigned int)cur.rlim_cur);
		return -1;
	}

	srand(time(NULL));
	for (i = 0; i < TEST_LOOP; i++) {
		/*
		 * - choose mlock/mlock2 randomly
		 * - choose lock_size randomly but lock_size < alloc_size
		 * - choose start_offset randomly but p+start_offset+lock_size
		 *   < p+alloc_size
		 */
		int is_mlock = !!(rand() % 2);
		int lock_size = rand() % alloc_size;
		int start_offset = rand() % (alloc_size - lock_size);

		if (is_mlock)
			ret = mlock(p + start_offset, lock_size);
		else
			ret = mlock2_(p + start_offset, lock_size,
				       MLOCK_ONFAULT);

		if (ret) {
			printf("%s() failure at |%p(%d)| mlock:|%p(%d)|\n",
					is_mlock ? "mlock" : "mlock2",
					p, alloc_size,
					p + start_offset, lock_size);
			return ret;
		}
	}

	/*
	 * Check VmLck left by the tests.
	 */
	locked_vm_size = get_proc_locked_vm_size();
	page_size = get_proc_page_size((unsigned long)p);
	if (page_size == 0) {
		printf("cannot get proc MMUPageSize\n");
		return -1;
	}

	if (locked_vm_size > PAGE_ALIGN(alloc_size, page_size) + page_size) {
		printf("test_mlock_within_limit() left VmLck:%d on %d chunk\n",
				locked_vm_size, alloc_size);
		return -1;
	}

	return 0;
}


/*
 * We expect the mlock/mlock2() to be fail (outof limitation)
 *
 * With allocated memory chunk [p, p + alloc_size), this
 * test will randomly choose start/len and perform mlock/mlock2
 * on [start, start+len] range.
 *
 * The memory region size alloc_size is above the rlimit.
 * And the len to be locked is higher than rlimit.
 * So we always expect a failure of mlock/mlock2.
 * No locked page number should be increased as a side effect.
 *
 *    return value: 0 - success
 *    else: failure
 */
int test_mlock_outof_limit(char *p, int alloc_size)
{
	int i;
	int ret = 0;
	int locked_vm_size = 0, old_locked_vm_size = 0;
	struct rlimit cur;

	getrlimit(RLIMIT_MEMLOCK, &cur);
	if (cur.rlim_cur >= alloc_size) {
		printf("alloc_size[%d] >%u rlimit, violates test condition\n",
				alloc_size, (unsigned int)cur.rlim_cur);
		return -1;
	}

	old_locked_vm_size = get_proc_locked_vm_size();
	srand(time(NULL));
	for (i = 0; i < TEST_LOOP; i++) {
		int is_mlock = !!(rand() % 2);
		int lock_size = (rand() % (alloc_size - cur.rlim_cur))
			+ cur.rlim_cur;
		int start_offset = rand() % (alloc_size - lock_size);

		if (is_mlock)
			ret = mlock(p + start_offset, lock_size);
		else
			ret = mlock2_(p + start_offset, lock_size,
					MLOCK_ONFAULT);
		if (ret == 0) {
			printf("%s() succeeds? on %p(%d) mlock%p(%d)\n",
					is_mlock ? "mlock" : "mlock2",
					p, alloc_size,
					p + start_offset, lock_size);
			return -1;
		}
	}

	locked_vm_size = get_proc_locked_vm_size();
	if (locked_vm_size != old_locked_vm_size) {
		printf("tests leads to new mlocked page: old[%d], new[%d]\n",
				old_locked_vm_size,
				locked_vm_size);
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	char *p = NULL;
	int ret = 0;

	if (set_cap_limits(MLOCK_RLIMIT_SIZE))
		return -1;

	p = malloc(MLOCK_WITHIN_LIMIT_SIZE);
	if (p == NULL) {
		perror("malloc() failure\n");
		return -1;
	}
	ret = test_mlock_within_limit(p, MLOCK_WITHIN_LIMIT_SIZE);
	if (ret)
		return ret;
	munlock(p, MLOCK_WITHIN_LIMIT_SIZE);
	free(p);


	p = malloc(MLOCK_OUTOF_LIMIT_SIZE);
	if (p == NULL) {
		perror("malloc() failure\n");
		return -1;
	}
	ret = test_mlock_outof_limit(p, MLOCK_OUTOF_LIMIT_SIZE);
	if (ret)
		return ret;
	munlock(p, MLOCK_OUTOF_LIMIT_SIZE);
	free(p);

	return 0;
}
