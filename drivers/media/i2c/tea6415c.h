/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __INCLUDED_TEA6415C__
#define __INCLUDED_TEA6415C__

/* the tea6415c's design is quite brain-dead. although there are
   8 inputs and 6 outputs, these aren't enumerated in any way. because
   I don't want to say "connect input pin 20 to output pin 17", I define
   a "virtual" pin-order. */

/* input pins */
#define TEA6415C_OUTPUT1 18
#define TEA6415C_OUTPUT2 14
#define TEA6415C_OUTPUT3 16
#define TEA6415C_OUTPUT4 17
#define TEA6415C_OUTPUT5 13
#define TEA6415C_OUTPUT6 15

/* output pins */
#define TEA6415C_INPUT1 5
#define TEA6415C_INPUT2 8
#define TEA6415C_INPUT3 3
#define TEA6415C_INPUT4 20
#define TEA6415C_INPUT5 6
#define TEA6415C_INPUT6 10
#define TEA6415C_INPUT7 1
#define TEA6415C_INPUT8 11

#endif
