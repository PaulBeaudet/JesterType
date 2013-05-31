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
}

word comboIndex[]={//index of wordlist
  0,25,225,1348      }; //reduces iterations
//#########################################Functions involving wordlist
byte likelyLetter(word chord)//#################!! future addition
{//suggest a letter based on the common word list
  for(word i=comboIndex[wordCount];i<comboIndex[wordCount+1];i++)
  {//for everthing in the appropriate part of the list
    findWord(i);//read the string to the buffer
    if(wordCount)
    {//if letters have been printed 
      //eg wordCount=1 then the first letter is in letter buffer
      byte k=0;//buffer edit possition
      for(byte plPlace=lineCount-wordCount;plPlace<lineCount;plPlace++)
      {//for the letters in buffer
        if(buffer[k]==printedLetters[editLine][plPlace] || buffer[k]-32==printedLetters[editLine][plPlace])
        {//if they match the typed letters, lowercase or capital
          if(lineCount==plPlace-1 && assign(buffer[k+1],chord,0))//if this is the last comparable letter
          {//and that letter affter that is assignable than go with that letter
            return buffer[k+1];//return the letter assigned to print
            //basically if we are on count 1 it returns the second letter
            //position one in the buffer array 
          } 
        }
        else// if the letters dont match
        {//short curcuit to the next word in list
          break;
        };
        k++;//increment the buffer edit possition
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
  if (wordCount && wordCount<4)//1,2,3
  {//given the typed letters meet the list capibilities 
    for(word i=comboIndex[wordCount-1];i<comboIndex[wordCount];i++)
    {//for everthing in the list
      findWord(i);//read the called string number into the buffer
      byte b=0;//buffer index place holder
      for(byte plPlace=lineCount-wordCount;plPlace<lineCount;plPlace++)//for the letters currently typed
      {//plPlace stand for printed letter place
        if(buffer[b]==printedLetters[editLine][plPlace] || buffer[b]-32==printedLetters[editLine][plPlace])
        {//If the two buffers lower or upper case match 
          if(plPlace==lineCount-1)//if this is the last comparable letter
          {//make the suggestion
            pressKey(' ');//print a space to seperate the suggestion from the edit area
            if(printedLetters[editLine][lineCount-wordCount]<91)
            {//if a capital pre-existed
              buffer[0]-=32;//uppercase the buffer to match what was intended
            }//to match printed output
            buffPrint();//print the suggestion
            movement(suggestSize()+1, LEFT);//move the cursor back to the edit possition
            return;//stop looking for suggestions
          }
        }
        else{//no match its not this word
          break;//short-curcuit        
        };
        b++;
      }    
    }
  }
  eraseBuffer(suggestSize());
}
void autoFill()
{
  backSpaces(wordCount+1);// delete current printout plus the space
  buffPrint();//print new suggestion
  pressKey(' ');//press a space to start a new word 
  wordStart=true;
  //print suggestion into the letter buffer
  byte j=0;
  for(byte i=lineCount-wordCount+1;i<BUFFSIZE;i++)
  {
    if(buffer[j])
    {
      printedLetters[editLine][i];
      j++;
    }
    else
    {
      printedLetters[editLine][i]=' ';//add the space to the buffer
      break;
    }
  }
  lineCount-=wordCount+1;//subtract the unused word
  lineCount+=j; // add the suggestion size to the line count
  cleanSug();//remove the previous suggestion 
  //clean suggestion only normally happens with letter printing
}

void buffPrint()
{
  for(byte i=0;i<BUFFSIZE;i++)
  {
    if(buffer[i])
    {//if there is a letter in the buffer
      pressKey(buffer[i]);//write the letter
    }
    else
    {//when the last character is reached
      return;
    };
  }
}
void cleanSug()//Clear suggestion
{
  byte scope=suggestSize();
  if(scope)
  { 
    movement(scope+1,RIGHT);//move cursor size of suggestion plus space
    backSpaces(scope+1);//remove suggestion + space
    eraseBuffer(scope);//reset buffer
  }
}
void eraseBuffer(byte scope)
{
  for(byte i=0;i<scope;i++)//for the size of the scope
  {
    buffer[i]=0;//reset buffer to null
  }
}
byte suggestSize()
{
  byte scope=0;
  for(byte i=0;i<BUFFSIZE;i++)
  {
    if(buffer[i])
    {
      scope++; 
    }
    else
    {
      return scope;
    }
  }
}



