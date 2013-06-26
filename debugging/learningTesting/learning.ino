//learning
#define BUFFSIZE 15//holds  letters for autofill functions
char buffer[BUFFSIZE];//global for wordlist
//large enough for the largest string it must hold
//Planned new learning functions will be base on the diriving commanality from the wordlist
//as opposed to the following frequency array
//common letter frequencies 2d array, data points are the letter's address space in eeprom
prog_char stagedLearning[][13] PROGMEM=
{
  {
    't','o','a','w','c','d','s','f','m','r','h','i','e'                      }
  ,
  {
    'b','y','g','l','n','u','j','k','p','v','q','x','z'                      }
};
//---------------------------------------------****************************************
/*CHALENGE 
 Finding the shorting out letters that have been found is difficult when the learning algorythm
 is unpredictable. How will each letter be recognised as learned? The obvious answer is the layout
 itself but I still scratching my head as to how to parse it without a massive amount of iteration
 evertime a letter needs to be learned.
 *//*
byte iLearn(word chord)
{
  byte suggestion=getSuggestion();
  if(suggestion)//if there is a suggestion base on the wordlist
  {
    return suggestion;
  }
  else
  {//other wise give a letter based on priority
    return prioritizedLearn(chord);
  }
}

byte getSuggestion()
{//return 0 if there is no suggustion
  //discern typed letters
  //look up typed letters
  return sLetter;//if they match return a suggestion of the next letter in the predicted word
}
*/
byte prioritizedLearn(word chord)
{//learn by 
  byte common=EEPROM.read(COMMON);
  byte uncommon=EEPROM.read(UNCOMMON);
  if(common<13 && simpleChord(chord) || uncommon== 13)
  {
    byte letter=pgm_read_byte(&stagedLearning[0][common]);
    if(assign(letter, chord, EEPROM.read(ONSECOND)))
    {
      EEPROM.write(COMMON, common+1);//increment to 12
      learningProgression();
      return letter;
    }
  }
  else if(uncommon<13 || common==13)
  {
    byte letter=pgm_read_byte(&stagedLearning[1][uncommon]);
    if(assign(letter, chord, EEPROM.read(ONSECOND)))
    {
      EEPROM.write(UNCOMMON, uncommon+1);//increment to 12
      learningProgression();
      return letter;
    }
  };
}
void learningProgression()
{//persistently interperts and rigts place in the learing process
    if(EEPROM.read(UNCOMMON)==13 && EEPROM.read(COMMON) ==13)
    {//if the this is the last letter of either case
      EEPROM.write(UNCOMMON, 0);//set i back to zero for the second layout
      EEPROM.write(COMMON, 0);
      if(EEPROM.read(ONSECOND))
      {//if a modifier value reads aka on second layout
        EEPROM.write(DONELEARNING, true);
      }
      else
      {//if its not on the second layout, put it there
        EEPROM.write(ONSECOND, SECONDLAY);//write the second layout offset to the flag
      };
    }
}

boolean simpleChord(word chord)//determines a quick press chord
{//if all digits are less then two return true
  for(byte i=0;i<5;i++)//for each digit in the chord
  {
    if(chord%10>1)
    {//check the chord agaist the incrementor
      return false;
    }
    else
    {//prep next test in the loop
      chord/=10;
    }
  }//if this for loop ends without catching a greater than case.
  return true;
}

word comboIndex[]={//index of wordlist
  0,25,225,1348      }; //reduces iterations
//#########################################Functions involving wordlist
void eraseBuffer(byte scope)
{
  for(byte i=0;i<scope;i++)//for the size of the scope
  {
    buffer[i]=0;//reset buffer to null
  }
}
byte suggestSize()
{
  for(byte scope=0;scope<BUFFSIZE;scope++)
  {//for all the possiblilities in the buffer
    if(!buffer[scope])
    {//if the examined byte in the buffer is null
      return scope; //return the examined possition
    }//in other words the size of the buffer
  }
}

