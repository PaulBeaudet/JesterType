#include<avr/pgmspace.h>
#include <EEPROM.h>
//#include <MemoryFree.h>//http://playground.arduino.cc/Code/AvailableMemory
//
/*
 ###########COPY NOTICE######################################################
 JesterType- a funny way to use an arduino to type with gestures
 Copyright (c) 2012-2013 Paul C Beaudet <inof8or@gmail.com>
 This program is free software; 
 you can redistribute it and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2 of the License
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with this program;
 if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
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
//eeprom session key, change to start over (if forgot yes/no assignment
//or are testing the learning process)
//############
#define KEY 6
//############
//-----------------------------------------------------------------define buttons
byte buttons[]=
{
  2,3,4,5,6
};
#define NUMBUTTONS sizeof(buttons)
//----------------------------------------------keyboard definitions
//yes and no structure counters and flag
#define LINESIZE 80//just needs to be under 255  
//"count" organized into an array for easy iteration 
//and function passing
byte count[7]={
  0,0,0,0,0,0,0};
//count KEY, to make addressing the array readable
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
#define BUFFSIZE 14
char letterBuffer[BUFFSIZE]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//holds  letters for autofill functions
char buffer[15];//global for wordlist
//large enough for the largest string it must hold
byte sugSize=0;//defines the size of the suggestion
//--condition chords for automated responses
boolean rJustify=false;
boolean cJustify=false;
boolean explicitMode=false;
#define YES 60//first byte address in EEPROM
#define NO 62//first byte address in EEPROM
//error correction and alternate assignments
#define ONSECOND 254 //location in EEPROM 254
#define DONELEARNING 255 //location in EEPROM 255
//3 steps Aquiring first layout F/F, second layout T/F, DONELEARNING T/T
#define LETTERSLEARNED 256 //location in EEPROM
// the modifier key to the second assignment
#define SECONDLAY 96
// 96 defines the amount of offset from the first assignment in eeprom
//modifiers are assign as variables to pass to functions
//!!Keyboard object specific 
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
//prog_char HOME = KEY_HOME;
//prog_char END = KEY_END;
//prog_char f2 = KEY_F2;
//prog_char f4 = KEY_F4;
//prog_char f5 = KEY_F5;
//prog_char f10 = KEY_F10;
//prog_char f12 = KEY_F12;
prog_char tab = KEY_TAB;
prog_char RTN = KEY_RETURN;
#define BACK 178
#define REACT 300// time for computer host to react to commands

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
    clearPROM(0, 257);//clear possible old assignments
    //wait();
    //paulsMacro();// this opens a text editor for testing, comment this out
    setControls();//promt and assign for yes/no
  }
}
//-----------------------------------------------------------------------------begin main loop
//KEY- following"//:" = file/tab dependency
void loop()
{
  while(true)//to provide a way to start from step one with a return
  {//JesterType defined linesize limiter
    autoLineReturn();//:controlMacros
    //get the current button status 
    word chordValue=getValue();//:valueAssement
    if(chordValue==readChord(YES))
    {// if yes or no do those and restart the loop//:ControlMacros
      yesCase();//:controlMacros
      return;//restart the loop
    }
    else if(chordValue==readChord(NO))
    {
      noCase();//:controlMacros
      count[YESC]=0;//set yes to 0 in order to define specific cases
      return;//restart the loop
    }
    /*else if(chordValue==meta)
     {
     metaCase();
     return;
     }*/
    //-------------------------------letter related steps
    cleanSug();//clear the old suggestion before printing the next letter
    //figure out if the chord is a letter that has an assignment in eeprom
    byte letter=check(chordValue);//:Assignments
    if(letter)// if the letter was assigned
    {//than print it
      printLetter(letter);//:KeyboardFunctions
    }
    else if(EEPROM.read(DONELEARNING))//If the learning is done
    {//but it wasn't in the assignment the put it through the filter
      // !! filter currently finds minimal differance from an assigned chord
      printLetter(filter(chordValue));//:noiseFiltering
    }
    else//learnig still needs to be done
    {
      printLetter(learnUser(chordValue));//:learing
      //print a newly guessed letter
    };
    autoSug();//make a suggestion based on the current letterBuffer
    //Keyboard.print(freeMemory());//test mem usage: last test:2288
  }
}
//--------------------------------------------------------------------------end main loop

