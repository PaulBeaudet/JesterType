char printedLetters[2][80]={
  {
    'I',' ','a','m',' ','w','r','i','t','i','n','g',' ','a',' ','s','e','n','t','e','n','c','e','.',' ','S','e','e',' ','s','e','n','t','e','n','c','e',' ','r','u','n','.'
  }
  ,{
  }
};//buffer for remembering printed letters

boolean editLine=0;//keeps track of which printed letter line is being written
byte lineCount=0;//keeps track of possition on the line
byte wordCount=0;//keeps track of size of working word
void setup()
{
  Serial.begin(9600);
  for(int i=0;i<44;i++)
  {
    Serial.print(printedLetters[editLine][i]);  
  }
  Serial.println();
  for(int i=0;i<44;i++)
  {
    test(i);
    if(i==5 || i==10 || i==15 || i==20 || i==25 || i==30 || i==35 || i==40)
    {
      Serial.println();
    }    
  }
  Serial.println();
  Serial.println(F("decerning the size of past words"));
  byte wp=1;
  boolean flag=false;
  for(int i=44;i>=0;i--)
  {
    if(flag)
    {
      test(i);
      Serial.print("for word ");
      Serial.println(wp);
      wp++;
      flag=false;
    }   
    if(printedLetters[editLine][i]==' ')
    {
      flag=true;
    } 
  }
}

void loop()
{
}

void test(byte readPosition)
{
  lineCount=readPosition;
  lastWordCount();
  Serial.print("wrd-");
  Serial.print(wordCount);
  Serial.print(" /psn-");
  Serial.print(readPosition);
  Serial.print(", ");
  wordCount=0;
}


void lastWordCount()
{//recursively add to the word count based on existing letters
  byte pl=printedLetters[editLine][lineCount-wordCount];//!!need a lineCount 0 exception
  if(lineCount-wordCount==0 && pl > 64 && pl < 123)
  {//as long as its greater then cap "A" and less then "z"
    wordCount++;
    return;
  }
  else if(pl==' ')
  {//base case
    return;
  };
  wordCount++;
  lastWordCount();
}


