//bluefruit Functions

#include <SoftwareSerial.h>

SoftwareSerial BT = SoftwareSerial(A4, A5);//first is rx, second is tx
//connect tx to rx on the bluefruit

#define BACK 178
#define KEY_RETURN byte(0x0A)
#define KEY_LEFT_ARROW byte(0x0B)
#define KEY_RIGHT_ARROW byte(0x07)
//-----------------------------
///*
void outputUp()
{
  
  BT.begin(9600);
}

void pressKeys(int presses, char key)
{
  for(int i=0;i<presses;i++)
  {
    BT.write(key);
  }
}
void pressKey(char key)
{
  BT.write(key);
}
//HID only / serial creative (new lining reproduction)
void backSpace()
{
  BT.write(BACK);
}
void backSpaces(int increment)
{
  for(int i=0;i<increment;i++)
  {
    BT.write(BACK);
  }
}
void enter()
{
  BT.write(KEY_RETURN);
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
    BT.write(going);
  }
}
//*/
