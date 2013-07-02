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

//Checking

byte check(word chordValue)
{
  for(int layoutOffset=0;layoutOffset<SECONDLAY+1;layoutOffset+=SECONDLAY)
  {//for each assignment
    for(int i=97;i<123;i++)
    {//for every letter
      byte address= i-layoutOffset;//define address area
      if(readChord(address*2) == chordValue)
      {//if that address matches the current chord
        return address;//return the assosiated letter
      }
    }
  }//total of 52 iterations if not found
  return 0;
};

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
