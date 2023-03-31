# Embedded printf
A tiny footprint printf function for 32bits embedded microcontroller applications based on [Kustaa Nyholm's Tiny printf](http://www.sparetimelabs.com/printfrevisited/printfrevisited.php)

Update 2023: If you are looking for a printf that has full functionality, but is also made to be small and fast for embedded systems, **and** even thread safe. Have a look at [eyalroz/printf](https://github.com/eyalroz/printf)

## A tiny printf for embedded use
Embedded printf finds its origin in the search for a small printf function to use with ARM Cortex-M microcontrollers. Printf in conjuction with a UART and serial-to-USB converter is a convenient way to get debug information out of the controller during development. However the C standard printf library function is not really usable in microcontrollers with small ROM (Flash) sizes as it takes a lot of space leaving little to none for the 'real' code.  

Kustaa Nyholm's Tiny printf and its revisited version were found through a Google search for a printf alternative. Embedded printf was born out of the desire not just to use tiny printf, but to actually understand what is going on as well and to help others to do so too. Compared to Tiny printf, Embedded printf is almost an exact copy of its structure and code. However with the following changes:
- Lots of comments to explain what is happening and why
- Using descriptive variable and function names
- No variable assignents inside the evaluation of a while/for/if statement
- No goto statements
- 32-bits architecture

## Supported printf functionality
Embedded printf can be used as a regular printf function, but with limited functionality.  

####Format tag prototype

The C standard printf function uses the following format tag prototype:  
```
%[flags][width][.precision][length]specifier
```
Embedded printf does **not support floating point** nor the **length parameter** and so has a stripped down format tag prototype:
```
%[flags][width]specifier
```
####Supported flags:   

flag | description
-----|----------
0 | zero padding (instead of spaces)

####Supported width:
255 (8 bits)

####Supported specifiers:   

specifier | output
----------|----------
c | single character
d | signed decimal integer
i	| (same as d)
s	| string of characters
u	| unsigned decimal integer
x	| unsigned hexadecimal integer
X	| unsigner hexadecimal integer with capital letters



## How to use the library
The function embedded_printf() can be used in the same way as printf is used but with limited functionality as described above. In order to get the library running in a project follow these steps:  

1. In embedded_printf.h, include the definition for **ASSERT**.
2. In embedded_printf.h, edit the macro **embedded_putChar(u8character)** to map it to the desired character output function. See code below which uses the UART_PutChar function as example:  

    ```c
    /*!< Macro to map the character output to the application specific output */
    #define embedded_putChar(u8character)			UART_PutChar(u8character)
    ```
3. If desired: in embedded_printf.h, define a macro to map printf to embedded_printf:  

    ```c
    /*!< Macro to map printf to embedded_printf */
    #undef  printf(x)      //optional
    #define printf(x)			embedded_printf(x)
    ```
	**Warning** modern compilers recognise printf() like statements and may ignore embedded_printf, and simply pull in the C standard code anyway. If you prefer a shorter function name it may be best to use emb_printf() for example;
    ```c
    /*!< Macro to map embedded_printf() to ebd_printf() so your code can use the shorter ebd_printf()*/
    #define embedded_printf(x)		ebd_printf(x)
    ```	

## License
Since embedded printf is mostly a rewrite of Tiny printf two licenses apply: the Tiny printf license and the Embedded printf license.

### Embedded printf license and disclaimer  

Copyright © 2023, Christean van der Mijden and Heart of Technology. All rights reserved.  

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option) any later version. The GNU Lesser General Public License v3.0 can be found here: http://www.gnu.org/licenses/lgpl-3.0.en.html.  

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:  

- Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.  

- Redistributions in binary form must reproduce the above copyright  notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.  

- Neither the name of Heart of Technology, nor the names of their contributors may be used to endorse or promote products derived from this software without specific prior written permission.  

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

### Tiny printf license  

Copyright © 2004, 2008,  Kustaa Nyholm  

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.  

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.  

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  

