//ControMacros---controling opporators that imply funtions
//such as space, enter, delete, tab, justify and so on
//!!have been advised that switch cases are probably a bad idea
// because they may me interperted differently my differnt MCUs
void setControls()
{
    wait();//starts when a button is pressed
    delay(REACT);
    Keyboard.print("Yes?");
    assign(30, getValue(),0);
    sKey(4, BACK);
    Keyboard.print("no?");
    assign(31, getValue(),0);
    sKey(3, BACK);
    /*Keyboard.print("meta?");
     assign(32, getValue(),0);
     sKey(5, BACK);*/
}
//-------------------------------------------YES CASE
void yesCase()
{
  //regular activity 
  switch(count[YESC])
  {
  case 0://yes
    space();
    break;
  case 1://yes, yes
    period();
     break;
  case 2://yes, yes, yes
    enter();
    break;
  default:
    count[YESC]=0;
    return;
  }
  count[YESC]++;
}
//#####################################NO
void noCase()
{
  if(count[YESC])
  {
    switch(count[YESC])
    {
    case 1:
      autoFill();//or after word//yes, no
      //whether autofill or spell correct is used depends
      //on the amount of letters in the buffer
      break;
    case 2:
      punctuation('?');
      break;
    case 3:
      punctuation('!');
      break;
    default:
      count[YESC]=0;//set yes to 0 in order to define specific cases
    }
    count[NOC]++;//add to the no count
  }
  else
  {//in all other cases 
    backSpace();//remove a character
    count[YESC]=0;//set yes to 0 in order to define specific cases
  };
}

//###########################################formating functions
void autoLineReturn()
{//for main loop
  if(count[LINEC]>LINESIZE)
    {//one the line size limit is reached
      if(count[CWORD])
      {//given there are printed letters 
        sKey(count[CWORD],left);//:KeyboardFunctions
        //move the cursor to the left of the current word prepping it
      }//to be pushed to the next line thus keeping word whole
      sKey(1, RTN);//regardless press enter and add the current word count
      count[LINEC]=count[CWORD];//to the line in case a word was in progress
    }
}
void backSpace()
{
  sKey(1,BACK);
  countChange(-1);
}
void backlast(byte last)
{
  sKey(count[last],BACK);
  count[LINEC]-=count[last];
  while(count[LINEC]>LINESIZE)
  {
    count[LINEC]+=LINESIZE;
  }
  count[last]=0;
}

void punctuation(char mark)
{
  sKey(2,BACK);
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
  sKey(1,BACK);
  Keyboard.print(". ");
  countChange(1);
  count[LWORD]++;
  count[LSENT]=count[CSENT];
  count[CSENT]=0;
}

//########## Meta or Magic functions 
/*void switchWindow()
{
  Keyboard.press(lalt);
  Keyboard.press(tab);
  delay(REACT);
  Keyboard.press(tab);
  delay(REACT);
  Keyboard.releaseAll();
}*/
