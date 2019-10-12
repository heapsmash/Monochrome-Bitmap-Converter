/*	Copyright (c) 2019, Tofu von Helmholtz aka Michael S. Walker
 *	All Rights Reserved in all Federations, including Alpha Centauris.
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions are met:
 *
 *	1. Redistributions of source code must retain the above copyright notice, this
 *	   list of conditions and the following disclaimer.
 *	2. Redistributions in binary form must reproduce the above copyright notice,
 *	   this list of conditions and the following disclaimer in the documentation
 * 	   and/or other materials provided with the distribution.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	The views and conclusions contained in the software and documentation are those
 *	of the authors and should not be interpreted as representing official policies,
 *	either expressed or implied, of the BITMAP ENDIAN project.
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include "syscalls.h"


int Open(const char *file_name, mode_t arg_flags, ...)
{
	va_list ap;
	mode_t mode = 0;
	int fd;

#ifdef __OPEN_NEEDS_MODE
	if ((__OPEN_NEEDS_MODE(arg_flags)) != 0) {
#else
		if ((arg_flags & O_CREAT) != 0) {
#endif
		va_start(ap, arg_flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	fd = (int) syscall(SYS_open, file_name, arg_flags, mode);
	if (fd < 0) {
		PRINT_ERRNO_AND_EXIT("open error")
	}

	return fd;
}


ssize_t IoRead(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = usrbuf;

	while (nleft > 0) {
		if ((nread = Read(fd, bufp, nleft)) < 0) {
			if (nread == -1) /* Interrupted by sig handler return and call Read() again */
				nread = 0;
		} else if (nread == 0)
			break;
		nleft -= nread;
		bufp += nread;
	}
	return (n - nleft);
}


ssize_t Read(int fd, void *buf, size_t count)
{
	ssize_t n_read = syscall(SYS_read, fd, buf, count);

	if (n_read >= 0)
		return n_read;

	if (errno == EINTR) /* Interrupted by sig handler return */
		return -1;

	PRINT_ERRNO_AND_EXIT("Read error") /* errno set by Read() */
}


ssize_t Write(int fd, const void *buf, size_t count)
{
	ssize_t n_written = syscall(SYS_write, fd, buf, count);

	if (n_written >= 0)
		return n_written;

	if (errno == EINTR) /* Interrupted by sig handler return */
		return -1;

	PRINT_ERRNO_AND_EXIT("Write error") /* errno set by Write() */
}


off_t Lseek(int fd, off_t offset, int whence)
{
	off_t result_offset = syscall(SYS_lseek, fd, offset, whence);
	if (offset < 0) {
		PRINT_ERRNO_AND_EXIT("lseek error")
	}

	return result_offset;
}


void Close(int fd)
{
	if ((int) syscall(SYS_close, fd) < 0) {
		PRINT_ERRNO_AND_EXIT("close error")
	}
}


void Print(int file_handle, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vdprintf(file_handle, fmt, ap);
	va_end(ap);
}


#pragma clang diagnostic pop