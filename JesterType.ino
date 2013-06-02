#include<avr/pgmspace.h>//explicitly stated read only memory
#include <EEPROM.h>//persistent write-able in runtime memory
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
 
 Build for the Arduino Micro, Leonardo or Usb Lilypad (ATMEGA32u4) w/KeyboardFunction.ino
 Build for the Arduino uno, mega ect w/serialFunction.ino(work in progress)
 
 ######### CURCUIT DISCRIPTION ################################################
 See valueAssement_buttons.ino for specific discription of button prototype which is 
 the current default 
 Other sensor arrangements will be used in the future 
 #$$$$$$$$$$$###################################################################*/
#define KEY 6//persistent session key, change to start over
//$$$$$$$$$$$(if forgot yes/no assignment or are testing the learning process)
//----------------------------------------------Defined EEPROM addresses
#define YES 60//first byte address in EEPROM
#define NO 62//first byte address in EEPROM
#define ONSECOND 254 //location in EEPROM 254//error correction and alternate assignments
#define DONELEARNING 255 //location in EEPROM 255
#define LETTERSLEARNED 256 //location in EEPROM
//3 steps Aquiring first layout F/F, second layout T/F, DONELEARNING T/T
// the modifier key to the second assignment
#define SECONDLAY 96// 96 defines the amount of offset from the first assignment in eeprom
#define REACT 300// time for computer host to react to commands
#define LEFT 1//movement() bearing key
#define RIGHT 0
//-----------------------------------------------------------------------Set up
void setup()
{
  outputUp();//comment out KeyboardFunction.ino or serialFunction.ino to switch output
  inputUp();//set up input (set pins for buttons) 
  if (session(512, KEY))//check if this arduino's eeprom has been "marked" w/ the key
  {// if Key is non-existent clear the eeprom starting the learning process againt
    clearPROM(0, 257);//clear possible old assignments//:SessionFunctions
    setControls();//learning starts w/ a promt to assign for yes/no
  }
}
//-----------------------------------------------------------------------------begin main loop
//KEY- following"//:" = file/tab dependency
void loop()
{
  while(true)//to provide a way to start from step one with a return
  {
    autoLineReturn();//:controlMacros//JesterType defined linesize limiter
    word chordValue=getValue();//:valueAssement//get the current button status 
    if(chordValue==readChord(YES))
    {// if yes or no do those and restart the loop//:ControlMacros
      yesCase();//:controlMacros
      return;//restart the loop
    }
    else if(chordValue==readChord(NO))
    {
      noCase();//:controlMacros
      return;//restart the loop
    }
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
