/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2020 Intel Corporation
 *                    Paweł Marczewski <pawel@invisiblethingslab.com>
 */

/*
 * Linux file type permission macros.
 */

#ifndef STAT_H
#define STAT_H

#ifdef S_IFREG
#error "Do not include <linux/stat.h> or <sys/stat.h> together with this file."
#endif

#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#if defined(__powerpc64__)

struct stat {
    unsigned long	st_dev;
    unsigned long	st_ino;
    unsigned long	st_nlink;
    unsigned int	st_mode;
    unsigned int	st_uid;
    unsigned int	st_gid;
    unsigned long	st_rdev;
    long		st_size;
    long		st_blksize;
    long		st_blocks;
    long		st_atime;
    long		st_mtime;
    long                pad1;
    long                pad2;
    long		st_ctime;
    long                _pad[4];
};

#endif /* powerpc64 */

#endif /* STAT_H */
