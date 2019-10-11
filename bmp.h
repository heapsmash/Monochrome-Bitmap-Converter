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

#ifndef BMP_RIP_BMP_H
#define BMP_RIP_BMP_H

#include <stdint.h> /* for uintxx_t */
#include "errlib.h" /* for CHECK_SIZE */
#include <errno.h>  /* for CHECK_SIZE */

#define MAX_DATA 40000

#if defined(__linux__) && defined(__x86_64__)
#define __FILE_MAX__ FILENAME_MAX
#define __ARRAY_MAX__ 25
#else
#define __FILE_MAX__ 10 /* Size depends on system, i'm to lazy to check the Atari ST sizes ... RTFM & replace */
#define __ARRAY_MAX__ __FILE_MAX__
#endif

char *g_file_name_to_save;
char *g_file_name_to_read;
char *g_array_name_to_store;

const char *g_usage = "g_usage: ./bmp_rip -f <file_name_to_read.bmp> -a <array_name_to_store> -s <file_name_to_save.c>";

#define OUTPUT_TO_FD 1 /* 1 for testing */

#define CHECK_SIZE(arg, x) if (strlen((arg)) >= (x)) { \
    errno = 36; \
    PRINT_ERRNO_AND_RETURN("optarg error"); \
    }

typedef struct _BMPHeader {    /* Total: 54 bytes */
	uint16_t type;             /* Magic identifier: 0x4d42 */
	uint32_t size;             /* File size in bytes */
	uint16_t reserved1;        /* NOT USED */
	uint16_t reserved2;        /* NOT USED */
	uint32_t offset;           /* Offset to image data in bytes from beginning of file (54 bytes) */
	uint32_t dib_header_size;  /* DIB Header size in bytes (40 bytes) */
	int32_t width_px;          /* Width of the image */
	int32_t height_px;         /* Height of the image */
	uint16_t num_planes;       /* Number of color planes */
	uint16_t bits_per_pixel;   /* Bits per pixel */
	uint32_t compression;      /* Compression type */
	uint32_t image_size_bytes; /* Image size in bytes */
	int32_t x_resolution_ppm;  /* Pixels per meter */
	int32_t y_resolution_ppm;  /* Pixels per meter */
	uint32_t num_colors;       /* Number of colors */
	uint32_t important_colors; /* Important colors */
} BMPHeader;

typedef struct _BMPImage {
	BMPHeader header;
	char data[MAX_DATA];
} BMPImage;

void PrintBmpHeader(BMPHeader header);
void FillBMPHeader(int fd, BMPHeader *bmp);
void WriteDataBigEndian(int bitmap_to_read_fd, int c_file_to_write_fd, BMPImage *image);
void ReplaceSpaces(char *str);

BMPImage ReadBmp(int fd);

#endif //BMP_RIP_BMP_H
