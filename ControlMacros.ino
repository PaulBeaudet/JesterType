//ControMacros---controling opporators that imply funtions
//such as space, enter, delete, tab, justify and so on
byte yesCount=0;//keeps track of yeses entered
byte noCount=0;//keeps track of noes entered
boolean sentenceStart=true;//for auto capitilization feature
boolean wordStart=true;//to reset the wordCount and pronoun clauses
//paramiter for line size limiter
#define LINESIZE 80//just needs to be under 255
char printedLetters[2][LINESIZE]={
  {
  }
  ,{
  }
};//buffer for remembering printed letters
boolean editLine=0;//keeps track of which printed letter line is being written
byte lineCount=0;//keeps track of possition on the line
byte wordCount=0;//keeps track of size of working word
//-------------------------------------------YES CASE
void yesCase()
{
  if(yesCount)
  {
    if(yesCount==1)
    {
      backSpace();
      pressKey('.');
      pressKey(' ');
      printedLetters[editLine][lineCount]='.';
      lineCount++;
      printedLetters[editLine][lineCount]=' ';
      wordCount=0;
      sentenceStart=true;
    }
    else
    {
      enter();
      editLineSwitch(LINESIZE);
      yesCount=0;
      lineCount=0;
      return;
    }
  }
  else
  {    
    pressKey(' ');
    printedLetters[editLine][lineCount]=' ';
    wordStart=true;
    wordCount++;//add to word count for formating functions
    lineCount++;
  };
  yesCount++;
}
//#####################################NO
void noCase()
{//if else tree i.e. one trunk
  if(yesCount)
  {
    if(yesCount==1)
    {//if the space has been placed
      if(noCount)//if no and yes are 1 (after autosug case)
      {
        wordStart=false;
        backSpace();
        printedLetters[editLine][lineCount]=0;
        lineCount--;//decrement edit possition
        yesCount=0;//set yes to zero as a proceeding period is now irrelevent
        //wordCount=lastWord()//!!function to find last word size needed
      }
      else
      {
        autoFill();//fill in the suggestion word
        noCount++;//so that autofill does not continue 
      };
    }
    else if(yesCount==2)
    {//if yes is at the puctuation phase
      if(noCount)
      {//and no has been previously pressed
        if(noCount==1)
        {//that is pressed once
          punctuation(',');//place a comma
        }//only period is recorded to simplify!!
        else if(noCount==2)
        {//its pressed a third time
          punctuation('!');//place an exclaimation
          //only period is recorded to simplify!!
          yesCount=0;//reset yes an no
          noCount=0;
          return;
        };
      }
      else//no has not been pressed ye
      {
        punctuation('?');//place a question
      };//only period is recorded to simplify!!
    };//no is incremented in most yes no cases
    noCount++;//add to the no count
  }
  else
  {//in all other cases 
    backSpace();//remove a character
    printedLetters[editLine][lineCount]=0;
    lineCount--;//decrement edit possition
    wordCount--;
    if (lineCount == 0 || printedLetters[editLine][lineCount-1]=='.')//or '!','?'
    {
      sentenceStart=true;
    }
    yesCount=0;//set yes to 0 in order to define specific cases
    return;//do not increment no count
  };
  //noCount++;//fall through case unless a return is called
}
//--------------------------------------------Typing functions
void printLetter(byte letterNum)
{
  if(sentenceStart)
  {
    pressKey(letterNum-32);
    printedLetters[editLine][lineCount]=letterNum-32;
    sentenceStart=false;
  }
  else
  {
    pressKey(letterNum);
    printedLetters[editLine][lineCount]=letterNum;
  };
  if(wordStart)
  {
    wordCount=0;
  }
  yesNoReset();
  wordStart=false;
  lineCount++;
  wordCount++;
}
//-----------------------------
void punctuation(char mark)
{
  backSpaces(2);
  pressKey(mark);
  pressKey(' ');
}
//------------------------batch count change
void yesNoReset()
{
  yesCount=0;
  noCount=0;  
}
//----------determine current wordsize
/*
byte sizeCurrent()
 {
 byte i= lineCount;
 char used=printedLetters[editLine][i];
 byte scope=0;//size
 while(used > 48 && i)//48 is 0 on the ascii table
 {//lesser values are non-characters
 i--;//decrease increment
 used=printedLetters[editLine][i];
 //read the letter to parse whether it has been used
 scope++;
 }
 if (used==' ')//or 32 in ascii
 {//presuming the end of a word is the first space
 return scope;
 }
 }*/
//###########################################formating functions
#define LINESWITCH 20//number at which edit line switches
void autoLineReturn()
{//for main loop
  if(lineCount>LINESIZE)
  {//one the line size limit is reached
    if(wordCount)
    {//given there are printed letters 
      movement(wordCount,LEFT);//:KeyboardFunctions
      //move the cursor to the left of the current word prepping it
      byte i=0;
      for(byte backtrack=LINESIZE-wordCount;backtrack<wordCount;backtrack++)
      {//move buffered word in printed letters
        printedLetters[!editLine][i]=printedLetters[editLine][backtrack];
        printedLetters[editLine][backtrack]=' ';
        i++;
      }
    }//to be pushed to the next line thus keeping word whole
    enter();//regardless press enter and add the current word count
    editLine=!editLine;//now edit line is the other editline
    lineCount=wordCount;//to the line in case a word was in progress
  }
  else
  {
    editLineSwitch(LINESWITCH);
  };
}
void editLineSwitch(byte compare)
{
  if(editLine && lineCount<compare)
  {
    for(byte i=0;i<lineCount;i++)
    {
      printedLetters[0][i]=printedLetters[1][i];
    }
    editLine=0;
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










