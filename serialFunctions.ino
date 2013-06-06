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
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
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
  for(byte i=0;i<lineCount;i++)
  {
    Serial.write(printedLetters[editLine][i]);
  }
}
void backSpaces(int increment)
{
  Serial.println();
  for(byte i=0;i<lineCount-increment;i++)
  {
    Serial.write(printedLetters[editLine][i]);
  }
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
    for(byte i=0;i<lineCount;i++)
    {
      Serial.write(printedLetters[editLine][i]);
    }
  }
  else
  {
    for(byte i=lineCount;i<increment;i++)
    {
      Serial.write(printedLetters[editLine][i]);
    }
  }
}
void displayBuffer()
{
  Serial.println();
  for(byte i=0;i<LINESIZE;i++)
  {
    if(printedLetters[editLine][i])
    {
      Serial.write(printedLetters[editLine][i]);
    }
  }
}
*/

