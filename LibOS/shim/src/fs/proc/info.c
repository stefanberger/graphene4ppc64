#define __KERNEL__

#include <asm/fcntl.h>
#include <asm/mman.h>
#if defined(__i386__) || defined(__x86_64__)
#include <asm/prctl.h>
#endif
#include <asm/unistd.h>
#include <errno.h>
#include <linux/fcntl.h>
#include <linux/stat.h>

#include <pal.h>
#include <pal_error.h>
#include <shim_fs.h>
#include <shim_internal.h>

static int proc_info_mode(const char* name, mode_t* mode) {
    // The path is implicitly set by calling this function
    __UNUSED(name);
    *mode = 0444;
    return 0;
}

static int proc_info_stat(const char* name, struct stat* buf) {
    // The path is implicitly set by calling this function
    __UNUSED(name);
    memset(buf, 0, sizeof(struct stat));
    buf->st_dev = buf->st_ino = 1;
    buf->st_mode              = 0444 | S_IFREG;
    buf->st_uid               = 0;
    buf->st_gid               = 0;
    buf->st_size              = 0;
    return 0;
}

static int proc_meminfo_open(struct shim_handle* hdl, const char* name, int flags) {
    // This function only serves one file
    __UNUSED(name);
    if (flags & (O_WRONLY | O_RDWR))
        return -EACCES;

    int len, max = 128;
    char* str = NULL;

    struct {
        const char* fmt;
        unsigned long val;
    } meminfo[] = {
        {
            "MemTotal:      %8lu kB\n",
            pal_control.mem_info.mem_total / 1024,
        },
        {
            "MemFree:       %8lu kB\n",
            DkMemoryAvailableQuota() / 1024,
        },
    };

retry:
    max *= 2;
    len = 0;
    free(str);
    str = malloc(max);
    if (!str)
        return -ENOMEM;

    for (size_t i = 0; i < ARRAY_SIZE(meminfo); i++) {
        int ret = snprintf(str + len, max - len, meminfo[i].fmt, meminfo[i].val);

        if (len + ret == max)
            goto retry;

        len += ret;
    }

    struct shim_str_data* data = malloc(sizeof(struct shim_str_data));
    if (!data) {
        free(str);
        return -ENOMEM;
    }

    memset(data, 0, sizeof(struct shim_str_data));
    data->str          = str;
    data->len          = len;
    hdl->type          = TYPE_STR;
    hdl->flags         = flags & ~O_RDONLY;
    hdl->acc_mode      = MAY_READ;
    hdl->info.str.data = data;
    return 0;
}

static int proc_cpuinfo_open(struct shim_handle* hdl, const char* name, int flags) {
    // This function only serves one file
    __UNUSED(name);

    if (flags & (O_WRONLY | O_RDWR))
        return -EACCES;

    int len, max = 128;
    char* str = NULL;

    struct {
        const char* fmt;
        unsigned long val;
    }
    /* below strings must match exactly the strings retrieved from
     * /proc/cpuinfo (see Linux's arch/x86/kernel/cpu/proc.c) */
    cpuinfo[] = {
        {
            "processor\t: %lu\n",
            0,
        },
        {
            "vendor_id\t: %s\n",
            (unsigned long)pal_control.cpu_info.cpu_vendor,
        },
        {
            "cpu family\t: %lu\n",
            pal_control.cpu_info.cpu_family,
        },
        {
            "model\t\t: %lu\n",
            pal_control.cpu_info.cpu_model,
        },
        {
            "model name\t: %s\n",
            (unsigned long)pal_control.cpu_info.cpu_brand,
        },
        {
            "stepping\t: %lu\n",
            pal_control.cpu_info.cpu_stepping,
        },
        {
            "core id\t\t: %lu\n",
            0,
        },
        {
            "cpu cores\t: %lu\n",
            pal_control.cpu_info.cpu_num,
        },
    };

retry:
    max *= 2;
    len = 0;
    free(str);
    str = malloc(max);
    if (!str)
        return -ENOMEM;

    for (size_t n = 0; n < pal_control.cpu_info.cpu_num; n++) {
        cpuinfo[0].val = n;
        cpuinfo[6].val = n;
        for (size_t i = 0; i < ARRAY_SIZE(cpuinfo); i++) {
            int ret = snprintf(str + len, max - len, cpuinfo[i].fmt, cpuinfo[i].val);

            if (len + ret == max)
                goto retry;

            len += ret;
        }

        if (len >= max - 1)
            goto retry;

        str[len++] = '\n';
        str[len]   = 0;
    }

    struct shim_str_data* data = calloc(1, sizeof(struct shim_str_data));
    if (!data) {
        free(str);
        return -ENOMEM;
    }

    data->str          = str;
    data->len          = len;
    hdl->type          = TYPE_STR;
    hdl->flags         = flags & ~O_RDONLY;
    hdl->acc_mode      = MAY_READ;
    hdl->info.str.data = data;
    return 0;
}

struct proc_fs_ops fs_meminfo = {
    .mode = &proc_info_mode,
    .stat = &proc_info_stat,
    .open = &proc_meminfo_open,
};

struct proc_fs_ops fs_cpuinfo = {
    .mode = &proc_info_mode,
    .stat = &proc_info_stat,
    .open = &proc_cpuinfo_open,
};
