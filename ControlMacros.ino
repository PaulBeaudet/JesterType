//ControMacros---controling opporators that imply funtions
//such as space, enter, delete, tab, justify and so on
//-------------------------------------------YES CASE
void yesCase()
{
  if(count[YESC])
  {
    if(count[YESC]==1)
    {
      period();
    }
    else
    {
      enter();
      count[YESC]=0;
      return;
    }
  }
  else
  {
    space();
  };
  count[YESC]++;
}
//#####################################NO
void noCase()
{
  if(count[YESC])
  {
    if(count[YESC]<2 && count[NOC]<1)
    {
      autoFill();
      count[NOC]=0;
      return;
    }
    else if(count[YESC]==2)
    {//if yes is at the puctuation phase
      if(count[NOC])
      {//and no has been previously pressed
        if(count[NOC]==1)
        {//that is pressed once
          punctuation(',');//place a comma
        }
        else if(count[NOC]==2)
        {//its pressed a third time
          punctuation('!');//place an exclaimation
          count[YESC]=0;//reset yes an no
          count[NOC]=0;
          return;
        };
      }
      else//no has not been pressed ye
      {
        punctuation('?');//place a question
      };
    };//no is incremented in most yes no cases
    count[NOC]++;//add to the no count
  }
  else
  {//in all other cases 
    backSpace();//remove a character
    count[YESC]=0;//set yes to 0 in order to define specific cases
  };
}
//--------------------------------------------Typing functions
void printLetter(byte letterNum)
{
  if(count[CSENT]==0)
  {
    pressKey(letterNum-32);
    letterBuffer[0]=letterNum-32;
  }
  else
  {
    pressKey(letterNum);
    if(count[CWORD]<14)
    {
      letterBuffer[count[CWORD]]=letterNum;
    }
    
  }
  count[YESC]=0;
  count[NOC]=0;
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
//-----------------------------
void punctuation(char mark)
{
  backSpaces(2);
  pressKey(mark);
  pressKey(' ');
  count[YESC]++;//why?
}
//------------------------
void space()
{
  pressKey(' ');
  countChange(1);
  count[LWORD]=count[CWORD];
  count[CWORD]=0;
}
//-----------------------
void period()
{
  backSpaces(1);//irony=1 backspaces
  pressKey('.');
  pressKey(' ');
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
      movement(count[CWORD],LEFT);//:KeyboardFunctions
      //move the cursor to the left of the current word prepping it
    }//to be pushed to the next line thus keeping word whole
    enter();//regardless press enter and add the current word count
    count[LINEC]=count[CWORD];//to the line in case a word was in progress
  }
}
//------------------------------------set-up function
void setControls()
{//prompt for setting up the control structure
  wait();//starts when a button is pressed//:valueAssement_buttons
  delay(REACT);//buffer for the desired value to be read
  pressKey('y');
  pressKey('e');
  pressKey('s');
  pressKey('?');//What would you like "yes" to be?
  assign(30, getValue(),0);//which is address 60 in EEPROM  
  pressKey('-');
  pressKey('n');
  pressKey('o');
  pressKey('?');//What would you like "no" to be?
  assign(31, getValue(),0);//which is address 62 in EEPROM
  backSpaces(8);//Remove the prompt
}



