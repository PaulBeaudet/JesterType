//serialFunctions
//This file is meant to interchanged with KeboardFunction.ino
//For testing on arduinos where Keyboard.methods do not compile
//like the uno and the mega
//uncomment this file and comment out KeboardFuntion.ino for
//compatibility
/*
void interfaceUp()
{
  Serial.begin(9600);
}
//Cross serial compatible
void pressKeys(int presses, char key)
{
  for(int i=0;i<presses;i++)
  {
    Serial.write(key);
  }
}
void pressKey(char key)
{
  Serial.write(key);
}
//HID only / serial creative (new lining reproduction)
void backSpace()
{
  Serial.println();
  if(count[CWORD])
  {
    letterBuffer[count[CWORD]-1]=0;
  }
  pressKeys(count[LINEC]-count[CWORD]-1,'#');
  countChange(-1);
  Serial.write(letterBuffer);
}
void backSpaces(int increment)
{//
  if(count[CWORD])
  {
    letterBuffer[count[CWORD]-1]=0;
  }
  pressKeys(count[LINEC]-count[CWORD],'#');
  Serial.print(letterBuffer);
}
void enter()
{
  Serial.println();
  count[LINEC]=0;
}
void movement(int increment, boolean bearing)
{
  if(bearing)//if true aka 1
  {
    Serial.println();
    pressKeys(count[LINEC]-count[CWORD],'#');
    Serial.print(letterBuffer);
  }
  else
  {
    Serial.println();
    pressKeys(count[LINEC]+1,'#');
  }
}
*/



