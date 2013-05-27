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
    if(explicitMode==false)
    {
      period();
      break;
    }//in other words fall through when explicit mode is true
  case 2://yes, yes, yes
    enter();
    break;
  /*case 3://yes, yes, yes, yes
    combo(rctrl,'s');
    //save();
    break;*/
  default:
    count[YESC]=0;
    return;
  }
  count[YESC]++;
}
//###################################META/MAGIC
/*void metaCase()//or "magic" keys
{
  switch(count[METAC])
  {
  case 0:
    //modeSwitch(); // changes compisitional mode
    explicitMode= !explicitMode;
    break;
  case 1:
    //search();//open program
    combo(rctrl, ' ');//SEARCH# bring up synaps, linux
    break;
  case 2://places or web search ##!!need to cater to OS
    //webSearch();//open web site
    sKey(1, right);//move one to the right for places option
    break;
  case 3://files or web search ##!!need to cater to OS
    //fileSearch();//openlocal file
    sKey(5, right);//move five to the right for web option
    break;
  case 4:
    combo(rctrl, ' ');//close search 
  default:
    count[METAC]=0;
    return;
  }
  count[METAC]++;
}*/
//#####################################NO
void noCase()
{
  /*if(count[METAC])
   {
   //closeProgram();//alt-f4//meta, no
   combo(lalt, f4);//CLOSE# close window
   }
  /*else if(count[CSENT]==0 && count[LINEC]==0)//nothing case?
   {
   //back();//alt-left//nothing, no..
   combo(lalt, left);//BACK#browser back
   }*/
  //else 
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
    }
  }
  else
  {//in all other cases 
    backSpace();//remove a character
  };
  count[NOC]++;//add to the no count
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
void centerJust()
{
  count[LSENT]=0;
  cJustify=true;
  explicitMode=true;
  sfill(LINESIZE/2-count[LINEC], ' ');
}
void rightJust()
{
  count[LSENT]=0;
  rJustify=true;
  explicitMode=true;
  sfill(LINESIZE-1-count[LINEC], ' ');
}
void enter()
{
  if(rJustify)
  {
    count[LSENT]=0;
    rJustify=false;
  }
  if(cJustify)
  {
    sKey(count[LSENT],left);
    sKey(count[LSENT]/2,BACK);
    sKey(count[LSENT],right);
    cJustify=false;
  }
  Keyboard.write(RTN);
  count[LINEC]=0;
  explicitMode=false;
}
void space()
{
  Keyboard.write(' ');
  countChange(1);
  count[LWORD]=count[CWORD];
  count[CWORD]=0;
  if(rJustify || cJustify)
  {
    count[LSENT]++;
  }
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
