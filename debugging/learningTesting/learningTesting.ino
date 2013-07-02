//testing for a version of the letters learner suggest letter based on predicted word
//!!not working yet!! testing proof of concept
#include <EEPROM.h>

int led = 13;

//EEPROM assignment guide
#define SECONDABEGIN 2//second layout of lowercase letters
#define SECONDAEND 56
#define YES 60//yes chord storage
#define NO 62//no chord storage
#define FIRSTASSIGNMENTBEGIN 194//divided by two equals ascii 'a'
#define FIRSTASSIGNMENTEND 246//to ascii 'z'
#define TRUECOUNT 248//count of learned letters in the proccess of the learning phase
#define REBUKE 249//boolean flag for rebuke phase
#define RPOSSITION 250//read possition for the learning process
#define LASTLETTER 251//last letter that was assigned
#define COMMON 252 //counts letters learned
#define UNCOMMON 253 //counts letters learned
#define ONSECOND 254 //location in EEPROM 254//error correction and alternate assignments
#define DONELEARNING 255 //location in EEPROM 255
#define STARTLEARNING 256
#define LASTLEARN 283//<end of addres space, doubles as place holder possition address
//3 steps Aquiring first layout F/F, second layout T/F, DONELEARNING T/T
// the modifier key to the second assignment
#define SECONDLAY 96// 96 defines the amount of offset from the first assignment in eeprom

#define NUBOFCHORDS 36
#define NO 11111
word testChords[NUBOFCHORDS]=
{
  1,2,3,10,20,NO,30,100,200,300,NO,300,1000,2000,3000,10000,20000,30000,NO,22222,33333,11000,11100,11110,20,30000,300,1,1,1,1,1,1,NO,1,1
};//list of chords to test, simulating interaction


prog_char staticCommons[26] PROGMEM=
{
  't','o','a','w','c','d','s','f','m','r','h','i','e','b','y','g','l','n','u','j','k','p','v','q','x','z' 
};

void setup()
{
  delay(3000);//give enough time to open the serial monitor, as to not clear eeprom twice
  //?? is there an explicit way to do this?
  Serial.begin(9600);
  clearPROM(2,256);//clear the assignment space
  Serial.print(F("# of chords to be tested="));
  Serial.println(NUBOFCHORDS);
  for (int i=0;i<NUBOFCHORDS;i++)
  {//for all the chords in the test chords list
    if(testChords[i]==NO)
    {
      Serial.print("backspace");
      unassign();
    }
    else
    {
      Serial.print(testChords[i]);//print the chord number
      Serial.print(" is now ");// and...
      char letter = char(learnMemByFreq(testChords[i]));
      if(letter)
      {
        Serial.print(letter);//print letter to be assigned
      }
      else
      {
        Serial.println("taken!");
      };
    };
    Serial.print(" -On print count-");
    Serial.println(EEPROM.read(TRUECOUNT));
  }
}

void loop()
{
}



