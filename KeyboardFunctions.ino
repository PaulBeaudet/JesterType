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
}
void countChange(byte increment)
{
  for(byte i=0;i<3;i++)
  {
    count[i]=count[i]+increment;
    //increments 'current' counts
  }
}

void pressKey(int presses, int key)
{
  for(int i=0;i<presses;i++)
  {
    Keyboard.write(key);
    /*if(Char==' ')
    {
      count[LINEC]++;
    }*/
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
//--------------------noCase functions
void backSpace()
{
  pressKey(1,BACK);
  countChange(-1);
}
void backlast(byte last)
{
  pressKey(count[last],BACK);
  count[LINEC]-=count[last];
  while(count[LINEC]>LINESIZE)
  {
    count[LINEC]+=LINESIZE;
  }
  count[last]=0;
}

void punctuation(char mark)
{
  pressKey(2,BACK);
  Keyboard.write(mark);
  Keyboard.write(' ');
  count[YESC]++;
}
//############ YES CASES
void enter()
{
  Keyboard.write(RTN);
  count[LINEC]=0;
}
void space()
{
  Keyboard.write(' ');
  countChange(1);
  count[LWORD]=count[CWORD];
  count[CWORD]=0;
}
void period()
{
  pressKey(1,BACK);
  Keyboard.print(". ");
  countChange(1);
  count[LWORD]++;
  count[LSENT]=count[CSENT];
  count[CSENT]=0;
}
//###########################################formating functions
void autoLineReturn()
{//for main loop
  if(count[LINEC]>LINESIZE)
    {//one the line size limit is reached
      if(count[CWORD])
      {//given there are printed letters 
        pressKey(count[CWORD],left);//:KeyboardFunctions
        //move the cursor to the left of the current word prepping it
      }//to be pushed to the next line thus keeping word whole
      pressKey(1, RTN);//regardless press enter and add the current word count
      count[LINEC]=count[CWORD];//to the line in case a word was in progress
    }
}
//------------------------------------set-up function
void setControls()
{
    wait();//starts when a button is pressed
    delay(REACT);
    Keyboard.print("Yes?");
    assign(30, getValue(),0);
    pressKey(4, BACK);
    Keyboard.print("no?");
    assign(31, getValue(),0);
    pressKey(3, BACK);
    /*Keyboard.print("meta?");
     assign(32, getValue(),0);
     pressKey(5, BACK);*/
}
