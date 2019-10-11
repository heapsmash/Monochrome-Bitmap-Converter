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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "syscalls.h"
#include "lib/errlib.h"
#include "lib/endianlib.h"

#include "bmp.h"


int main(int argc, char *argv[], char *envp[])
{
	int opt, fd;
	BMPImage bitmap;
	char *file_name = NULL;

	g_endian = 1; /* default 1 means big endian */
	while ((opt = getopt(argc, argv, "e:f:a:")) != -1) {
		switch (opt) {
			case 'f':
				file_name = optarg;
				break;
			case 'e':
				if (!memcmp("little", optarg, strlen("little")))
					g_endian = 0;
				break;
			default :
			PRINT_ERR_AND_RETURN("%s", g_usage);
		}
	}

	if (file_name == NULL) {
		PRINT_ERR_AND_RETURN("%s", g_usage);
	}

	fd = Open(file_name, O_RDONLY, 0);
	bitmap = ReadBmp(fd);

	PrintBitmapDetails(bitmap.header);
	ReadBmpImageLittleEndian(fd, &bitmap);
	return 0;
}


BMPImage ReadBmp(int fd)
{
	BMPImage img;

	FillBMPHeader(fd, &img.header);
	return img;
}


void ReadBmpImageLittleEndian(int fd, BMPImage *image)
{
	int i;
	uint32_t hex_image_data[image->header.height_px];

	lseek(fd, image->header.offset, SEEK_SET);

	for (i = image->header.height_px - 1; i >= 0; i--) {
		Read(fd, &hex_image_data[i], 4);
		hex_image_data[i] = Swap2Bytes(~hex_image_data[i]);
	}

	for (i = 0; i < image->header.height_px; i++) {
		printf("0x%x, ", (uint16_t) hex_image_data[i]);
	}
}


BMPImage ReadBmpImageBigEndian(int fd)
{

}


void FillBMPHeader(int fd, BMPHeader *bmp)
{
	/* (54 bytes) */
	IoRead(fd, &bmp->type, 2);              /* Magic identifier: 0x4d42 */
	IoRead(fd, &bmp->size, 4);              /* File size in bytes */
	IoRead(fd, &bmp->reserved1, 2);         /* NOT USED */
	IoRead(fd, &bmp->reserved2, 2);         /* NOT USED */
	IoRead(fd, &bmp->offset, 4);            /* Offset to image data in bytes from beginning of file */
	IoRead(fd, &bmp->dib_header_size, 4);   /* DIB Header size in bytes (40 bytes) */
	IoRead(fd, &bmp->width_px, 4);          /* Width of the image */
	IoRead(fd, &bmp->height_px, 4);         /* Height of the image */
	IoRead(fd, &bmp->num_planes, 2);        /* Number of color planes */
	IoRead(fd, &bmp->bits_per_pixel, 2);    /* Bits per pixel */
	IoRead(fd, &bmp->compression, 4);       /* Compression type */
	IoRead(fd, &bmp->image_size_bytes, 4);  /* Image size in bytes */
	IoRead(fd, &bmp->x_resolution_ppm, 4);  /* Pixels per meter */
	IoRead(fd, &bmp->y_resolution_ppm, 4);  /* Pixels per meter */
	IoRead(fd, &bmp->num_colors, 4);        /* Number of colors */
	IoRead(fd, &bmp->important_colors, 4);  /* Important colors */
}


void PrintBitmapDetails(BMPHeader header)
{
	printf("\n===== BITMAP DETAILS =====\n\n");

	printf("type: %d\n", header.type);
	printf("size: %d\n", header.size);
	printf("reserved1: %d\n", header.reserved1);
	printf("reserved2: %d\n", header.reserved2);
	printf("header.offset: %d\n", header.offset);
	printf("header.dib_header_size: %d\n", header.dib_header_size);
	printf("header.width_px: %d\n", header.width_px);
	printf("header.height_px: %d\n", header.height_px);
	printf("header.num_planes: %d\n", header.num_planes);
	printf("header.bits_per_pixel: %d\n", header.bits_per_pixel);
	printf("header.compression: %d\n", header.compression);
	printf("header.image_size_bytes: %d\n", header.image_size_bytes);
	printf("header.x_resolution_ppm: %d\n", header.x_resolution_ppm);
	printf("header.y_resolution_ppm: %d\n", header.y_resolution_ppm);
	printf("header.num_colors: %d\n", header.num_colors);
	printf("header.important_colors: %d\n", header.important_colors);

}


#pragma clang diagnostic pop
