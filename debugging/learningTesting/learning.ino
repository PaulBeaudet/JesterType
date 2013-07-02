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
    't','o','a','w','c','d','s','f','m','r','h','i','e'                                }
  ,
  {
    'b','y','g','l','n','u','j','k','p','v','q','x','z'                                }
};
//---------------------------------------------****************************************
/*CHALENGE 
 Finding the shorting out letters that have been found is difficult when the learning algorythm
 is unpredictable. How will each letter be recognised as learned? The obvious answer is the layout
 itself but I still scratching my head as to how to parse it without a massive amount of iteration
 evertime a letter needs to be learned.
 *//*
byte iLearn(word chord)//suggestion based learner !!draft
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
byte rLearn(word chord)
{
  byte letter;
  if(EEPROM.read(REBUKE) && EEPROM.read(RPOSSITION))//flag to time to rebuke, and at least on rebuked letter exist
  {//assign rebuked letters
    letter=rAvailLearn(chord);
    if(letter)
    {
      return letter;
    }
    else
    {
      EEPROM.write(REBUKE, false);
    };
  }
  else
  {
    letter=prioritizedLearn(chord);
    if(letter)
    {
      return letter;
    }
  };
}

byte availablityLearn(word chord)
{//check for rebuked letter to be learned
  byte letter=EEPROM.read(RPOSSITION);//gather current read possition
  byte firstGuess=letter;
  while(!assign(letter,chord,EEPROM.read(ONSECOND)))
  {
    if(letter==122)//if we are at z
    {//some where an infinacy test needs to be done
      letter=97;//set back to a
    }
    else
    {
      letter++;
    }
    if(letter==firstGuess)//escape case everything has been checked
    {
      EEPROM.write(RPOSSITION, 0);
    }
  }
  EEPROM.write(RPOSSITION, letter+1);
  return letter;
}

byte rAvailLearn(word chord)//recursive version of availible learn
{
  static byte rletter=EEPROM.read(RPOSSITION);//gather current read possition
  static byte firstGuess=rletter;
  if(assign(rletter,chord,EEPROM.read(ONSECOND)))//primary base case
  {
    rletter++;
    EEPROM.write(RPOSSITION,rletter);
    return rletter;
  }
  if(rletter==122)//if we are at 'z'
  {
    rletter=97;//set back to 'a'
  }
  else
  {
    rletter++;//increment the letter to test next
  }
  if(rletter==firstGuess)//alternitive base case given everything has been checked
  {
    EEPROM.write(RPOSSITION, 0);
    return 0;//hey couldn't find anything that is availible!
  }
  rAvailLearn(chord);
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
byte learnMemByFreq(word chord)//learning function that accounts for assignment rejection
{
  byte count=EEPROM.read(TRUECOUNT);//varify number of assigned letters
  if(count==26)//if every assignment has been made
  {//proceed to increment to a different learning phase
    if(EEPROM.read(ONSECOND))//if ONSECOND was the current phase
    {//if a modifier value reads aka on second layout
      EEPROM.write(DONELEARNING, true);//mark learning as done
    }
    else
    {//if its not on the second layout, put it there
      EEPROM.write(ONSECOND, SECONDLAY);//write the second layout offset to the flag
    };
    count=0;
  }/*
  for(byte pos=EEPROM.read(RPOSSITION);pos<26;pos++)//for every letter
  {
    byte letter = pgm_read_byte(&staticCommons[pos]);
    if(assign(letter,chord,EEPROM.read(ONSECOND)))//!!what if this is false?
    { //check for vacancy, if an assignment is made true is returned
      EEPROM.write(TRUECOUNT, count+1);//increment the count of assigned letters//decressed with unassign()
      EEPROM.write(RPOSSITION, posTicker(pos));//cycle the read possition
      EEPROM.write(LASTLETTER, letter);//earmark the last letter that was printed
      return letter;//return the letter to be printed
    }
  }*/
  byte pos=EEPROM.read(RPOSSITION);
  while(true)//dont stop untill a letter is found
  {//in the case that all of the letters are found this part of the function will have been shorted
    byte letter = pgm_read_byte(&staticCommons[pos]);
    if(assign(letter,chord,EEPROM.read(ONSECOND)))//if false moves on to next possition
    { //check for vacancy, if an assignment is made true is returned
      EEPROM.write(TRUECOUNT, count+1);//increment the count of assigned letters//decressed with unassign()
      EEPROM.write(RPOSSITION, posTicker(pos));//cycle the read possition
      EEPROM.write(LASTLETTER, letter);//earmark the letter being returned in order to be able to unassign it
      return letter;//return the letter to be printed
    }
    pos=posTicker(pos);//continue to increment 0 to 26 over and over
  }
}



byte posTicker(byte pos)//abstracted position cycling
{  
  pos=pos+1;//increment the possition number to avoid reguessing the same number
  if(pos==26)
  {//if the possition is now past 'z' set it back to 'a'
    pos=0;//reset the possition number to zero or 'a'
  }
  return pos;
}
//-------------#################################################################### Existing
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
//--------------------
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

//!! proposed
boolean lettersBuffered()//return true if there is stuff in the buffer
{
  for(int i=STARTLEARNING;i<LASTLEARN;i++)//cycle through the letter buffered in eeprom
  {
    if(EEPROM.read(i))
    {
      return true;
    }//trip flag giving something is in the buffer
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
//------------------------------------------------------------------------------

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






