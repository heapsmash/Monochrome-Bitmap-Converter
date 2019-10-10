/*	Copyright (c) 2019, Tofu von Helmholtz aka Michael S. Walker
 *	All rights reserved.
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

#include <stdint.h>

const char *usage = "usage: ./%s [-e] <endian> -f <file_name.bmp>";

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

BMPHeader ReadBmpHeader(int fd);
void PrintBitmapDetails(BMPHeader header);

#endif //BMP_RIP_BMP_H
