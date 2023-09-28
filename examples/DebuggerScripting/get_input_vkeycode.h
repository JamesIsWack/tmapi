/****************************************************************************/
//
// Copyright SN Systems Ltd 2011
// This is "get_input" function with "Virtual Key Code" support -> "get_input_vkeycode"
// 
// v0.1 - Kentarou Susuzuki (16.May.2007)  initial version
//

#ifndef GET_INPUT_VKEYCODE_H
#define GET_INPUT_VKEYCODE_H

////////////////////////////////////////////////////
/* Keyboard Language Type */
/* 1 = English   2 = Japanese   3 = other */
#define KEYBOARD_LANG_TYPE    2


////////////////////////////////////////////////////
/* Virtual Key Code */
#define VK_BACK        0x08
#define VK_TAB         0x09
#define VK_RETURN      0x0D
#define VK_SHIFT       0x10
#define VK_SPACE       0x20
#define VK_0           0x30
#define VK_1           0x31
#define VK_2           0x32
#define VK_3           0x33
#define VK_4           0x34
#define VK_5           0x35
#define VK_6           0x36
#define VK_7           0x37
#define VK_8           0x38
#define VK_9           0x39
#define VK_A           0x41
#define VK_B           0x42
#define VK_C           0x43
#define VK_D           0x44
#define VK_E           0x45
#define VK_F           0x46
#define VK_G           0x47
#define VK_H           0x48
#define VK_I           0x49
#define VK_J           0x4A
#define VK_K           0x4B
#define VK_L           0x4C
#define VK_M           0x4D
#define VK_N           0x4E
#define VK_O           0x4F
#define VK_P           0x50
#define VK_Q           0x51
#define VK_R           0x52
#define VK_S           0x53
#define VK_T           0x54
#define VK_U           0x55
#define VK_V           0x56
#define VK_W           0x57
#define VK_X           0x58
#define VK_Y           0x59
#define VK_Z           0x5A
#define VK_NUMPAD0     0x60
#define VK_NUMPAD1     0x61
#define VK_NUMPAD2     0x62
#define VK_NUMPAD3     0x63
#define VK_NUMPAD4     0x64
#define VK_NUMPAD5     0x65
#define VK_NUMPAD6     0x66
#define VK_NUMPAD7     0x67
#define VK_NUMPAD8     0x68
#define VK_NUMPAD9     0x69
#define VK_MULTIPLY    0x6A
#define VK_ADD         0x6B
#define VK_SEPARATOR   0x6C
#define VK_SUBTRACT    0x6D
#define VK_DECIMAL     0x6E
#define VK_DIVIDE      0x6F
#define VK_LSHIFT      0xA0
#define VK_RSHIFT      0xA1
#define VK_OEM_1       0xBA
#define VK_OEM_PLUS    0xBB
#define VK_OEM_COMMA   0xBC
#define VK_OEM_MINUS   0xBD
#define VK_OEM_PERIOD  0xBE
#define VK_OEM_2       0xBF
#define VK_OEM_3       0xC0
#define VK_OEM_4       0xDB
#define VK_OEM_5       0xDC
#define VK_OEM_6       0xDD
#define VK_OEM_7       0xDE
#define VK_OEM_102     0xE2


////////////////////////////////////////////////////
/* Keyboard conversion table */
#if KEYBOARD_LANG_TYPE == 1
/* Key Code Table of English Keyboard */
char KeyCodeTable[][3] = {\
  /* vKeycode */  /* char code */     /* char code with shiftkey */
{ VK_BACK,        0x00 /* NULL */,    0x00 /* NULL */},\
/* ... */
/* ... */
/* ... */
/* ... */
/* terminator */
{ 0x00, 0x00, 0x00}\
};

#elif KEYBOARD_LANG_TYPE == 2
/* Key Code Table of Japanese Keyboard (106/109) */
char KeyCodeTable[][3] = {\
  /* vKeycode */  /* char code */     /* char code with shiftkey */
{ VK_BACK,        0x00 /* NULL */,    0x00 /* NULL */},\
{ VK_TAB,         0x09 /* TAB */,     0x09 /* TAB */},\
{ VK_RETURN,      0x00 /* NULL */,    0x00 /* NULL */},\
{ VK_SHIFT,       0x00 /* NULL */,    0x00 /* NULL */},\
{ VK_SPACE,       ' ',                ' '},\
{ VK_0,           '0',                0x00 /* NULL */},\
{ VK_1,           '1',                '!' },\
{ VK_2,           '2',                '"' },\
{ VK_3,           '3',                '#' },\
{ VK_4,           '4',                '$' },\
{ VK_5,           '5',                '%' },\
{ VK_6,           '6',                '&' },\
{ VK_7,           '7',                '\'' },\
{ VK_8,           '8',                '(' },\
{ VK_9,           '9',                ')' },\
{ VK_A,           'a',                'A' },\
{ VK_B,           'b',                'B' },\
{ VK_C,           'c',                'C' },\
{ VK_D,           'd',                'D' },\
{ VK_E,           'e',                'E' },\
{ VK_F,           'f',                'F' },\
{ VK_G,           'g',                'G' },\
{ VK_H,           'h',                'H' },\
{ VK_I,           'i',                'I' },\
{ VK_J,           'j',                'J' },\
{ VK_K,           'k',                'K' },\
{ VK_L,           'l',                'L' },\
{ VK_M,           'm',                'M' },\
{ VK_N,           'n',                'N' },\
{ VK_O,           'o',                'O' },\
{ VK_P,           'p',                'P' },\
{ VK_Q,           'q',                'Q' },\
{ VK_R,           'r',                'R' },\
{ VK_S,           's',                'S' },\
{ VK_T,           't',                'T' },\
{ VK_U,           'u',                'U' },\
{ VK_V,           'v',                'V' },\
{ VK_W,           'w',                'W' },\
{ VK_X,           'x',                'X' },\
{ VK_Y,           'y',                'Y' },\
{ VK_Z,           'z',                'Z' },\
{ VK_NUMPAD0,     '0',                '0' },\
{ VK_NUMPAD1,     '1',                '1' },\
{ VK_NUMPAD2,     '2',                '2' },\
{ VK_NUMPAD3,     '3',                '3' },\
{ VK_NUMPAD4,     '4',                '4' },\
{ VK_NUMPAD5,     '5',                '5' },\
{ VK_NUMPAD6,     '6',                '6' },\
{ VK_NUMPAD7,     '7',                '7' },\
{ VK_NUMPAD8,     '8',                '8' },\
{ VK_NUMPAD9,     '9',                '9' },\
{ VK_MULTIPLY,    '*',                '*' },\
{ VK_ADD,         '+',                '+' },\
{ VK_SEPARATOR,   0x00 /* NULL */,    0x00 /* NULL */},\
{ VK_SUBTRACT,    '-',                '-' },\
{ VK_DECIMAL,     '.',                '.' },\
{ VK_DIVIDE,      '/',                '/' },\
{ VK_LSHIFT,      0x00 /* NULL */,    0x00 /* NULL */},\
{ VK_RSHIFT,      0x00 /* NULL */,    0x00 /* NULL */},\
{ VK_OEM_1,       ':',                '*' },\
{ VK_OEM_PLUS,    ';',                '+' },\
{ VK_OEM_COMMA,   ',',                '<' },\
{ VK_OEM_MINUS,   '-',                '=' },\
{ VK_OEM_PERIOD,  '.',                '>' },\
{ VK_OEM_2,       '/',                '?' },\
{ VK_OEM_3,       '@',                '`' },\
{ VK_OEM_4,       '[',                '{' },\
{ VK_OEM_5,       '\\',               '|' },\
{ VK_OEM_6,       ']',                '}' },\
{ VK_OEM_7,       '^',                '~' },\
{ VK_OEM_102,     '\\',               '_' },\
/* terminator */
{ 0x00, 0x00, 0x00}\
};

#else /* KEYBOARD_LANG_TYPE */
/* Key Code Table of Other Type Keyboard */
char KeyCodeTable[][3] = {\
  /* vKeycode */  /* char code */     /* char code with shiftkey */
{ VK_BACK,        0x00 /* NULL */,    0x00 /* NULL */},\
/* ... */
/* ... */
/* ... */
/* ... */
/* terminator */
{ 0x00, 0x00, 0x00}\
};

#endif /* KEYBOARD_LANG_TYPE */


////////////////////////////////////////////////////
/* get_input function for "Virtual Key Code"      */
////////////////////////////////////////////////////
sn_uint32 get_input_vkeycode(char *pBuffer) 
{
	BOOL bShiftKey = FALSE;
	char vKeyCode = 0;
	sn_notify Notify;
	sn_uint32 char_count = 0;
	
	printf(">>");
	
	RequestNotification(NT_KEYDOWN);
	RequestNotification(NT_KEYUP);

	GetNotification(NT_ANY, &Notify);
	while(TRUE)
	{
		vKeyCode = (sn_uint32)(Notify.pParam1);
		/*printf("[%x]", (vKeyCode&0x00ff));*/
		if (Notify.eNotifyType == NT_KEYUP)
		{
			if( vKeyCode == VK_SHIFT )
			{
				bShiftKey = FALSE;
			}
			else if( vKeyCode == VK_BACK && char_count != 0 )
			{
				char_count--;
				pBuffer[char_count] = 0x00;
				printf("\n>>%s", pBuffer);
			}
		}
		else if (Notify.eNotifyType == NT_KEYDOWN)
		{
			if( vKeyCode == VK_RETURN || vKeyCode == VK_SEPARATOR )
			{
				break;
			}
			else if( vKeyCode == VK_SHIFT )
			{
				bShiftKey = TRUE;
			} else {
				int nIndex;
				char charCode;
				for( nIndex = 0 ; KeyCodeTable[nIndex][0] != 0x00 ; nIndex++ )
				{
					if( vKeyCode == KeyCodeTable[nIndex][0] )
					{
						if( bShiftKey )
						{
							charCode = KeyCodeTable[nIndex][2];
						} else {
							charCode = KeyCodeTable[nIndex][1];
						}
						if( charCode != 0x00 )
						{
							pBuffer[char_count] = charCode;
							printf("%c", pBuffer[char_count]);
							char_count++;
						}
					}
				}
			}
		}
		GetNotification(NT_ANY, &Notify);
	}
	pBuffer[char_count] = '\0';
	return char_count+1;
}

#endif /* GET_INPUT_VKEYCODE_H */
