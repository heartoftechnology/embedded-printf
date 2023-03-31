/*
 * 		Copyright (C) 2023, Christean van der Mijden and Heart of Technology
 * 		All rights reserved.
 *
 *		Filename   	: embedded_printf.h
 *		Author	  	: Christean van der Mijden
 *		Date		: 31 March 2023
 *		Version		: 1.01
 *
 *		Project		: N/A
 *		Processor	: N/A
 *		Component	: stripped down printf for embedded applications
 *		Compiler	: GCC ARM
 *
 *	Revision History:
 *	------------------------------------------------------------------------
 *	5  May 2016				version 1
 *	31 March 2023
 *
 *
 *
 *	@license
 *
 *	This library is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU Lesser General Public License as published by the
 *	Free Software Foundation; either version 3.0 of the License, or (at your
 *	option) any later version.
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
 *
 *
 * Tiny printf license
 *
 * Copyright (C) 2004, 2008, Kustaa Nyholm
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#ifndef __EMBEDDED_PRINTF_H_
#define __EMBEDDED_PRINTF_H_


/*! @file
 *
 * Embedded printf is a stripped down version of the c standard printf function:
 * A tiny footprint printf function for 32bits embedded microcontroller
 * applications based on Kustaa Nyholm's Tiny printf
 *
 *
 * Embedded printf finds its origin in the search for a small printf function to
 * use with ARM Cortex-M microcontrollers. Printf in conjuction with a UART and
 * serial-to-USB converter is a convenient way to get debug information out of
 * the controller during development. However the C standard printf library
 * function is not really usable in microcontrollers with small ROM (Flash)
 * sizes as it takes a lot of space leaving little to none for the 'real' code.
 *
 * Kustaa Nyholm's Tiny printf and its revisited version were found through a
 * Google search for a printf alternative. Embedded printf was born out of the
 * desire not just to use tiny printf, but to actually understand what is going
 * on as well and to help others to do so too. Compared to Tiny printf, Embedded
 * printf is almost an exact copy of its structure and code. However with the
 * following changes:
 *
 * - Lots of comments to explain what is happening and why
 * - Using descriptive variable and function names
 * - No variable assignents inside the evaluation of a while/for/if statement
 * - No goto statements
 * - 32-bits architecture
 *
 * Tiny printf can be found here:
 *
 * 		http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *
 * Or its's revisited and even smaller footprint version here:
 *
 * 		http://www.sparetimelabs.com/printfrevisited/printfrevisited.php
 *
 *
 * To use this library:
 *
 * 1: Provide a definition of ASSERT(x) by including your header
 * 2: Provide a means to output the characters:
 * 	  - include the header providing the function definition for your function
 * 	  - use the macro #define embedded_putChar(u8character) to map this to your
 * 	    putChar (or similar) function.
 * 3: Optional: define a macro to map the standard printf to embedded_printf
 *
 */
#include <stdarg.h> 	/*<! required for the va_list library functions */
#include <stdint.h>		/*<! definition for platform independent types */
//#include "assert.h"	/*<! provide your own macro for ASSERT here */


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef ASSERT
	#error "Please provide a definition for ASSERT(x)"
#endif


/*!< Macro to map the character output to the application specific output */
#define embedded_putChar(u8character)			UART_PutChar(TERMINAL_PORT, \
														 	 u8character)
/*!< Macro to map printf to embedded_printf */
//#define printf(x)								embedded_printf(x)

/*!< Macro to map embedded_printf() to ebd_printf() so your code can use the
 * shorter ebd_printf() */
//#define embedded_printf(x)		ebd_printf(x)




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
 * %[flags][width][.precision][length]specifier
 *
 * Embedded printf stripped down formatting tag prototype:
 *
 * %[flags][width]specifier
 *
 * Supported flags:
 * 0	zero padding
 *
 * Supported width:
 * Up to 255
 *
 * Supported precision
 * none
 *
 * Supported length
 * none	see note 1.
 *
 * Supported specifiers:
 * c	single character
 * d	signed decimal integer
 * i	<same as d>
 * s	string of characters
 * u	unsigned decimal integer
 * x	unsigned hexadecimal integer
 * X	unsigned hexadecimal integer with capital letters
 *
 * @Note
 * 1. All integers are interpreted as 32 bits
 * 2. Characters (char) are unsigned 8 bits
 * 3. All hexadecimal output is preceded with 0x;
 */
void embedded_printf(const uint8_t *format, ...);


#if defined(__cplusplus)
}
#endif


#endif /* __EMBEDDED_PRINTF_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
