======================================
Parallel port LCD/Keypad Panel support
======================================

Some LCDs allow you to define up to 8 characters, mapped to ASCII
characters 0 to 7. The escape code to define a new character is
'\e[LG' followed by one digit from 0 to 7, representing the character
number, and up to 8 couples of hex digits terminated by a semi-colon
(';'). Each couple of digits represents a line, with 1-bits for each
illuminated pixel with LSB on the right. Lines are numbered from the
top of the character to the bottom. On a 5x7 matrix, only the 5 lower
bits of the 7 first bytes are used for each character. If the string
is incomplete, only complete lines will be redefined. Here are some
examples::

  printf "\e[LG0010101050D1F0C04;"  => 0 = [enter]
  printf "\e[LG1040E1F0000000000;"  => 1 = [up]
  printf "\e[LG2000000001F0E0400;"  => 2 = [down]
  printf "\e[LG3040E1F001F0E0400;"  => 3 = [up-down]
  printf "\e[LG40002060E1E0E0602;"  => 4 = [left]
  printf "\e[LG500080C0E0F0E0C08;"  => 5 = [right]
  printf "\e[LG60016051516141400;"  => 6 = "IP"

  printf "\e[LG00103071F1F070301;"  => big speaker
  printf "\e[LG00002061E1E060200;"  => small speaker

Willy
