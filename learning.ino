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
prog_char stagedLearning[][13] PROGMEM=
{
  {
    't','o','a','w','c','d','s','f','m','r','h','i','e'                    }
  ,
  {
    'b','y','g','l','n','u','j','k','p','v','q','x','z'                    }
};

prog_char staticCommons[26] PROGMEM=
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

word comboIndex[]={//index of wordlist
  0,25,225,1348      }; //reduces iterations
//#########################################Functions involving wordlist
/*
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
 return freqLookup(0,chord,0);//lookup the most frequent first letter//!!no assignment??
 //will return 0 when first assignment is done
 };
 }//given the word list is exsusted of options
 }*/
//#########################################Auto suggest
void autoSug()
{//makes a suggestion based on typed letters
  if (wordCount && wordCount<4)//1,2,3 !! If word count is zero this is a no go, which is the case after an auto correct!!
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
  backSpaces(wordCount);// delete current printout plus the space ###!! space is included in the word count??
  buffPrint();//print new suggestion
  pressKey(' ');//press a space to start a new word 
  //print suggestion into the letter buffer
  byte j=0;//write the new word into the printed letters buffer
  for(byte i=lineCount-wordCount;i<BUFFSIZE;i++)
  {
    if(buffer[j])
    {
      printedLetters[editLine][i]=buffer[j];
      j++;
    }
    else
    {
      j++;
      printedLetters[editLine][i]=' ';//add the space to the buffer
      break;
    }
  }
  lineCount-=wordCount;//subtract the unused word
  lineCount+=j; // add the suggestion size to the line count
  wordCount=0;
  cleanSug();//remove the previous suggestion for a clean out put
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
  byte scope=suggestSize();//get the size of the value in the buffer
  if(scope)
  {//if there is anything in the buffer 
    movement(scope+1,RIGHT);//move cursor size of suggestion plus space
    backSpaces(scope+1);//remove suggestion + space
    eraseBuffer(scope);//reset wordlist buffer
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
  for(byte scope=0;scope<BUFFSIZE;scope++)
  {//for all the possiblilities in the buffer
    if(!buffer[scope])
    {//if the examined byte in the buffer is null
      return scope; //return the examined possition
    }//in other words the size of the buffer
  }
}




