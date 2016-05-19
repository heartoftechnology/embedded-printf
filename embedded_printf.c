/*
 * 		Copyright (c) 2016, Christean van der Mijden
 * 		All rights reserved.
 *
 *		Filename   	: embedded_printf.c
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
#include "embedded_printf.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FLAG_HEX_USE_CAPITALS	(0x1)
#define FLAG_USE_ZERO_PADDING	(0x2)
#define FLAG_IS_NOT_FIRST_DIGIT (0x4)


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*
 * The output buffer is used to store the formatted variable before sending it
 * to the output. It's length of 12 characters is derived as follows:
 *
 * - A 32bits unsigned int requires 10 digits
 * - A 32 bits int requires 10 digits + sign digit = 11 digits
 * - A 32bits hex requires 8 digits + 2 for 0x = 10 digits
 * - Every string needs to be '\0' terminated = 1 digit
 *
 * So the maximum characters supporting up to 32bits variables is 12.
 *
 */
/* buffer to temporary store a formatted number as a string */
static uint8_t outputBuffer[12];

/* pointer used to reference the outputBuffer */
static uint8_t * outputBufferPtr;

/* Variable to hold internal flags */
static uint8_t embpf_InternalFlags = 0u;


/*******************************************************************************
 * Private function declaration
 ******************************************************************************/

/*
 * Puts a character into the output buffer string and increases the position
 * pointer by 1
 */
static void putInOutputBuffer(uint8_t character);

/*
 * Turns the decimal or hexadecimal digit into it's ASCII equivalent and puts
 * it into the output buffer
 */
static void putDigitInOutputBuffer(uint8_t digit);

/*
 * Divides the u32integerNumber by the dividend and puts the number of
 * divisions into the output buffer
 */
static void divideAndPutInOutputBuffer(uint32_t * number, uint32_t dividend);



/*******************************************************************************
 * API
 ******************************************************************************/


/*FUNCTION**********************************************************************
 *
 * Function Name : embedded_printf
 * Description   : A stripped down version of the c standard printf function.
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
 *
 * Comments:
 * - I decided to create this function without any use of 'goto'
 * - Remember "var = *(p++)" results in "1: var = *p; 2: p = p+1;" This is since
 *   i++ means increase i, but return the value if i before increasing.
 *
 *
 *END**************************************************************************/
void embedded_printf(const uint8_t *format, ...)
{
	/* Variable to contain the list of arguments */
	va_list arguments;

	/* Variable to temporarily contain the character that is evaluated */
	uint8_t currentCharacter;

	/* Pointer to the start of the string that is to be passed to the output */
	uint8_t * outputStringPtr;

	/* Variable to keep track of the width */
	uint8_t formatWidth = 0u;

	/* temporary value to pass a number for formatting */
	static uint32_t u32integerNumber;

	/* Clear all flags */
	embpf_InternalFlags = 0u;

	/* Initialize the pointer to the variable length argument list. */
	va_start(arguments, format);

	/* Put the first character of the format into the evaluation variable */
	currentCharacter = *format;

	/*
	 * Step through the input string and evaluate each character:
	 */
	currentCharacter = *(format++);
	while(currentCharacter)
	{
		/*
		 *	As long as the character is NOT a %: directly pass the current
		 * 	character directly to the output function.
		 */
		if('%' != currentCharacter)
		{
			embedded_putChar(currentCharacter);
		}

		/* When a '%' is encountered it means formatting is required. */
		else
		{
			formatWidth = 0u;

			/* Get next character (i.e. the one following the '%') */
			currentCharacter = *(format++);

			/*
			 *	Check if the current character is a '0'. if so, set a flag
			 *	indicating zero padding must be done.
			 *	Then get the next character.
			 */
			if('0' == currentCharacter)
			{
				embpf_InternalFlags |= FLAG_USE_ZERO_PADDING;
				currentCharacter = *(format++);
			}

			/*
			 * See if there is a character between '0' and '9'. If so, this
			 * specifies the width.
			 */
			if(('0' <= currentCharacter) && ('9' >= currentCharacter))
			{
				/*
				 * For every decimal number, the previous width must be *10.
				 * Then add the newly found value. (Starting at width = 0u)
				 * Repeat until no characters between '0' and '9' are found.
				 */
				while(('0' <= currentCharacter) && ('9' >= currentCharacter))
				{
					/*
					 * Shifting is faster and smaller than multiplication so:
					 * formatWidth*10 is done as follows:
					 * 1) Shift left by 2. This is the same as *4
					 * 2) Add its original self. To the result: Now we have *5
					 * 3) Shift left by 1. This is the same as *2. Now the total
					 * 	  is *10
					 */
					formatWidth = (((formatWidth << 2u) + formatWidth) << 1u);

					/* Add integer value of current decimal character */
					formatWidth += (currentCharacter - '0');

					currentCharacter = *(format++);
				}
			}

			outputBufferPtr = outputBuffer;
			outputStringPtr	= outputBuffer;

			/* Now determine the specifier and act accordingly */
			switch(currentCharacter)
			{
				case 'u':
				case 'i':
				case 'd':
					u32integerNumber = va_arg(arguments, uint32_t);

					/* Check if integer is actually signed */
					if(('d' == currentCharacter) || ('i' == currentCharacter))
					{
						/*
						 * Check if the signed integer < 0
						 * If so take 2's complement and put a '-' sign to the
						 * outputBuffer
						 */
						if((int32_t)u32integerNumber < 0)
						{
							u32integerNumber = ((~u32integerNumber) + 1u);
							putInOutputBuffer('-');
						}
					}

					/*
					 * Now to determine all digits that are in the integer.
					 * Say we have a number of 654321. Somehow we need to get
					 * the 6 first, then the 5, then the 4 etc..
					 *
					 * Now let's subtract 100000 from the number and count how
					 * many times we can do that until the remainder is smaller
					 * than 100000.
					 *
					 * We end up with a count of 6 and a remainder of 54321
					 * The count is our first digit and can be put into the
					 * output buffer.
					 *
					 * Now repeat this with a dividend of 10000. We end up with
					 * a count of 5 and a remainder of 4321.
					 *
					 * See where we're going?
					 *
					 * We can stop dividing after we did the 10s. As only the
					 * 1s are left and they can be put directly into the output
					 * buffer.
					 *
					 * Now for a 32bits integer we need to start dividing by
					 * 1.000.000.000
					 *
					 */
					divideAndPutInOutputBuffer(&u32integerNumber, 1000000000u);
					divideAndPutInOutputBuffer(&u32integerNumber,  100000000u);
					divideAndPutInOutputBuffer(&u32integerNumber,   10000000u);
					divideAndPutInOutputBuffer(&u32integerNumber,    1000000u);
					divideAndPutInOutputBuffer(&u32integerNumber,     100000u);
					divideAndPutInOutputBuffer(&u32integerNumber,      10000u);
					divideAndPutInOutputBuffer(&u32integerNumber,       1000u);
					divideAndPutInOutputBuffer(&u32integerNumber,        100u);
					divideAndPutInOutputBuffer(&u32integerNumber,         10u);
					putDigitInOutputBuffer(u32integerNumber);

					/* Clear flag in case another number must be formatted */
					embpf_InternalFlags &= ~FLAG_IS_NOT_FIRST_DIGIT;

					break;

				case 'x':
				case 'X':
					if('X' == currentCharacter)
					{
						embpf_InternalFlags |= FLAG_HEX_USE_CAPITALS;
					}

					u32integerNumber = va_arg(arguments, uint32_t);

					/*
					 * The principle of getting the digits out is the same as
					 * above with the decimal integers. Now however we need to
					 * divide by powers of 16.
					 *
					 * For a 32bits hexadecimal value we start to divide with
					 * 0x10000000
					 */
					divideAndPutInOutputBuffer(&u32integerNumber, 0x10000000);
					divideAndPutInOutputBuffer(&u32integerNumber, 0x1000000);
					divideAndPutInOutputBuffer(&u32integerNumber, 0x100000);
					divideAndPutInOutputBuffer(&u32integerNumber, 0x10000);
					divideAndPutInOutputBuffer(&u32integerNumber, 0x1000);
					divideAndPutInOutputBuffer(&u32integerNumber, 0x100);
					divideAndPutInOutputBuffer(&u32integerNumber, 0x10);
					putDigitInOutputBuffer(u32integerNumber);
					break;

				case 'c':
					/*
					 * Get the character from the arguments list and put it into
					 * the buffer
					 */
					putInOutputBuffer((uint8_t)(va_arg(arguments, uint32_t)));
					break;

				case 's':
					/*
					 * The variable is already a string, so set the
					 * outputStringPtr to this string instead
					 */
					outputStringPtr = (uint8_t *)(va_arg(arguments, uint8_t *));
					break;

				case '%':
					putInOutputBuffer('%');
					break;

				default:
					break;
			} /* switch(currentCharacter) */

			/* Add string terminator to buffer */
			*outputBufferPtr = '\0';
			outputBufferPtr = outputStringPtr;

			/* Subtract the buffered string length from the formatWidth */
			while((*(outputBufferPtr++)) && (formatWidth > 0u))
			{
				formatWidth--;
			}

			/*
			 * If the width > string: put the required zeros or spaces to the
			 * output function
			 */
			while(formatWidth-- > 0u)
			{
				if(embpf_InternalFlags && FLAG_USE_ZERO_PADDING)
				{
					embedded_putChar('0');
				}
				else
				{
					embedded_putChar(' ');
				}
			}

			/*
			 * Pass the formatted string to the output function, one character
			 * at at time
			 */
			currentCharacter = *(outputStringPtr++);
			while(currentCharacter)
			{
				embedded_putChar(currentCharacter);
				currentCharacter = *(outputStringPtr++);
			}

		} /* '%' == currentCharacter */

		/* Get next character */
		currentCharacter = *(format++);

	} /* while(currentCharacter = *(format++)) */

	/* Cleanup the variable length argument list. */
	va_end(arguments);

	return;
}


/*******************************************************************************
 * Private functions
 ******************************************************************************/


/*FUNCTION**********************************************************************
 *
 * Function Name : putInOutputBuffer
 * Description   : Puts a character into the output buffer string and increases
 * 				   the position pointer by 1
 *
 * Param [in] character : the character to be put into the buffer
 *
 *END**************************************************************************/
static void putInOutputBuffer(uint8_t character)
{
	*(outputBufferPtr++) = character;
	return;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : putDigitInOutputBuffer
 * Description   : Puts a character into the output buffer string and increases
 * 				   the position pointer by 1
 *
 * Param [in] outputDigit :
 * 						a decimal unsigned integer value between 0 an 9 	OR
 * 					  	a hexadecimal unsigned integer value between 0 and 15
 *
 *END**************************************************************************/
static void putDigitInOutputBuffer(uint8_t outputDigit)
{
	ASSERT(15u >= outputDigit);

	/* A digit between 0 and 9 can be turned into ASCII directly */
	if(10u > outputDigit)
	{
		putInOutputBuffer(outputDigit + '0');
	}
	/* A digit between 10 and 15 is a hexadecimal letter. */
	else
	{
		if(embpf_InternalFlags && FLAG_HEX_USE_CAPITALS)
		{
			putInOutputBuffer((outputDigit - 10u) + 'A');
		}
		else
		{
			putInOutputBuffer((outputDigit - 10u) + 'a');
		}
	}

	embpf_InternalFlags |= FLAG_IS_NOT_FIRST_DIGIT;

	return;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : divideAndPutInOutputBuffer
 * Description   : Divides the u32integerNumber by the dividend and puts the
 * 				   number of divisions into the output buffer
 *
 * Param [in] dividend : the value to divide by
 *
 *END**************************************************************************/
static void divideAndPutInOutputBuffer(uint32_t * number, uint32_t dividend)
{
	uint8_t outputDigit = 0u;

	/* Count how many times the division can be done */
	while((*number) > dividend)
	{
		(*number) -= dividend;
		outputDigit++;
	}

	/* Print the count if >0, OR if it's a zero that follows a non-zero digit */
	if((outputDigit > 0u) || (embpf_InternalFlags && FLAG_IS_NOT_FIRST_DIGIT))
	{
		putDigitInOutputBuffer(outputDigit);
	}

	return;
}


/*******************************************************************************
 * EOF
 ******************************************************************************/
