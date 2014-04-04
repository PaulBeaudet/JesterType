//learning
#define BUFFSIZE 15//holds  letters for autofill functions
char buffer[BUFFSIZE];//global for wordlist
//large enough for the largest string it must hold
//Planned new learning functions will be base on the diriving commanality from the wordlist
//as opposed to the following frequency array
//common letter frequencies 2d array, data points are the letter's address space in eeprom
/*
#define LPLACES 7
 #define FREQ 13
 
 prog_char commonLetters[8][FREQ] PROGMEM=
 //the eighth place accounts for uncommon letters
 {
 {
 't','a','s','h','w','i','o','b','m','f','c','l','d'                                                }
 ,
 {
 'e','h','o','a','n','i','f','r','u','t','l','c','d'                                                }
 ,
 {
 'e','t','a','i','o','n','r','h','s','d','l','c','u'                                                }
 ,
 {
 'e','t','a','o','i','n','s','h','r','d','l','c','u'                                                }
 ,
 {
 'e','r','i','o','t','n','s','a','d','l','h','c','u'                                                }
 ,
 {
 'e','t','a','r','i','n','s','h','o','d','l','c','u'                                                }
 ,
 {
 'e','t','a','d','i','n','s','h','r','o','l','c','u'                                                }
 ,
 {
 'm','f','p','g','w','y','b','v','k','x','j','q','z'                                                }
 };
 
 //------------------------------------------------------------------------learning functions 
 */
prog_char const stagedLearning[][13] PROGMEM=
{
  {
    't','o','a','w','c','d','s','f','m','r','h','i','e'                    }
  ,
  {
    'b','y','g','l','n','u','j','k','p','v','q','x','z'                    }
};

prog_char const staticCommons[26] PROGMEM=
{
  't','o','a','w','c','d','s','f','m','r','h','i','e','b','y','g','l','n','u','j','k','p','v','q','x','z' 
};

//---------------------------------------------****************************************
byte prioritizedLearn(word chord)
{//learn by priority 
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
// Alternitive learning functions
byte learnMemByFreq(word chord)//learning function that accounts for assignment rejection
{
  byte count=EEPROM.read(TRUECOUNT);//varify number of assigned letters
  if(count==26)//if every assignment has been made
  {//proceed to increment to a different learning phase
    if(EEPROM.read(ONSECOND))//if ONSECOND was the current phase
    {//if a modifier value reads aka on second layout
      EEPROM.write(DONELEARNING, true);//mark learning as done
      return filter(chord);//given there are no more letters to learn treat the chord as noise
    }//pass the chord through the noise filter and return it
    else
    {//if its not on the second layout, put it there
      EEPROM.write(ONSECOND, SECONDLAY);//write the second layout offset to the flag
    };
    count=0;
  }
  byte pos=EEPROM.read(RPOSSITION);//read persistent possition number
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
/*
byte learnUser(word chord)//simple sub to the original 
 {//for one assignment
 byte letter=learningSeq(chord, 0);
 if(letter)
 {
 return letter;
 }
 else
 {
 letter=freqLookup(8, chord, 0);
 if(letter)
 {
 return letter;
 }
 }
 EEPROM.write(DONELEARNING, true);
 }
 
 byte learningSeq(word chord, byte modifier)
 {
 byte letter;
 //Give most common unassiged letter based on possition in the word
 if(wordCount<LPLACES)//for the amount of places acounted for
 { //look up the appropriate letter in the 2d array
 letter=freqLookup(wordCount, chord, modifier);
 if (letter==0)// in the case they are taken
 {
 letter=freqLookup(8, chord, modifier);
 }
 return letter;
 }
 else
 {
 letter=freqLookup(2, chord, modifier);
 if (letter==0)
 {
 letter=freqLookup(8, chord, modifier);
 }
 return letter;
 };
 return 0;
 }
 
 byte freqLookup(int place, word chord, byte modifier)
 {
 byte letterNumber;
 for (int freq=0; freq<FREQ; freq++)
 //interate frequencies
 {
 letterNumber= pgm_read_byte(&commonLetters[place][freq]);
 if (assign(letterNumber, chord, modifier))
 //check if the frequency is assigned for the given possition
 {
 return letterNumber;
 //return the availible letter that is most fequently used for this possition
 }
 }
 return 0;
 }*/

