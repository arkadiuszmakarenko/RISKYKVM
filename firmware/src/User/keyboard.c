#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "keyboard.h"
#include "usb_keyboard.h"
#include "gpio.h"


uint8_t charToSend;
uint8_t processFlag = 1;
uint8_t i = 0;



static HID_Keyboard_Data prevkeycode = {
		.lctrl          = 0,
		.lshift         = 0,
		.lalt           = 0,
		.lgui           = 0,
		.rctrl          = 0,
		.rshift         = 0,
		.ralt           = 0,
		.rgui           = 0,
		.keys[0]        = 0,
		.keys[1]        = 0,
		.keys[2]        = 0,
		.keys[3]        = 0,
		.keys[4]        = 0,
		.keys[5]        = 0,
};


#define KEY_BUFF_SIZE							50




#define KEY_NONE                               0x00
#define KEY_ERRORROLLOVER                      0x01
#define KEY_POSTFAIL                           0x02
#define KEY_ERRORUNDEFINED                     0x03
#define KEY_A                                  0x04
#define KEY_B                                  0x05
#define KEY_C                                  0x06
#define KEY_D                                  0x07
#define KEY_E                                  0x08
#define KEY_F                                  0x09
#define KEY_G                                  0x0A
#define KEY_H                                  0x0B
#define KEY_I                                  0x0C
#define KEY_J                                  0x0D
#define KEY_K                                  0x0E
#define KEY_L                                  0x0F
#define KEY_M                                  0x10
#define KEY_N                                  0x11
#define KEY_O                                  0x12
#define KEY_P                                  0x13
#define KEY_Q                                  0x14
#define KEY_R                                  0x15
#define KEY_S                                  0x16
#define KEY_T                                  0x17
#define KEY_U                                  0x18
#define KEY_V                                  0x19
#define KEY_W                                  0x1A
#define KEY_X                                  0x1B
#define KEY_Y                                  0x1C
#define KEY_Z                                  0x1D
#define KEY_1_EXCLAMATION_MARK                 0x1E
#define KEY_2_AT                               0x1F
#define KEY_3_NUMBER_SIGN                      0x20
#define KEY_4_DOLLAR                           0x21
#define KEY_5_PERCENT                          0x22
#define KEY_6_CARET                            0x23
#define KEY_7_AMPERSAND                        0x24
#define KEY_8_ASTERISK                         0x25
#define KEY_9_OPARENTHESIS                     0x26
#define KEY_0_CPARENTHESIS                     0x27
#define KEY_ENTER                              0x28
#define KEY_ESCAPE                             0x29
#define KEY_BACKSPACE                          0x2A
#define KEY_TAB                                0x2B
#define KEY_SPACEBAR                           0x2C
#define KEY_MINUS_UNDERSCORE                   0x2D
#define KEY_EQUAL_PLUS                         0x2E
#define KEY_OBRACKET_AND_OBRACE                0x2F
#define KEY_CBRACKET_AND_CBRACE                0x30
#define KEY_BACKSLASH_VERTICAL_BAR             0x31
#define KEY_NONUS_NUMBER_SIGN_TILDE            0x32
#define KEY_SEMICOLON_COLON                    0x33
#define KEY_SINGLE_AND_DOUBLE_QUOTE            0x34
#define KEY_GRAVE_ACCENT_AND_TILDE             0x35
#define KEY_COMMA_AND_LESS                     0x36
#define KEY_DOT_GREATER                        0x37
#define KEY_SLASH_QUESTION                     0x38
#define KEY_CAPS_LOCK                          0x39
#define KEY_F1                                 0x3A
#define KEY_F2                                 0x3B
#define KEY_F3                                 0x3C
#define KEY_F4                                 0x3D
#define KEY_F5                                 0x3E
#define KEY_F6                                 0x3F
#define KEY_F7                                 0x40
#define KEY_F8                                 0x41
#define KEY_F9                                 0x42
#define KEY_F10                                0x43
#define KEY_F11                                0x44
#define KEY_F12                                0x45
#define KEY_PRINTSCREEN                        0x46
#define KEY_SCROLL_LOCK                        0x47
#define KEY_PAUSE                              0x48
#define KEY_INSERT                             0x49
#define KEY_HOME                               0x4A
#define KEY_PAGEUP                             0x4B
#define KEY_DELETE                             0x4C
#define KEY_END1                               0x4D
#define KEY_PAGEDOWN                           0x4E
#define KEY_RIGHTARROW                         0x4F
#define KEY_LEFTARROW                          0x50
#define KEY_DOWNARROW                          0x51
#define KEY_UPARROW                            0x52
#define KEY_KEYPAD_NUM_LOCK_AND_CLEAR          0x53
#define KEY_KEYPAD_SLASH                       0x54
#define KEY_KEYPAD_ASTERIKS                    0x55
#define KEY_KEYPAD_MINUS                       0x56
#define KEY_KEYPAD_PLUS                        0x57
#define KEY_KEYPAD_ENTER                       0x58
#define KEY_KEYPAD_1_END                       0x59
#define KEY_KEYPAD_2_DOWN_ARROW                0x5A
#define KEY_KEYPAD_3_PAGEDN                    0x5B
#define KEY_KEYPAD_4_LEFT_ARROW                0x5C
#define KEY_KEYPAD_5                           0x5D
#define KEY_KEYPAD_6_RIGHT_ARROW               0x5E
#define KEY_KEYPAD_7_HOME                      0x5F
#define KEY_KEYPAD_8_UP_ARROW                  0x60
#define KEY_KEYPAD_9_PAGEUP                    0x61
#define KEY_KEYPAD_0_INSERT                    0x62
#define KEY_KEYPAD_DECIMAL_SEPARATOR_DELETE    0x63
#define KEY_NONUS_BACK_SLASH_VERTICAL_BAR      0x64
#define KEY_APPLICATION                        0x65
#define KEY_POWER                              0x66
#define KEY_KEYPAD_EQUAL                       0x67
#define KEY_F13                                0x68
#define KEY_F14                                0x69
#define KEY_F15                                0x6A
#define KEY_F16                                0x6B
#define KEY_F17                                0x6C
#define KEY_F18                                0x6D
#define KEY_F19                                0x6E
#define KEY_F20                                0x6F
#define KEY_F21                                0x70
#define KEY_F22                                0x71
#define KEY_F23                                0x72
#define KEY_F24                                0x73
#define KEY_EXECUTE                            0x74
#define KEY_HELP                               0x75
#define KEY_MENU                               0x76
#define KEY_SELECT                             0x77
#define KEY_STOP                               0x78
#define KEY_AGAIN                              0x79
#define KEY_UNDO                               0x7A
#define KEY_CUT                                0x7B
#define KEY_COPY                               0x7C
#define KEY_PASTE                              0x7D
#define KEY_FIND                               0x7E
#define KEY_MUTE                               0x7F
#define KEY_VOLUME_UP                          0x80
#define KEY_VOLUME_DOWN                        0x81
#define KEY_LOCKING_CAPS_LOCK                  0x82
#define KEY_LOCKING_NUM_LOCK                   0x83
#define KEY_LOCKING_SCROLL_LOCK                0x84
#define KEY_KEYPAD_COMMA                       0x85
#define KEY_KEYPAD_EQUAL_SIGN                  0x86
#define KEY_INTERNATIONAL1                     0x87
#define KEY_INTERNATIONAL2                     0x88
#define KEY_INTERNATIONAL3                     0x89
#define KEY_INTERNATIONAL4                     0x8A
#define KEY_INTERNATIONAL5                     0x8B
#define KEY_INTERNATIONAL6                     0x8C
#define KEY_INTERNATIONAL7                     0x8D
#define KEY_INTERNATIONAL8                     0x8E
#define KEY_INTERNATIONAL9                     0x8F
#define KEY_LANG1                              0x90
#define KEY_LANG2                              0x91
#define KEY_LANG3                              0x92
#define KEY_LANG4                              0x93
#define KEY_LANG5                              0x94
#define KEY_LANG6                              0x95
#define KEY_LANG7                              0x96
#define KEY_LANG8                              0x97
#define KEY_LANG9                              0x98
#define KEY_ALTERNATE_ERASE                    0x99
#define KEY_SYSREQ                             0x9A
#define KEY_CANCEL                             0x9B
#define KEY_CLEAR                              0x9C
#define KEY_PRIOR                              0x9D
#define KEY_RETURN                             0x9E
#define KEY_SEPARATOR                          0x9F
#define KEY_OUT                                0xA0
#define KEY_OPER                               0xA1
#define KEY_CLEAR_AGAIN                        0xA2
#define KEY_CRSEL                              0xA3
#define KEY_EXSEL                              0xA4
#define KEY_KEYPAD_00                          0xB0
#define KEY_KEYPAD_000                         0xB1
#define KEY_THOUSANDS_SEPARATOR                0xB2
#define KEY_DECIMAL_SEPARATOR                  0xB3
#define KEY_CURRENCY_UNIT                      0xB4
#define KEY_CURRENCY_SUB_UNIT                  0xB5
#define KEY_KEYPAD_OPARENTHESIS                0xB6
#define KEY_KEYPAD_CPARENTHESIS                0xB7
#define KEY_KEYPAD_OBRACE                      0xB8
#define KEY_KEYPAD_CBRACE                      0xB9
#define KEY_KEYPAD_TAB                         0xBA
#define KEY_KEYPAD_BACKSPACE                   0xBB
#define KEY_KEYPAD_A                           0xBC
#define KEY_KEYPAD_B                           0xBD
#define KEY_KEYPAD_C                           0xBE
#define KEY_KEYPAD_D                           0xBF
#define KEY_KEYPAD_E                           0xC0
#define KEY_KEYPAD_F                           0xC1
#define KEY_KEYPAD_XOR                         0xC2
#define KEY_KEYPAD_CARET                       0xC3
#define KEY_KEYPAD_PERCENT                     0xC4
#define KEY_KEYPAD_LESS                        0xC5
#define KEY_KEYPAD_GREATER                     0xC6
#define KEY_KEYPAD_AMPERSAND                   0xC7
#define KEY_KEYPAD_LOGICAL_AND                 0xC8
#define KEY_KEYPAD_VERTICAL_BAR                0xC9
#define KEY_KEYPAD_LOGIACL_OR                  0xCA
#define KEY_KEYPAD_COLON                       0xCB
#define KEY_KEYPAD_NUMBER_SIGN                 0xCC
#define KEY_KEYPAD_SPACE                       0xCD
#define KEY_KEYPAD_AT                          0xCE
#define KEY_KEYPAD_EXCLAMATION_MARK            0xCF
#define KEY_KEYPAD_MEMORY_STORE                0xD0
#define KEY_KEYPAD_MEMORY_RECALL               0xD1
#define KEY_KEYPAD_MEMORY_CLEAR                0xD2
#define KEY_KEYPAD_MEMORY_ADD                  0xD3
#define KEY_KEYPAD_MEMORY_SUBTRACT             0xD4
#define KEY_KEYPAD_MEMORY_MULTIPLY             0xD5
#define KEY_KEYPAD_MEMORY_DIVIDE               0xD6
#define KEY_KEYPAD_PLUSMINUS                   0xD7
#define KEY_KEYPAD_CLEAR                       0xD8
#define KEY_KEYPAD_CLEAR_ENTRY                 0xD9
#define KEY_KEYPAD_BINARY                      0xDA
#define KEY_KEYPAD_OCTAL                       0xDB
#define KEY_KEYPAD_DECIMAL                     0xDC
#define KEY_KEYPAD_HEXADECIMAL                 0xDD
#define KEY_LEFTCONTROL                        0xE0
#define KEY_LEFTSHIFT                          0xE1
#define KEY_LEFTALT                            0xE2
#define KEY_LEFT_GUI                           0xE3
#define KEY_RIGHTCONTROL                       0xE4
#define KEY_RIGHTSHIFT                         0xE5
#define KEY_RIGHTALT                           0xE6
#define KEY_RIGHT_GUI                          0xE7

#define KEYCODE_TAB_SIZE      0x72 /* da 0x00 a 0x6F */
#define OK_RESET    3 /* 3 special keys to have a KBRESET */

static const uint8_t scancodeamiga[KEYCODE_TAB_SIZE][2] =
{
 	// SCANCODE USB to AMIGA
 	// --------------------------------------------------------------
 	// Need the real scancode by testing all keys on a real keyboard.
 	// --------------------------------------------------------------
	{KEY_GRAVE_ACCENT_AND_TILDE, 0x00 }, // ~
	{KEY_1_EXCLAMATION_MARK,     0x01 }, // 1!
	{KEY_2_AT,                   0x02 }, // 2@
	{KEY_3_NUMBER_SIGN,          0x03 }, // 3#
	{KEY_4_DOLLAR,               0x04 }, // 4$
	{KEY_5_PERCENT,              0x05 }, // 5%
	{KEY_6_CARET,                0x06 }, // 6^
	{KEY_7_AMPERSAND,            0x07 }, // 7&
	{KEY_8_ASTERISK,             0x08 }, // 8*
	{KEY_9_OPARENTHESIS,         0x09 }, // 9(
	{KEY_0_CPARENTHESIS,         0x0A }, // 0)
	{KEY_MINUS_UNDERSCORE,       0x0B }, // -_
	{KEY_EQUAL_PLUS,             0x0C }, // +=
	{KEY_BACKSLASH_VERTICAL_BAR, 0x0D }, // |
	{KEY_NONUS_BACK_SLASH_VERTICAL_BAR, 0x0D},
	{KEY_KEYPAD_0_INSERT,        0x0F }, // NUM 0
	{KEY_Q,                      0x10 }, // Q
	{KEY_W,                      0x11 }, // W
	{KEY_E,                      0x12 }, // E
	{KEY_R,                      0x13 }, // R
	{KEY_T,                      0x14 }, // T
	{KEY_Y,                      0x15 }, // Y
	{KEY_U,                      0x16 }, // U
	{KEY_I,                      0x17 }, // I
	{KEY_O,                      0x18 }, // O
	{KEY_P,                      0x19 }, // P
	{KEY_OBRACKET_AND_OBRACE,    0x1A }, // [{
	{KEY_CBRACKET_AND_CBRACE,    0x1B }, // }]
	{KEY_KEYPAD_1_END,           0x1D }, // NUM 1
	{KEY_KEYPAD_2_DOWN_ARROW,    0x1E }, // NUM 2
	{KEY_KEYPAD_3_PAGEDN,        0x1F }, // NUM 3
	{KEY_A,                      0x20 }, // A
	{KEY_S,                      0x21 }, // S
	{KEY_D,                      0x22 }, // D
	{KEY_F,                      0x23 }, // F
	{KEY_G,                      0x24 }, // G
	{KEY_H,                      0x25 }, // H
	{KEY_J,                      0x26 }, // J
	{KEY_K,                      0x27 }, // K
	{KEY_L,                      0x28 }, // L
	{KEY_SEMICOLON_COLON,        0x29 }, // :;
	{KEY_SINGLE_AND_DOUBLE_QUOTE,0x2A }, // "'
	{KEY_ENTER,                  0x44 }, // <Enter>
	{KEY_KEYPAD_4_LEFT_ARROW,    0x2D }, // NUM 4
	{KEY_KEYPAD_5,               0x2E }, // NUM 5
	{KEY_KEYPAD_6_RIGHT_ARROW,   0x2F }, // NUM 6
	{KEY_INTERNATIONAL2,         0x30 }, // <SHIFT> international?
	{KEY_Z,                      0x31 }, // Z
	{KEY_X,                      0x32 }, // X
	{KEY_C,                      0x33 }, // C
	{KEY_V,                      0x34 }, // V
	{KEY_B,                      0x35 }, // B
	{KEY_N,                      0x36 }, // N
	{KEY_M,                      0x37 }, // M
	{KEY_COMMA_AND_LESS,         0x38 }, // <,
	{KEY_KEYPAD_COMMA,           0x38 }, // NUM ,
	{KEY_DOT_GREATER,            0x39 }, // >.
	{KEY_SLASH_QUESTION,         0x3A }, // ?/
	{KEY_KEYPAD_7_HOME,          0x3D }, // NUM 7
	{KEY_KEYPAD_8_UP_ARROW,      0x3E }, // NUM 8
	{KEY_KEYPAD_9_PAGEUP,        0x3F }, // NUM 9
	{KEY_SPACEBAR,               0x40 }, // SPACE
	{KEY_BACKSPACE,              0x41 }, // BACKSPACE
	{KEY_TAB,                    0x42 }, // TAB
	{KEY_KEYPAD_ENTER,           0x43 }, // ENTER
	{KEY_RETURN,                 0x2B }, // RETURN
	{KEY_ESCAPE,                 0x45 }, // ESC
	{KEY_DELETE,                 0x46 }, // DEL
	{KEY_KEYPAD_MINUS,           0x4A }, // NUM -
	{KEY_UPARROW,                0x4C }, // CURSOR U
	{KEY_DOWNARROW,              0x4D }, // CURSOR D
	{KEY_RIGHTARROW,             0x4E }, // CURSOR R
	{KEY_LEFTARROW,              0x4F }, // CURSOR L
	{KEY_F1,                     0x50 }, // F1
	{KEY_F2,                     0x51 }, // F2
	{KEY_F3,                     0x52 }, // F3
	{KEY_F4,                     0x53 }, // F4
	{KEY_F5,                     0x54 }, // F5
	{KEY_F6,                     0x55 }, // F6
	{KEY_F7,                     0x56 }, // F7
	{KEY_F8,                     0x57 }, // F8
	{KEY_F9,                     0x58 }, // F9
	{KEY_F10,                    0x59 }, // F10
	{KEY_HOME,		             0x5A }, // (
	{KEY_END1,        			 0x5B }, // )
	{KEY_KEYPAD_SLASH,           0x5C }, // /
	{KEY_KEYPAD_ASTERIKS,        0x5D }, // NUM *
	{KEY_KEYPAD_PLUS,            0x5E }, // NUM +
	{KEY_F12,                    0x5F }, // HELP
	{KEY_LEFTSHIFT,              0x60 }, // LSHIFT
	{KEY_RIGHTSHIFT,             0x61 }, // RSHIFT
	{KEY_CAPS_LOCK,              0x62 }, // CAPS
	{KEY_LEFTCONTROL,            0x63 }, // LCTRL
	{KEY_LEFTALT,                0x64 }, // LALT
	{KEY_RIGHTALT,               0x65 }, // RALT
	{KEY_LEFT_GUI,               0x66 }, // LWIN
	{KEY_RIGHT_GUI,              0x67 }, // RWIN
	{KEY_RIGHTCONTROL,           0x67 }, // RWIN
	{KEY_APPLICATION,            0x5F }, // APP - HELP
	{KEY_KEYPAD_DECIMAL_SEPARATOR_DELETE, 0x3C }, // KEYPAD '.'
	{KEY_KEYPAD_NUM_LOCK_AND_CLEAR, 0x68 }, // NUMLOCK & CLEAR
	{KEY_PRINTSCREEN,            0x0D }, // '\\' (next to Erase)
	{KEY_SCROLL_LOCK,            0x1C }, // SPARE
	{KEY_PAUSE,                  0x2C }, // SPARE
	//{KEY_HOME,                   0x3B }, // SPARE
	{KEY_PAGEUP,                 0x3F }, // PGUP
	{KEY_PAGEDOWN,               0x1F }, // PGDOWN
	//{KEY_END1,                   0x49 }, // SPARE
	{KEY_INSERT,                 0x5F }, // HELP
	{KEY_NONE,                   0x5B }, // SPARE
	{KEY_NONE,                   0x6A }, // SPARE
	{KEY_NONE,                   0x6B }, // SPARE
	{KEY_NONE,                   0x6C }, // SPARE
	{KEY_NONE,                   0x6D }, // SPARE
	{KEY_NONE,                   0x6E }, // SPARE
	{KEY_NONE,                   0x6F }, // SPARE
};


/**
	The keyboard transmits 8-bit data words serially to the main unit. Before
	the transmission starts, both KCLK and KDAT are high.  The keyboard starts
	the transmission by putting out the first data bit (on KDAT), followed by
	a pulse on KCLK (low then high); then it puts out the second data bit and
	pulses KCLK until all eight data bits have been sent.  After the end of
	the last KCLK pulse, the keyboard pulls KDAT high again.

	When the computer has received the eighth bit, it must pulse KDAT low for
	at least 1 (one) microsecond, as a handshake signal to the keyboard.  The
	handshake detection on the keyboard end will typically use a hardware
	latch.  The keyboard must be able to detect pulses greater than or equal
	to 1 microsecond.  Software MUST pulse the line low for 85 microseconds to
	ensure compatibility with all keyboard models.

	All codes transmitted to the computer are rotated one bit before
	transmission.  The transmitted order is therefore 6-5-4-3-2-1-0-7. The
	reason for this is to transmit the  up/down flag  last, in order to cause
	a key-up code to be transmitted in case the keyboard is forced to restore
	 lost sync  (explained in more detail below).

	The KDAT line is active low; that is, a high level (+5V) is interpreted as
	0, and a low level (0V) is interpreted as 1.

				 _____   ___   ___   ___   ___   ___   ___   ___   _________
			KCLK      \_/   \_/   \_/   \_/   \_/   \_/   \_/   \_/
				 ___________________________________________________________
			KDAT    \_____X_____X_____X_____X_____X_____X_____X_____/
					  (6)   (5)   (4)   (3)   (2)   (1)   (0)   (7)

					 First                                     Last
					 sent                                      sent

	The keyboard processor sets the KDAT line about 20 microseconds before it
	pulls KCLK low.  KCLK stays low for about 20 microseconds, then goes high
	again.  The processor waits another 20 microseconds before changing KDAT.

	Therefore, the bit rate during transmission is about 60 microseconds per
	bit, or 17 kbits/sec.
	*
	*
	*
	*
	*
	* Is this LookUpTable needed for translating from ascii to internal keycode?


			Row 5   Row 4   Row 3   Row 2   Row 1   Row 0
	Column    (Bit 7) (Bit 6) (Bit 5) (Bit 4) (Bit 3) (Bit 2)
		   +-------+-------+-------+-------+-------+-------+
	 15    |(spare)|(spare)|(spare)|(spare)|(spare)|(spare)|
	(PD.7) |       |       |       |       |       |       |
		   | (0E)  | (1C)  | (2C)  | (47)  | (48)  | (49)  |
		   +-------+-------+-------+-------+-------+-------+
	 14    |   *   |<SHIFT>| CAPS  |  TAB  |   ~   |  ESC  |
	(PD.6) |note 1 |note 2 | LOCK  |       |   `   |       |
		   | (5D)  | (30)  | (62)  | (42)  | (00)  | (45)  |
		   +-------+-------+-------+-------+-------+-------+
	 13    |   +   |   Z   |   A   |   Q   |   !   |   (   |
	(PD.5) |note 1 |       |       |       |   1   |note 1 |
		   | (5E)  | (31)  | (20)  | (10)  | (01)  | (5A)  |
		   +-------+-------+-------+-------+-------+-------+
	 12    |  9    |   X   |   S   |   W   |   @   |  F1   |
	(PD.4) |note 3 |       |       |       |   2   |       |
		   | (3F)  | (32)  | (21)  | (11)  | (02)  | (50)  |
		   +-------+-------+-------+-------+-------+-------+
	 11    |  6    |   C   |   D   |   E   |   #   |  F2   |
	(PD.3) |note 3 |       |       |       |   3   |       |
		   | (2F)  | (33)  | (22)  | (12)  | (03)  | (51)  |
		   +-------+-------+-------+-------+-------+-------+
	 10    |  3    |   V   |   F   |   R   |   $   |  F3   |
	(PD.2) |note 3 |       |       |       |   4   |       |
		   | (1F)  | (34)  | (23)  | (13)  | (04)  | (52)  |
		   +-------+-------+-------+-------+-------+-------+
	  9    |  .    |   B   |   G   |   T   |   %   |  F4   |
	(PD.1) |note 3 |       |       |       |   5   |       |
		   | (3C)  | (35)  | (24)  | (14)  | (05)  | (53)  |
		   +-------+-------+-------+-------+-------+-------+
	  8    |  8    |   N   |   H   |   Y   |   ^   |  F5   |
	(PD.0) |note 3 |       |       |       |   6   |       |
		   | (3E)  | (36)  | (25)  | (15)  | (06)  | (54)  |
		   +-------+-------+-------+-------+-------+-------+
	  7    |  5    |   M   |   J   |   U   |   &   |   )   |
	(PC.7) |note 3 |       |       |       |   7   |note 1 |
		   | (2E)  | (37)  | (26)  | (16)  | (07)  | (5B)  |
		   +-------+-------+-------+-------+-------+-------+
	  6    |  2    |   <   |   K   |   I   |   *   |  F6   |
	(PC.6) |note 3 |   ,   |       |       |   8   |       |
		   | (1E)  | (38)  | (27)  | (17)  | (08)  | (55)  |
		   +-------+-------+-------+-------+-------+-------+
	  5    | ENTER |   >   |   L   |   O   |   (   |   /   |
	(PC.5) |note 3 |   .   |       |       |   9   |note 1 |
		   | (43)  | (39)  | (28)  | (18)  | (09)  | (5C)  |
		   +-------+-------+-------+-------+-------+-------+
	  4    |  7    |   ?   |   :   |   P   |   )   |  F7   |
	(PC.4) |note 3 |   /   |   ;   |       |   0   |       |
		   | (3D)  | (3A)  | (29)  | (19)  | (0A)  | (56)  |
		   +-------+-------+-------+-------+-------+-------+
	  3    |  4    |(spare)|   "   |   {   |   _   |  F8   |
	(PC.3) |note 3 |       |   '   |   [   |   -   |       |
		   | (2D)  | (3B)  | (2A)  | (1A)  | (0B)  | (57)  |
		   +-------+-------+-------+-------+-------+-------+
	  2    |  1    | SPACE | <RET> |   }   |   +   |  F9   |
	(PC.2) |note 3 |  BAR  |note 2 |   ]   |   =   |       |
		   | (1D)  | (40)  | (2B)  | (1B)  | (0C)  | (58)  |
		   +-------+-------+-------+-------+-------+-------+
	  1    |  0    | BACK  |  DEL  |RETURN |   |   |  F10  |
	(PC.1) |note 3 | SPACE |       |       |   \   |       |
		   | (0F)  | (41)  | (46)  | (44)  | (0D)  | (59)  |
		   +-------+-------+-------+-------+-------+-------+
	  0    |  -    | CURS  | CURS  | CURS  | CURS  | HELP  |
	(PC.0) |note 3 | DOWN  | RIGHT | LEFT  |  UP   |       |
		   | (4A)  | (4D)  | (4E)  | (4F)  | (4C)  | (5F)  |
		   +-------+-------+-------+-------+-------+-------+

	note 1: A500, A2000 and A3000 keyboards only (numeric pad )
	note 2: International keyboards only (these keys are cutouts of the
		   larger key on the US ASCII version.)  The key that generates
		   $30 is cut out of the left Shift key.  Key $2B is cut out of
		   return.  These keys are labeled with country-specific markings.
	note 3: Numeric pad.


	The following table shows which keys are independently readable.  These
	keys never generate ghosts or phantoms.


		(Bit 6) (Bit 5) (Bit 4) (Bit 3) (Bit 2) (Bit 1) (Bit 0)
	   +-------+-------+-------+-------+-------+-------+-------+
	   | LEFT  | LEFT  | LEFT  | CTRL  | RIGHT | RIGHT | RIGHT |
	   | AMIGA | ALT   | SHIFT |       | AMIGA |  ALT  | SHIFT |
	   | (66)  | (64)  | (60)  | (63)  | (67)  | (65)  | (61)  |
	   +-------+-------+-------+-------+-------+-------+-------+

	About Hard Reset.
	   -----------------
	   Hard Reset happens after  Reset Warning . Valid for all keyboards
	   except the Amiga 500.

	The keyboard Hard Resets the Amiga by pulling KCLK low and starting a 500
	millisecond timer.   When one or more of the keys is released and 500
	milliseconds have passed, the keyboard will release KCLK. 500 milliseconds
	is the minimum time KCLK must be held low.  The maximum KCLK time depends
	on how long the user holds the three  reset keys  down.  Circuitry on the
	Amiga motherboard detects the 500 millisecond KCLK pulse.

	After releasing KCLK, the keyboard jumps to its start-up code (internal
	RESET).  This will initialize the keyboard in the same way as cold
	 power-on .

 **/

typedef enum {
	DAT_OUTPUT = 0,
	DAT_INPUT,
} kbd_dir;

static unsigned char prev_keycode = 0xff;
static unsigned char capslk = 0;
static unsigned char numlk = 0;
static unsigned char scrolllk = 0;

static led_status_t amikb_send(uint8_t code, int press);

static uint8_t scancode_to_amiga(uint8_t lkey)
{
	uint8_t i = 0, keyvalue = lkey;
	for (i = 0; i < KEYCODE_TAB_SIZE; i++)
	{

		if (lkey == scancodeamiga[i][0])
		{
			keyvalue = scancodeamiga[i][1];
			break;
		}
	}
	return keyvalue;
}

// **************************




void amikb_startup(void)
{
	uint8_t AMIGA_INITPOWER = 0xFD; //11111101
	uint8_t AMIGA_TERMPOWER = 0xFE; //11111110
	Delay_Us(200);
	// De-assert nRESET for Amiga...
   	//amikb_reset();
   	Delay_Us(200);           // wait for sync
	amikb_send((uint8_t) AMIGA_INITPOWER, 0); // send "initiate power-up"
	Delay_Us(200);
	amikb_send((uint8_t) AMIGA_TERMPOWER, 0); // send "terminate power-up"
}

static int keyboard_is_present = 0;
void amikb_ready(int isready)
{
	keyboard_is_present = isready;
}


static led_status_t amikb_send(uint8_t keycode, int press)
{
	led_status_t rval = NO_LED;

	if (keycode == 0x62 || keycode == 0x68 || keycode == 0x1c) // Caps Lock, Num Lock or Scroll Lock Pressed or Released
	{
		// caps lock doesn't get a key release event when the key is released
		// but rather when the caps lock is toggled off again
		// But what about num lock?

		switch (keycode)
		{
			case 0x62: // CAPS LOCK LED
				if (!capslk)
				{
					if (press)
					{
						rval = LED_CAPS_LOCK_ON;
						prev_keycode = 0;
						break;
					}
					else
					{
						// Toggle for next time press
						capslk = 1;
						prev_keycode = 0;
						return NO_LED;
					}
				}
				else
				{
					if (press)
					{
						prev_keycode = 0;
						return NO_LED;
					}
					else
					{
						capslk = 0;
						rval = LED_CAPS_LOCK_OFF;
						prev_keycode = 0;
						break;
					}
				}
				break;
			case 0x68: // NUM LOCK LED
				if (!numlk)
				{
					if (press)
					{
						rval = LED_NUM_LOCK_ON;
						prev_keycode = 0;
						break;
					}
					else
					{
						// Toggle for next time press
						numlk = 1;
						prev_keycode = 0;
						return NO_LED;
					}
				}
				else
				{
					if (press)
					{
						prev_keycode = 0;
						return NO_LED;
					}
					else
					{
						numlk = 0;
						rval = LED_NUM_LOCK_OFF;
						prev_keycode = 0;
						break;
					}
				}
				break;
			case 0x1c: // SCROLL LOCK LED
				if (!scrolllk)
				{
					if (press)
					{
						rval = LED_SCROLL_LOCK_ON;
						prev_keycode = 0;
						break;
					}
					else
					{
						// Toggle for next time press
						scrolllk = 1;
						prev_keycode = 0;
						return NO_LED;
					}
				}
				else
				{
					if (press)
					{
						prev_keycode = 0;
						return NO_LED;
					}
					else
					{
						scrolllk = 0;
						rval = LED_SCROLL_LOCK_OFF;
						prev_keycode = 0;
						break;
					}
				}
				break;
			default:
				{
						//numlk, capslk, scrolllk, press);
					return NO_LED;
				}
				break;
		}
	}

	// keycode bit transfer order: 6 5 4 3 2 1 0 7 (7 is pressed flag)
	keycode = (keycode << 1) | (~press & 1);
	if (keycode == prev_keycode)
	{
		//DBG_N("NO SENDING THE SAME KEYCODE TWO TIMES IN A ROW\r\n");
		return NO_LED;
	}

	prev_keycode = keycode;

	//send to Amiga

	GPIO_WriteBit(KBD_DATA_GPIO_Port, KBD_DATA_Pin, Bit_SET); // Normally KBD_DATA pin is HIGH

	// pulse the data line and wait for about 100us
	GPIO_WriteBit(KBD_DATA_GPIO_Port, KBD_DATA_Pin, Bit_RESET); // KBD_DATA pin is LOW
	Delay_Us(100);
	GPIO_WriteBit(KBD_DATA_GPIO_Port, KBD_DATA_Pin, Bit_SET); // KBD_DATA pin is HIGH
	Delay_Us(100);

	for (i = 0; i < 8; i++)
	{
		// data line is inverted
		if (keycode & 0x80)
		{
			// a logic 1 is low in hardware
			GPIO_WriteBit(KBD_DATA_GPIO_Port, KBD_DATA_Pin, Bit_RESET);
		}
		else
		{
			// a logic 0 is high in hardware
			GPIO_WriteBit(KBD_DATA_GPIO_Port, KBD_DATA_Pin, Bit_SET);
		}
		keycode <<= 1;
		Delay_Us(10);
		/* pulse the clock */
		GPIO_WriteBit(KBD_CLOCK_GPIO_Port, KBD_CLOCK_Pin, Bit_RESET); // Clear KBD_CLOCK pin
		Delay_Us(10);
		GPIO_WriteBit(KBD_CLOCK_GPIO_Port, KBD_CLOCK_Pin, Bit_SET); // Set KBD_CLOCK pin
		Delay_Us(10);
	}
	Delay_Us(100);
	GPIO_WriteBit(KBD_DATA_GPIO_Port, KBD_DATA_Pin, Bit_SET); // Set KBD_DATA pin
	Delay_Us(100);


	return rval;
}

// **************************
void amikb_reset(void)
{
    GPIO_WriteBit(KB_RESET_GPIO_Port, KB_RESET_GPIO_Pin, Bit_RESET);
    GPIO_WriteBit(KBD_CLOCK_GPIO_Port, KBD_CLOCK_Pin, Bit_RESET);
    Delay_Ms(500);
    GPIO_WriteBit(KBD_CLOCK_GPIO_Port, KBD_CLOCK_Pin, Bit_SET);
    GPIO_WriteBit(KB_RESET_GPIO_Port, KB_RESET_GPIO_Pin, Bit_SET);

    prev_keycode = 0xff;
	capslk = 0;
	numlk = 0;
	scrolllk = 0;
}

void amikb_irq(void)
{


}

// ****************************






void amikb_process(HID_Keyboard_Data *kbdata)
{


	if (kbdata == NULL) return;

	int i;
	int j;
	led_status_t rval = NO_LED; /* 0 means no USB interaction such as leds, ... */

	if(kbdata->keys[0] == KEY_F12)
	{
	    GPIO_WriteBit(LED_GPIO_Port, LED_Pin, RESET);
	    GPIO_WriteBit(GPIOA,GPIO_Pin_13,Bit_RESET);
	    GPIO_WriteBit(GPIOA,GPIO_Pin_14,Bit_RESET);
	    Delay_Ms(500);
	    GPIO_WriteBit(GPIOA,GPIO_Pin_13,Bit_SET);
	    GPIO_WriteBit(GPIOA,GPIO_Pin_14,Bit_SET);
	    GPIO_WriteBit(LED_GPIO_Port, LED_Pin, SET);
	    return;
	}

	//check for reset
	if(kbdata->lctrl == 1 && kbdata->lalt ==1 &&kbdata->keys[0]==KEY_DELETE  )
	{
		amikb_reset();
		return;
	}

    if(kbdata->lgui == 1 && kbdata->rgui ==1 &&kbdata->rctrl )
    {
        amikb_reset();
        return;
    }

    if(kbdata->lctrl == 1 && kbdata->lgui ==1 &&kbdata->keys[0]==101  )
    {
        amikb_reset();
        return;
    }

	// ----------------------------------------------- LEFT

	// LEFT SHIFT
	if (prevkeycode.lshift != kbdata->lshift)
	{
		prevkeycode.lshift = kbdata->lshift;
		rval |= amikb_send(scancode_to_amiga(KEY_LEFTSHIFT), kbdata->lshift);
	}

	// LEFT ALT
	if (prevkeycode.lalt != kbdata->lalt)
	{
		prevkeycode.lalt = kbdata->lalt;
		rval |= amikb_send(scancode_to_amiga(KEY_LEFTALT), kbdata->lalt);
	}

	// LEFT CTRL
	if (prevkeycode.lctrl != kbdata->lctrl)
	{
		prevkeycode.lctrl = kbdata->lctrl;
		rval |= amikb_send(scancode_to_amiga(KEY_LEFTCONTROL), kbdata->lctrl);
	}

	// LEFT GUI
	if (prevkeycode.lgui != kbdata->lgui)
	{
		prevkeycode.lgui = kbdata->lgui;
		rval |= amikb_send(scancode_to_amiga(KEY_LEFT_GUI),  kbdata->lgui);
	}

	// ----------------------------------------------- RIGHT
	// RIGHT SHIFT
	if (prevkeycode.rshift != kbdata->rshift)
	{
		prevkeycode.rshift = kbdata->rshift;
		rval |= amikb_send(scancode_to_amiga(KEY_RIGHTSHIFT), kbdata->rshift);
	}

	// RIGHT ALT
	if (prevkeycode.ralt != kbdata->ralt)
	{
		prevkeycode.ralt = kbdata->ralt;
		rval |= amikb_send(scancode_to_amiga(KEY_RIGHTALT), kbdata->ralt);
	}

	// RIGHT CTRL
	if (prevkeycode.rctrl != kbdata->rctrl)
	{
		prevkeycode.rctrl = kbdata->rctrl;
		rval |= amikb_send(scancode_to_amiga(KEY_RIGHTCONTROL), kbdata->rctrl);
	}

	// RIGHT GUI
	if (prevkeycode.rgui != kbdata->rgui)
	{
		prevkeycode.rgui = kbdata->rgui;
		rval |= amikb_send(scancode_to_amiga(KEY_RIGHT_GUI), kbdata->rgui);
	}



	// Send all pressed key
	uint8_t keysToPress[KEY_PRESSED_MAX] = {0};
	uint8_t keysToRelease[KEY_PRESSED_MAX] = {0} ;

	int idxPress= 0;
	int idxRelease = 0;

	//Find keys to release
	for (i = 0; i < KEY_PRESSED_MAX; i++)
	{
		int found = 0;
		for (j = 0; j < KEY_PRESSED_MAX; j++)
		{
			if (prevkeycode.keys[i] == kbdata->keys[j] )
			{
				found = 1;
			}
		}

		if(found == 0)
		{
		keysToRelease[idxRelease] = prevkeycode.keys[i];
		idxRelease++;
		}

	}

	//Find keys to press (not already pressed)
	for (i = 0; i < KEY_PRESSED_MAX; i++)
	{
		int found = 0;
		for (j = 0; j < KEY_PRESSED_MAX; j++)
		{
			if ( kbdata->keys[i] == prevkeycode.keys[j])
			{
				found = 1;
			}
		}

		 if (found == 0)
		 {
			keysToPress[idxPress] = kbdata->keys[i];
			idxPress++;
		 }
	}

	//Send release keys
	for (i = 0; i < KEY_PRESSED_MAX; i++)
	{
		if (keysToRelease[i] != 0x00) // Previous key was released
		{
	        rval |= amikb_send(scancode_to_amiga(keysToRelease[i]), 0 /* Released */);
	    }
	}

	//send press keys
	for (i = 0; i < KEY_PRESSED_MAX; i++)
	{
		if (keysToPress[i] != 0x00)
		{
			rval |= amikb_send(scancode_to_amiga(keysToPress[i]), 1 /* Pressed */);
		}
	}

	//copy keys for next handling
	for (i = 0; i < KEY_PRESSED_MAX; i++)
		{
		   prevkeycode.keys[i] = kbdata->keys[i];

		}

}
