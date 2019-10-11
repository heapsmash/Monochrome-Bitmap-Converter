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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "syscalls.h"
#include "errlib.h"
#include "endianlib.h"

#include "bmp.h"


int main(int argc, char *argv[], char *envp[])
{
	int opt, bitmap_to_read_fd, c_file_to_write_fd;
	BMPImage bitmap;

	g_file_name_to_read = g_array_name_to_store = g_file_name_to_save = NULL;
	while ((opt = getopt(argc, argv, "f:a:s:")) != -1) {
		switch (opt) {
			case 'f':
				CHECK_SIZE(optarg, __FILE_MAX__)
				g_file_name_to_read = optarg; /* the file_name.bmp to extract */
				break;
			case 'a':
				CHECK_SIZE(optarg, __ARRAY_MAX__)
				g_array_name_to_store = optarg; /* the array name */
				break;
			case 's':
				CHECK_SIZE(optarg, __FILE_MAX__)
				g_file_name_to_save = optarg; /* the file_name.c to save (.c must be included in the name) */
				break;
			default :
			PRINT_ERR_AND_RETURN("%s", g_usage)
		}
	}

	/* if NULL argv was left out */
	if (!g_file_name_to_read || !g_array_name_to_store || !g_file_name_to_save) {
		PRINT_ERR_AND_RETURN("%s", g_usage)
	}

	bitmap_to_read_fd = Open(g_file_name_to_read, O_RDONLY, 0);
	c_file_to_write_fd = Open(g_file_name_to_save, O_RDWR | O_CREAT | O_TRUNC, 0644);

	bitmap = ReadBmp(bitmap_to_read_fd);
	PrintBmpHeader(bitmap.header);
	WriteDataBigEndian(bitmap_to_read_fd, c_file_to_write_fd, &bitmap);

	return 0;
}


BMPImage ReadBmp(int fd)
{
	BMPImage img;
	FillBMPHeader(fd, &img.header);
	return img;
}


void WriteDataBigEndian(int bitmap_to_read_fd, int c_file_to_write_fd, BMPImage *image)
{
	int i, j;
	char temp_str[image->header.width_px];
	uint32_t hex_image_data[image->header.height_px];

	memset(image->data, '\0', sizeof image->data);
	if (Lseek(bitmap_to_read_fd, image->header.offset, SEEK_SET) != image->header.offset) {
		PRINT_ERR_AND_EXIT("Lseek failed")
	}
	Print(OUTPUT_TO_FD, "uint16 %s[] = {\n", g_array_name_to_store);
	for (i = image->header.height_px - 1; i >= 0; i--) { /* flip bitmap */
		Read(bitmap_to_read_fd, &hex_image_data[i], 4);
		hex_image_data[i] = Swap2Bytes((~hex_image_data[i])); /* negate, then correct endian */
		sprintf(temp_str, "0x%4x,\t", hex_image_data[i]);
		ReplaceSpaces(temp_str);
		Print(OUTPUT_TO_FD, "\t%s\n", temp_str);
	}
	Print(OUTPUT_TO_FD, "};\n\n");
}


void ReplaceSpaces(char *str)
{
	while (*str) {
		if (*str == ' ')
			*str = '0';
		str++;
	}
}


void FillBMPHeader(int fd, BMPHeader *bmp)
{
	/* (54 bytes) */
	IoRead(fd, &bmp->type, 2);
	IoRead(fd, &bmp->size, 4);
	IoRead(fd, &bmp->reserved1, 2);
	IoRead(fd, &bmp->reserved2, 2);
	IoRead(fd, &bmp->offset, 4);
	IoRead(fd, &bmp->dib_header_size, 4);
	IoRead(fd, &bmp->width_px, 4);
	IoRead(fd, &bmp->height_px, 4);
	IoRead(fd, &bmp->num_planes, 2);
	IoRead(fd, &bmp->bits_per_pixel, 2);
	IoRead(fd, &bmp->compression, 4);
	IoRead(fd, &bmp->image_size_bytes, 4);
	IoRead(fd, &bmp->x_resolution_ppm, 4);
	IoRead(fd, &bmp->y_resolution_ppm, 4);
	IoRead(fd, &bmp->num_colors, 4);
	IoRead(fd, &bmp->important_colors, 4);
}


void PrintBmpHeader(BMPHeader header)
{
	Print(OUTPUT_TO_FD, "/*\tCopyright (c) 2019, Tofu von Helmholtz aka Michael S. Walker\n"
						" *\tAll Rights Reserved in all Federations, including Alpha Centauris.\n"
						" *\n"
						" *\tGenerated with BitmapRipper:\n"
						" *\twww.github.com/heapsmash/Monochrome-Bitmap-Converter\n"
						" *\n"
						" *\t\t\t-= [Bitmap Header Data] =-\n"
						" *\ttype: %x\t\t\t(Magic identifier: 0x4d42)\n"
						" *\tsize: %d\t\t\t(File size in bytes)\n"
						" *\treserved1: %d\t\t\t(NOT USED)\n"
						" *\treserved2: %d\t\t\t(NOT USED)\n"
						" *\toffset: %d\t\t\t(Offset to image data in bytes from beginning of file [54 bytes])\n"
						" *\tdib_header_size: %d\t\t(DIB Header size in bytes [40 bytes])\n"
						" *\twidth_px: %d\t\t\t(Width of the image)\n"
						" *\theight_px: %d\t\t\t(Height of the image)\n"
						" *\tnum_planes: %d\t\t\t(Number of color planes)\n"
						" *\tbits_per_pixel: %d\t\t(Bits per pixel)\n"
						" *\tcompression: %d\t\t\t(Compression type)\n"
						" *\timage_size_bytes: %d\t\t(Image size in bytes)\n"
						" *\tx_resolution_ppm: %d\t\t(Pixels per meter)\n"
						" *\ty_resolution_ppm: %d\t\t(Pixels per meter)\n"
						" *\tnum_colors: %d\t\t\t(Number of colors)\n"
						" *\timportant_colors: %d\t\t(Important colors)\n"
						" *\t\t\t-====================-\n"
						" */\n\n", header.type, header.size, header.reserved1, header.reserved2,
		  header.offset, header.dib_header_size, header.width_px, header.height_px, header.num_planes,
		  header.bits_per_pixel, header.compression, header.image_size_bytes, header.x_resolution_ppm,
		  header.y_resolution_ppm, header.num_colors, header.important_colors);
}


#pragma clang diagnostic pop
