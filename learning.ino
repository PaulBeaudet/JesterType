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
  if(count[CWORD]<LPLACES)//for the amount of places acounted for
  { //look up the appropriate letter in the 2d array
    letter=freqLookup(count[CWORD], chord, modifier);
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
}


word oneCheck(byte letter, byte mod)
{//checks if the letter has been assigned in eeprom
  byte pHold= letter-mod;
  return word(EEPROM.read(pHold*2), EEPROM.read(pHold*2+1));
  //!!maybe this should be modified to check based on assignment phase
  //thus eliminating the modification argument
}

word comboIndex[]={//index of wordlist
  0,25,225,1348      }; //reduces iterations

//#########################################Functions involving wordlist
byte likelyLetter(word chord)//#################!! future addition
{//suggest a letter based on the common word list
  for(word i=comboIndex[count[CWORD]];i<comboIndex[count[CWORD]+1];i++)
  {//for everthing in the appropriate part of the list
    //strcpy_P(buffer, (char*)pgm_read_word(&(oneCombos[i])));
    findWord(i);
    //read the string to the buffer
    if(count[CWORD])
    {//if letters have been printed 
    //eg count[CWORD]=1 then the first letter is in letter buffer
      for(byte j=0;j<count[CWORD];j++)
      {//for the letters in buffer
        if(buffer[j]==letterBuffer[j] || letterBuffer[j]+32==buffer[j])
        {//if they match the typed letters, lowercase or capital
          if(count[CWORD]==j+1 && assign(buffer[count[CWORD]],chord,0))
          {//and they all match by the last letter to check//
            return buffer[count[CWORD]];
            //basically if we are on count 1 it returns the second letter
           //position one in the buffer array 
          } 
        }
        else// if the letters dont match
        {//short curcuit to the next word in list
          break;
        };
      }
    }
    else//given that this is the first letter to guess
    {//short curcuit by returning the most common unnassigned first letter
      return freqLookup(0,chord,0);//lookup the most frequent first letter
      //will return 0 when first assignment is done
    };
  }//given the word list is exsusted of options
}
//#########################################Auto suggest
void autoSug()
{//makes a suggestion based on typed letters
  if (count[CWORD] && count[CWORD]<4)//1,2,3
  {//given the typed letters meet the list capibilities 
    for(word i=comboIndex[count[CWORD]-1];i<comboIndex[count[CWORD]];i++)
    {//for everthing in the list
      //strcpy_P(buffer, (char*)pgm_read_word(&(oneCombos[i])));
      findWord(i);
      //read the called string number into the buffer
      for(word l=0;l<count[CWORD];l++)
        //for the letters currently typed
      {
        if(letterBuffer[l]==buffer[l] || letterBuffer[l]+32==buffer[l])
        {//If the buffers lower or upper case match 
          if(l==count[CWORD]-1)//if this is the last comparable letter
          {
            //make the suggestion
            Keyboard.print(" ");
            sugSize=1;
            if(letterBuffer[0]<91)
            {//if a capital was printed
              buffer[0]-=32;//uppercase the buffer
            }//to match printed output
            buffPrint();
            pressKey(sugSize, left);
            return;//stop looking for suggestions
          }
        }
        else{//no match its not this word
          break;//short-curcuit        
        };
      }
      //no suggestion case here;
      sugSize=0;
    }
  }
}
void autoFill()
{
  pressKey(sugSize, right);// move the currsor right
  pressKey(sugSize+count[LWORD],BACK);// delete previous suggestion
  count[LINEC]+=sugSize; // add the suggestion size to the line count
  count[LINEC]-=count[LWORD];//subtract the unused word
  count[CSENT]+=sugSize; //add the suggestion to current sentance count
  count[CSENT]-=count[LWORD];//subtract the unused word
  count[LWORD]=sugSize;//the suggestion is now the size of the last word
  buffPrint();//print the word
  //count[YESC]++;//increse the yes count as it would be when word is complete.
  sugSize=0;//reset suggestion size so that clearSug() 
}//doest remove correction previous next loop

void buffPrint()
{
  for(byte i=0;i<BUFFSIZE;i++)
  {
    if(buffer[i])
    {
      Keyboard.write(buffer[i]);//write the letter
      sugSize++;//compile the size of the suggestion
    }
    else
    {//when the last character is reached
      Keyboard.write(' ');
      sugSize++;
      return;
    };
  }
}

void cleanSug()//Clear suggestion
{
  buffer[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];//reset buffer
  pressKey(sugSize,right);//move cursor
  pressKey(sugSize,BACK);//remove suggestion
  sugSize=0;//set its size to zero !!unessisary??
}
