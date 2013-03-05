#include<avr/pgmspace.h>
#include <EEPROM.h>
//
/*
JesterType// real name pending
 ###########COPY NOTICE######################################################
 Copyright (c) 2013 Paul C Beaudet <inof8or@gmail.com>
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without limitation the rights to use, copy and modify subject
 to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ###########LICENCE CHANGE#####################################################
 NOTICE: Licence will be changed! My intention is much more permissive for persons
 that do business and or work under legal name and have more restrictive terms for 
 commercial reuse. This will be sorted out as soon as I find a licence that suits
 my purposes. Contact me if you need distribution terms in the mean time or have 
 any ideas.
 <inof8or@gmail.com> 
 I really care about Freedom of ideas, however Freedom isn't free as in gifts. Nor
 is Freedom of ideas the defacto of business in our current society. 
 It is very clear to me that some entities respect the aspiration for freedom
 more then others. Established OOS licences as far I can tell do not account for this,
 in a way the effectively deals with the social realities of collaborative efforts as 
 they relate to bussiness.
 ##############################################################################
 
 Build for the Arduino Micro, Leonardo or Usb Lilypad (ATMEGA32u4) 
 
 ######### CURCUIT DISCRIPTION ################################################
 Five momentary switches are physically wired in the following fashion starting from 
 the assigned pins (in the current case 2-6) for each pin.
 From pin to switch
 switch to 330om resistor // likely flexablity in resistor size.
 resistor to ground
 Thats it! Very simple for this prototype. Note that Internal pull-ups are used.
 ################################################################################
 */
//-----------------eeprom session key, change to start over (if you forgot yes/no assignment)
#define KEY 53
//-----------------------------------------------------------------define buttons
byte buttons[]=
{
  2,3,4,5,6
};
#define NUMBUTTONS sizeof(buttons)

//constants related to getting the value
#define WINDOW 125 // time window in which chords are recorded
#define BOUNCE 30// time to debounce
#define RESTING 11111 // this is the resting chord value

//----------------------------------------------keyboard definitions
//yes and no structure counters and flag
#define LINESIZE 80//just needs to be under 255
boolean capflag;
int yesCount=0;
int noCount=0;
//word building
byte printCount=0;
int wordCount=0;
int lastWordCount=0;
int sentenceCount=0;
int lastSentenceCount=0;
char lastLetter;//holds the last letter
//--condition chords for automated responses
unsigned int no;
unsigned int yes;

//common letter frequencies 2d array, data points are the letter's address space in eeprom
#define LPLACES 7
PROGMEM byte commonLetters[LPLACES][26]=
{
  {
    232,194,230,208,238,210,222,196,218,204,198,216,200,224,220,202,206,228,242,234,236,212,214,226,240,244}
  ,
  {
    202,208,222,194,220,210,204,228,234,232,216,198,200,218,238,230,206,242,224,196,236,214,212,240,226,244}
  ,
  {
    202,232,194,210,222,220,228,208,230,200,216,198,234,218,238,204,206,242,224,196,236,214,212,240,226,244}
  ,
  {
    202,232,194,222,210,220,230,208,228,200,216,198,234,218,238,204,206,242,224,196,236,214,212,240,226,244}
  ,
  {
    202,228,210,222,232,220,230,194,200,216,208,198,234,218,238,204,206,242,224,196,236,214,212,240,226,244}
  ,
  {
    202,232,194,228,210,220,230,208,222,200,216,198,234,218,238,204,206,242,224,196,236,214,212,240,226,244}
  ,
  {
    202,232,194,200,210,220,230,208,228,222,216,198,234,218,238,204,206,242,224,196,236,214,212,240,226,244}
};
//This could be reduced in size by only accounting for the first 13 most common letters

//modifiers are assign as variables to pass to functions
prog_char supeRight= KEY_RIGHT_GUI;
prog_char lctrl = KEY_LEFT_CTRL;
prog_char rctrl = KEY_RIGHT_CTRL;
prog_char lshift = KEY_LEFT_SHIFT;
prog_char rshift = KEY_RIGHT_SHIFT;
prog_char lalt = KEY_LEFT_ALT;
prog_char ralt = KEY_RIGHT_ALT;
prog_char up = KEY_UP_ARROW;
prog_char down = KEY_DOWN_ARROW;
prog_char right = KEY_RIGHT_ARROW;
prog_char left = KEY_LEFT_ARROW;
prog_char HOME = KEY_HOME;
prog_char END = KEY_END;
prog_char f2 = KEY_F2;
prog_char f4 = KEY_F4;
prog_char f5 = KEY_F5;
prog_char f10 = KEY_F10;
prog_char f12 = KEY_F12;
prog_char tab = KEY_TAB;
prog_char RTN = KEY_RETURN;


#define BACK 178
#define TAB 179

#define REACT 300// time for computer host to react to commands

//substitutes
char compSug[] = {
  "word "};// this is a temp sub for autofill 

//-----------------------------------------------------------------------Set up
void setup()
{
  Keyboard.begin();
  delay(2000);
  //paulsMacro();// this opens a text editor for testing, comment this out
  // A personal macro placed here may help for testing purposes 
  //------------input setting
  for (byte set=0;set<NUMBUTTONS;set++)
  {
    //sets the button pins
    pinMode(buttons[set], INPUT);
    digitalWrite(buttons[set], HIGH); 
  }  
  //this sets pull-up resistor/ie input through 20k to 5v
  //in this way| input to button/button to ground, is proper and will read as low when pressed
  //-------------------------------------------------------------------------------------------- 

  //--check if this arduino's eeprom has been used for something else if it has clear the eeprom
  if (session(512, KEY))
  {
    clearPROM(0, 254);
    //clear possible old assignments
    delay(REACT);//!!added!!sometimes it misses yes prompt
    Keyboard.print("Yes?");
    assign(30, getValue());
    sKey(4, BACK);
    Keyboard.print("no?");
    assign(31, getValue());
    sKey(3, BACK);
    //promt and assign for yes/no
  }

  yes = word(EEPROM.read(60), EEPROM.read(61));
  no = word(EEPROM.read(62), EEPROM.read(63));
  //put personal yes/no in ram so it doesn't need to be parsed from EEPROM
}
//-----------------------------------------------------------------------------begin main loop
void loop()
{
  while(true)//to provide a way to start from step one with a return
  {
    //linesize limiter
    if(printCount>LINESIZE)
    {
      sKey(1, RTN);
      printCount=0;
    }
    //get the current button status 
    unsigned int chordValue=getValue();
    // if yes or no do those and restart the loop
    if(chordValue==yes)
    {
      yesCase();
      return;//restart the loop
    }
    if(chordValue==no)
    {
      noCase();
      return;//restart the loop
    }
    //-------------------------------letter related steps
    //figure out if the chord is a letter that has an assignment in eeprom
    byte letteR=check(chordValue);
    //if no assignment take an educational guess and make one
    if(letteR == 0)
    {
      letteR=learnUser();
      //receive a letter from the learning algorithm
      assign(letteR, chordValue);
      // assign that letter to the kepmaping
    }
    //print the result
    printLetter(letteR);
  }
}
//--------------------------------------------------------------------------end main loop
//----------------------------storage fuctions---making assignments
unsigned int assign(byte letter, unsigned int chordValue)
{
  int modifier=0;
  unsigned int storedVal=word(EEPROM.read(letter*2), EEPROM.read(letter*2+1));
  //allow letters to posibily have more then one value
  if(storedVal>0)
  {
    if (letter>96 && letter<123)
    {
      modifier=-95;
    }
    storedVal=word(EEPROM.read(letter*2+modifier), EEPROM.read(letter*2+1+modifier));
  }
  EEPROM.write(letter*2+modifier, highByte(chordValue));
  delay(5);//!! delays after writes maybe redundent
  EEPROM.write(letter*2+1+modifier, lowByte(chordValue));
  delay(5);
  return storedVal;
}

//Checking

byte check(unsigned int chordValue)
{
  byte letter=0;
  for(int address=2;address<254;address+=2)//for the space pre-allocated in eeprom
  {
    //check there is something the equals the current chord
    if(word(EEPROM.read(address), EEPROM.read(address+1)) == chordValue)
    {
      // the address represents the letter assosiated with the chord
      if(address>65)
      {
        //half of the address number equals assosiated letter 
        //ie char 97==a, int values take twice the address space, therefore int 128 == a
        letter=address/2;//luckly we are just dealing with even numbers
        return letter;
      }
      //some of the addresses need more translating to comprehend their chord assosiation 
      if(address<65)
      {
        //this allows storage of duplicate chord representations of lowercase values
        letter=address/2+95;
        return letter;
      }
    }
  }
  //nothing was in eeprom return 0
  return letter;
}

//------------------------------------------------------------------------learning function  
byte learnUser()//!!symantically a misnomer as of current, better discribes intention
{
  //Give most common unassiged letter based on possition in the word
  if(wordCount<LPLACES)
  { 
    for (int freq=0; freq<26; freq++)
      //interate frequencies
    {
      byte letterNumber= pgm_read_byte(&(commonLetters[wordCount][freq]));
      //explicitly read byte from progmem
      unsigned int chord=word(EEPROM.read(letterNumber), EEPROM.read(letterNumber+1));
      if (chord==0)
        //check if the frequency is assigned for the given possition
      {
        return letterNumber/2;
        //return the availible letter that is most fequently used for this possition
      }
    }
    //lowercase have been found assigned at this point
    //flag to make alternate assignments
    //if possition one go for caps then numbers and certain first possition rellated symbols
    //if > possition one go for synonomous chords except only upto the 13 most common vs 26
  }
  // old sequential search
  //first priority assign lower case letter range
  for(int address=194;address<252;address+=2)
  {
    unsigned int filled = word(EEPROM.read(address), EEPROM.read(address+1));
    if(filled==0)
    {
      int letterNum=address/2;
      return letterNum;
    }
  }
  //second priority symbols/numbers
  for(int address=66;address<127;address+=2)
  {
    unsigned int filled = word(EEPROM.read(address), EEPROM.read(address+1));
    if(filled==0)
    {
      int letterNum=address/2;
      return letterNum;
    }
  }
}

//------------------------------------------------------------------------------------------button functions
//---------------------------------------------------------------------------------returning the chord value
//return the raw value, monitor the buttons in a time window
unsigned int getValue()
{
  boolean basePress=true;
  boolean regularPress=true;
  boolean longPress=true;
  unsigned int builtChord=0;
  unsigned int oldChord=RESTING;
  unsigned int baseChord=RESTING;//assumed inactivity when called
  unsigned long past=millis()-1;//first millis diff is engineered here to make sure things get on the ball
  int windowCount=0;
  while(windowCount<=WINDOW || getDebouncedState()>RESTING)//gather chordValue with in this time frame
  {
    unsigned long currentTime=millis();
    if(baseChord!=RESTING || windowCount>BOUNCE)// activity detection, start timer
    {
      unsigned long diff=currentTime-past;
      windowCount+=diff;
    }
    //--gather chord 
    baseChord=getDebouncedState();
    if(baseChord > RESTING || baseChord!=oldChord)// in short, when active and different
    {
      oldChord=baseChord;
      if(basePress && windowCount > 50)
      {
        builtChord+=baseChord-RESTING;//this will allow for a base of 15 values
        basePress=false;
      }
      if(regularPress && windowCount > 400)
      {
        unsigned int temp=baseChord-RESTING;
        builtChord+=temp*2;
        regularPress=false;
      }
      if(longPress && windowCount > 750)
      {
        unsigned int temp=baseChord-RESTING;
        builtChord+=temp*3;
        longPress=false;
      }

    }
    //--set the past time
    past=currentTime;
  }
  //Serial.println(windowCount);
  return builtChord;
}
//---------------------------------------------------------debouncing
// debounce the raw chord values derived from rawInput();
int getDebouncedState()
{
  static int oldState = 0;
  static int debouncedState = 0;
  static unsigned long lastChangeTime = millis();

  int newState = rawInput();
  if(newState != oldState)
  {
    // one of the inputs has changed state
    lastChangeTime = millis();
    oldState = newState;
  }
  else
  {
    // the inputs have not changed
    if((oldState != debouncedState) && (millis() - lastChangeTime > BOUNCE))
    {
      // the inputs are stable and in a new state
      debouncedState = newState;
    }
  }
  return debouncedState;
}

// Read the values of all the buttons and represent them as an int
int rawInput()
{
  int incrementor=1;
  int value=RESTING;//2 will represent high as zero may prove problematic for low
  for(int i=0;i<NUMBUTTONS;i++)
  {
    if(digitalRead(buttons[i])==LOW)
    {
      //build an signal variable for all inputs
      value+=incrementor;
      //each button is represented by a seperate place value
      incrementor*=10;
    } 
    else
    {
      //given state has not changed incrementor still needs to progress
      incrementor*=10;
    }
  }
  return value;
}

//------------------------------------------------------------------User facing automation and response 

//-------------------------------------------NO CASE
// this is a test
//!!both cases need work, there is a symantic issue with the number counting
void noCase()
{
  if(noCount==0)
  {
    //comp suggestion case, no after word complete || first word 
    if(yesCount==1 || sentenceCount==0)
    {
      byte addCount=sizeof(compSug);
      addCount--;//-1 to take care of null
      if(yesCount==1)
      {
        sKey(lastWordCount, BACK);
        lastLetter=compSug[addCount-2];//this is the last letter of the computer suggestion
        printCount-=lastWordCount;
        sentenceCount-=lastWordCount;
      }
      Keyboard.print(compSug);
      printCount+=addCount;
      sentenceCount+=addCount;
      lastWordCount=addCount;
      return;
    }
    if(yesCount==2)
    {
      sKey(2, 178);//backspace twice
      Keyboard.print("? ");
      noCount++;
      return;
    }
    //backspace, no count==0 general behaviour above are exceptions
    sKey(1, BACK);
    printCount--;
    wordCount--;
    sentenceCount--;
    //Pronoun case
    if(wordCount==0 && lastLetter>96)
    {
      capflag=true;
      printLetter(lastLetter);
      return;
    }
    noCount++;
    return;
  }
  //end noCount 0 cases---begin nocount 1, ie double back
  if(noCount==1)
  {
    if(yesCount==2)
    {
      sKey(2, BACK);
      Keyboard.print("! ");
      noCount++;
      return;
    }
    if(wordCount==0 || sentenceCount==0)
    {
      return;
    }
    //double back general behaviour above are exceptions/ delete word
    sKey(wordCount, BACK);
    printCount-=wordCount;
    sentenceCount-=wordCount;
    noCount++;
    return;
  }
}

//------------------------------------------------------------------------YES CASE
void yesCase()
{
  if(yesCount==0)
  {
    if(printCount==0)
    {
      sfill(4, ' ');
      return;
    }
    //default, make a word
    Keyboard.print(" ");
    printCount++;
    lastWordCount=wordCount+1;//accounts for space!!!
    wordCount=0;//time for a new word!!
    sentenceCount++;
    yesCount++;
    return;
  }
  if(yesCount==1)
  {
    if(printCount==1)
    {
      sfill(LINESIZE/2-4, ' ');
    }
    sKey(1,BACK);//remove space from last word
    lastWordCount++;
    Keyboard.print(". ");
    printCount++;// only goes up one because a space is removed
    lastSentenceCount=sentenceCount+1;
    sentenceCount=0;
    yesCount++;
    //noCount=0;
    return;
  }
}
//--------------------------------------------keyboard functions
//---------------------------letter printing
void printLetter(byte letterNum)
{
  if(sentenceCount==0 || capflag)
  {
    Keyboard.write(letterNum-32);
    lastLetter=letterNum-32;
    capflag=false;
  }
  else
  {
    Keyboard.write(letterNum);
    lastLetter=letterNum;
    yesCount=0;
    noCount=0;
  }
  printCount++;
  wordCount++;
  sentenceCount++;
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
      printCount++;
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

//---------------------------------macros
void paulsMacro()
{
  combo(lshift, lalt);
  //change layout
  delay(REACT);
  combo(lctrl, ' ');
  //bring up app search
  delay(REACT);
  Keyboard.print("pluma");
  // open mint's text editor
  delay(REACT);
  Keyboard.println(  );
  Keyboard.press(lalt);
  delay(REACT);
  Keyboard.press(tab);
  //change window focus
  delay(REACT);
  Keyboard.releaseAll();
}
//-----------------------------------------------------------------------session related functions


//------------------------------------clear EEPROM
void clearPROM(int start, int finish)
{
  for(int i=start;i<finish;i++)
  {
    EEPROM.write(i, 0);
    delay(6);//again percautionary, not sure is this is nessisary
  }
}

//-----------------------------check for persistent key
boolean session(int address, byte code)
{
  //establishes if source has been run by checkig is a key/code has been written
  if(code==EEPROM.read(address))
  {
    return false;
    // false, ie skip things that would need to be done in an unastablished session
  }
  else
  {
    // write the key to establish the session
    EEPROM.write(address, code);
    return true;
    // true, ie do things unique to an unestablished session 
  };
}

