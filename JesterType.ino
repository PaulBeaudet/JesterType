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
 From pin to switch--switch to 330om resistor--resistor to ground
 //resistor is a safety for prototyping, not needed in purpose built application
 Thats it! Very simple for this prototype. Note that Internal pull-ups are used.
 ################################################################################
 */
//-----------------eeprom session key, change to start over (if you forgot yes/no assignment)
#define KEY 56
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
//resting need to be changed to reflected the number of buttons

//----------------------------------------------keyboard definitions
//yes and no structure counters and flag
#define LINESIZE 80//just needs to be under 255

//counts organized into an array for easy iteration 
//and function passing
byte count[7]={0,0,0,0,0,0,0};
//pCount KEY, to make the code readable outside of iteration 
#define LINEC 0 // line return sensor
#define CWORD 1 //current word
#define CSENT 2 //current sentence
#define LWORD 3 // last word
#define LSENT 4 // last sentence
#define YESC 5 // yes count
#define NOC 6 // no count
#define METAC 7 // meta count
//KEY: 0=line return/printed, 1=word, 2=sentence,    
//3=last word, 4=last sentence, 5=yes, 6=no

char lastLetter;//holds the last letter
char letterBuffer[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//holds up to 8 letters for autofill fucntions
//--condition chords for automated responses
boolean rJustify=false;
boolean cJustify=false;
boolean explicitMode=false;
word no;
word yes;
word meta;

//error correction and alternate assignments
boolean learningPhase[2];
// the modifier key to the second assignment
#define SECONDLAY 96
// it define the amount of offset from the first assignment in eeprom

//common letter frequencies 2d array, data points are the letter's address space in eeprom
#define LPLACES 7
#define FREQ 13

prog_char commonLetters[8][FREQ] PROGMEM=
//the eighth place accounts for uncommon letters
{
  {
    't','a','s','h','w','i','o','b','m','f','c','l','d'              }
  ,
  {
    'e','h','o','a','n','i','f','r','u','t','l','c','d'              }
  ,
  {
    'e','t','a','i','o','n','r','h','s','d','l','c','u'              }
  ,
  {
    'e','t','a','o','i','n','s','h','r','d','l','c','u'              }
  ,
  {
    'e','r','i','o','t','n','s','a','d','l','h','c','u'              }
  ,
  {
    'e','t','a','r','i','n','s','h','o','d','l','c','u'              }
  ,
  {
    'e','t','a','d','i','n','s','h','r','o','l','c','u'              }
  ,
  {
    'm','f','p','g','w','y','b','v','k','x','j','q','z'              }
};
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

#define REACT 300// time for computer host to react to commands

//substitutes
char compSug[] = {
  "word "};// this is a temp sub for autofill 

//-----------------------------------------------------------------------Set up
void setup()
{
  Keyboard.begin();
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
    clearPROM(0, 256);
    //clear possible old assignments
    //paulsMacro();// this opens a text editor for testing, comment this out
    while(digitalRead(buttons[0])==HIGH)
    {
      // waits for input to prompt yes and no
      ;
    }
    delay(REACT);
    Keyboard.print("Yes?");
    letterWrite(30, getValue(),0);
    sKey(4, BACK);
    Keyboard.print("no?");
    letterWrite(31, getValue(),0);
    sKey(3, BACK);
    Keyboard.print("meta?");
    letterWrite(32, getValue(),0);
    sKey(5, BACK);
    //promt and assign for yes/no
  }

  yes = word(EEPROM.read(60), EEPROM.read(61));
  no = word(EEPROM.read(62), EEPROM.read(63));
  meta= word(EEPROM.read(64), EEPROM.read(65));
  //put personal yes/no in ram so it doesn't need to be parsed from EEPROM
  learningPhase[0] = EEPROM.read(255);
  learningPhase[1]= EEPROM.read(254);
  //ascertain where we are in the learning process
}
//-----------------------------------------------------------------------------begin main loop
void loop()
{
  while(true)//to provide a way to start from step one with a return
  {
    //linesize limiter
    if(count[LINEC]>LINESIZE)
    {
      /*if(rJustify)
      {
        backTxt();
      }
      else
      {*/
        if(count[CWORD])
        {  
          sKey(count[CWORD],left);
        }
        sKey(1, RTN);
        count[LINEC]=count[CWORD];
      //};
    }
    //get the current button status 
    word chordValue=getValue();
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
    if(chordValue==meta)
    {
      metaCase();
      return;
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
    if(letteR == 0 && learningPhase[1])
    {
      // !! filter currently finds minimal differance assigned chord
      //learningDone=true;
      letteR= filter(chordValue);//just returns # to represent noise
    }
    //print the result
    printLetter(letteR);
  }
}
//--------------------------------------------------------------------------end main loop
//----------------------------storage fuctions---making assignments

void assign(byte letter, word chordValue)
{
  if(learningPhase[0])
  {
    letterWrite(letter, chordValue,SECONDLAY);
  }
  else
  {
    letterWrite(letter, chordValue, 0);
  };
  //make the assignment

}

void letterWrite(byte letter, word chordValue, byte modifier)
{
  //Writes unsigned ints into EEPROM
  byte hold = letter-modifier;
  EEPROM.write(hold*2, highByte(chordValue));
  delay(5);//!! delays after writes maybe redundent
  EEPROM.write(hold*2+1, lowByte(chordValue));
  delay(5);
}

//Checking

byte check(word chordValue)
{
  for(int address=194;address<246;address+=2)
  //for the first layout
  {
    //check there is something the equals the current chord
    if(word(EEPROM.read(address), EEPROM.read(address+1)) == chordValue)
    {
      return address/2;
      //translates to ascii number for the represented letter
      //first layout defined by a mutipul of 2
      // because ints need to be seperated into 2 bytes
    }
  }
  //continue given no matches are found
  for(int address=2;address<56; address+=2)
  //for the second layout
  {
    if(word(EEPROM.read(address), EEPROM.read(address+1)) == chordValue)
    {
      return address/2+SECONDLAY;
      //translates to ascii number for the represented letter
      //second layout defined by an offset
    }
  }
  //no assignments made for given cord, return 0 or false
  return 0;
}

//------------------------------------------------------------------------learning function  
byte learnUser()//!!symantically a misnomer as of current, better discribes intention
{
  byte letter=0;
  if(learningPhase[0])//if the first assignment has been made
  {
    //look for something unfilled in the second assignment
    letter=learningSeq(SECONDLAY);
    // test success
    if (letter==0)
      // if nothing is there we can check for symbols
    {
      /*
      //sybols and numbers
      for(int address=66;address<127;address+=2)
      {
        if(oneCheck(address,0)==0)
        {
          return address/2;
        }
        //if all of these are assigned letter still be 0
      }*/
      //passing an ultimate "false" or 0 to the main loop
      learningPhase[1]=true;
      EEPROM.write(254,1);
    }
    else
    {
      return letter;
    };
  }
  else
  {
    //make the first assignments
    letter=learningSeq(0);
    if (letter==0)
    {
      // if there is nothing there check the second
      letter=learningSeq(SECONDLAY);
      // and flag that the first assignment has been done
      learningPhase[0]=true;
      EEPROM.write(255, true);
    }
    else
    {
      return letter;
    };
  };
}

unsigned int oneCheck(byte letter, byte mod)
{
  byte pHold= letter-mod;
  return word(EEPROM.read(pHold*2), EEPROM.read(pHold*2+1));
}

byte learningSeq(byte modifier)
{
  byte letter;
  //Give most common unassiged letter based on possition in the word
  if(count[CWORD]<LPLACES)
  { 
    letter=freqLookup(count[CWORD], modifier);
    if (letter==0)
    {
      letter=freqLookup(8, modifier);
    }
    return letter;
  }
  else
  {
    letter=freqLookup(2, modifier);
    if (letter==0)
    {
      letter=freqLookup(8, modifier);
    }
    return letter;
  };
  return 0;
}

byte freqLookup(int place, byte modifier)
{
  byte letterNumber;
  for (int freq=0; freq<FREQ; freq++)
    //interate frequencies
  {
    letterNumber= pgm_read_byte(&commonLetters[place][freq]);
    if (oneCheck(letterNumber, modifier)==0)
      //check if the frequency is assigned for the given possition
    {
      return letterNumber;
      //return the availible letter that is most fequently used for this possition
    }
  }
  return 0;
}

//------------------------------------------noise filtering
byte filter(word noise)
{
  unsigned int correctToValue=9;
  //cant be nine so if it prints this something is wrong
  int lowPoint=15;
  for(int address=194;address<246;address+=2)
  {
    unsigned int largerNum;
    unsigned int compare=word(EEPROM.read(address), EEPROM.read(address+1));
    unsigned int sComp=compare; //temporary second comparison
    unsigned int fComp=noise; //temp first comparison
    int pointCompare=0;
    if(fComp>sComp)
    {
      largerNum=fComp;
    }
    else
    {
      largerNum=sComp;
    };
    unsigned long mag= 1;
    while (mag*10<=largerNum)
    {
      mag*=10;
    }
    while(mag>0)
    {
      if(fComp>sComp)
        //overflow prevention
      {
        pointCompare+= fComp/mag - sComp/mag;
      }
      else
      {
        pointCompare+= sComp/mag-fComp/mag;
      }
      fComp%=mag;
      sComp%=mag;
      mag/=10;
    }
    if(pointCompare<lowPoint)
      //filters to the path of least resistence 
    {
      lowPoint=pointCompare;
      //set a new lowpoint
      correctToValue=compare;
      //remember the lowpoint value
    }
  }
  return check(correctToValue);
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
void yesCase()
{
  if(count[METAC]>1)
  {
    switch(count[METAC])
    {
    case 2://alt-tab//meta, yes..
      switchWindow(); 
      //will continue with case 1 unless meta increments
      return;
    case 3://2-4 fall through and mean the same thing
    case 4:
    case 5:
      enter();
      count[METAC]=0;
      return;
    }
  }
  /*else if(count[CSENT]==0 && count[LINEC]==0)//not sure this will work
    //inactivity case
  {
    switch(count[YESC])
    {
    case 0:
      sfill(4, ' ');
      break;
    case 1:
      centerJust();//nothing, yes, yes
      break;
    case 2:
      rightJust();//nothing, yes, yes, yes
      break;
    case 3:
      enter();//nothing, yes, yes, yes, yes
      break;
    default:
      count[YESC]=0;
      return;
    }
  }*/
  else
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
    case 3://yes, yes, yes, yes
      combo(rctrl,'s');
      //save();
      break;
    default:
      count[YESC]=0;
      return;
    }
  };
  count[YESC]++;
}
//###################################META/MAGIC
void metaCase()//or "magic" keys
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
}
//#####################################NO
void noCase()
{
  if(count[METAC])
  {
    //closeProgram();//alt-f4//meta, no
    combo(lalt, f4);//CLOSE# close window
  }
  /*else if(count[CSENT]==0 && count[LINEC]==0)//nothing case?
  {
    //back();//alt-left//nothing, no..
    combo(lalt, left);//BACK#browser back
  }*/
  else if(count[YESC])
  {
    switch(count[YESC])
    {
    case 1:
      autofill();//or after word//yes, no
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
  else if(count[CWORD])
  {
    backSpace();//no... forSpaces inWord
  }
  else
  {
    if(count[LWORD] && count[LINEC])
    {
      count[CSENT]-=count[LWORD];
      backlast(LWORD);//..no after aWord backspaced
    }
    else if (count[CSENT] && count[CSENT]<175 && count[LINEC])
    {
      backlast(CSENT);//..no after backword
      //revert to backspace after sentence removal
    }
    else
    {
      count[LSENT]=0;
      backSpace();
    };
  };
  count[NOC]++;
}
//##########################yes no functions
//###########################################formating functions
void autofill()
{
}
void backSpace()
{
  sKey(1,BACK);
  countChange(-1);
  //pronoun case
  if(count[CWORD]==0 && letterBuffer[0]>96)
  {
    printLetter(letterBuffer[0]-32);
  }
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
void backTxt()
{
  sKey(count[LSENT],left);
  sKey(count[LSENT],BACK);
  sKey(count[LSENT],right);
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
void switchWindow()
{
  Keyboard.press(lalt);
  Keyboard.press(tab);
  delay(REACT);
  Keyboard.press(tab);
  delay(REACT);
  Keyboard.releaseAll();
}
//--------------------------------------------keyboard functions
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
