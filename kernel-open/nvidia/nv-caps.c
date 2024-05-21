/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nv-linux.h"
#include "nv-caps.h"
#include "nv-procfs.h"
#include "nv-hash.h"

#include "nvmisc.h"

extern int NVreg_ModifyDeviceFiles;

/* sys_close() or __close_fd() */
#include <linux/syscalls.h>

#define NV_CAP_DRV_MINOR_COUNT 8192

/* Hash table with 512 buckets */
#define NV_CAP_HASH_BITS 9
NV_DECLARE_HASHTABLE(g_nv_cap_hash_table, NV_CAP_HASH_BITS);

#define NV_CAP_HASH_SIZE NV_HASH_SIZE(g_nv_cap_hash_table)

#define nv_cap_hash_key(path) (nv_string_hash(path) % NV_CAP_HASH_SIZE)

typedef struct nv_cap_table_entry
{
    /* name must be the first element */
    const char *name;
    int minor;
    struct hlist_node hlist;
} nv_cap_table_entry_t;

#define NV_CAP_NUM_ENTRIES(_table) (NV_ARRAY_ELEMENTS(_table))

static nv_cap_table_entry_t g_nv_cap_nvlink_table[] =
{
    {"/driver/nvidia-nvlink/capabilities/fabric-mgmt"}
};

static nv_cap_table_entry_t g_nv_cap_mig_table[] =
{
    {"/driver/nvidia/capabilities/mig/config"},
    {"/driver/nvidia/capabilities/mig/monitor"}
};

static nv_cap_table_entry_t g_nv_cap_sys_table[] =
{
    {"/driver/nvidia/capabilities/fabric-imex-mgmt"}
};

#define NV_CAP_MIG_CI_ENTRIES(_gi)  \
    {_gi "/ci0/access"},            \
    {_gi "/ci1/access"},            \
    {_gi "/ci2/access"},            \
    {_gi "/ci3/access"},            \
    {_gi "/ci4/access"},            \
    {_gi "/ci5/access"},            \
    {_gi "/ci6/access"},            \
    {_gi "/ci7/access"}

#define NV_CAP_MIG_GI_ENTRIES(_gpu)       \
    {_gpu "/gi0/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi0"),   \
    {_gpu "/gi1/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi1"),   \
    {_gpu "/gi2/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi2"),   \
    {_gpu "/gi3/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi3"),   \
    {_gpu "/gi4/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi4"),   \
    {_gpu "/gi5/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi5"),   \
    {_gpu "/gi6/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi6"),   \
    {_gpu "/gi7/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi7"),   \
    {_gpu "/gi8/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi8"),   \
    {_gpu "/gi9/access"},                 \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi9"),   \
    {_gpu "/gi10/access"},                \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi10"),  \
    {_gpu "/gi11/access"},                \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi11"),  \
    {_gpu "/gi12/access"},                \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi12"),  \
    {_gpu "/gi13/access"},                \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi13"),  \
    {_gpu "/gi14/access"},                \
    NV_CAP_MIG_CI_ENTRIES(_gpu "/gi14")

static nv_cap_table_entry_t g_nv_cap_mig_gpu_table[] =
{
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu0/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu1/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu2/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu3/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu4/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu5/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu6/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu7/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu8/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu9/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu10/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu11/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu12/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu13/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu14/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu15/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu16/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu17/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu18/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu19/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu20/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu21/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu22/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu23/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu24/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu25/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu26/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu27/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu28/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu29/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu30/mig"),
    NV_CAP_MIG_GI_ENTRIES("/driver/nvidia/capabilities/gpu31/mig")
};

struct nv_cap
{
    char *path;
    char *name;
    int minor;
    int permissions;
    int modify;
    struct proc_dir_entry *parent;
    struct proc_dir_entry *entry;
};

#define NV_CAP_PROCFS_WRITE_BUF_SIZE 128

typedef struct nv_cap_file_private
{
    int minor;
    int permissions;
    int modify;
    char buffer[NV_CAP_PROCFS_WRITE_BUF_SIZE];
    off_t offset;
} nv_cap_file_private_t;

struct
{
    NvBool initialized;
    struct cdev cdev;
    dev_t devno;
} g_nv_cap_drv;

#define NV_CAP_PROCFS_DIR "driver/nvidia-caps"
#define NV_CAP_NAME_BUF_SIZE 128

static struct proc_dir_entry *nv_cap_procfs_dir;

static int nv_procfs_read_nvlink_minors(struct seq_file *s, void *v)
{
    int i, count;
    char name[NV_CAP_NAME_BUF_SIZE];

    count = NV_CAP_NUM_ENTRIES(g_nv_cap_nvlink_table);
    for (i = 0; i < count; i++)
    {
        if (sscanf(g_nv_cap_nvlink_table[i].name,
                   "/driver/nvidia-nvlink/capabilities/%s", name) == 1)
        {
            name[sizeof(name) - 1] = '\0';
            seq_printf(s, "%s %d\n", name, g_nv_cap_nvlink_table[i].minor);
        }
    }

    return 0;
}

static int nv_procfs_read_sys_minors(struct seq_file *s, void *v)
{
    int i, count;
    char name[NV_CAP_NAME_BUF_SIZE];

    count = NV_CAP_NUM_ENTRIES(g_nv_cap_sys_table);
    for (i = 0; i < count; i++)
    {
        if (sscanf(g_nv_cap_sys_table[i].name,
                   "/driver/nvidia/capabilities/%s", name) == 1)
        {
            name[sizeof(name) - 1] = '\0';
            seq_printf(s, "%s %d\n", name, g_nv_cap_sys_table[i].minor);
        }
    }

    return 0;
}

static int nv_procfs_read_mig_minors(struct seq_file *s, void *v)
{
    int i, count, gpu;
    char name[NV_CAP_NAME_BUF_SIZE];

    count = NV_CAP_NUM_ENTRIES(g_nv_cap_mig_table);
    for (i = 0; i < count; i++)
    {
        if (sscanf(g_nv_cap_mig_table[i].name,
                   "/driver/nvidia/capabilities/mig/%s", name) == 1)
        {
            name[sizeof(name) - 1] = '\0';
            seq_printf(s, "%s %d\n", name, g_nv_cap_mig_table[i].minor);
        }
    }

    count = NV_CAP_NUM_ENTRIES(g_nv_cap_mig_gpu_table);
    for (i = 0; i < count; i++)
    {
        if (sscanf(g_nv_cap_mig_gpu_table[i].name,
                   "/driver/nvidia/capabilities/gpu%d/mig/%s", &gpu, name) == 2)
        {
            name[sizeof(name) - 1] = '\0';
            seq_printf(s, "gpu%d/%s %d\n",
                       gpu, name, g_nv_cap_mig_gpu_table[i].minor);
        }
    }

    return 0;
}

NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY(nvlink_minors, nv_system_pm_lock);

NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY(mig_minors, nv_system_pm_lock);

NV_DEFINE_SINGLE_PROCFS_FILE_READ_ONLY(sys_minors, nv_system_pm_lock);

static void nv_cap_procfs_exit(void)
{
    if (!nv_cap_procfs_dir)
    {
        return;
    }

#if defined(CONFIG_PROC_FS)
    proc_remove(nv_cap_procfs_dir);
#endif
    nv_cap_procfs_dir = NULL;
}

int nv_cap_procfs_init(void)
{
    static struct proc_dir_entry *file_entry;

    nv_cap_procfs_dir = NV_CREATE_PROC_DIR(NV_CAP_PROCFS_DIR, NULL);
    if (nv_cap_procfs_dir == NULL)
    {
        return -EACCES;
    }

    file_entry = NV_CREATE_PROC_FILE("mig-minors", nv_cap_procfs_dir,
                                     mig_minors, NULL);
    if (file_entry == NULL)
    {
        goto cleanup;
    }

    file_entry = NV_CREATE_PROC_FILE("nvlink-minors", nv_cap_procfs_dir,
                                     nvlink_minors, NULL);
    if (file_entry == NULL)
    {
        goto cleanup;
    }

    file_entry = NV_CREATE_PROC_FILE("sys-minors", nv_cap_procfs_dir,
                                     sys_minors, NULL);
    if (file_entry == NULL)
    {
        goto cleanup;
    }

    return 0;

cleanup:
    nv_cap_procfs_exit();

    return -EACCES;
}

static int nv_cap_find_minor(char *path)
{
    unsigned int key = nv_cap_hash_key(path);
    nv_cap_table_entry_t *entry;

    nv_hash_for_each_possible(g_nv_cap_hash_table, entry, hlist, key)
    {
        if (strcmp(path, entry->name) == 0)
        {
            return entry->minor;
        }
    }

    return -1;
}

static void _nv_cap_table_init(nv_cap_table_entry_t *table, int count)
{
    int i;
    unsigned int key;
    static int minor = 0;

    for (i = 0; i < count; i++)
    {
        table[i].minor = minor++;
        INIT_HLIST_NODE(&table[i].hlist);
        key = nv_cap_hash_key(table[i].name);
        nv_hash_add(g_nv_cap_hash_table, &table[i].hlist, key);
    }

    WARN_ON(minor > NV_CAP_DRV_MINOR_COUNT);
}

#define nv_cap_table_init(table) \
    _nv_cap_table_init(table, NV_CAP_NUM_ENTRIES(table))

static void nv_cap_tables_init(void)
{
    BUILD_BUG_ON(offsetof(nv_cap_table_entry_t, name) != 0);

    nv_hash_init(g_nv_cap_hash_table);

    nv_cap_table_init(g_nv_cap_nvlink_table);
    nv_cap_table_init(g_nv_cap_mig_table);
    nv_cap_table_init(g_nv_cap_mig_gpu_table);
    nv_cap_table_init(g_nv_cap_sys_table);
}

static ssize_t nv_cap_procfs_write(struct file *file,
                                    const char __user *buffer,
                                    size_t count, loff_t *pos)
{
    nv_cap_file_private_t *private = NULL;
    unsigned long bytes_left;
    char *proc_buffer;
    int status;

    status = nv_down_read_interruptible(&nv_system_pm_lock);
    if (status < 0)
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps: failed to lock the nv_system_pm_lock!\n");
        return status;
    }

    private = ((struct seq_file *)file->private_data)->private;
    bytes_left = (sizeof(private->buffer) - private->offset - 1);

    if (count == 0)
    {
        count = -EINVAL;
        goto done;
    }

    if ((bytes_left == 0) || (count > bytes_left))
    {
        count = -ENOSPC;
        goto done;
    }

    proc_buffer = &private->buffer[private->offset];

    if (copy_from_user(proc_buffer, buffer, count))
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps: failed to copy in proc data!\n");
        count = -EFAULT;
        goto done;
    }

    private->offset += count;
    proc_buffer[count] = '\0';

    *pos = private->offset;

done:
    up_read(&nv_system_pm_lock);

    return count;
}

static int nv_cap_procfs_read(struct seq_file *s, void *v)
{
    int status;
    nv_cap_file_private_t *private = s->private;

    status = nv_down_read_interruptible(&nv_system_pm_lock);
    if (status < 0)
    {
        return status;
    }

    seq_printf(s, "%s: %d\n", "DeviceFileMinor", private->minor);
    seq_printf(s, "%s: %d\n", "DeviceFileMode", private->permissions);
    seq_printf(s, "%s: %d\n", "DeviceFileModify", private->modify);

    up_read(&nv_system_pm_lock);
    return 0;
}

static int nv_cap_procfs_open(struct inode *inode, struct file *file)
{
    nv_cap_file_private_t *private = NULL;
    int rc;
    nv_cap_t *cap = NV_PDE_DATA(inode);

    NV_KMALLOC(private, sizeof(nv_cap_file_private_t));
    if (private == NULL)
    {
        return -ENOMEM;
    }

    private->minor = cap->minor;
    private->permissions = cap->permissions;
    private->offset = 0;
    private->modify = cap->modify;

    rc = single_open(file, nv_cap_procfs_read, private);
    if (rc < 0)
    {
        NV_KFREE(private, sizeof(nv_cap_file_private_t));
    }

    return rc;
}

static int nv_cap_procfs_release(struct inode *inode, struct file *file)
{
    struct seq_file *s = file->private_data;
    nv_cap_file_private_t *private = NULL;
    char *buffer;
    int modify;
    nv_cap_t *cap = NV_PDE_DATA(inode);

    if (s != NULL)
    {
        private = s->private;
    }

    single_release(inode, file);

    if (private != NULL)
    {
        buffer = private->buffer;

        if (private->offset != 0)
        {
            if (sscanf(buffer, "DeviceFileModify: %d", &modify) == 1)
            {
                cap->modify = modify;
            }
        }

        NV_KFREE(private, sizeof(nv_cap_file_private_t));
    }

    /*
     * All open files using the proc entry will be invalidated
     * if the entry is removed.
     */
    file->private_data = NULL;

    return 0;
}

static nv_proc_ops_t g_nv_cap_procfs_fops = {
    NV_PROC_OPS_SET_OWNER()
    .NV_PROC_OPS_OPEN    = nv_cap_procfs_open,
    .NV_PROC_OPS_RELEASE = nv_cap_procfs_release,
    .NV_PROC_OPS_WRITE   = nv_cap_procfs_write,
    .NV_PROC_OPS_READ    = seq_read,
    .NV_PROC_OPS_LSEEK   = seq_lseek,
};

/* forward declaration of g_nv_cap_drv_fops */
static struct file_operations g_nv_cap_drv_fops;

int NV_API_CALL nv_cap_validate_and_dup_fd(const nv_cap_t *cap, int fd)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
    struct file *file;
    int dup_fd;
    struct inode *inode = NULL;
    dev_t rdev = 0;
    struct files_struct *files = current->files;
    struct fdtable *fdt;

    if (cap == NULL)
    {
        return -1;
    }

    file = fget(fd);
    if (file == NULL)
    {
        return -1;
    }

    inode = NV_FILE_INODE(file);
    if (inode == NULL)
    {
        goto err;
    }

    /* Make sure the fd belongs to the nv-cap-drv */
    if (file->f_op != &g_nv_cap_drv_fops)
    {
        goto err;
    }

    /* Make sure the fd has the expected capability */
    rdev = inode->i_rdev;
    if (MINOR(rdev) != cap->minor)
    {
        goto err;
    }

    dup_fd = NV_GET_UNUSED_FD_FLAGS(O_CLOEXEC);
    if (dup_fd < 0)
    {
        dup_fd = NV_GET_UNUSED_FD();
        if (dup_fd < 0)
        {
            goto err;
        }

        /*
         * Set CLOEXEC before installing the FD.
         *
         * If fork() happens in between, the opened unused FD will have
         * a NULL struct file associated with it, which is okay.
         *
         * The only well known bug here is the race with dup(2), which is
         * already documented in the kernel, see fd_install()'s description.
         */

        spin_lock(&files->file_lock);
        fdt = files_fdtable(files);
        __set_bit(dup_fd, fdt->close_on_exec);
        spin_unlock(&files->file_lock);
    }

    fd_install(dup_fd, file);
    return dup_fd;

err:
    fput(file);
    return -1;
#else
    return -1;
#endif
}

void NV_API_CALL nv_cap_close_fd(int fd)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
    if (fd == -1)
    {
        return;
    }

    /*
     * Acquire task_lock as we access current->files explicitly (__close_fd)
     * and implicitly (sys_close), and it will race with the exit path.
     */
    task_lock(current);

    /* Nothing to do, we are in exit path */
    if (current->files == NULL)
    {
        task_unlock(current);
        return;
    }

/*
 * From v4.17-rc1 (to v5.10.8) kernels have stopped exporting sys_close(fd)
 * and started exporting __close_fd, as of this commit:
 * 2018-04-02 2ca2a09d6215 ("fs: add ksys_close() wrapper; remove in-kernel
 * calls to sys_close()")
 * Kernels v5.11-rc1 onwards have stopped exporting __close_fd, and started
 * exporting close_fd, as of this commit:
 * 2020-12-20 8760c909f54a ("file: Rename __close_fd to close_fd and remove
 * the files parameter")
 */
#if NV_IS_EXPORT_SYMBOL_PRESENT_close_fd
    close_fd(fd);
#elif NV_IS_EXPORT_SYMBOL_PRESENT___close_fd
    __close_fd(current->files, fd);
#else
    sys_close(fd);
#endif

    task_unlock(current);
#endif
}

static nv_cap_t* nv_cap_alloc(nv_cap_t *parent_cap, const char *name)
{
    nv_cap_t *cap;
    int len;

    if (parent_cap == NULL || name == NULL)
    {
        return NULL;
    }

    NV_KMALLOC(cap, sizeof(nv_cap_t));
    if (cap == NULL)
    {
        return NULL;
    }

    len = strlen(name) + strlen(parent_cap->path) + 2;
    NV_KMALLOC(cap->path, len);
    if (cap->path == NULL)
    {
        NV_KFREE(cap, sizeof(nv_cap_t));
        return NULL;
    }

    strcpy(cap->path, parent_cap->path);
    strcat(cap->path, "/");
    strcat(cap->path, name);

    len = strlen(name) + 1;
    NV_KMALLOC(cap->name, len);
    if (cap->name == NULL)
    {
        NV_KFREE(cap->path, strlen(cap->path) + 1);
        NV_KFREE(cap, sizeof(nv_cap_t));
        return NULL;
    }

    strcpy(cap->name, name);

    cap->minor = -1;
    cap->modify = NVreg_ModifyDeviceFiles;

    return cap;
}

static void nv_cap_free(nv_cap_t *cap)
{
    if (cap == NULL)
    {
        return;
    }

    NV_KFREE(cap->path, strlen(cap->path) + 1);
    NV_KFREE(cap->name, strlen(cap->name) + 1);
    NV_KFREE(cap, sizeof(nv_cap_t));
}

nv_cap_t* NV_API_CALL nv_cap_create_file_entry(nv_cap_t *parent_cap,
                                               const char *name, int mode)
{
    nv_cap_t *cap = NULL;
    int minor;

    cap = nv_cap_alloc(parent_cap, name);
    if (cap == NULL)
    {
        return NULL;
    }

    cap->parent = parent_cap->entry;
    cap->permissions = mode;

    mode = (S_IFREG | S_IRUGO);

    minor = nv_cap_find_minor(cap->path);
    if (minor < 0)
    {
        nv_cap_free(cap);
        return NULL;
    }

    cap->minor = minor;

    cap->entry = proc_create_data(name, mode, parent_cap->entry,
                                  &g_nv_cap_procfs_fops, (void*)cap);
    if (cap->entry == NULL)
    {
        nv_cap_free(cap);
        return NULL;
    }

    return cap;
}

nv_cap_t* NV_API_CALL nv_cap_create_dir_entry(nv_cap_t *parent_cap,
                                              const char *name, int mode)
{
    nv_cap_t *cap = NULL;

    cap = nv_cap_alloc(parent_cap, name);
    if (cap == NULL)
    {
        return NULL;
    }

    cap->parent = parent_cap->entry;
    cap->permissions = mode;
    cap->minor = -1;

    mode = (S_IFDIR | S_IRUGO | S_IXUGO);

    cap->entry = NV_PROC_MKDIR_MODE(name, mode, parent_cap->entry);
    if (cap->entry == NULL)
    {
        nv_cap_free(cap);
        return NULL;
    }

    return cap;
}

nv_cap_t* NV_API_CALL nv_cap_init(const char *path)
{
    nv_cap_t parent_cap;
    nv_cap_t *cap;
    int mode;
    char *name = NULL;
    char dir[] = "/capabilities";

    if (path == NULL)
    {
        return NULL;
    }

    NV_KMALLOC(name, (strlen(path) + strlen(dir)) + 1);
    if (name == NULL)
    {
        return NULL;
    }

    strcpy(name, path);
    strcat(name, dir);
    parent_cap.entry = NULL;
    parent_cap.path = "";
    parent_cap.name = "";
    mode =  S_IRUGO | S_IXUGO;
    cap = nv_cap_create_dir_entry(&parent_cap, name, mode);

    NV_KFREE(name, strlen(name) + 1);
    return cap;
}

void NV_API_CALL nv_cap_destroy_entry(nv_cap_t *cap)
{
    if (WARN_ON(cap == NULL))
    {
        return;
    }

    remove_proc_entry(cap->name, cap->parent);
    nv_cap_free(cap);
}

static int nv_cap_drv_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int nv_cap_drv_release(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations g_nv_cap_drv_fops =
{
    .owner = THIS_MODULE,
    .open    = nv_cap_drv_open,
    .release = nv_cap_drv_release
};

int NV_API_CALL nv_cap_drv_init(void)
{
    int rc;

    nv_cap_tables_init();

    if (g_nv_cap_drv.initialized)
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps-drv is already initialized.\n");
        return -EBUSY;
    }

    rc = alloc_chrdev_region(&g_nv_cap_drv.devno,
                             0,
                             NV_CAP_DRV_MINOR_COUNT,
                             "nvidia-caps");
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps-drv failed to create cdev region.\n");
        return rc;
    }

    cdev_init(&g_nv_cap_drv.cdev, &g_nv_cap_drv_fops);

    g_nv_cap_drv.cdev.owner = THIS_MODULE;

    rc = cdev_add(&g_nv_cap_drv.cdev, g_nv_cap_drv.devno,
                  NV_CAP_DRV_MINOR_COUNT);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps-drv failed to create cdev.\n");
        goto cdev_add_fail;
    }

    rc = nv_cap_procfs_init();
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps-drv: unable to init proc\n");
        goto proc_init_fail;
    }

    g_nv_cap_drv.initialized = NV_TRUE;

    return 0;

proc_init_fail:
    cdev_del(&g_nv_cap_drv.cdev);

cdev_add_fail:
    unregister_chrdev_region(g_nv_cap_drv.devno, NV_CAP_DRV_MINOR_COUNT);

    return rc;
}

void NV_API_CALL nv_cap_drv_exit(void)
{
    if (!g_nv_cap_drv.initialized)
    {
        return;
    }

    nv_cap_procfs_exit();

    cdev_del(&g_nv_cap_drv.cdev);

    unregister_chrdev_region(g_nv_cap_drv.devno, NV_CAP_DRV_MINOR_COUNT);

    g_nv_cap_drv.initialized = NV_FALSE;
}
