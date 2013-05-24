//------------------------------------------noise filtering
byte filter(word noise)
{
  word correctToValue=9;//redundant?
  //cant be nine so if it prints this something is wrong
  int lowPoint=15;
  for(int address=194;address<246;address+=2)
  {//compare everything in the first assignment
    word largerNum;
    word compare=word(EEPROM.read(address), EEPROM.read(address+1));
    word sComp=compare; //temporary second comparison
    word fComp=noise; //temp first comparison
    int pointCompare=0;
    if(fComp>sComp)
    {
      largerNum=fComp;
    }
    else
    {
      largerNum=sComp;
    };
    unsigned long mag= 1;
    while (mag*10<=largerNum)
    {
      mag*=10;
    }
    while(mag>0)
    {
      if(fComp>sComp)
        //overflow prevention
      {
        pointCompare+= fComp/mag - sComp/mag;
      }
      else
      {
        pointCompare+= sComp/mag-fComp/mag;
      }
      fComp%=mag;
      sComp%=mag;
      mag/=10;
    }
    if(pointCompare<lowPoint)
      //filters to the path of least resistence 
    {
      lowPoint=pointCompare;
      //set a new lowpoint
      correctToValue=compare;
      //remember the lowpoint value
    }
  }
  return check(correctToValue);
}

