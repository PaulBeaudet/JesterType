//----------------------------storage fuctions---making assignments

boolean assign(byte letter, word chord, byte modifier)
{//does the assignment and returns true
  if(letter)
  {
    byte hold = letter-modifier;//offset the reading if using second layout
    if(readChord(hold*2))//multiply letter by two to find address space
    {//check if letter has been assigned
      return false;
    } //if so short-curcuit 
    else
    { //otherwise write word into EEPROM
      EEPROM.write(hold*2, highByte(chord));
      delay(5);//!! delays after writes maybe redundent
      EEPROM.write(hold*2+1, lowByte(chord));
      delay(5);
      return true;
    }
  }
  else//if a zero was passed to assign
  {
    return false;
  };//so the function knows it could not be passed
}

void unassign()
{
  byte letter=EEPROM.read(LASTLETTER);//gather last guessed letter
  if(letter)//only if there is a letter in the assignment
  {//basically this is necissary when testing on every backspace
    byte address= letter-EEPROM.read(ONSECOND);
    EEPROM.write(address*2, 0);//overwrite first half of word
    EEPROM.write(address*2+1, 0);//overwrite second half of word
    byte count=EEPROM.read(TRUECOUNT);
    EEPROM.write(TRUECOUNT,count-1);
    EEPROM.write(LASTLETTER,0);//reset last letter to prevent subsequent unassigns
  }
}

//checking by frequency
byte checkByFreq(word chordValue)
{
  for(int layoutOffset=0;layoutOffset<SECONDLAY+1;layoutOffset+SECONDLAY)
  {//for each assignment
    for(int letter=0;letter<26;letter++)//for every letter
    {
      byte address = pgm_read_byte(&staticCommons[letter])-layoutOffset;
      if(readChord(address*2) == chordValue)
      { //check there is something the equals the current chord
        return staticCommons[address];
      }
    }
  }
  return 0;//if nothing was found return 0 signifing no chords found
}
//Checking
byte check(word chordValue)
{
  for(int address=FIRSTASSIGNMENTBEGIN;address<FIRSTASSIGNMENTEND;address+=2)
    //for the first layout 194-246
  {
    //check there is something the equals the current chord
    if(readChord(address) == chordValue)
    {
      return address/2;
      //translates to ascii number for the represented letter
      //first layout defined by a mutipul of 2
      // because ints need to be seperated into 2 bytes
    }
  }
  //continue given no matches are found
  for(int address=SECONDABEGIN;address<SECONDAEND; address+=2)
    //for the second layout 2-56
  {
    if(readChord(address) == chordValue)
    {
      return address/2+SECONDLAY;
      //translates to ascii number for the represented letter
      //second layout defined by an offset
    }
  }
  //no assignments made for given cord, return 0 or false
  return 0;
}

word readChord(int address)
{
  return word(EEPROM.read(address), EEPROM.read(address+1));
}
//---------------------------------------SessionFunctions
//-----------clear EEPROM
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


