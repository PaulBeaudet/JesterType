
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

