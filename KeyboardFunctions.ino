//keyboardFunctions
//would need to be a serial equivilent of this file to debug
//on uno and other non-atmega32u4
//!!some of the controlMacro functions need to be moved here
//to encompass all keyboard functions over the scope of JesterType
//---------------------------letter printing
void printLetter(byte letterNum)
{
  if(count[CSENT]==0)
  {
    Keyboard.write(letterNum-32);
    letterBuffer[0]=letterNum-32;
  }
  else
  {
    Keyboard.write(letterNum);
    if(count[CWORD]<14)
    {
      letterBuffer[count[CWORD]]=letterNum;
    }
    count[YESC]=0;
    count[NOC]=0;
    //count[METAC]=0;
  }
  countChange(1);
  if(rJustify || cJustify)
  {
    count[LSENT]++;
  }
}
void countChange(byte increment)
{
  for(byte i=0;i<3;i++)
  {
    count[i]=count[i]+increment;
    //increments 'current' counts
  }
}
//this is possibly kruft at this point
//spacer
void sfill(byte spacing, char Char)
{
  for(int i=0;i<spacing;i++)
  {
    Keyboard.print(Char);
    if(Char==' ')
    {
      count[LINEC]++;
    }
  }
}
//Multi key press
void sKey(int presses, int key)
{
  for(int i=0;i<presses;i++)
  {
    Keyboard.write(key);
  }
}

// Key binding function
void combo(char key1, char key2)
{
  Keyboard.press(key1);
  Keyboard.press(key2);
  delay(REACT);
  Keyboard.releaseAll();
}

void combo(char key1, char key2, char key3)
{
  Keyboard.press(key1);
  Keyboard.press(key2);
  Keyboard.press(key3);
  delay(REACT);
  Keyboard.releaseAll();
}

