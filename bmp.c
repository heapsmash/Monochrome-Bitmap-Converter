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
#include <zconf.h>

#include "syscalls.h"
#include "errlib.h"
#include "endianlib.h"
#include "bmp.h"


int main(int argc, char *argv[], char *envp[])
{
	int opt;
	BitRipTools bitmap;

	bitmap.ncols_per_row = 1;
	bitmap.file_name_to_read = bitmap.array_name_to_store = bitmap.file_name_to_save = NULL;
	while ((opt = getopt(argc, argv, "f:a:s:")) != -1) {
		switch (opt) {
			case 'f':
				CHECK_SIZE(optarg, __FILE_MAX__)
				bitmap.file_name_to_read = optarg; /* the file_name.bmp to extract */
				break;
			case 'a':
				CHECK_SIZE(optarg, __ARRAY_MAX__)
				bitmap.array_name_to_store = optarg; /* the array name */
				break;
			case 's':
				CHECK_SIZE(optarg, __FILE_MAX__)
				bitmap.file_name_to_save = optarg; /* the file_name.c to save (.c must be included in the name) */
				break;
			case 'n':
				bitmap.ncols_per_row = strtol(optarg, NULL, 10);
				break;
			default :
			PRINT_ERR_AND_RETURN("%s", g_usage)
		}
	}

	/* if NULL argv was left out */
	if (!bitmap.file_name_to_read || !bitmap.array_name_to_store || !bitmap.file_name_to_save) {
		PRINT_ERR_AND_RETURN("%s", g_usage)
	}

	if ((errno == ERANGE && (bitmap.ncols_per_row == LONG_MAX || bitmap.ncols_per_row == LONG_MIN)) ||
		(errno != 0 && bitmap.ncols_per_row == 0)) {
		PRINT_ERRNO_AND_RETURN("-n value obfuscated")
	}

	bitmap.bitmap_to_read_fd = Open(bitmap.file_name_to_read, O_RDONLY, 0);
	bitmap.c_file_to_write_fd = Open(bitmap.file_name_to_save, O_RDWR | O_CREAT | O_TRUNC, 0664);

	ReadBitmapHeader(&bitmap);
	WriteCommentToFile(bitmap);
	WriteArrayToFile(&bitmap);

	return 0;
}


void WriteArrayToFile(BitRipTools *data)
{
	int i;
	uint32_t hex_image_data[data->header.height_px];

	if (Lseek(data->bitmap_to_read_fd, data->header.offset, SEEK_SET) != data->header.offset) {
		PRINT_ERR_AND_EXIT("Lseek failed")
	}

	Print(data->c_file_to_write_fd, "uint16 %s[] = {\n", data->array_name_to_store);
	for (i = data->header.height_px - 1; i >= 0; i--) { /* flip bitmap */
		Read(data->bitmap_to_read_fd, &hex_image_data[i], 4);
		hex_image_data[i] = Swap2Bytes((~hex_image_data[i])); /* negate, then correct endian */
		Print(data->c_file_to_write_fd, "0x%04x,\t", hex_image_data[i]);
	}

	Print(data->c_file_to_write_fd, "};\n\n");
}


void ReadBitmapHeader(BitRipTools *data)
{
	/* (54 bytes) */
	IoRead(data->bitmap_to_read_fd, &data->header.type, 2);
	IoRead(data->bitmap_to_read_fd, &data->header.size, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.reserved1, 2);
	IoRead(data->bitmap_to_read_fd, &data->header.reserved2, 2);
	IoRead(data->bitmap_to_read_fd, &data->header.offset, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.dib_header_size, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.width_px, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.height_px, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.num_planes, 2);
	IoRead(data->bitmap_to_read_fd, &data->header.bits_per_pixel, 2);
	IoRead(data->bitmap_to_read_fd, &data->header.compression, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.image_size_bytes, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.x_resolution_ppm, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.y_resolution_ppm, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.num_colors, 4);
	IoRead(data->bitmap_to_read_fd, &data->header.important_colors, 4);
}


void WriteCommentToFile(BitRipTools data)
{
	size_t i;
	const int header_values[] = {
			data.header.type,
			data.header.size,
			data.header.reserved1,
			data.header.reserved2,
			data.header.offset,
			data.header.dib_header_size,
			data.header.width_px,
			data.header.height_px,
			data.header.num_planes,
			data.header.bits_per_pixel,
			data.header.compression,
			data.header.image_size_bytes,
			data.header.x_resolution_ppm,
			data.header.y_resolution_ppm,
			data.header.num_colors,
			data.header.important_colors
	};

	Print(data.c_file_to_write_fd, "/*\tCopyright (c) 2019, Michael S. Walker (Tofu)\n");
	Print(data.c_file_to_write_fd, " *\tAll Rights Reserved in all Federations, including Alpha Centauris.\n");
	Print(data.c_file_to_write_fd, " *\n");
	Print(data.c_file_to_write_fd, " *\t.--.https://github.com/heapsmash/Monochrome-Bitmap-Converter-----------.\n");
	Print(data.c_file_to_write_fd, " *\t|  |    Header Name    | Value |              Description              |\n");
	Print(data.c_file_to_write_fd, " *\t:--+-------------------+-------+---------------------------------------:\n");
	Print(data.c_file_to_write_fd, " *\t|  | %-17s | %-5x | %-37s |\n", g_header_names[0], header_values[0], g_header_description[0]);
	Print(data.c_file_to_write_fd, " *\t:--+-------------------+-------+---------------------------------------:\n");
	for (i = 1; i < data.header.height_px; i++) {
		Print(data.c_file_to_write_fd, " *\t|  | %-17s | %-5d | %-37s |\n", g_header_names[i], header_values[i], g_header_description[i]);
		Print(data.c_file_to_write_fd, " *\t:--+-------------------+-------+---------------------------------------:\n");
	}
	Print(data.c_file_to_write_fd, " */\n\n");
}


#pragma clang diagnostic pop
