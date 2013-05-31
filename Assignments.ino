//----------------------------storage fuctions---making assignments

boolean assign(byte letter, word chord, byte modifier)
{//does the assignment and returns true
  if(letter)
  {
    byte hold = letter-modifier;
    if(word(EEPROM.read(hold*2), EEPROM.read(hold*2+1)))
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
  };//so the function know it could not be passed

}

//Checking
byte check(word chordValue)
{
  for(int address=194;address<246;address+=2)
    //for the first layout
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
  for(int address=2;address<56; address+=2)
    //for the second layout
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




