//learning
//Planned new learning functions will be base on the diriving commanality from the wordlist
//as opposed to the following frequency array
//common letter frequencies 2d array, data points are the letter's address space in eeprom
#define LPLACES 7
#define FREQ 13

prog_char commonLetters[8][FREQ] PROGMEM=
//the eighth place accounts for uncommon letters
{
  {
    't','a','s','h','w','i','o','b','m','f','c','l','d'                                          }
  ,
  {
    'e','h','o','a','n','i','f','r','u','t','l','c','d'                                          }
  ,
  {
    'e','t','a','i','o','n','r','h','s','d','l','c','u'                                          }
  ,
  {
    'e','t','a','o','i','n','s','h','r','d','l','c','u'                                          }
  ,
  {
    'e','r','i','o','t','n','s','a','d','l','h','c','u'                                          }
  ,
  {
    'e','t','a','r','i','n','s','h','o','d','l','c','u'                                          }
  ,
  {
    'e','t','a','d','i','n','s','h','r','o','l','c','u'                                          }
  ,
  {
    'm','f','p','g','w','y','b','v','k','x','j','q','z'                                          }
};

//------------------------------------------------------------------------learning functions 
//commit 1b3666ec7428a21a0a0f3eb8e5620b1c7ee8dfb3 has commited out previous learner

byte learnUser()//simple sub to the original 
{//for one assignment
  byte letter=learningSeq(0);
  if(letter)
  {
    return letter;
  }
  else
  {
    letter=freqLookup(8,0);
    if(letter)
    {
      return letter;
    }
  }
  learningPhase[1]=true;
  EEPROM.write(255, true);
}

byte learningSeq(byte modifier)
{
  byte letter;
  //Give most common unassiged letter based on possition in the word
  if(count[CWORD]<LPLACES)//for the amount of places acounted for
  { //look up the appropriate letter in the 2d array
    letter=freqLookup(count[CWORD], modifier);
    if (letter==0)// in the case they are taken
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


word oneCheck(byte letter, byte mod)
{//checks if the letter has been assigned in eeprom
  byte pHold= letter-mod;
  return word(EEPROM.read(pHold*2), EEPROM.read(pHold*2+1));
  //!!maybe this should be modified to check based on assignment phase
  //thus eliminating the modification argument
}

