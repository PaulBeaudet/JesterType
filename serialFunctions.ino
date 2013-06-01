//serialFunctions
//This file is meant to interchanged with KeboardFunction.ino
//For testing on arduinos where Keyboard.methods do not compile
//like the uno and the mega
//uncomment this file and comment out KeboardFuntion.ino for
//compatibility
/*
void outputUp()
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
  if(wordSize[CURRENT])
  {
    letterBuffer[wordSize[CURRENT]-1]=0;
  }
  pressKeys(lineCount-wordSize[CURRENT]-1,'#');
  countChange(-1);
  Serial.write(letterBuffer);
}
void backSpaces(int increment)
{//
  if(wordSize[CURRENT])
  {
    letterBuffer[wordSize[CURRENT]-1]=0;
  }
  pressKeys(lineCount-wordSize[CURRENT],'#');
  Serial.print(letterBuffer);
}
void enter()
{
  Serial.println();
}
void movement(int increment, boolean bearing)
{
  if(bearing)//if true aka 1
  {
    Serial.println();
    pressKeys(lineCount-wordSize[CURRENT],'#');
    Serial.print(letterBuffer);
  }
  else
  {
    Serial.println();
    pressKeys(lineCount+1,'#');
  }
}
*/



