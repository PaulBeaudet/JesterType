/*valueAssesment_buttons
 ######### CURCUIT DISCRIPTION ################################################
 Five momentary switches are physically wired in the following fashion starting from 
 the assigned pins (in the current case 2-6) for each pin.
 From pin to switch--switch to 330om resistor--resistor to ground
 //resistor is a safety for prototyping, not needed in purpose built application
 Thats it! Very simple for this prototype. Note that Internal pull-ups are used.
 ##############################################################################*/
#define WINDOW 125 // time window in which chords are recorded
#define BOUNCE 30// time to debounce
#define RESTING 11111 // this is the resting chord value
//resting need to be changed to reflected the number of buttons
//-----------------------------------------------------------------define buttons
byte buttons[]=
{
  2,3,4,5,6
};
#define NUMBUTTONS sizeof(buttons)
void inputUp()
{ 
  for (byte set=0;set<NUMBUTTONS;set++)
  {
    //sets the button pins
    pinMode(buttons[set], INPUT);
    digitalWrite(buttons[set], HIGH); 
  }  
  //this sets pull-up resistor/ie input through 20k to 5v
  //in this way| input to button/button to ground, is proper and will read as low when pressed
}
//--------------------------------------returning the chord value
//return the raw value, monitor the buttons in a time window
word getValue()
{
  boolean basePress=true;
  boolean regularPress=true;
  boolean longPress=true;
  word builtChord=0;
  word oldChord=RESTING;
  word baseChord=RESTING;//assumed inactivity when called
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
        word temp=baseChord-RESTING;
        builtChord+=temp*2;
        regularPress=false;
      }
      if(longPress && windowCount > 750)
      {
        word temp=baseChord-RESTING;
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

//extra button functions
void wait()//hold till button promt
{
  while(digitalRead(buttons[0])==HIGH)
  {
    // waits for input to prompt yes and no
    ;
  }
}

