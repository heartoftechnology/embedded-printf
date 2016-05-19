/*
 * 		Copyright (c) 2016, Christean van der Mijden
 * 		All rights reserved.
 *
 *		Filename   	: embedded_printf.h
 *		Author	  	: Christean van der Mijden
 *		Date		: 5 May 2016
 *		Version		: 1.00
 *
 *		Project		: N/A
 *		Processor	: N/A
 *		Component	: stripped down printf for embedded applications
 *		Compiler	: Cross GCC ARM
 *
 *
 *	This library is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU Lesser General Public License as published by the
 *	Free Software Foundation; either version 3.0 of the License, or (at your
 *	option) any later version.
 *
 *	This library is distributed in the hope that it will be useful, but WITHOUT
 *	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 *	for more details.
 *
 *	The GNU Lesser General Public License v3.0 can be found here:
 *
 *			http://www.gnu.org/licenses/lgpl-3.0.en.html
 *
 *
 *	In addition the following applies:
 *
 * 	Redistribution and use in source and binary forms, with or without
 * 	modification, are permitted provided that the following conditions
 * 	are met:
 *
 * 	o Redistributions of source code must retain the above copyright
 * 	  notice, this list of conditions and the following disclaimer.
 *
 * 	o Redistributions in binary form must reproduce the above copyright
 * 	  notice, this list of conditions and the following disclaimer in the
 * 	  documentation and/or other materials provided with the distribution.
 *
 * 	o Neither the name of Christean van der Mijden, Heart of Technology, nor the
 * 	  names of their contributors may be used to endorse or promote products
 * 	  derived from this software without specific prior written permission.
 *
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * 	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * 	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * 	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * 	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * 	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * 	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * 	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * 	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * 	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __EMBEDDED_PRINTF_H_
#define __EMBEDDED_PRINTF_H_


/*! @file
 *
 * Embedded printf is a stripped down version of the c standard printf function.
 *
 * It is almost a 1-1 copy of Tiny printf by Kustaa Nyholm. Yet with a lot of
 * comments explaining the code as well as descriptive variable names. In
 * addition embedded printf is written for 32bits ints.
 *
 * Tiny printf can be found here:
 *
 * 		http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *
 * Or its's revisited and even smaller footprint version here:
 *
 * 		http://www.sparetimelabs.com/printfrevisited/printfrevisited.php
 *
 * Instead of just implementing any version of Tiny printf I wanted to create a
 * source code that also helps to understand non-seasoned embedded programmers
 * what happens under the hood.
 *
 *
 * Note: Embedded printf is written for a 32bits microcontroller architecture.
 *
 *
 * To use this library:
 *
 * 1: Provide definitions for the standard types uint8_t, uint32_t and int32_t
 * 	  by including your header
 * 2: Provide a definition of ASSERT by including your header
 * 3: Provide a means to output the characters:
 * 	  - include the header providing the function definition for your function
 * 	  - use the macro #define embedded_putChar(u8character) to map this to your
 * 	    putChar (or similar) function.
 *
 */
#include <stdarg.h> 	/*<! required for the va_list library functions */


/* Include(s) to provide standard types and ASSERT */




/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!< Macro to map the character output to the application specific output */
#define embedded_putChar(u8character)			//UART_PutChar(u8character)



/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Prints a formatted string to the specified output
 *
 * @param [in]  format  The string to be formatted and printed to the output
 * @param [in]  ...		list of all the variables that need formatting into the
 * 						input string
 * @param [out] none
 *
 * @Description
 *
 * Standard c library printf formatting tag prototype:
 *
 * 				%[flags][width][.precision][length]specifier
 *
 * Embedded printf stripped down formatting tag prototype:
 *
 * 				%[flags][width]specifier
 *
 * Supported flags:
 * 		0	zero padding
 *
 * Supported width:
 * 		Up to 255
 *
 * Supported precision
 * 		none
 *
 * Supported length
 * 		none	see note 1.
 *
 * Supported specifiers:
 * 		c	single character
 * 		d	signed decimal integer
 * 		i	<same as d>
 * 		s	string of characters
 * 		u	unsigned decimal integer
 * 		x	unsigned hexadecimal integer
 * 		X	unsigner hexadecimal integer with capital letters
 *
 * Notes:
 * 		1. All integers are interpreted as 32 bits
 * 		2. Characters (char) are unsigned 8 bits
 * 		3. All hexadecimal output is preceded with 0x;
 */
void embedded_printf(const uint8_t *format, ...);


#if defined(__cplusplus)
}
#endif


#endif /* __EMBEDDED_PRINTF_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
