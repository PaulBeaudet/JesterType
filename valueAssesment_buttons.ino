//--------------------------------------------------------button functions
#define WINDOW 125 // time window in which chords are recorded
#define BOUNCE 30// time to debounce
#define RESTING 11111 // this is the resting chord value
//resting need to be changed to reflected the number of buttons

//--------------------------------------returning the chord value
//return the raw value, monitor the buttons in a time window
unsigned int getValue()
{
  boolean basePress=true;
  boolean regularPress=true;
  boolean longPress=true;
  unsigned int builtChord=0;
  unsigned int oldChord=RESTING;
  unsigned int baseChord=RESTING;//assumed inactivity when called
  unsigned long past=millis()-1;//first millis diff is engineered here to make sure things get on the ball
  int windowCount=0;
  while(windowCount<=WINDOW || getDebouncedState()>RESTING)//gather chordValue with in this time frame
  {
    unsigned long currentTime=millis();
    if(baseChord!=RESTING || windowCount>BOUNCE)// activity detection, start timer
    {
      unsigned long diff=currentTime-past;
      windowCount+=diff;
    }
    //--gather chord 
    baseChord=getDebouncedState();
    if(baseChord > RESTING || baseChord!=oldChord)// in short, when active and different
    {
      oldChord=baseChord;
      if(basePress && windowCount > 50)
      {
        builtChord+=baseChord-RESTING;//this will allow for a base of 15 values
        basePress=false;
      }
      if(regularPress && windowCount > 400)
      {
        unsigned int temp=baseChord-RESTING;
        builtChord+=temp*2;
        regularPress=false;
      }
      if(longPress && windowCount > 750)
      {
        unsigned int temp=baseChord-RESTING;
        builtChord+=temp*3;
        longPress=false;
      }

    }
    //--set the past time
    past=currentTime;
  } 
  //Serial.println(windowCount);
  return builtChord;
}
//---------------------------------------------------------debouncing
// debounce the raw chord values derived from rawInput();
int getDebouncedState()
{
  static int oldState = 0;
  static int debouncedState = 0;
  static unsigned long lastChangeTime = millis();

  int newState = rawInput();
  if(newState != oldState)
  {
    // one of the inputs has changed state
    lastChangeTime = millis();
    oldState = newState;
  }
  else
  {
    // the inputs have not changed
    if((oldState != debouncedState) && (millis() - lastChangeTime > BOUNCE))
    {
      // the inputs are stable and in a new state
      debouncedState = newState;
    }
  }
  return debouncedState;
}

// Read the values of all the buttons and represent them as an int
int rawInput()
{
  int incrementor=1;
  int value=RESTING;//2 will represent high as zero may prove problematic for low
  for(int i=0;i<NUMBUTTONS;i++)
  {
    if(digitalRead(buttons[i])==LOW)
    {
      //build an signal variable for all inputs
      value+=incrementor;
      //each button is represented by a seperate place value
      incrementor*=10;
    } 
    else
    {
      //given state has not changed incrementor still needs to progress
      incrementor*=10;
    }
  }
  return value;
}

