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

#ifndef BMP_RIP_ERRLIB_H
#define BMP_RIP_ERRLIB_H

#define ERR(format, ...) fprintf(stderr, "-> error in %s() line %d\n"format"\n", __func__, __LINE__, ##__VA_ARGS__)
#define ERRNO(format, ...) fprintf(stderr, "-> error in %s() line %d\n"format": %s\n", __func__, __LINE__, ##__VA_ARGS__, strerror(errno))

#define PRINT_ERRNO_AND_RETURN(format, ...) ERRNO(format, ##__VA_ARGS__); \
        return(EXIT_FAILURE)
#define PRINT_ERRNO_AND_EXIT(format, ...) ERRNO(format, ##__VA_ARGS__); \
        exit(EXIT_FAILURE)

#define PRINT_ERR_AND_RETURN(format, ...) ERR(format, ##__VA_ARGS__); \
        return(EXIT_FAILURE)
#define PRINT_ERR_AND_EXIT(format, ...) ERR(format, ##__VA_ARGS__); \
        exit(EXIT_FAILURE)

#endif //BMP_RIP_ERRLIB_H