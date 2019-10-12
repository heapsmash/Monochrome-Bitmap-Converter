/*	Copyright (c) 2019, Michael S. Walker
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
#include <errno.h>  /* for CHECK_SIZE */

#if defined(__linux__) && defined(__x86_64__)

#define __FILE_MAX__ FILENAME_MAX
#define __ARRAY_MAX__ 25
#else

#define __FILE_MAX__ 10 /* Size depends on system, 10 seems like a safe number */
#define __ARRAY_MAX__ __FILE_MAX__
#endif

#define CHECK_SIZE(arg, x) if (strlen((arg)) >= (x)) { \
    errno = 36; \
    PRINT_ERRNO_AND_RETURN("optarg error"); \
    }

#define UMASK 0664 /* [User: rw] [Group: rw] [Other: r] */

const char *g_header_names[] = {
		"type:",
		"size:",
		"reserved1:",
		"reserved2:",
		"offset:",
		"dib_header_size:",
		"width_px:",
		"height_px:",
		"num_planes:",
		"bits_per_pixel:",
		"compression:",
		"image_size_bytes:",
		"x_resolution_ppm:",
		"y_resolution_ppm:",
		"num_colors:",
		"important_colors:"
};

const char *g_header_description[] = {
		"(Magic identifier: 0x4d42)",
		"(File size in bytes)",
		"(NOT USED)",
		"(NOT USED)",
		"(Offset to image data [54 bytes])",
		"(DIB Header size in bytes [40 bytes])",
		"(Width of the image)",
		"(Height of the image)",
		"(Number of color planes)",
		"(Bits per pixel)",
		"(Compression type)",
		"(Image size in bytes)",
		"(Pixels per meter)",
		"(Pixels per meter)",
		"(Number of colors)",
		"(Important colors)"
};

const char *g_usage = "g_usage: ./bit_rip [-n number of columns to write] -f <input filename . bmp>  -a <array name for bitmap> -s <output file name . c>";

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

typedef struct _BMPImageTools {
	BMPHeader header;
	size_t ncols_per_row;
	int bitmap_to_read_fd, c_file_to_write_fd;
	char *file_name_to_save, *file_name_to_read, *array_name_to_store;
} BitRipTools;

uint32_t Swap8(uint32_t x);
uint32_t Swap16(uint32_t x);
uint32_t Swap32(uint32_t x);

void WriteArrayToFile(BitRipTools *data, uint32_t(*Swap)(uint32_t));
void WriteCommentToFile(BitRipTools data);
void ReadBitmapHeader(BitRipTools *data);

#endif //BMP_RIP_BMP_H
