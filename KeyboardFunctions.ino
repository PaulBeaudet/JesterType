/*keyboardFunctions
//would need to be a serial equivilent of this file to debug
//on uno and other non-atmega32u4
//!!some of the controlMacro functions need to be moved here
//to encompass all keyboard functions over the scope of JesterType
//################################################################
*/
//modifiers are assign as variables to pass to functions
//prog_char supeRight= KEY_RIGHT_GUI;
//prog_char lctrl = KEY_LEFT_CTRL;
//prog_char rctrl = KEY_RIGHT_CTRL;
//prog_char lshift = KEY_LEFT_SHIFT;
//prog_char rshift = KEY_RIGHT_SHIFT;
//prog_char lalt = KEY_LEFT_ALT;
//prog_char ralt = KEY_RIGHT_ALT;
//prog_char up = KEY_UP_ARROW;
//prog_char down = KEY_DOWN_ARROW;
//prog_char right = KEY_RIGHT_ARROW;
//prog_char left = KEY_LEFT_ARROW;
//prog_char HOME = KEY_HOME;
//prog_char END = KEY_END;
//prog_char f2 = KEY_F2;
//prog_char f4 = KEY_F4;
//prog_char f5 = KEY_F5;
//prog_char f10 = KEY_F10;
//prog_char f12 = KEY_F12;
//prog_char tab = KEY_TAB;
//prog_char RTN = KEY_RETURN;
#define BACK 178
//-----------------------------
void interfaceUp()
{
  Keyboard.begin();
}
//Cross serial compatible
void pressKeys(int presses, char key)
{
  for(int i=0;i<presses;i++)
  {
    Keyboard.write(key);
  }
}
void pressKey(char key)
{
  Keyboard.write(key);
}
//HID only / serial creative (new lining reproduction)
void backSpace()
{
  Keyboard.write(BACK);
}
void backSpaces(int increment)
{
  for(int i=0;i<increment;i++)
  {
    Keyboard.write(BACK);
  }
}
void enter()
{
  Keyboard.write(KEY_RETURN);
}
void movement(int increment, boolean bearing)
{
  char going;
  if(bearing)//if true aka 1
  {
    going=KEY_LEFT_ARROW;//LEFT=1 defined golobally
  }
  else//if false aka 0
  {
    going=KEY_RIGHT_ARROW;//RIGHT=0 defined golobally
  }
  for (int i=0;i<increment;i++)
  {//go that direction as many times as asked
    Keyboard.write(going);
  }
}
