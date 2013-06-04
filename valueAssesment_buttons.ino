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
#define NOTICABLELAPSE 400//ms, human discernable time differance in button press durration
#define BASELAPSE 50//ms, quick press distinction
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
  {//set up the buttons 
    pinMode(buttons[set], INPUT);
    digitalWrite(buttons[set], HIGH);//this sets pull-up resistor/ie input through 20k to 5v
  }//in this way| input to button/button to ground, is proper and will read as low when pressed
}
//--------------------------------------returning the chord value
//return the raw value, monitor the buttons in a time window
word getValue()
{
  word builtChord=0;//unique chord value to be created
  word baseChord=RESTING;//assumed inactivity when called
  unsigned long past=millis()-1;//first millis diff is engineered here to make sure things get on the ball
  int windowCount=0;//time window starts at zero
  int noticablElapse=0;//incrementing the sample point in the time window

  while(windowCount<=WINDOW || getDebouncedState()>RESTING)//gather chordValue with in this time frame
  {//while window has elapsed or input is still active
    unsigned long currentTime=millis();//hold current time
    baseChord=getDebouncedState();//gather chord
    if(baseChord > RESTING)//when active or different || baseChord!=oldChord
    {
      windowCount+=(currentTime-past);//add increment
      if(windowCount > BASELAPSE+noticablElapse)
      {//test the window against the elapsed time, which stagers the sampling
        builtChord+=baseChord-RESTING;//add one raw value to the base
        noticablElapse+=NOTICABLELAPSE;//increment the elapse time
      }
    }
    past=currentTime;//set the past time
  } 
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
  {// one of the inputs has changed state
    lastChangeTime = millis();
    oldState = newState;
  }
  else
  {// the inputs have not changed
    if((oldState != debouncedState) && (millis() - lastChangeTime > BOUNCE))
    {// the inputs are stable and in a new state
      debouncedState = newState;
    }
  }
  return debouncedState;
}

// Read the values of all the buttons and represent them as an int
int rawInput()//could be converted to word return
{
  int incrementor=1;//changes digit edit possition
  int value=RESTING;//2 will represent high as zero may prove problematic for low
  for(int i=0;i<NUMBUTTONS;i++)
  {//for every button
    if(digitalRead(buttons[i])==LOW)
    {//check if the button is low/pressed
      value+=incrementor;//build an signal variable for all inputs
      incrementor*=10;//each button is represented by a seperate place value
    } 
    else//maybe the button wasn't press, but another might have been
    {//given state has not changed incrementor still needs to progress
      incrementor*=10;
    }
  }
  return value;
}
//extra button functions
void wait()//hold till button promt
{//as long as button 0 has yet to be pressed
  while(digitalRead(buttons[0])==HIGH)
  {// waits for input to prompt yes and no
    ;//nothingness
  }
}
