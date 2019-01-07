/* -*-mode:c; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
  Copyright (c) 2018,2019 Rian Hunter et. al, see AUTHORS file.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 */

#ifndef __WASMJIT__EMSCRIPTEN_RUNTIME_SYS_H__
#define __WASMJIT__EMSCRIPTEN_RUNTIME_SYS_H__

#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <net/if.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/statvfs.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>

/* for system's getaddrinfo */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#ifdef __APPLE__

typedef struct {
} sem_t;

#else

#include <semaphore.h>

#endif

#if defined(_POSIX_VERSION) && _POSIX_VERSION >= 200809
#define st_get_nsec(m, st) ((st)->st_ ## m ## tim.tv_nsec)
#else
#define st_get_nsec(m, st) 0
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct msghdr user_msghdr_t;

#define SYS_CMSG_NXTHDR(msg, cmsg) CMSG_NXTHDR((msg), (cmsg))

#if defined(__linux__)

struct linux_dirent64 {
	uint64_t d_ino;
	int64_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[1];
};

#endif

/* use something reasonable */
#define MAX_ARG_STRLEN (4096 * 32)

typedef struct statvfs user_statvfs;

#define statvfs_get_type(pstvfs) 0
#define statvfs_get_low_fsid(pstvfs) ((pstvfs)->f_fsid & 0xffffffff)
#define statvfs_get_high_fsid(pstvfs) ((pstvfs)->f_fsid >> 32)
#define statvfs_get_flags(pstvfs) ((pstvfs)->f_flag)
#define statvfs_get_namemax(pstvfs) ((pstvfs)->f_namemax)

/* OpenBSD doesn't implement posix_fadvise */
#if !defined(EM_POSIX_FADV_NORMAL) && !defined(POSIX_FADV_RANDOM) && !defined(POSIX_FADV_SEQUENTIAL) && !defined(POSIX_FADV_WILLNEED) && !defined(POSIX_FADV_DONTNEED) && !defined(POSIX_FADV_NOREUSE)

int posix_fadvise(int fd, off_t offset, off_t len, int advice);
#define NEED_POSIX_FADVISE

#endif

#ifndef POSIX_FADV_NORMAL
#define POSIX_FADV_NORMAL 0
#endif

#ifndef POSIX_FADV_RANDOM
#define POSIX_FADV_RANDOM 0
#endif

#ifndef POSIX_FADV_SEQUENTIAL
#define POSIX_FADV_SEQUENTIAL 0
#endif

#ifndef POSIX_FADV_WILLNEED
#define POSIX_FADV_WILLNEED 0
#endif

#ifndef POSIX_FADV_DONTNEED
#define POSIX_FADV_DONTNEED 0
#endif

#ifndef POSIX_FADV_NOREUSE
#define POSIX_FADV_NOREUSE 0
#endif

#include <wasmjit/util.h>

/* declare all sys calls */

#define __KDECL(to,n,t) t _##n

#define VOID0 void
#define VOID1
#define VOID2
#define VOID3
#define VOID4
#define VOID5
#define VOID6

#define KWSC0(f, ...) KWSCx(0, f, __VA_ARGS__)
#define KWSC1(f, ...) KWSCx(1, f, __VA_ARGS__)
#define KWSC2(f, ...) KWSCx(2, f, __VA_ARGS__)
#define KWSC3(f, ...) KWSCx(3, f, __VA_ARGS__)
#define KWSC4(f, ...) KWSCx(4, f, __VA_ARGS__)
#define KWSC5(f, ...) KWSCx(5, f, __VA_ARGS__)
#define KWSC6(f, ...) KWSCx(6, f, __VA_ARGS__)

typedef struct stat sys_stat_t;

#define KWSCx(_n, err, _pre, _name, ...) long _pre ## sys_ ## _name(__KMAP(_n, __KDECL, __VA_ARGS__));

#include <wasmjit/posix_sys_def.h>

#undef KWSC0
#undef KWSC1
#undef KWSC2
#undef KWSC3
#undef KWSC4
#undef KWSC5
#undef KWSC6
#undef KWSCx
#undef __KDECL
#undef VOID0
#undef VOID1
#undef VOID2
#undef VOID3
#undef VOID4
#undef VOID5
#undef VOID6

#if defined(__linux__)
#define EM_IS_LINUX 1
#else
#define EM_IS_LINUX 0
#endif

typedef struct tm user_tm;

__attribute__((unused))
static time_t user_mktime(user_tm *timeptr)
{
	return mktime(timeptr);
}

#endif
