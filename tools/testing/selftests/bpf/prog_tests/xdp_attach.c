// SPDX-License-Identifier: GPL-2.0
#include <test_progs.h>

#define IFINDEX_LO 1
#define XDP_FLAGS_REPLACE		(1U << 4)

void test_xdp_attach(void)
{
	__u32 duration = 0, id1, id2, id0 = 0, len;
	struct bpf_object *obj1, *obj2, *obj3;
	const char *file = "./test_xdp.o";
	struct bpf_prog_info info = {};
	int err, fd1, fd2, fd3;
	DECLARE_LIBBPF_OPTS(bpf_xdp_set_link_opts, opts,
			    .old_fd = -1);

	len = sizeof(info);

	err = bpf_prog_load(file, BPF_PROG_TYPE_XDP, &obj1, &fd1);
	if (CHECK_FAIL(err))
		return;
	err = bpf_obj_get_info_by_fd(fd1, &info, &len);
	if (CHECK_FAIL(err))
		goto out_1;
	id1 = info.id;

	err = bpf_prog_load(file, BPF_PROG_TYPE_XDP, &obj2, &fd2);
	if (CHECK_FAIL(err))
		goto out_1;

	memset(&info, 0, sizeof(info));
	err = bpf_obj_get_info_by_fd(fd2, &info, &len);
	if (CHECK_FAIL(err))
		goto out_2;
	id2 = info.id;

	err = bpf_prog_load(file, BPF_PROG_TYPE_XDP, &obj3, &fd3);
	if (CHECK_FAIL(err))
		goto out_2;

	err = bpf_set_link_xdp_fd_opts(IFINDEX_LO, fd1, XDP_FLAGS_REPLACE,
				       &opts);
	if (CHECK(err, "load_ok", "initial load failed"))
		goto out_close;

	err = bpf_get_link_xdp_id(IFINDEX_LO, &id0, 0);
	if (CHECK(err || id0 != id1, "id1_check",
		  "loaded prog id %u != id1 %u, err %d", id0, id1, err))
		goto out_close;

	err = bpf_set_link_xdp_fd_opts(IFINDEX_LO, fd2, XDP_FLAGS_REPLACE,
				       &opts);
	if (CHECK(!err, "load_fail", "load with expected id didn't fail"))
		goto out;

	opts.old_fd = fd1;
	err = bpf_set_link_xdp_fd_opts(IFINDEX_LO, fd2, 0, &opts);
	if (CHECK(err, "replace_ok", "replace valid old_fd failed"))
		goto out;
	err = bpf_get_link_xdp_id(IFINDEX_LO, &id0, 0);
	if (CHECK(err || id0 != id2, "id2_check",
		  "loaded prog id %u != id2 %u, err %d", id0, id2, err))
		goto out_close;

	err = bpf_set_link_xdp_fd_opts(IFINDEX_LO, fd3, 0, &opts);
	if (CHECK(!err, "replace_fail", "replace invalid old_fd didn't fail"))
		goto out;

	err = bpf_set_link_xdp_fd_opts(IFINDEX_LO, -1, 0, &opts);
	if (CHECK(!err, "remove_fail", "remove invalid old_fd didn't fail"))
		goto out;

	opts.old_fd = fd2;
	err = bpf_set_link_xdp_fd_opts(IFINDEX_LO, -1, 0, &opts);
	if (CHECK(err, "remove_ok", "remove valid old_fd failed"))
		goto out;

	err = bpf_get_link_xdp_id(IFINDEX_LO, &id0, 0);
	if (CHECK(err || id0 != 0, "unload_check",
		  "loaded prog id %u != 0, err %d", id0, err))
		goto out_close;
out:
	bpf_set_link_xdp_fd(IFINDEX_LO, -1, 0);
out_close:
	bpf_object__close(obj3);
out_2:
	bpf_object__close(obj2);
out_1:
	bpf_object__close(obj1);
}
