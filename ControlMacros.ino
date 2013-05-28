//ControMacros---controling opporators that imply funtions
//such as space, enter, delete, tab, justify and so on
//!!have been advised that switch cases are probably a bad idea
// because they may me interperted differently my differnt MCUs
//-------------------------------------------YES CASE
void yesCase()
{
  //regular activity 
  switch(count[YESC])
  {
  case 0://yes
    space();
    break;
  case 1://yes, yes
    period();
     break;
  case 2://yes, yes, yes
    enter();
    break;
  default:
    count[YESC]=0;
    return;
  }
  count[YESC]++;
}
//#####################################NO
void noCase()
{
  if(count[YESC])
  {
    switch(count[YESC])
    {
    case 1:
      autoFill();//or after word//yes, no
      //whether autofill or spell correct is used depends
      //on the amount of letters in the buffer
      break;
    case 2:
      punctuation('?');
      break;
    case 3:
      punctuation('!');
      break;
    default:
      count[YESC]=0;//set yes to 0 in order to define specific cases
    }
    count[NOC]++;//add to the no count
  }
  else
  {//in all other cases 
    backSpace();//remove a character
    count[YESC]=0;//set yes to 0 in order to define specific cases
  };
}

