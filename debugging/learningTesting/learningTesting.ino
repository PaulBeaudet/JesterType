//testing for a version of the letters learner suggest letter based on predicted word
//!!not working yet!! testing proof of concept
#include <EEPROM.h>

int led = 13;

#define SECONDABEGIN 2//second layout of lowercase letters
#define SECONDAEND 56
#define YES 60//yes chord storage
#define NO 62//no chord storage
#define FIRSTASSIGNMENTBEGIN 194//divided by two equals ascii 'a'
#define FIRSTASSIGNMENTEND 246//to ascii 'z'
#define COMMON 252 //counts letters learned
#define UNCOMMON 253 //counts letters learned
#define ONSECOND 254 //location in EEPROM 254//error correction and alternate assignments
#define DONELEARNING 255 //location in EEPROM 255
//3 steps Aquiring first layout F/F, second layout T/F, DONELEARNING T/T
// the modifier key to the second assignment
#define SECONDLAY 96// 96 defines the amount of offset from the first assignment in eeprom

#define NUBOFCHORDS 21
word testChords[NUBOFCHORDS]=
{
  1,2,3,10,20,30,100,200,300,1000,2000,3000,10000,20000,30000,11111,22222,33333,11000,11100,11110
};//list of chords to test, simulating interaction

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
    Serial.print(testChords[i]);//print the chord number
    Serial.print(" is now ");// and...
    Serial.println(char(prioritizedLearn(testChords[i])));//print letter to be assigned
  }
}

void loop()
{
}


