#include<avr/pgmspace.h>
#include <EEPROM.h>
//
/*
 ###########COPY NOTICE######################################################
 JesterType- a funny way to use an arduino to type with gestures
 Copyright (c) 2012-2013 Paul C Beaudet <inof8or@gmail.com>
 This program is free software; 
 you can redistribute it and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2 of the License
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with this program;
 if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 ##############################################################################
 
 Build for the Arduino Micro, Leonardo or Usb Lilypad (ATMEGA32u4) 
 
 ######### CURCUIT DISCRIPTION ################################################
 Five momentary switches are physically wired in the following fashion starting from 
 the assigned pins (in the current case 2-6) for each pin.
 From pin to switch--switch to 330om resistor--resistor to ground
 //resistor is a safety for prototyping, not needed in purpose built application
 Thats it! Very simple for this prototype. Note that Internal pull-ups are used.
 ################################################################################
 */
//-----------------eeprom session key, change to start over (if you forgot yes/no assignment)
#define KEY 5
//-----------------------------------------------------------------define buttons
byte buttons[]=
{
  2,3,4,5,6
};
#define NUMBUTTONS sizeof(buttons)

//constants related to getting the value
#define WINDOW 125 // time window in which chords are recorded
#define BOUNCE 30// time to debounce
#define RESTING 11111 // this is the resting chord value
//resting need to be changed to reflected the number of buttons

//----------------------------------------------keyboard definitions
//yes and no structure counters and flag
#define LINESIZE 80//just needs to be under 255

//counts organized into an array for easy iteration 
//and function passing
byte count[7]={
  0,0,0,0,0,0,0};
//pCount KEY, to make the code readable outside of iteration 
#define LINEC 0 // line return sensor
#define CWORD 1 //current word
#define CSENT 2 //current sentence
#define LWORD 3 // last word
#define LSENT 4 // last sentence
#define YESC 5 // yes count
#define NOC 6 // no count
#define METAC 7 // meta count
//KEY: 0=line return/printed, 1=word, 2=sentence,    
//3=last word, 4=last sentence, 5=yes, 6=no

char lastLetter;//holds the last letter
char letterBuffer[14]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//holds  letters for autofill functions
byte sugSize=0;//defines the size of the suggestion
//--condition chords for automated responses
boolean rJustify=false;
boolean cJustify=false;
boolean explicitMode=false;
word no;
word yes;
word meta;

//error correction and alternate assignments
boolean learningPhase[2];
// the modifier key to the second assignment
#define SECONDLAY 96
// it define the amount of offset from the first assignment in eeprom

//common letter frequencies 2d array, data points are the letter's address space in eeprom
#define LPLACES 7
#define FREQ 13
//wordlist

//common words to autocorrect based on first letter
prog_char str0[] PROGMEM = "the";
prog_char str1[] PROGMEM = "be";
prog_char str2[] PROGMEM = "of";
prog_char str3[] PROGMEM = "and";
prog_char str4[] PROGMEM = "in";
prog_char str5[] PROGMEM = "have";
prog_char str6[] PROGMEM = "for";
prog_char str7[] PROGMEM = "you";
prog_char str8[] PROGMEM = "with";
prog_char str9[] PROGMEM = "do";
prog_char str10[] PROGMEM = "not";
prog_char str11[] PROGMEM = "she";
prog_char str12[] PROGMEM = "can";
prog_char str13[] PROGMEM = "go";
prog_char str14[] PROGMEM = "make";
prog_char str15[] PROGMEM = "up";
prog_char str16[] PROGMEM = "know";
prog_char str17[] PROGMEM = "last";
prog_char str18[] PROGMEM = "just";
prog_char str19[] PROGMEM = "people";
prog_char str20[] PROGMEM = "very";
prog_char str21[] PROGMEM = "even";
prog_char str22[] PROGMEM = "really";
prog_char str23[] PROGMEM = "quite";
prog_char str24[] PROGMEM = "zone";
//prog_char str25[] PROGMEM = "xenon";
// end of 1 letter shortcuts
//x is the only exclusion "xenon" is not a word someone would likly use
//begin two letter combos
prog_char str25[] PROGMEM = "that";
prog_char str26[] PROGMEM = "but";
prog_char str27[] PROGMEM = "from";
prog_char str28[] PROGMEM = "his";
prog_char str29[] PROGMEM = "which";
prog_char str30[] PROGMEM = "say";
prog_char str31[] PROGMEM = "will";
prog_char str32[] PROGMEM = "would";
prog_char str33[] PROGMEM = "all";
prog_char str34[] PROGMEM = "get";
prog_char str35[] PROGMEM = "her";
prog_char str36[] PROGMEM = "out";
prog_char str37[] PROGMEM = "see";
prog_char str38[] PROGMEM = "time";
prog_char str39[] PROGMEM = "take";
prog_char str40[] PROGMEM = "some";
prog_char str41[] PROGMEM = "could";
prog_char str42[] PROGMEM = "year";
prog_char str43[] PROGMEM = "into";
prog_char str44[] PROGMEM = "its";
prog_char str45[] PROGMEM = "more";
prog_char str46[] PROGMEM = "about";
prog_char str47[] PROGMEM = "now";
prog_char str48[] PROGMEM = "your";
prog_char str49[] PROGMEM = "other";
prog_char str50[] PROGMEM = "give";
prog_char str51[] PROGMEM = "should";
prog_char str52[] PROGMEM = "well";
prog_char str53[] PROGMEM = "any";
prog_char str54[] PROGMEM = "only";
prog_char str55[] PROGMEM = "new";
prog_char str56[] PROGMEM = "may";
prog_char str57[] PROGMEM = "way";
prog_char str58[] PROGMEM = "look";
prog_char str59[] PROGMEM = "like";
prog_char str60[] PROGMEM = "use";
prog_char str61[] PROGMEM = "such";
prog_char str62[] PROGMEM = "how";
prog_char str63[] PROGMEM = "because";
prog_char str64[] PROGMEM = "good";
prog_char str65[] PROGMEM = "find";
prog_char str66[] PROGMEM = "day";
prog_char str67[] PROGMEM = "after";
prog_char str68[] PROGMEM = "down";
prog_char str69[] PROGMEM = "tell";
prog_char str70[] PROGMEM = "back";
prog_char str71[] PROGMEM = "still";
prog_char str72[] PROGMEM = "must";
prog_char str73[] PROGMEM = "child";
prog_char str74[] PROGMEM = "over";
prog_char str75[] PROGMEM = "too";
prog_char str76[] PROGMEM = "put";
prog_char str77[] PROGMEM = "own";
prog_char str78[] PROGMEM = "old";
prog_char str79[] PROGMEM = "mean";
prog_char str80[] PROGMEM = "part";
prog_char str81[] PROGMEM = "leave";
prog_char str82[] PROGMEM = "great";
prog_char str83[] PROGMEM = "case";
prog_char str84[] PROGMEM = "feel";
prog_char str85[] PROGMEM = "system";
prog_char str86[] PROGMEM = "each";
prog_char str87[] PROGMEM = "might";
prog_char str88[] PROGMEM = "ask";
prog_char str89[] PROGMEM = "number";
prog_char str90[] PROGMEM = "again";
prog_char str91[] PROGMEM = "area";
prog_char str92[] PROGMEM = "under";
prog_char str93[] PROGMEM = "problem";
prog_char str94[] PROGMEM = "try";
prog_char str95[] PROGMEM = "hand";
prog_char str96[] PROGMEM = "school";
prog_char str97[] PROGMEM = "small";
prog_char str98[] PROGMEM = "place";
prog_char str99[] PROGMEM = "away";
prog_char str100[] PROGMEM = "keep";
prog_char str101[] PROGMEM = "point";
prog_char str102[] PROGMEM = "different";
prog_char str103[] PROGMEM = "large";
prog_char str104[] PROGMEM = "off";
prog_char str105[] PROGMEM = "follow";
prog_char str106[] PROGMEM = "turn";
prog_char str107[] PROGMEM = "end";
prog_char str108[] PROGMEM = "during";
prog_char str109[] PROGMEM = "bring";
prog_char str110[] PROGMEM = "example";
prog_char str111[] PROGMEM = "family";
prog_char str112[] PROGMEM = "rather";
prog_char str113[] PROGMEM = "write";
prog_char str114[] PROGMEM = "percent";
prog_char str115[] PROGMEM = "both";
prog_char str116[] PROGMEM = "run";
prog_char str117[] PROGMEM = "right";
prog_char str118[] PROGMEM = "every";
prog_char str119[] PROGMEM = "side";
prog_char str120[] PROGMEM = "night";
prog_char str121[] PROGMEM = "important";
prog_char str122[] PROGMEM = "eye";
prog_char str123[] PROGMEM = "question";
prog_char str124[] PROGMEM = "order";
prog_char str125[] PROGMEM = "development";
prog_char str126[] PROGMEM = "national";
prog_char str127[] PROGMEM = "room";
prog_char str128[] PROGMEM = "effect";
prog_char str129[] PROGMEM = "big";
prog_char str130[] PROGMEM = "idea";
prog_char str131[] PROGMEM = "job";
prog_char str132[] PROGMEM = "result";
prog_char str133[] PROGMEM = "authority";
prog_char str134[] PROGMEM = "view";
prog_char str135[] PROGMEM = "appear";
prog_char str136[] PROGMEM = "full";
prog_char str137[] PROGMEM = "voice";
prog_char str138[] PROGMEM = "kind";
prog_char str139[] PROGMEM = "add";
prog_char str140[] PROGMEM = "available";
prog_char str141[] PROGMEM = "action";
prog_char str142[] PROGMEM = "issue";
prog_char str143[] PROGMEM = "type";
prog_char str144[] PROGMEM = "education";
prog_char str145[] PROGMEM = "speak";
prog_char str146[] PROGMEM = "open";
prog_char str147[] PROGMEM = "centre";
prog_char str148[] PROGMEM = "value";
prog_char str149[] PROGMEM = "class";
prog_char str150[] PROGMEM = "economic";
prog_char str151[] PROGMEM = "upon";
prog_char str152[] PROGMEM = "among";
prog_char str153[] PROGMEM = "draw";
prog_char str154[] PROGMEM = "city";
prog_char str155[] PROGMEM = "create";
prog_char str156[] PROGMEM = "game";
prog_char str157[] PROGMEM = "else";
prog_char str158[] PROGMEM = "black";
prog_char str159[] PROGMEM = "air";
prog_char str160[] PROGMEM = "cut";
prog_char str161[] PROGMEM = "especially";
prog_char str162[] PROGMEM = "human";
prog_char str163[] PROGMEM = "picture";
prog_char str164[] PROGMEM = "occur";
prog_char str165[] PROGMEM = "either";
prog_char str166[] PROGMEM = "knowledge";
prog_char str167[] PROGMEM = "attention";
prog_char str168[] PROGMEM = "floor";
prog_char str169[] PROGMEM = "myself";
prog_char str170[] PROGMEM = "obtain";
prog_char str171[] PROGMEM = "glass";
prog_char str172[] PROGMEM = "skill";
prog_char str173[] PROGMEM = "oil";
prog_char str174[] PROGMEM = "okay";
prog_char str175[] PROGMEM = "version";
prog_char str176[] PROGMEM = "employment";
prog_char str177[] PROGMEM = "physical";
prog_char str178[] PROGMEM = "equipment";
prog_char str179[] PROGMEM = "slightly";
prog_char str180[] PROGMEM = "ahead";
prog_char str181[] PROGMEM = "ignore";
prog_char str182[] PROGMEM = "judge";
prog_char str183[] PROGMEM = "yard";
prog_char str184[] PROGMEM = "twice";
prog_char str185[] PROGMEM = "egg";
prog_char str186[] PROGMEM = "error";
prog_char str187[] PROGMEM = "guest";
prog_char str188[] PROGMEM = "lunch";
prog_char str189[] PROGMEM = "urban";
prog_char str190[] PROGMEM = "aye";
prog_char str191[] PROGMEM = "illustrate";
prog_char str192[] PROGMEM = "switch";
prog_char str193[] PROGMEM = "iron";
prog_char str194[] PROGMEM = "odd";
prog_char str195[] PROGMEM = "ice";
prog_char str196[] PROGMEM = "cycle";
prog_char str197[] PROGMEM = "jacket";
prog_char str198[] PROGMEM = "square";
prog_char str199[] PROGMEM = "snow";
prog_char str200[] PROGMEM = "ultimately";
prog_char str201[] PROGMEM = "psychological";
prog_char str202[] PROGMEM = "vulnerable";
prog_char str203[] PROGMEM = "hypothesis";
prog_char str204[] PROGMEM = "ethnic";
prog_char str205[] PROGMEM = "jew";
prog_char str206[] PROGMEM = "yield";
prog_char str207[] PROGMEM = "rhythm";
prog_char str208[] PROGMEM = "episode";
prog_char str209[] PROGMEM = "oxygen";
prog_char str210[] PROGMEM = "ghost";
prog_char str211[] PROGMEM = "bye";
prog_char str212[] PROGMEM = "dynamic";
prog_char str213[] PROGMEM = "utility";
prog_char str214[] PROGMEM = "ugly";
prog_char str215[] PROGMEM = "omit";
prog_char str216[] PROGMEM = "oak";
prog_char str217[] PROGMEM = "dwelling";
prog_char str218[] PROGMEM = "ozone";
prog_char str219[] PROGMEM = "aesthetic";
prog_char str220[] PROGMEM = "axis";
prog_char str221[] PROGMEM = "umbrella";
prog_char str222[] PROGMEM = "ion";
prog_char str223[] PROGMEM = "aquarium";
prog_char str224[] PROGMEM = "ivory";
//end two letter combos
//begin three letter combos
prog_char str225[] PROGMEM = "this";
prog_char str226[] PROGMEM = "they";
prog_char str227[] PROGMEM = "what";
prog_char str228[] PROGMEM = "come";
prog_char str229[] PROGMEM = "than";
prog_char str230[] PROGMEM = "also";
prog_char str231[] PROGMEM = "when";
prog_char str232[] PROGMEM = "want";
prog_char str233[] PROGMEM = "between";
prog_char str234[] PROGMEM = "many";
prog_char str235[] PROGMEM = "those";
prog_char str236[] PROGMEM = "yeah";
prog_char str237[] PROGMEM = "through";
prog_char str238[] PROGMEM = "here";
prog_char str239[] PROGMEM = "work";
prog_char str240[] PROGMEM = "become";
prog_char str241[] PROGMEM = "government";
prog_char str242[] PROGMEM = "life";
prog_char str243[] PROGMEM = "woman";
prog_char str244[] PROGMEM = "seem";
prog_char str245[] PROGMEM = "same";
prog_char str246[] PROGMEM = "need";
prog_char str247[] PROGMEM = "much";
prog_char str248[] PROGMEM = "group";
prog_char str249[] PROGMEM = "however";
prog_char str250[] PROGMEM = "another";
prog_char str251[] PROGMEM = "show";
prog_char str252[] PROGMEM = "course";
prog_char str253[] PROGMEM = "shall";
prog_char str254[] PROGMEM = "against";
prog_char str255[] PROGMEM = "never";
prog_char str256[] PROGMEM = "most";
prog_char str257[] PROGMEM = "service";
prog_char str258[] PROGMEM = "call";
prog_char str259[] PROGMEM = "party";
prog_char str260[] PROGMEM = "high";
prog_char str261[] PROGMEM = "something";
prog_char str262[] PROGMEM = "before";
prog_char str263[] PROGMEM = "while";
prog_char str264[] PROGMEM = "house";
prog_char str265[] PROGMEM = "provide";
prog_char str266[] PROGMEM = "week";
prog_char str267[] PROGMEM = "hold";
prog_char str268[] PROGMEM = "member";
prog_char str269[] PROGMEM = "always";
prog_char str270[] PROGMEM = "next";
prog_char str271[] PROGMEM = "without";
prog_char str272[] PROGMEM = "local";
prog_char str273[] PROGMEM = "begin";
prog_char str274[] PROGMEM = "although";
prog_char str275[] PROGMEM = "fact";
prog_char str276[] PROGMEM = "social";
prog_char str277[] PROGMEM = "state";
prog_char str278[] PROGMEM = "long";
prog_char str279[] PROGMEM = "help";
prog_char str280[] PROGMEM = "home";
prog_char str281[] PROGMEM = "month";
prog_char str282[] PROGMEM = "head";
prog_char str283[] PROGMEM = "information";
prog_char str284[] PROGMEM = "business";
prog_char str285[] PROGMEM = "play";
prog_char str286[] PROGMEM = "power";
prog_char str287[] PROGMEM = "change";
prog_char str288[] PROGMEM = "move";
prog_char str289[] PROGMEM = "interest";
prog_char str290[] PROGMEM = "book";
prog_char str291[] PROGMEM = "often";
prog_char str292[] PROGMEM = "young";
prog_char str293[] PROGMEM = "water";
prog_char str294[] PROGMEM = "form";
prog_char str295[] PROGMEM = "perhaps";
prog_char str296[] PROGMEM = "meet";
prog_char str297[] PROGMEM = "level";
prog_char str298[] PROGMEM = "until";
prog_char str299[] PROGMEM = "policy";
prog_char str300[] PROGMEM = "include";
prog_char str301[] PROGMEM = "believe";
prog_char str302[] PROGMEM = "already";
prog_char str303[] PROGMEM = "possible";
prog_char str304[] PROGMEM = "nothing";
prog_char str305[] PROGMEM = "line";
prog_char str306[] PROGMEM = "allow";
prog_char str307[] PROGMEM = "lead";
prog_char str308[] PROGMEM = "study";
prog_char str309[] PROGMEM = "live";
prog_char str310[] PROGMEM = "since";
prog_char str311[] PROGMEM = "name";
prog_char str312[] PROGMEM = "body";
prog_char str313[] PROGMEM = "happen";
prog_char str314[] PROGMEM = "friend";
prog_char str315[] PROGMEM = "almost";
prog_char str316[] PROGMEM = "carry";
prog_char str317[] PROGMEM = "early";
prog_char str318[] PROGMEM = "himself";
prog_char str319[] PROGMEM = "public";
prog_char str320[] PROGMEM = "together";
prog_char str321[] PROGMEM = "talk";
prog_char str322[] PROGMEM = "report";
prog_char str323[] PROGMEM = "market";
prog_char str324[] PROGMEM = "continue";
prog_char str325[] PROGMEM = "able";
prog_char str326[] PROGMEM = "later";
prog_char str327[] PROGMEM = "rate";
prog_char str328[] PROGMEM = "door";
prog_char str329[] PROGMEM = "office";
prog_char str330[] PROGMEM = "reason";
prog_char str331[] PROGMEM = "less";
prog_char str332[] PROGMEM = "minister";
prog_char str333[] PROGMEM = "subject";
prog_char str334[] PROGMEM = "term";
prog_char str335[] PROGMEM = "involve";
prog_char str336[] PROGMEM = "sort";
prog_char str337[] PROGMEM = "require";
prog_char str338[] PROGMEM = "suggest";
prog_char str339[] PROGMEM = "towards";
prog_char str340[] PROGMEM = "anything";
prog_char str341[] PROGMEM = "mother";
prog_char str342[] PROGMEM = "once";
prog_char str343[] PROGMEM = "lose";
prog_char str344[] PROGMEM = "expect";
prog_char str345[] PROGMEM = "ever";
prog_char str346[] PROGMEM = "price";
prog_char str347[] PROGMEM = "little";
prog_char str348[] PROGMEM = "remember";
prog_char str349[] PROGMEM = "cost";
prog_char str350[] PROGMEM = "matter";
prog_char str351[] PROGMEM = "figure";
prog_char str352[] PROGMEM = "research";
prog_char str353[] PROGMEM = "actually";
prog_char str354[] PROGMEM = "fall";
prog_char str355[] PROGMEM = "today";
prog_char str356[] PROGMEM = "enough";
prog_char str357[] PROGMEM = "moment";
prog_char str358[] PROGMEM = "girl";
prog_char str359[] PROGMEM = "stop";
prog_char str360[] PROGMEM = "send";
prog_char str361[] PROGMEM = "decide";
prog_char str362[] PROGMEM = "main";
prog_char str363[] PROGMEM = "understand";
prog_char str364[] PROGMEM = "develop";
prog_char str365[] PROGMEM = "industry";
prog_char str366[] PROGMEM = "receive";
prog_char str367[] PROGMEM = "several";
prog_char str368[] PROGMEM = "return";
prog_char str369[] PROGMEM = "build";
prog_char str370[] PROGMEM = "spend";
prog_char str371[] PROGMEM = "itself";
prog_char str372[] PROGMEM = "paper";
prog_char str373[] PROGMEM = "major";
prog_char str374[] PROGMEM = "describe";
prog_char str375[] PROGMEM = "agree";
prog_char str376[] PROGMEM = "general";
prog_char str377[] PROGMEM = "century";
prog_char str378[] PROGMEM = "father";
prog_char str379[] PROGMEM = "section";
prog_char str380[] PROGMEM = "patient";
prog_char str381[] PROGMEM = "around";
prog_char str382[] PROGMEM = "road";
prog_char str383[] PROGMEM = "table";
prog_char str384[] PROGMEM = "church";
prog_char str385[] PROGMEM = "likely";
prog_char str386[] PROGMEM = "team";
prog_char str387[] PROGMEM = "death";
prog_char str388[] PROGMEM = "soon";
prog_char str389[] PROGMEM = "certain";
prog_char str390[] PROGMEM = "half";
prog_char str391[] PROGMEM = "language";
prog_char str392[] PROGMEM = "walk";
prog_char str393[] PROGMEM = "difficult";
prog_char str394[] PROGMEM = "department";
prog_char str395[] PROGMEM = "morning";
prog_char str396[] PROGMEM = "hope";
prog_char str397[] PROGMEM = "across";
prog_char str398[] PROGMEM = "letter";
prog_char str399[] PROGMEM = "evidence";
prog_char str400[] PROGMEM = "foot";
prog_char str401[] PROGMEM = "clear";
prog_char str402[] PROGMEM = "role";
prog_char str403[] PROGMEM = "practice";
prog_char str404[] PROGMEM = "bank";
prog_char str405[] PROGMEM = "support";
prog_char str406[] PROGMEM = "sell";
prog_char str407[] PROGMEM = "range";
prog_char str408[] PROGMEM = "behind";
prog_char str409[] PROGMEM = "sure";
prog_char str410[] PROGMEM = "pass";
prog_char str411[] PROGMEM = "thus";
prog_char str412[] PROGMEM = "accept";
prog_char str413[] PROGMEM = "further";
prog_char str414[] PROGMEM = "club";
prog_char str415[] PROGMEM = "cause";
prog_char str416[] PROGMEM = "history";
prog_char str417[] PROGMEM = "trade";
prog_char str418[] PROGMEM = "situation";
prog_char str419[] PROGMEM = "whose";
prog_char str420[] PROGMEM = "relation";
prog_char str421[] PROGMEM = "strong";
prog_char str422[] PROGMEM = "field";
prog_char str423[] PROGMEM = "free";
prog_char str424[] PROGMEM = "break";
prog_char str425[] PROGMEM = "yesterday";
prog_char str426[] PROGMEM = "window";
prog_char str427[] PROGMEM = "wait";
prog_char str428[] PROGMEM = "usually";
prog_char str429[] PROGMEM = "wife";
prog_char str430[] PROGMEM = "cover";
prog_char str431[] PROGMEM = "apply";
prog_char str432[] PROGMEM = "raise";
prog_char str433[] PROGMEM = "sale";
prog_char str434[] PROGMEM = "light";
prog_char str435[] PROGMEM = "claim";
prog_char str436[] PROGMEM = "base";
prog_char str437[] PROGMEM = "rule";
prog_char str438[] PROGMEM = "similar";
prog_char str439[] PROGMEM = "quality";
prog_char str440[] PROGMEM = "nature";
prog_char str441[] PROGMEM = "data";
prog_char str442[] PROGMEM = "necessary";
prog_char str443[] PROGMEM = "pound";
prog_char str444[] PROGMEM = "method";
prog_char str445[] PROGMEM = "unit";
prog_char str446[] PROGMEM = "board";
prog_char str447[] PROGMEM = "true";
prog_char str448[] PROGMEM = "detail";
prog_char str449[] PROGMEM = "model";
prog_char str450[] PROGMEM = "bear";
prog_char str451[] PROGMEM = "join";
prog_char str452[] PROGMEM = "reduce";
prog_char str453[] PROGMEM = "establish";
prog_char str454[] PROGMEM = "easy";
prog_char str455[] PROGMEM = "hospital";
prog_char str456[] PROGMEM = "scheme";
prog_char str457[] PROGMEM = "choose";
prog_char str458[] PROGMEM = "wish";
prog_char str459[] PROGMEM = "achieve";
prog_char str460[] PROGMEM = "financial";
prog_char str461[] PROGMEM = "poor";
prog_char str462[] PROGMEM = "director";
prog_char str463[] PROGMEM = "drive";
prog_char str464[] PROGMEM = "along";
prog_char str465[] PROGMEM = "amount";
prog_char str466[] PROGMEM = "operation";
prog_char str467[] PROGMEM = "fail";
prog_char str468[] PROGMEM = "opportunity";
prog_char str469[] PROGMEM = "firm";
prog_char str470[] PROGMEM = "source";
prog_char str471[] PROGMEM = "wide";
prog_char str472[] PROGMEM = "kill";
prog_char str473[] PROGMEM = "labour";
prog_char str474[] PROGMEM = "various";
prog_char str475[] PROGMEM = "test";
prog_char str476[] PROGMEM = "close";
prog_char str477[] PROGMEM = "love";
prog_char str478[] PROGMEM = "colour";
prog_char str479[] PROGMEM = "benefit";
prog_char str480[] PROGMEM = "animal";
prog_char str481[] PROGMEM = "election";
prog_char str482[] PROGMEM = "purpose";
prog_char str483[] PROGMEM = "rise";
prog_char str484[] PROGMEM = "hard";
prog_char str485[] PROGMEM = "music";
prog_char str486[] PROGMEM = "hair";
prog_char str487[] PROGMEM = "prepare";
prog_char str488[] PROGMEM = "piece";
prog_char str489[] PROGMEM = "discuss";
prog_char str490[] PROGMEM = "front";
prog_char str491[] PROGMEM = "royal";
prog_char str492[] PROGMEM = "tree";
prog_char str493[] PROGMEM = "population";
prog_char str494[] PROGMEM = "catch";
prog_char str495[] PROGMEM = "pick";
prog_char str496[] PROGMEM = "page";
prog_char str497[] PROGMEM = "enjoy";
prog_char str498[] PROGMEM = "instead";
prog_char str499[] PROGMEM = "wear";
prog_char str500[] PROGMEM = "size";
prog_char str501[] PROGMEM = "environment";
prog_char str502[] PROGMEM = "success";
prog_char str503[] PROGMEM = "wrong";
prog_char str504[] PROGMEM = "near";
prog_char str505[] PROGMEM = "round";
prog_char str506[] PROGMEM = "list";
prog_char str507[] PROGMEM = "argue";
prog_char str508[] PROGMEM = "future";
prog_char str509[] PROGMEM = "analysis";
prog_char str510[] PROGMEM = "enter";
prog_char str511[] PROGMEM = "space";
prog_char str512[] PROGMEM = "arrive";
prog_char str513[] PROGMEM = "ensure";
prog_char str514[] PROGMEM = "demand";
prog_char str515[] PROGMEM = "pull";
prog_char str516[] PROGMEM = "doctor";
prog_char str517[] PROGMEM = "refer";
prog_char str518[] PROGMEM = "feature";
prog_char str519[] PROGMEM = "step";
prog_char str520[] PROGMEM = "following";
prog_char str521[] PROGMEM = "machine";
prog_char str522[] PROGMEM = "association";
prog_char str523[] PROGMEM = "film";
prog_char str524[] PROGMEM = "region";
prog_char str525[] PROGMEM = "effort";
prog_char str526[] PROGMEM = "award";
prog_char str527[] PROGMEM = "village";
prog_char str528[] PROGMEM = "organisation";
prog_char str529[] PROGMEM = "news";
prog_char str530[] PROGMEM = "nice";
prog_char str531[] PROGMEM = "cell";
prog_char str532[] PROGMEM = "current";
prog_char str533[] PROGMEM = "legal";
prog_char str534[] PROGMEM = "energy";
prog_char str535[] PROGMEM = "degree";
prog_char str536[] PROGMEM = "mile";
prog_char str537[] PROGMEM = "means";
prog_char str538[] PROGMEM = "above";
prog_char str539[] PROGMEM = "task";
prog_char str540[] PROGMEM = "affect";
prog_char str541[] PROGMEM = "please";
prog_char str542[] PROGMEM = "function";
prog_char str543[] PROGMEM = "identify";
prog_char str544[] PROGMEM = "defence";
prog_char str545[] PROGMEM = "garden";
prog_char str546[] PROGMEM = "technology";
prog_char str547[] PROGMEM = "style";
prog_char str548[] PROGMEM = "feeling";
prog_char str549[] PROGMEM = "science";
prog_char str550[] PROGMEM = "doubt";
prog_char str551[] PROGMEM = "horse";
prog_char str552[] PROGMEM = "answer";
prog_char str553[] PROGMEM = "suffer";
prog_char str554[] PROGMEM = "announce";
prog_char str555[] PROGMEM = "user";
prog_char str556[] PROGMEM = "normal";
prog_char str557[] PROGMEM = "quickly";
prog_char str558[] PROGMEM = "army";
prog_char str559[] PROGMEM = "husband";
prog_char str560[] PROGMEM = "visit";
prog_char str561[] PROGMEM = "capital";
prog_char str562[] PROGMEM = "season";
prog_char str563[] PROGMEM = "significant";
prog_char str564[] PROGMEM = "economy";
prog_char str565[] PROGMEM = "duty";
prog_char str566[] PROGMEM = "attempt";
prog_char str567[] PROGMEM = "save";
prog_char str568[] PROGMEM = "suddenly";
prog_char str569[] PROGMEM = "brother";
prog_char str570[] PROGMEM = "improve";
prog_char str571[] PROGMEM = "avoid";
prog_char str572[] PROGMEM = "wonder";
prog_char str573[] PROGMEM = "tend";
prog_char str574[] PROGMEM = "title";
prog_char str575[] PROGMEM = "hotel";
prog_char str576[] PROGMEM = "aspect";
prog_char str577[] PROGMEM = "summer";
prog_char str578[] PROGMEM = "daughter";
prog_char str579[] PROGMEM = "exist";
prog_char str580[] PROGMEM = "baby";
prog_char str581[] PROGMEM = "smile";
prog_char str582[] PROGMEM = "left";
prog_char str583[] PROGMEM = "customer";
prog_char str584[] PROGMEM = "outside";
prog_char str585[] PROGMEM = "total";
prog_char str586[] PROGMEM = "division";
prog_char str587[] PROGMEM = "king";
prog_char str588[] PROGMEM = "image";
prog_char str589[] PROGMEM = "obviously";
prog_char str590[] PROGMEM = "unless";
prog_char str591[] PROGMEM = "circumstance";
prog_char str592[] PROGMEM = "mention";
prog_char str593[] PROGMEM = "client";
prog_char str594[] PROGMEM = "admit";
prog_char str595[] PROGMEM = "original";
prog_char str596[] PROGMEM = "exactly";
prog_char str597[] PROGMEM = "beyond";
prog_char str598[] PROGMEM = "encourage";
prog_char str599[] PROGMEM = "addition";
prog_char str600[] PROGMEM = "goal";
prog_char str601[] PROGMEM = "miss";
prog_char str602[] PROGMEM = "drop";
prog_char str603[] PROGMEM = "item";
prog_char str604[] PROGMEM = "reveal";
prog_char str605[] PROGMEM = "maybe";
prog_char str606[] PROGMEM = "ability";
prog_char str607[] PROGMEM = "campaign";
prog_char str608[] PROGMEM = "advice";
prog_char str609[] PROGMEM = "library";
prog_char str610[] PROGMEM = "pupil";
prog_char str611[] PROGMEM = "dark";
prog_char str612[] PROGMEM = "culture";
prog_char str613[] PROGMEM = "blood";
prog_char str614[] PROGMEM = "bill";
prog_char str615[] PROGMEM = "extent";
prog_char str616[] PROGMEM = "apart";
prog_char str617[] PROGMEM = "text";
prog_char str618[] PROGMEM = "mouth";
prog_char str619[] PROGMEM = "fish";
prog_char str620[] PROGMEM = "enable";
prog_char str621[] PROGMEM = "trouble";
prog_char str622[] PROGMEM = "payment";
prog_char str623[] PROGMEM = "best";
prog_char str624[] PROGMEM = "lady";
prog_char str625[] PROGMEM = "article";
prog_char str626[] PROGMEM = "object";
prog_char str627[] PROGMEM = "immediately";
prog_char str628[] PROGMEM = "television";
prog_char str629[] PROGMEM = "agency";
prog_char str630[] PROGMEM = "except";
prog_char str631[] PROGMEM = "check";
prog_char str632[] PROGMEM = "laugh";
prog_char str633[] PROGMEM = "weight";
prog_char str634[] PROGMEM = "solution";
prog_char str635[] PROGMEM = "medical";
prog_char str636[] PROGMEM = "budget";
prog_char str637[] PROGMEM = "river";
prog_char str638[] PROGMEM = "push";
prog_char str639[] PROGMEM = "tomorrow";
prog_char str640[] PROGMEM = "edge";
prog_char str641[] PROGMEM = "opinion";
prog_char str642[] PROGMEM = "drug";
prog_char str643[] PROGMEM = "option";
prog_char str644[] PROGMEM = "occasion";
prog_char str645[] PROGMEM = "software";
prog_char str646[] PROGMEM = "lack";
prog_char str647[] PROGMEM = "blue";
prog_char str648[] PROGMEM = "radio";
prog_char str649[] PROGMEM = "bird";
prog_char str650[] PROGMEM = "green";
prog_char str651[] PROGMEM = "message";
prog_char str652[] PROGMEM = "afternoon";
prog_char str653[] PROGMEM = "fully";
prog_char str654[] PROGMEM = "race";
prog_char str655[] PROGMEM = "gain";
prog_char str656[] PROGMEM = "scene";
prog_char str657[] PROGMEM = "kitchen";
prog_char str658[] PROGMEM = "arise";
prog_char str659[] PROGMEM = "network";
prog_char str660[] PROGMEM = "peace";
prog_char str661[] PROGMEM = "employee";
prog_char str662[] PROGMEM = "scale";
prog_char str663[] PROGMEM = "otherwise";
prog_char str664[] PROGMEM = "owner";
prog_char str665[] PROGMEM = "corner";
prog_char str666[] PROGMEM = "sport";
prog_char str667[] PROGMEM = "hang";
prog_char str668[] PROGMEM = "civil";
prog_char str669[] PROGMEM = "crime";
prog_char str670[] PROGMEM = "ball";
prog_char str671[] PROGMEM = "safety";
prog_char str672[] PROGMEM = "copy";
prog_char str673[] PROGMEM = "sister";
prog_char str674[] PROGMEM = "trial";
prog_char str675[] PROGMEM = "rock";
prog_char str676[] PROGMEM = "damage";
prog_char str677[] PROGMEM = "adopt";
prog_char str678[] PROGMEM = "essential";
prog_char str679[] PROGMEM = "length";
prog_char str680[] PROGMEM = "branch";
prog_char str681[] PROGMEM = "deep";
prog_char str682[] PROGMEM = "none";
prog_char str683[] PROGMEM = "pain";
prog_char str684[] PROGMEM = "spirit";
prog_char str685[] PROGMEM = "cash";
prog_char str686[] PROGMEM = "target";
prog_char str687[] PROGMEM = "male";
prog_char str688[] PROGMEM = "executive";
prog_char str689[] PROGMEM = "slowly";
prog_char str690[] PROGMEM = "huge";
prog_char str691[] PROGMEM = "nuclear";
prog_char str692[] PROGMEM = "rich";
prog_char str693[] PROGMEM = "emerge";
prog_char str694[] PROGMEM = "ring";
prog_char str695[] PROGMEM = "skin";
prog_char str696[] PROGMEM = "island";
prog_char str697[] PROGMEM = "separate";
prog_char str698[] PROGMEM = "danger";
prog_char str699[] PROGMEM = "credit";
prog_char str700[] PROGMEM = "candidate";
prog_char str701[] PROGMEM = "merely";
prog_char str702[] PROGMEM = "plus";
prog_char str703[] PROGMEM = "debate";
prog_char str704[] PROGMEM = "vote";
prog_char str705[] PROGMEM = "western";
prog_char str706[] PROGMEM = "settle";
prog_char str707[] PROGMEM = "largely";
prog_char str708[] PROGMEM = "observe";
prog_char str709[] PROGMEM = "limit";
prog_char str710[] PROGMEM = "deny";
prog_char str711[] PROGMEM = "writer";
prog_char str712[] PROGMEM = "video";
prog_char str713[] PROGMEM = "vehicle";
prog_char str714[] PROGMEM = "cross";
prog_char str715[] PROGMEM = "farm";
prog_char str716[] PROGMEM = "grant";
prog_char str717[] PROGMEM = "phone";
prog_char str718[] PROGMEM = "flower";
prog_char str719[] PROGMEM = "injury";
prog_char str720[] PROGMEM = "stick";
prog_char str721[] PROGMEM = "battle";
prog_char str722[] PROGMEM = "sleep";
prog_char str723[] PROGMEM = "volume";
prog_char str724[] PROGMEM = "ship";
prog_char str725[] PROGMEM = "background";
prog_char str726[] PROGMEM = "touch";
prog_char str727[] PROGMEM = "sexual";
prog_char str728[] PROGMEM = "ordinary";
prog_char str729[] PROGMEM = "cabinet";
prog_char str730[] PROGMEM = "engine";
prog_char str731[] PROGMEM = "tonight";
prog_char str732[] PROGMEM = "adult";
prog_char str733[] PROGMEM = "author";
prog_char str734[] PROGMEM = "song";
prog_char str735[] PROGMEM = "wood";
prog_char str736[] PROGMEM = "wage";
prog_char str737[] PROGMEM = "domestic";
prog_char str738[] PROGMEM = "warm";
prog_char str739[] PROGMEM = "dinner";
prog_char str740[] PROGMEM = "package";
prog_char str741[] PROGMEM = "coffee";
prog_char str742[] PROGMEM = "sheet";
prog_char str743[] PROGMEM = "equally";
prog_char str744[] PROGMEM = "session";
prog_char str745[] PROGMEM = "loan";
prog_char str746[] PROGMEM = "bind";
prog_char str747[] PROGMEM = "victim";
prog_char str748[] PROGMEM = "audience";
prog_char str749[] PROGMEM = "famous";
prog_char str750[] PROGMEM = "master";
prog_char str751[] PROGMEM = "potential";
prog_char str752[] PROGMEM = "exhibition";
prog_char str753[] PROGMEM = "chief";
prog_char str754[] PROGMEM = "middle";
prog_char str755[] PROGMEM = "unable";
prog_char str756[] PROGMEM = "acquire";
prog_char str757[] PROGMEM = "favour";
prog_char str758[] PROGMEM = "flat";
prog_char str759[] PROGMEM = "rural";
prog_char str760[] PROGMEM = "deliver";
prog_char str761[] PROGMEM = "nobody";
prog_char str762[] PROGMEM = "onto";
prog_char str763[] PROGMEM = "aircraft";
prog_char str764[] PROGMEM = "bright";
prog_char str765[] PROGMEM = "unemployment";
prog_char str766[] PROGMEM = "spread";
prog_char str767[] PROGMEM = "flight";
prog_char str768[] PROGMEM = "suitable";
prog_char str769[] PROGMEM = "ought";
prog_char str770[] PROGMEM = "reject";
prog_char str771[] PROGMEM = "dream";
prog_char str772[] PROGMEM = "pension";
prog_char str773[] PROGMEM = "afraid";
prog_char str774[] PROGMEM = "murder";
prog_char str775[] PROGMEM = "hide";
prog_char str776[] PROGMEM = "absence";
prog_char str777[] PROGMEM = "average";
prog_char str778[] PROGMEM = "acid";
prog_char str779[] PROGMEM = "bottle";
prog_char str780[] PROGMEM = "bedroom";
prog_char str781[] PROGMEM = "temperature";
prog_char str782[] PROGMEM = "wave";
prog_char str783[] PROGMEM = "magazine";
prog_char str784[] PROGMEM = "silence";
prog_char str785[] PROGMEM = "educational";
prog_char str786[] PROGMEM = "tape";
prog_char str787[] PROGMEM = "elsewhere";
prog_char str788[] PROGMEM = "code";
prog_char str789[] PROGMEM = "hill";
prog_char str790[] PROGMEM = "screen";
prog_char str791[] PROGMEM = "sequence";
prog_char str792[] PROGMEM = "female";
prog_char str793[] PROGMEM = "phase";
prog_char str794[] PROGMEM = "welcome";
prog_char str795[] PROGMEM = "tiny";
prog_char str796[] PROGMEM = "initiative";
prog_char str797[] PROGMEM = "fast";
prog_char str798[] PROGMEM = "gold";
prog_char str799[] PROGMEM = "shut";
prog_char str800[] PROGMEM = "journey";
prog_char str801[] PROGMEM = "violence";
prog_char str802[] PROGMEM = "noise";
prog_char str803[] PROGMEM = "escape";
prog_char str804[] PROGMEM = "score";
prog_char str805[] PROGMEM = "justice";
prog_char str806[] PROGMEM = "upper";
prog_char str807[] PROGMEM = "tooth";
prog_char str808[] PROGMEM = "nurse";
prog_char str809[] PROGMEM = "narrow";
prog_char str810[] PROGMEM = "coal";
prog_char str811[] PROGMEM = "wild";
prog_char str812[] PROGMEM = "neither";
prog_char str813[] PROGMEM = "guide";
prog_char str814[] PROGMEM = "pocket";
prog_char str815[] PROGMEM = "fruit";
prog_char str816[] PROGMEM = "ride";
prog_char str817[] PROGMEM = "foundation";
prog_char str818[] PROGMEM = "vital";
prog_char str819[] PROGMEM = "burn";
prog_char str820[] PROGMEM = "jump";
prog_char str821[] PROGMEM = "derive";
prog_char str822[] PROGMEM = "satisfy";
prog_char str823[] PROGMEM = "focus";
prog_char str824[] PROGMEM = "border";
prog_char str825[] PROGMEM = "wash";
prog_char str826[] PROGMEM = "gather";
prog_char str827[] PROGMEM = "ancient";
prog_char str828[] PROGMEM = "elderly";
prog_char str829[] PROGMEM = "overall";
prog_char str830[] PROGMEM = "rare";
prog_char str831[] PROGMEM = "drawing";
prog_char str832[] PROGMEM = "hurt";
prog_char str833[] PROGMEM = "mechanism";
prog_char str834[] PROGMEM = "atmosphere";
prog_char str835[] PROGMEM = "slip";
prog_char str836[] PROGMEM = "panel";
prog_char str837[] PROGMEM = "roof";
prog_char str838[] PROGMEM = "typical";
prog_char str839[] PROGMEM = "negotiation";
prog_char str840[] PROGMEM = "hence";
prog_char str841[] PROGMEM = "fuel";
prog_char str842[] PROGMEM = "liability";
prog_char str843[] PROGMEM = "hate";
prog_char str844[] PROGMEM = "vast";
prog_char str845[] PROGMEM = "soil";
prog_char str846[] PROGMEM = "nose";
prog_char str847[] PROGMEM = "lord";
prog_char str848[] PROGMEM = "ticket";
prog_char str849[] PROGMEM = "editor";
prog_char str850[] PROGMEM = "bone";
prog_char str851[] PROGMEM = "knee";
prog_char str852[] PROGMEM = "unfortunately";
prog_char str853[] PROGMEM = "academic";
prog_char str854[] PROGMEM = "lawyer";
prog_char str855[] PROGMEM = "rapidly";
prog_char str856[] PROGMEM = "citizen";
prog_char str857[] PROGMEM = "ourselves";
prog_char str858[] PROGMEM = "height";
prog_char str859[] PROGMEM = "knock";
prog_char str860[] PROGMEM = "inner";
prog_char str861[] PROGMEM = "crucial";
prog_char str862[] PROGMEM = "boundary";
prog_char str863[] PROGMEM = "queen";
prog_char str864[] PROGMEM = "virtually";
prog_char str865[] PROGMEM = "inquiry";
prog_char str866[] PROGMEM = "topic";
prog_char str867[] PROGMEM = "unknown";
prog_char str868[] PROGMEM = "badly";
prog_char str869[] PROGMEM = "licence";
prog_char str870[] PROGMEM = "buyer";
prog_char str871[] PROGMEM = "gift";
prog_char str872[] PROGMEM = "angry";
prog_char str873[] PROGMEM = "tank";
prog_char str874[] PROGMEM = "alive";
prog_char str875[] PROGMEM = "abandon";
prog_char str876[] PROGMEM = "blame";
prog_char str877[] PROGMEM = "quote";
prog_char str878[] PROGMEM = "diet";
prog_char str879[] PROGMEM = "glance";
prog_char str880[] PROGMEM = "frame";
prog_char str881[] PROGMEM = "acknowledge";
prog_char str882[] PROGMEM = "fault";
prog_char str883[] PROGMEM = "port";
prog_char str884[] PROGMEM = "unusual";
prog_char str885[] PROGMEM = "pilot";
prog_char str886[] PROGMEM = "obligation";
prog_char str887[] PROGMEM = "yellow";
prog_char str888[] PROGMEM = "phrase";
prog_char str889[] PROGMEM = "lucky";
prog_char str890[] PROGMEM = "novel";
prog_char str891[] PROGMEM = "poem";
prog_char str892[] PROGMEM = "suspect";
prog_char str893[] PROGMEM = "wake";
prog_char str894[] PROGMEM = "daily";
prog_char str895[] PROGMEM = "abroad";
prog_char str896[] PROGMEM = "keen";
prog_char str897[] PROGMEM = "guess";
prog_char str898[] PROGMEM = "valuable";
prog_char str899[] PROGMEM = "layer";
prog_char str900[] PROGMEM = "judgment";
prog_char str901[] PROGMEM = "illness";
prog_char str902[] PROGMEM = "cake";
prog_char str903[] PROGMEM = "tired";
prog_char str904[] PROGMEM = "beer";
prog_char str905[] PROGMEM = "federal";
prog_char str906[] PROGMEM = "boss";
prog_char str907[] PROGMEM = "urge";
prog_char str908[] PROGMEM = "being";
prog_char str909[] PROGMEM = "guard";
prog_char str910[] PROGMEM = "bomb";
prog_char str911[] PROGMEM = "dollar";
prog_char str912[] PROGMEM = "mood";
prog_char str913[] PROGMEM = "habit";
prog_char str914[] PROGMEM = "cook";
prog_char str915[] PROGMEM = "kiss";
prog_char str916[] PROGMEM = "wedding";
prog_char str917[] PROGMEM = "bishop";
prog_char str918[] PROGMEM = "mirror";
prog_char str919[] PROGMEM = "symptom";
prog_char str920[] PROGMEM = "global";
prog_char str921[] PROGMEM = "aside";
prog_char str922[] PROGMEM = "philosophy";
prog_char str923[] PROGMEM = "gallery";
prog_char str924[] PROGMEM = "emotional";
prog_char str925[] PROGMEM = "cigarette";
prog_char str926[] PROGMEM = "adequate";
prog_char str927[] PROGMEM = "pale";
prog_char str928[] PROGMEM = "input";
prog_char str929[] PROGMEM = "phenomenon";
prog_char str930[] PROGMEM = "kick";
prog_char str931[] PROGMEM = "smoke";
prog_char str932[] PROGMEM = "festival";
prog_char str933[] PROGMEM = "rose";
prog_char str934[] PROGMEM = "swing";
prog_char str935[] PROGMEM = "sweet";
prog_char str936[] PROGMEM = "rent";
prog_char str937[] PROGMEM = "enquiry";
prog_char str938[] PROGMEM = "abuse";
prog_char str939[] PROGMEM = "cottage";
prog_char str940[] PROGMEM = "pause";
prog_char str941[] PROGMEM = "mixture";
prog_char str942[] PROGMEM = "sick";
prog_char str943[] PROGMEM = "lake";
prog_char str944[] PROGMEM = "sand";
prog_char str945[] PROGMEM = "knife";
prog_char str946[] PROGMEM = "sake";
prog_char str947[] PROGMEM = "maximum";
prog_char str948[] PROGMEM = "rush";
prog_char str949[] PROGMEM = "numerous";
prog_char str950[] PROGMEM = "honour";
prog_char str951[] PROGMEM = "fellow";
prog_char str952[] PROGMEM = "tight";
prog_char str953[] PROGMEM = "anxiety";
prog_char str954[] PROGMEM = "poverty";
prog_char str955[] PROGMEM = "pipe";
prog_char str956[] PROGMEM = "split";
prog_char str957[] PROGMEM = "joke";
prog_char str958[] PROGMEM = "fewer";
prog_char str959[] PROGMEM = "inevitably";
prog_char str960[] PROGMEM = "nervous";
prog_char str961[] PROGMEM = "dose";
prog_char str962[] PROGMEM = "alcohol";
prog_char str963[] PROGMEM = "enhance";
prog_char str964[] PROGMEM = "tail";
prog_char str965[] PROGMEM = "smell";
prog_char str966[] PROGMEM = "architecture";
prog_char str967[] PROGMEM = "barrier";
prog_char str968[] PROGMEM = "pitch";
prog_char str969[] PROGMEM = "tackle";
prog_char str970[] PROGMEM = "evaluation";
prog_char str971[] PROGMEM = "dimension";
prog_char str972[] PROGMEM = "awful";
prog_char str973[] PROGMEM = "pink";
prog_char str974[] PROGMEM = "vessel";
prog_char str975[] PROGMEM = "lower";
prog_char str976[] PROGMEM = "fixed";
prog_char str977[] PROGMEM = "bite";
prog_char str978[] PROGMEM = "lecture";
prog_char str979[] PROGMEM = "leisure";
prog_char str980[] PROGMEM = "fabric";
prog_char str981[] PROGMEM = "cease";
prog_char str982[] PROGMEM = "isolated";
prog_char str983[] PROGMEM = "snap";
prog_char str984[] PROGMEM = "reward";
prog_char str985[] PROGMEM = "vegetable";
prog_char str986[] PROGMEM = "junior";
prog_char str987[] PROGMEM = "mystery";
prog_char str988[] PROGMEM = "upset";
prog_char str989[] PROGMEM = "till";
prog_char str990[] PROGMEM = "tissue";
prog_char str991[] PROGMEM = "turnover";
prog_char str992[] PROGMEM = "shrug";
prog_char str993[] PROGMEM = "ceiling";
prog_char str994[] PROGMEM = "sail";
prog_char str995[] PROGMEM = "adjust";
prog_char str996[] PROGMEM = "running";
prog_char str997[] PROGMEM = "adapt";
prog_char str998[] PROGMEM = "blind";
prog_char str999[] PROGMEM = "wire";
prog_char str1000[] PROGMEM = "squad";
prog_char str1001[] PROGMEM = "bowl";
prog_char str1002[] PROGMEM = "uncertainty";
prog_char str1003[] PROGMEM = "reinforce";
prog_char str1004[] PROGMEM = "dust";
prog_char str1005[] PROGMEM = "album";
prog_char str1006[] PROGMEM = "amendment";
prog_char str1007[] PROGMEM = "tube";
prog_char str1008[] PROGMEM = "coin";
prog_char str1009[] PROGMEM = "diary";
prog_char str1010[] PROGMEM = "flesh";
prog_char str1011[] PROGMEM = "rugby";
prog_char str1012[] PROGMEM = "psychology";
prog_char str1013[] PROGMEM = "seize";
prog_char str1014[] PROGMEM = "button";
prog_char str1015[] PROGMEM = "ultimate";
prog_char str1016[] PROGMEM = "venture";
prog_char str1017[] PROGMEM = "timber";
prog_char str1018[] PROGMEM = "gesture";
prog_char str1019[] PROGMEM = "jury";
prog_char str1020[] PROGMEM = "gaze";
prog_char str1021[] PROGMEM = "sophisticated";
prog_char str1022[] PROGMEM = "taxation";
prog_char str1023[] PROGMEM = "asleep";
prog_char str1024[] PROGMEM = "aids";
prog_char str1025[] PROGMEM = "multiple";
prog_char str1026[] PROGMEM = "evolution";
prog_char str1027[] PROGMEM = "grin";
prog_char str1028[] PROGMEM = "punishment";
prog_char str1029[] PROGMEM = "twin";
prog_char str1030[] PROGMEM = "tunnel";
prog_char str1031[] PROGMEM = "wipe";
prog_char str1032[] PROGMEM = "nowhere";
prog_char str1033[] PROGMEM = "loose";
prog_char str1034[] PROGMEM = "crash";
prog_char str1035[] PROGMEM = "logic";
prog_char str1036[] PROGMEM = "alarm";
prog_char str1037[] PROGMEM = "wrap";
prog_char str1038[] PROGMEM = "inadequate";
prog_char str1039[] PROGMEM = "ambition";
prog_char str1040[] PROGMEM = "anticipate";
prog_char str1041[] PROGMEM = "fibre";
prog_char str1042[] PROGMEM = "petrol";
prog_char str1043[] PROGMEM = "maker";
prog_char str1044[] PROGMEM = "acute";
prog_char str1045[] PROGMEM = "rubbish";
prog_char str1046[] PROGMEM = "duke";
prog_char str1047[] PROGMEM = "dictionary";
prog_char str1048[] PROGMEM = "fence";
prog_char str1049[] PROGMEM = "bike";
prog_char str1050[] PROGMEM = "eliminate";
prog_char str1051[] PROGMEM = "plot";
prog_char str1052[] PROGMEM = "mutual";
prog_char str1053[] PROGMEM = "hesitate";
prog_char str1054[] PROGMEM = "rope";
prog_char str1055[] PROGMEM = "gear";
prog_char str1056[] PROGMEM = "swallow";
prog_char str1057[] PROGMEM = "rabbit";
prog_char str1058[] PROGMEM = "tide";
prog_char str1059[] PROGMEM = "brush";
prog_char str1060[] PROGMEM = "piano";
prog_char str1061[] PROGMEM = "wound";
prog_char str1062[] PROGMEM = "bible";
prog_char str1063[] PROGMEM = "hire";
prog_char str1064[] PROGMEM = "gastric";
prog_char str1065[] PROGMEM = "humour";
prog_char str1066[] PROGMEM = "bulk";
prog_char str1067[] PROGMEM = "emission";
prog_char str1068[] PROGMEM = "naked";
prog_char str1069[] PROGMEM = "toilet";
prog_char str1070[] PROGMEM = "molecule";
prog_char str1071[] PROGMEM = "fiction";
prog_char str1072[] PROGMEM = "cinema";
prog_char str1073[] PROGMEM = "myth";
prog_char str1074[] PROGMEM = "enforce";
prog_char str1075[] PROGMEM = "pond";
prog_char str1076[] PROGMEM = "diplomatic";
prog_char str1077[] PROGMEM = "rebel";
prog_char str1078[] PROGMEM = "loyalty";
prog_char str1079[] PROGMEM = "physically";
prog_char str1080[] PROGMEM = "biological";
prog_char str1081[] PROGMEM = "hierarchy";
prog_char str1082[] PROGMEM = "seminar";
prog_char str1083[] PROGMEM = "verse";
prog_char str1084[] PROGMEM = "echo";
prog_char str1085[] PROGMEM = "cupboard";
prog_char str1086[] PROGMEM = "gang";
prog_char str1087[] PROGMEM = "unhappy";
prog_char str1088[] PROGMEM = "aggressive";
prog_char str1089[] PROGMEM = "rumour";
prog_char str1090[] PROGMEM = "availability";
prog_char str1091[] PROGMEM = "ocean";
prog_char str1092[] PROGMEM = "sadly";
prog_char str1093[] PROGMEM = "oral";
prog_char str1094[] PROGMEM = "hungry";
prog_char str1095[] PROGMEM = "romantic";
prog_char str1096[] PROGMEM = "hint";
prog_char str1097[] PROGMEM = "nasty";
prog_char str1098[] PROGMEM = "duration";
prog_char str1099[] PROGMEM = "amazing";
prog_char str1100[] PROGMEM = "crystal";
prog_char str1101[] PROGMEM = "loud";
prog_char str1102[] PROGMEM = "fade";
prog_char str1103[] PROGMEM = "unnecessary";
prog_char str1104[] PROGMEM = "ingredient";
prog_char str1105[] PROGMEM = "dull";
prog_char str1106[] PROGMEM = "sculpture";
prog_char str1107[] PROGMEM = "lion";
prog_char str1108[] PROGMEM = "fantasy";
prog_char str1109[] PROGMEM = "lamp";
prog_char str1110[] PROGMEM = "pavement";
prog_char str1111[] PROGMEM = "peer";
prog_char str1112[] PROGMEM = "nest";
prog_char str1113[] PROGMEM = "lung";
prog_char str1114[] PROGMEM = "teenager";
prog_char str1115[] PROGMEM = "heel";
prog_char str1116[] PROGMEM = "update";
prog_char str1117[] PROGMEM = "giant";
prog_char str1118[] PROGMEM = "sphere";
prog_char str1119[] PROGMEM = "sword";
prog_char str1120[] PROGMEM = "tutor";
prog_char str1121[] PROGMEM = "downstairs";
prog_char str1122[] PROGMEM = "cafe";
prog_char str1123[] PROGMEM = "fetch";
prog_char str1124[] PROGMEM = "duck";
prog_char str1125[] PROGMEM = "digital";
prog_char str1126[] PROGMEM = "inherit";
prog_char str1127[] PROGMEM = "chronic";
prog_char str1128[] PROGMEM = "orchestra";
prog_char str1129[] PROGMEM = "systematic";
prog_char str1130[] PROGMEM = "navy";
prog_char str1131[] PROGMEM = "juice";
prog_char str1132[] PROGMEM = "nightmare";
prog_char str1133[] PROGMEM = "slave";
prog_char str1134[] PROGMEM = "elbow";
prog_char str1135[] PROGMEM = "smart";
prog_char str1136[] PROGMEM = "vanish";
prog_char str1137[] PROGMEM = "upwards";
prog_char str1138[] PROGMEM = "geographical";
prog_char str1139[] PROGMEM = "embrace";
prog_char str1140[] PROGMEM = "endless";
prog_char str1141[] PROGMEM = "dawn";
prog_char str1142[] PROGMEM = "neutral";
prog_char str1143[] PROGMEM = "vague";
prog_char str1144[] PROGMEM = "tumour";
prog_char str1145[] PROGMEM = "dump";
prog_char str1146[] PROGMEM = "segment";
prog_char str1147[] PROGMEM = "aunt";
prog_char str1148[] PROGMEM = "fluid";
prog_char str1149[] PROGMEM = "riot";
prog_char str1150[] PROGMEM = "hazard";
prog_char str1151[] PROGMEM = "fitness";
prog_char str1152[] PROGMEM = "rally";
prog_char str1153[] PROGMEM = "vein";
prog_char str1154[] PROGMEM = "awkward";
prog_char str1155[] PROGMEM = "liquid";
prog_char str1156[] PROGMEM = "nail";
prog_char str1157[] PROGMEM = "atom";
prog_char str1158[] PROGMEM = "auction";
prog_char str1159[] PROGMEM = "takeover";
prog_char str1160[] PROGMEM = "rigid";
prog_char str1161[] PROGMEM = "hedge";
prog_char str1162[] PROGMEM = "eagle";
prog_char str1163[] PROGMEM = "peculiar";
prog_char str1164[] PROGMEM = "bold";
prog_char str1165[] PROGMEM = "saying";
prog_char str1166[] PROGMEM = "yacht";
prog_char str1167[] PROGMEM = "fossil";
prog_char str1168[] PROGMEM = "lump";
prog_char str1169[] PROGMEM = "mobility";
prog_char str1170[] PROGMEM = "goodness";
prog_char str1171[] PROGMEM = "onwards";
prog_char str1172[] PROGMEM = "cave";
prog_char str1173[] PROGMEM = "erect";
prog_char str1174[] PROGMEM = "tuck";
prog_char str1175[] PROGMEM = "ruin";
prog_char str1176[] PROGMEM = "dedicate";
prog_char str1177[] PROGMEM = "bucket";
prog_char str1178[] PROGMEM = "migration";
prog_char str1179[] PROGMEM = "ritual";
prog_char str1180[] PROGMEM = "dioxide";
prog_char str1181[] PROGMEM = "soap";
prog_char str1182[] PROGMEM = "bias";
prog_char str1183[] PROGMEM = "eyebrow";
prog_char str1184[] PROGMEM = "irrelevant";
prog_char str1185[] PROGMEM = "ankle";
prog_char str1186[] PROGMEM = "tobacco";
prog_char str1187[] PROGMEM = "sauce";
prog_char str1188[] PROGMEM = "bunch";
prog_char str1189[] PROGMEM = "hook";
prog_char str1190[] PROGMEM = "wicket";
prog_char str1191[] PROGMEM = "donation";
prog_char str1192[] PROGMEM = "tyre";
prog_char str1193[] PROGMEM = "elaborate";
prog_char str1194[] PROGMEM = "dilemma";
prog_char str1195[] PROGMEM = "utterly";
prog_char str1196[] PROGMEM = "unpleasant";
prog_char str1197[] PROGMEM = "sacred";
prog_char str1198[] PROGMEM = "obey";
prog_char str1199[] PROGMEM = "jeans";
prog_char str1200[] PROGMEM = "boyfriend";
prog_char str1201[] PROGMEM = "viewer";
prog_char str1202[] PROGMEM = "melt";
prog_char str1203[] PROGMEM = "pump";
prog_char str1204[] PROGMEM = "erosion";
prog_char str1205[] PROGMEM = "lemon";
prog_char str1206[] PROGMEM = "doing";
prog_char str1207[] PROGMEM = "sunlight";
prog_char str1208[] PROGMEM = "skull";
prog_char str1209[] PROGMEM = "inland";
prog_char str1210[] PROGMEM = "bless";
prog_char str1211[] PROGMEM = "toss";
prog_char str1212[] PROGMEM = "jewellery";
prog_char str1213[] PROGMEM = "fond";
prog_char str1214[] PROGMEM = "mammal";
prog_char str1215[] PROGMEM = "sovereignty";
prog_char str1216[] PROGMEM = "enzyme";
prog_char str1217[] PROGMEM = "laser";
prog_char str1218[] PROGMEM = "synthesis";
prog_char str1219[] PROGMEM = "fusion";
prog_char str1220[] PROGMEM = "onion";
prog_char str1221[] PROGMEM = "rage";
prog_char str1222[] PROGMEM = "toxic";
prog_char str1223[] PROGMEM = "apology";
prog_char str1224[] PROGMEM = "cylinder";
prog_char str1225[] PROGMEM = "cognitive";
prog_char str1226[] PROGMEM = "gothic";
prog_char str1227[] PROGMEM = "hydrogen";
prog_char str1228[] PROGMEM = "usage";
prog_char str1229[] PROGMEM = "jail";
prog_char str1230[] PROGMEM = "fertility";
prog_char str1231[] PROGMEM = "wolf";
prog_char str1232[] PROGMEM = "orthodox";
prog_char str1233[] PROGMEM = "jazz";
prog_char str1234[] PROGMEM = "boil";
prog_char str1235[] PROGMEM = "keyboard";
prog_char str1236[] PROGMEM = "hammer";
prog_char str1237[] PROGMEM = "given";
prog_char str1238[] PROGMEM = "dual";
prog_char str1239[] PROGMEM = "pepper";
prog_char str1240[] PROGMEM = "ethical";
prog_char str1241[] PROGMEM = "bicycle";
prog_char str1242[] PROGMEM = "luxury";
prog_char str1243[] PROGMEM = "glimpse";
prog_char str1244[] PROGMEM = "alert";
prog_char str1245[] PROGMEM = "node";
prog_char str1246[] PROGMEM = "lodge";
prog_char str1247[] PROGMEM = "ignorance";
prog_char str1248[] PROGMEM = "abbey";
prog_char str1249[] PROGMEM = "lobby";
prog_char str1250[] PROGMEM = "tolerate";
prog_char str1251[] PROGMEM = "pudding";
prog_char str1252[] PROGMEM = "cage";
prog_char str1253[] PROGMEM = "ashamed";
prog_char str1254[] PROGMEM = "amid";
prog_char str1255[] PROGMEM = "exotic";
prog_char str1256[] PROGMEM = "coherent";
prog_char str1257[] PROGMEM = "amuse";
prog_char str1258[] PROGMEM = "torch";
prog_char str1259[] PROGMEM = "arbitrary";
prog_char str1260[] PROGMEM = "haul";
prog_char str1261[] PROGMEM = "sediment";
prog_char str1262[] PROGMEM = "lexical";
prog_char str1263[] PROGMEM = "uphold";
prog_char str1264[] PROGMEM = "sketch";
prog_char str1265[] PROGMEM = "rifle";
prog_char str1266[] PROGMEM = "yarn";
prog_char str1267[] PROGMEM = "bizarre";
prog_char str1268[] PROGMEM = "sniff";
prog_char str1269[] PROGMEM = "daylight";
prog_char str1270[] PROGMEM = "nuisance";
prog_char str1271[] PROGMEM = "vivid";
prog_char str1272[] PROGMEM = "rude";
prog_char str1273[] PROGMEM = "velocity";
prog_char str1274[] PROGMEM = "vocational";
prog_char str1275[] PROGMEM = "fever";
prog_char str1276[] PROGMEM = "cutting";
prog_char str1277[] PROGMEM = "agony";
prog_char str1278[] PROGMEM = "poison";
prog_char str1279[] PROGMEM = "ribbon";
prog_char str1280[] PROGMEM = "unwilling";
prog_char str1281[] PROGMEM = "irony";
prog_char str1282[] PROGMEM = "toast";
prog_char str1283[] PROGMEM = "aluminium";
prog_char str1284[] PROGMEM = "unreasonable";
prog_char str1285[] PROGMEM = "dash";
prog_char str1286[] PROGMEM = "vigorous";
prog_char str1287[] PROGMEM = "asylum";
prog_char str1288[] PROGMEM = "rhetoric";
prog_char str1289[] PROGMEM = "jockey";
prog_char str1290[] PROGMEM = "viable";
prog_char str1291[] PROGMEM = "unsuccessful";
prog_char str1292[] PROGMEM = "hemisphere";
prog_char str1293[] PROGMEM = "nominate";
prog_char str1294[] PROGMEM = "arena";
prog_char str1295[] PROGMEM = "vacuum";
prog_char str1296[] PROGMEM = "jerk";
prog_char str1297[] PROGMEM = "rehearsal";
prog_char str1298[] PROGMEM = "bake";
prog_char str1299[] PROGMEM = "vector";
prog_char str1300[] PROGMEM = "robbery";
prog_char str1301[] PROGMEM = "aviation";
prog_char str1302[] PROGMEM = "kettle";
prog_char str1303[] PROGMEM = "gown";
prog_char str1304[] PROGMEM = "gospel";
prog_char str1305[] PROGMEM = "hectare";
prog_char str1306[] PROGMEM = "hobby";
prog_char str1307[] PROGMEM = "boxing";
prog_char str1308[] PROGMEM = "upgrade";
prog_char str1309[] PROGMEM = "sideways";
prog_char str1310[] PROGMEM = "noun";
prog_char str1311[] PROGMEM = "sodium";
prog_char str1312[] PROGMEM = "rotation";
prog_char str1313[] PROGMEM = "syllable";
prog_char str1314[] PROGMEM = "veteran";
prog_char str1315[] PROGMEM = "ulcer";
prog_char str1316[] PROGMEM = "voucher";
prog_char str1317[] PROGMEM = "odour";
prog_char str1318[] PROGMEM = "pioneer";
prog_char str1319[] PROGMEM = "nitrogen";
prog_char str1320[] PROGMEM = "pedestrian";
prog_char str1321[] PROGMEM = "microphone";
prog_char str1322[] PROGMEM = "hastily";
prog_char str1323[] PROGMEM = "dozen";
prog_char str1324[] PROGMEM = "omission";
prog_char str1325[] PROGMEM = "orbit";
prog_char str1326[] PROGMEM = "slump";
prog_char str1327[] PROGMEM = "puzzle";
prog_char str1328[] PROGMEM = "cemetery";
prog_char str1329[] PROGMEM = "nephew";
prog_char str1330[] PROGMEM = "astonishing";
prog_char str1331[] PROGMEM = "bubble";
prog_char str1332[] PROGMEM = "ascertain";
prog_char str1333[] PROGMEM = "pigeon";
prog_char str1334[] PROGMEM = "municipal";
prog_char str1335[] PROGMEM = "sulphur";
prog_char str1336[] PROGMEM = "voyage";
prog_char str1337[] PROGMEM = "siege";
prog_char str1338[] PROGMEM = "eternal";
prog_char str1339[] PROGMEM = "ample";
prog_char str1340[] PROGMEM = "lazy";
prog_char str1341[] PROGMEM = "kidney";
prog_char str1342[] PROGMEM = "pest";
prog_char str1343[] PROGMEM = "bump";
prog_char str1344[] PROGMEM = "athlete";
prog_char str1345[] PROGMEM = "bail";
prog_char str1346[] PROGMEM = "abnormal";
prog_char str1347[] PROGMEM = "hover";
//end three letter combos
//begin four letter combos
//end four letter combos
PROGMEM const char *oneCombos[] = 	   
{   
  str0,
  str1,
  str2,
  str3,
  str4,
  str5,
  str6,
  str7,
  str8,
  str9,
  str10,
  str11,
  str12,
  str13,
  str14,
  str15,
  str16,
  str17,
  str18,
  str19,
  str20,
  str21,
  str22,
  str23,
  str24,
  str25,//start 2 combos
  str26,
  str27,
  str28,
  str29,
  str30,
  str31,
  str32,
  str33,
  str34,
  str35,
  str36,
  str37,
  str38,
  str39,
  str40,
  str41,
  str42,
  str43,
  str44,
  str45,
  str46,
  str47,
  str48,
  str49,
  str50,
  str51,
  str52,
  str53,
  str54,
  str55,
  str56,
  str57,
  str58,
  str59,
  str60,
  str61,
  str62,
  str63,
  str64,
  str65,
  str66,
  str67,
  str68,
  str69,
  str70,
  str71,
  str72,
  str73,
  str74,
  str75,
  str76,
  str77,
  str78,
  str79,
  str80,
  str81,
  str82,
  str83,
  str84,
  str85,
  str86,
  str87,
  str88,
  str89,
  str90,
  str91,
  str92,
  str93,
  str94,
  str95,
  str96,
  str97,
  str98,
  str99,
  str100,
  str101,
  str102,
  str103,
  str104,
  str105,
  str106,
  str107,
  str108,
  str109,
  str110,
  str111,
  str112,
  str113,
  str114,
  str115,
  str116,
  str117,
  str118,
  str119,
  str120,
  str121,
  str122,
  str123,
  str124,
  str125,
  str126,
  str127,
  str128,
  str129,
  str130,
  str131,
  str132,
  str133,
  str134,
  str135,
  str136,
  str137,
  str138,
  str139,
  str140,
  str141,
  str142,
  str143,
  str144,
  str145,
  str146,
  str147,
  str148,
  str149,
  str150,
  str151,
  str152,
  str153,
  str154,
  str155,
  str156,
  str157,
  str158,
  str159,
  str160,
  str161,
  str162,
  str163,
  str164,
  str165,
  str166,
  str167,
  str168,
  str169,
  str170,
  str171,
  str172,
  str173,
  str174,
  str175,
  str176,
  str177,
  str178,
  str179,
  str180,
  str181,
  str182,
  str183,
  str184,
  str185,
  str186,
  str187,
  str188,
  str189,
  str190,
  str191,
  str192,
  str193,
  str194,
  str195,
  str196,
  str197,
  str198,
  str199,
  str200,
  str201,
  str202,
  str203,
  str204,
  str205,
  str206,
  str207,
  str208,
  str209,
  str210,
  str211,
  str212,
  str213,
  str214,
  str215,
  str216,
  str217,
  str218,
  str219,
  str220,
  str221,
  str222,
  str223,
  str224,
  str225,
  str226,//start 3 combos
  str227,
  str228,
  str229,
  str230,
  str231,
  str232,
  str233,
  str234,
  str235,
  str236,
  str237,
  str238,
  str239,
  str240,
  str241,
  str242,
  str243,
  str244,
  str245,
  str246,
  str247,
  str248,
  str249,
  str250,
  str251,
  str252,
  str253,
  str254,
  str255,
  str256,
  str257,
  str258,
  str259,
  str260,
  str261,
  str262,
  str263,
  str264,
  str265,
  str266,
  str267,
  str268,
  str269,
  str270,
  str271,
  str272,
  str273,
  str274,
  str275,
  str276,
  str277,
  str278,
  str279,
  str280,
  str281,
  str282,
  str283,
  str284,
  str285,
  str286,
  str287,
  str288,
  str289,
  str290,
  str291,
  str292,
  str293,
  str294,
  str295,
  str296,
  str297,
  str298,
  str299,
  str300,
  str301,
  str302,
  str303,
  str304,
  str305,
  str306,
  str307,
  str308,
  str309,
  str310,
  str311,
  str312,
  str313,
  str314,
  str315,
  str316,
  str317,
  str318,
  str319,
  str320,
  str321,
  str322,
  str323,
  str324,
  str325,
  str326,
  str327,
  str328,
  str329,
  str330,
  str331,
  str332,
  str333,
  str334,
  str335,
  str336,
  str337,
  str338,
  str339,
  str340,
  str341,
  str342,
  str343,
  str344,
  str345,
  str346,
  str347,
  str348,
  str349,
  str350,
  str351,
  str352,
  str353,
  str354,
  str355,
  str356,
  str357,
  str358,
  str359,
  str360,
  str361,
  str362,
  str363,
  str364,
  str365,
  str366,
  str367,
  str368,
  str369,
  str370,
  str371,
  str372,
  str373,
  str374,
  str375,
  str376,
  str377,
  str378,
  str379,
  str380,
  str381,
  str382,
  str383,
  str384,
  str385,
  str386,
  str387,
  str388,
  str389,
  str390,
  str391,
  str392,
  str393,
  str394,
  str395,
  str396,
  str397,
  str398,
  str399,
  str400,
  str401,
  str402,
  str403,
  str404,
  str405,
  str406,
  str407,
  str408,
  str409,
  str410,
  str411,
  str412,
  str413,
  str414,
  str415,
  str416,
  str417,
  str418,
  str419,
  str420,
  str421,
  str422,
  str423,
  str424,
  str425,
  str426,
  str427,
  str428,
  str429,
  str430,
  str431,
  str432,
  str433,
  str434,
  str435,
  str436,
  str437,
  str438,
  str439,
  str440,
  str441,
  str442,
  str443,
  str444,
  str445,
  str446,
  str447,
  str448,
  str449,
  str450,
  str451,
  str452,
  str453,
  str454,
  str455,
  str456,
  str457,
  str458,
  str459,
  str460,
  str461,
  str462,
  str463,
  str464,
  str465,
  str466,
  str467,
  str468,
  str469,
  str470,
  str471,
  str472,
  str473,
  str474,
  str475,
  str476,
  str477,
  str478,
  str479,
  str480,
  str481,
  str482,
  str483,
  str484,
  str485,
  str486,
  str487,
  str488,
  str489,
  str490,
  str491,
  str492,
  str493,
  str494,
  str495,
  str496,
  str497,
  str498,
  str499,
  str500,
  str501,
  str502,
  str503,
  str504,
  str505,
  str506,
  str507,
  str508,
  str509,
  str510,
  str511,
  str512,
  str513,
  str514,
  str515,
  str516,
  str517,
  str518,
  str519,
  str520,
  str521,
  str522,
  str523,
  str524,
  str525,
  str526,
  str527,
  str528,
  str529,
  str530,
  str531,
  str532,
  str533,
  str534,
  str535,
  str536,
  str537,
  str538,
  str539,
  str540,
  str541,
  str542,
  str543,
  str544,
  str545,
  str546,
  str547,
  str548,
  str549,
  str550,
  str551,
  str552,
  str553,
  str554,
  str555,
  str556,
  str557,
  str558,
  str559,
  str560,
  str561,
  str562,
  str563,
  str564,
  str565,
  str566,
  str567,
  str568,
  str569,
  str570,
  str571,
  str572,
  str573,
  str574,
  str575,
  str576,
  str577,
  str578,
  str579,
  str580,
  str581,
  str582,
  str583,
  str584,
  str585,
  str586,
  str587,
  str588,
  str589,
  str590,
  str591,
  str592,
  str593,
  str594,
  str595,
  str596,
  str597,
  str598,
  str599,
  str600,
  str601,
  str602,
  str603,
  str604,
  str605,
  str606,
  str607,
  str608,
  str609,
  str610,
  str611,
  str612,
  str613,
  str614,
  str615,
  str616,
  str617,
  str618,
  str619,
  str620,
  str621,
  str622,
  str623,
  str624,
  str625,
  str626,
  str627,
  str628,
  str629,
  str630,
  str631,
  str632,
  str633,
  str634,
  str635,
  str636,
  str637,
  str638,
  str639,
  str640,
  str641,
  str642,
  str643,
  str644,
  str645,
  str646,
  str647,
  str648,
  str649,
  str650,
  str651,
  str652,
  str653,
  str654,
  str655,
  str656,
  str657,
  str658,
  str659,
  str660,
  str661,
  str662,
  str663,
  str664,
  str665,
  str666,
  str667,
  str668,
  str669,
  str670,
  str671,
  str672,
  str673,
  str674,
  str675,
  str676,
  str677,
  str678,
  str679,
  str680,
  str681,
  str682,
  str683,
  str684,
  str685,
  str686,
  str687,
  str688,
  str689,
  str690,
  str691,
  str692,
  str693,
  str694,
  str695,
  str696,
  str697,
  str698,
  str699,
  str700,
  str701,
  str702,
  str703,
  str704,
  str705,
  str706,
  str707,
  str708,
  str709,
  str710,
  str711,
  str712,
  str713,
  str714,
  str715,
  str716,
  str717,
  str718,
  str719,
  str720,
  str721,
  str722,
  str723,
  str724,
  str725,
  str726,
  str727,
  str728,
  str729,
  str730,
  str731,
  str732,
  str733,
  str734,
  str735,
  str736,
  str737,
  str738,
  str739,
  str740,
  str741,
  str742,
  str743,
  str744,
  str745,
  str746,
  str747,
  str748,
  str749,
  str750,
  str751,
  str752,
  str753,
  str754,
  str755,
  str756,
  str757,
  str758,
  str759,
  str760,
  str761,
  str762,
  str763,
  str764,
  str765,
  str766,
  str767,
  str768,
  str769,
  str770,
  str771,
  str772,
  str773,
  str774,
  str775,
  str776,
  str777,
  str778,
  str779,
  str780,
  str781,
  str782,
  str783,
  str784,
  str785,
  str786,
  str787,
  str788,
  str789,
  str790,
  str791,
  str792,
  str793,
  str794,
  str795,
  str796,
  str797,
  str798,
  str799,
  str800,
  str801,
  str802,
  str803,
  str804,
  str805,
  str806,
  str807,
  str808,
  str809,
  str810,
  str811,
  str812,
  str813,
  str814,
  str815,
  str816,
  str817,
  str818,
  str819,
  str820,
  str821,
  str822,
  str823,
  str824,
  str825,
  str826,
  str827,
  str828,
  str829,
  str830,
  str831,
  str832,
  str833,
  str834,
  str835,
  str836,
  str837,
  str838,
  str839,
  str840,
  str841,
  str842,
  str843,
  str844,
  str845,
  str846,
  str847,
  str848,
  str849,
  str850,
  str851,
  str852,
  str853,
  str854,
  str855,
  str856,
  str857,
  str858,
  str859,
  str860,
  str861,
  str862,
  str863,
  str864,
  str865,
  str866,
  str867,
  str868,
  str869,
  str870,
  str871,
  str872,
  str873,
  str874,
  str875,
  str876,
  str877,
  str878,
  str879,
  str880,
  str881,
  str882,
  str883,
  str884,
  str885,
  str886,
  str887,
  str888,
  str889,
  str890,
  str891,
  str892,
  str893,
  str894,
  str895,
  str896,
  str897,
  str898,
  str899,
  str900,
  str901,
  str902,
  str903,
  str904,
  str905,
  str906,
  str907,
  str908,
  str909,
  str910,
  str911,
  str912,
  str913,
  str914,
  str915,
  str916,
  str917,
  str918,
  str919,
  str920,
  str921,
  str922,
  str923,
  str924,
  str925,
  str926,
  str927,
  str928,
  str929,
  str930,
  str931,
  str932,
  str933,
  str934,
  str935,
  str936,
  str937,
  str938,
  str939,
  str940,
  str941,
  str942,
  str943,
  str944,
  str945,
  str946,
  str947,
  str948,
  str949,
  str950,
  str951,
  str952,
  str953,
  str954,
  str955,
  str956,
  str957,
  str958,
  str959,
  str960,
  str961,
  str962,
  str963,
  str964,
  str965,
  str966,
  str967,
  str968,
  str969,
  str970,
  str971,
  str972,
  str973,
  str974,
  str975,
  str976,
  str977,
  str978,
  str979,
  str980,
  str981,
  str982,
  str983,
  str984,
  str985,
  str986,
  str987,
  str988,
  str989,
  str990,
  str991,
  str992,
  str993,
  str994,
  str995,
  str996,
  str997,
  str998,
  str999,
  str1000,
  str1001,
  str1002,
  str1003,
  str1004,
  str1005,
  str1006,
  str1007,
  str1008,
  str1009,
  str1010,
  str1011,
  str1012,
  str1013,
  str1014,
  str1015,
  str1016,
  str1017,
  str1018,
  str1019,
  str1020,
  str1021,
  str1022,
  str1023,
  str1024,
  str1025,
  str1026,
  str1027,
  str1028,
  str1029,
  str1030,
  str1031,
  str1032,
  str1033,
  str1034,
  str1035,
  str1036,
  str1037,
  str1038,
  str1039,
  str1040,
  str1041,
  str1042,
  str1043,
  str1044,
  str1045,
  str1046,
  str1047,
  str1048,
  str1049,
  str1050,
  str1051,
  str1052,
  str1053,
  str1054,
  str1055,
  str1056,
  str1057,
  str1058,
  str1059,
  str1060,
  str1061,
  str1062,
  str1063,
  str1064,
  str1065,
  str1066,
  str1067,
  str1068,
  str1069,
  str1070,
  str1071,
  str1072,
  str1073,
  str1074,
  str1075,
  str1076,
  str1077,
  str1078,
  str1079,
  str1080,
  str1081,
  str1082,
  str1083,
  str1084,
  str1085,
  str1086,
  str1087,
  str1088,
  str1089,
  str1090,
  str1091,
  str1092,
  str1093,
  str1094,
  str1095,
  str1096,
  str1097,
  str1098,
  str1099,
  str1100,
  str1101,
  str1102,
  str1103,
  str1104,
  str1105,
  str1106,
  str1107,
  str1108,
  str1109,
  str1110,
  str1111,
  str1112,
  str1113,
  str1114,
  str1115,
  str1116,
  str1117,
  str1118,
  str1119,
  str1120,
  str1121,
  str1122,
  str1123,
  str1124,
  str1125,
  str1126,
  str1127,
  str1128,
  str1129,
  str1130,
  str1131,
  str1132,
  str1133,
  str1134,
  str1135,
  str1136,
  str1137,
  str1138,
  str1139,
  str1140,
  str1141,
  str1142,
  str1143,
  str1144,
  str1145,
  str1146,
  str1147,
  str1148,
  str1149,
  str1150,
  str1151,
  str1152,
  str1153,
  str1154,
  str1155,
  str1156,
  str1157,
  str1158,
  str1159,
  str1160,
  str1161,
  str1162,
  str1163,
  str1164,
  str1165,
  str1166,
  str1167,
  str1168,
  str1169,
  str1170,
  str1171,
  str1172,
  str1173,
  str1174,
  str1175,
  str1176,
  str1177,
  str1178,
  str1179,
  str1180,
  str1181,
  str1182,
  str1183,
  str1184,
  str1185,
  str1186,
  str1187,
  str1188,
  str1189,
  str1190,
  str1191,
  str1192,
  str1193,
  str1194,
  str1195,
  str1196,
  str1197,
  str1198,
  str1199,
  str1200,
  str1201,
  str1202,
  str1203,
  str1204,
  str1205,
  str1206,
  str1207,
  str1208,
  str1209,
  str1210,
  str1211,
  str1212,
  str1213,
  str1214,
  str1215,
  str1216,
  str1217,
  str1218,
  str1219,
  str1220,
  str1221,
  str1222,
  str1223,
  str1224,
  str1225,
  str1226,
  str1227,
  str1228,
  str1229,
  str1230,
  str1231,
  str1232,
  str1233,
  str1234,
  str1235,
  str1236,
  str1237,
  str1238,
  str1239,
  str1240,
  str1241,
  str1242,
  str1243,
  str1244,
  str1245,
  str1246,
  str1247,
  str1248,
  str1249,
  str1250,
  str1251,
  str1252,
  str1253,
  str1254,
  str1255,
  str1256,
  str1257,
  str1258,
  str1259,
  str1260,
  str1261,
  str1262,
  str1263,
  str1264,
  str1265,
  str1266,
  str1267,
  str1268,
  str1269,
  str1270,
  str1271,
  str1272,
  str1273,
  str1274,
  str1275,
  str1276,
  str1277,
  str1278,
  str1279,
  str1280,
  str1281,
  str1282,
  str1283,
  str1284,
  str1285,
  str1286,
  str1287,
  str1288,
  str1289,
  str1290,
  str1291,
  str1292,
  str1293,
  str1294,
  str1295,
  str1296,
  str1297,
  str1298,
  str1299,
  str1300,
  str1301,
  str1302,
  str1303,
  str1304,
  str1305,
  str1306,
  str1307,
  str1308,
  str1309,
  str1310,
  str1311,
  str1312,
  str1313,
  str1314,
  str1315,
  str1316,
  str1317,
  str1318,
  str1319,
  str1320,
  str1321,
  str1322,
  str1323,
  str1324,
  str1325,
  str1326,
  str1327,
  str1328,
  str1329,
  str1330,
  str1331,
  str1332,
  str1333,
  str1334,
  str1335,
  str1336,
  str1337,
  str1338,
  str1339,
  str1340,
  str1341,
  str1342,
  str1343,
  str1344,
  str1345,
  str1346,
  str1347,
};
word comboIndex[]={
  0,25,225,1348      }; //index reduces iterations

char buffer[15];
//large enough for the largest string it must hold
//###frequencies 
prog_char commonLetters[8][FREQ] PROGMEM=
//the eighth place accounts for uncommon letters
{
  {
    't','a','s','h','w','i','o','b','m','f','c','l','d'                                      }
  ,
  {
    'e','h','o','a','n','i','f','r','u','t','l','c','d'                                      }
  ,
  {
    'e','t','a','i','o','n','r','h','s','d','l','c','u'                                      }
  ,
  {
    'e','t','a','o','i','n','s','h','r','d','l','c','u'                                      }
  ,
  {
    'e','r','i','o','t','n','s','a','d','l','h','c','u'                                      }
  ,
  {
    'e','t','a','r','i','n','s','h','o','d','l','c','u'                                      }
  ,
  {
    'e','t','a','d','i','n','s','h','r','o','l','c','u'                                      }
  ,
  {
    'm','f','p','g','w','y','b','v','k','x','j','q','z'                                      }
};
//modifiers are assign as variables to pass to functions
prog_char supeRight= KEY_RIGHT_GUI;
prog_char lctrl = KEY_LEFT_CTRL;
prog_char rctrl = KEY_RIGHT_CTRL;
prog_char lshift = KEY_LEFT_SHIFT;
prog_char rshift = KEY_RIGHT_SHIFT;
prog_char lalt = KEY_LEFT_ALT;
prog_char ralt = KEY_RIGHT_ALT;
prog_char up = KEY_UP_ARROW;
prog_char down = KEY_DOWN_ARROW;
prog_char right = KEY_RIGHT_ARROW;
prog_char left = KEY_LEFT_ARROW;
prog_char HOME = KEY_HOME;
prog_char END = KEY_END;
prog_char f2 = KEY_F2;
prog_char f4 = KEY_F4;
prog_char f5 = KEY_F5;
prog_char f10 = KEY_F10;
prog_char f12 = KEY_F12;
prog_char tab = KEY_TAB;
prog_char RTN = KEY_RETURN;
#define BACK 178
#define REACT 300// time for computer host to react to commands

//substitutes
char compSug[] = {
  "word "};// this is a temp sub for autofill 

//-----------------------------------------------------------------------Set up
void setup()
{
  Keyboard.begin();
  //------------input setting
  for (byte set=0;set<NUMBUTTONS;set++)
  {
    //sets the button pins
    pinMode(buttons[set], INPUT);
    digitalWrite(buttons[set], HIGH); 
  }  
  //this sets pull-up resistor/ie input through 20k to 5v
  //in this way| input to button/button to ground, is proper and will read as low when pressed
  //-------------------------------------------------------------------------------------------- 

  //--check if this arduino's eeprom has been used for something else if it has clear the eeprom
  if (session(512, KEY))
  {
    clearPROM(0, 256);
    //clear possible old assignments
    //wait();
    //paulsMacro();// this opens a text editor for testing, comment this out
    wait();
    delay(REACT);
    Keyboard.print("Yes?");
    letterWrite(30, getValue(),0);
    sKey(4, BACK);
    Keyboard.print("no?");
    letterWrite(31, getValue(),0);
    sKey(3, BACK);
    Keyboard.print("meta?");
    letterWrite(32, getValue(),0);
    sKey(5, BACK);
    //promt and assign for yes/no
  }

  yes = word(EEPROM.read(60), EEPROM.read(61));
  no = word(EEPROM.read(62), EEPROM.read(63));
  meta= word(EEPROM.read(64), EEPROM.read(65));
  //put personal yes/no in ram so it doesn't need to be parsed from EEPROM
  learningPhase[0] = EEPROM.read(254);
  learningPhase[1]= EEPROM.read(255);
  //ascertain where we are in the learning process
}
//-----------------------------------------------------------------------------begin main loop
void loop()
{
  while(true)//to provide a way to start from step one with a return
  {
    //linesize limiter
    if(count[LINEC]>LINESIZE)
    {
      /*if(rJustify)
       {
       backTxt();
       }
       else
       {*/
      if(count[CWORD])
      {  
        sKey(count[CWORD],left);
      }
      sKey(1, RTN);
      count[LINEC]=count[CWORD];
      //};
    }
    //get the current button status 
    word chordValue=getValue();
    // if yes or no do those and restart the loop
    if(chordValue==yes)
    {
      yesCase();
      return;//restart the loop
    }
    if(chordValue==no)
    {
      noCase();
      return;//restart the loop
    }
    if(chordValue==meta)
    {
      metaCase();
      return;
    }
    //-------------------------------letter related steps
    //figure out if the chord is a letter that has an assignment in eeprom
    byte letteR=check(chordValue);
    //if no assignment take an educational guess and make one
    if(letteR == 0)
    {
      letteR=learnUser();
      //receive a letter from the learning algorithm
      assign(letteR, chordValue);
      // assign that letter to the kepmaping
    }
    if(letteR == 0 && learningPhase[1])
    {
      // !! filter currently finds minimal differance from an assigned chord
      //learningDone=true;
      letteR= filter(chordValue);//just returns # to represent noise
    }
    //print the result
    cleanSug();
    printLetter(letteR);
    autoSug();
  }
}
//--------------------------------------------------------------------------end main loop
//----------------------------storage fuctions---making assignments

void assign(byte letter, word chordValue)
{
  if(learningPhase[0])
  {
    letterWrite(letter, chordValue,SECONDLAY);
  }
  else
  {
    letterWrite(letter, chordValue, 0);
  };
  //make the assignment

}

void letterWrite(byte letter, word chordValue, byte modifier)
{
  //Writes unsigned ints into EEPROM
  byte hold = letter-modifier;
  EEPROM.write(hold*2, highByte(chordValue));
  delay(5);//!! delays after writes maybe redundent
  EEPROM.write(hold*2+1, lowByte(chordValue));
  delay(5);
}

//Checking
byte check(word chordValue)
{
  for(int address=194;address<246;address+=2)
    //for the first layout
  {
    //check there is something the equals the current chord
    if(word(EEPROM.read(address), EEPROM.read(address+1)) == chordValue)
    {
      return address/2;
      //translates to ascii number for the represented letter
      //first layout defined by a mutipul of 2
      // because ints need to be seperated into 2 bytes
    }
  }
  //continue given no matches are found
  for(int address=2;address<56; address+=2)
    //for the second layout
  {
    if(word(EEPROM.read(address), EEPROM.read(address+1)) == chordValue)
    {
      return address/2+SECONDLAY;
      //translates to ascii number for the represented letter
      //second layout defined by an offset
    }
  }
  //no assignments made for given cord, return 0 or false
  return 0;
}
//------------------------------------------------------------------------learning function 
//commit 1b3666ec7428a21a0a0f3eb8e5620b1c7ee8dfb3 has commited out previous learner

byte learnUser()//simple sub to the original 
{//for one assignment
  byte letter=learningSeq(0);
  if(letter)
  {
    return letter;
  }
  else
  {
    letter=freqLookup(8,0);
    if(letter)
    {
      return letter;
    }
  }
  learningPhase[1]=true;
  EEPROM.write(255, true);
}

byte learningSeq(byte modifier)
{
  byte letter;
  //Give most common unassiged letter based on possition in the word
  if(count[CWORD]<LPLACES)//for the amount of places acounted for
  { //look up the appropriate letter in the 2d array
    letter=freqLookup(count[CWORD], modifier);
    if (letter==0)// in the case they are taken
    {
      letter=freqLookup(8, modifier);
    }
    return letter;
  }
  else
  {
    letter=freqLookup(2, modifier);
    if (letter==0)
    {
      letter=freqLookup(8, modifier);
    }
    return letter;
  };
  return 0;
}

byte freqLookup(int place, byte modifier)
{
  byte letterNumber;
  for (int freq=0; freq<FREQ; freq++)
    //interate frequencies
  {
    letterNumber= pgm_read_byte(&commonLetters[place][freq]);
    if (oneCheck(letterNumber, modifier)==0)
      //check if the frequency is assigned for the given possition
    {
      return letterNumber;
      //return the availible letter that is most fequently used for this possition
    }
  }
  return 0;
}

byte likelyLetter()//#################
{
  for(word i=comboIndex[count[CWORD]];i<comboIndex[count[CWORD]+1];i++)
  {//for everthing in the appropriate part of the list
    strcpy_P(buffer, (char*)pgm_read_word(&(oneCombos[i])));
    //read the string to the buffer
    //!! needs to check by previous letters
    if (count[CWORD])
    {
      for(word j=0;j<count[CWORD];j++)
      {
        if(buffer[j]==letterBuffer[j])
        {
          if(count[CWORD]==j-1)//??
          {
            return buffer[count[CWORD]+1]
          } 
        }
        else
        {
          break;
        };
      }
    }
    else if (oneCheck(buffer[0],0)==0)
    {
      return buffer[0];
    }
  }
}

word oneCheck(byte letter, byte mod)
{
  byte pHold= letter-mod;
  return word(EEPROM.read(pHold*2), EEPROM.read(pHold*2+1));
}

//------------------------------------------noise filtering
byte filter(word noise)
{
  word correctToValue=9;//redundant?
  //cant be nine so if it prints this something is wrong
  int lowPoint=15;
  for(int address=194;address<246;address+=2)
  {
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

//------------------------------------------------------------------------------------------button functions
//---------------------------------------------------------------------------------returning the chord value
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
//------------------------------------------------------------------User facing automation and response 

//-------------------------------------------NO CASE
// this is a test
//!!both cases need work, there is a symantic issue with the number counting
void yesCase()
{
  //regular activity 
  switch(count[YESC])
  {
  case 0://yes
    space();
    break;
  case 1://yes, yes
    if(explicitMode==false)
    {
      period();
      break;
    }//in other words fall through when explicit mode is true
  case 2://yes, yes, yes
    enter();
    break;
  case 3://yes, yes, yes, yes
    combo(rctrl,'s');
    //save();
    break;
  default:
    count[YESC]=0;
    return;
  }
  count[YESC]++;
}
//###################################META/MAGIC
void metaCase()//or "magic" keys
{
  switch(count[METAC])
  {
  case 0:
    //modeSwitch(); // changes compisitional mode
    explicitMode= !explicitMode;
    break;
  case 1:
    //search();//open program
    combo(rctrl, ' ');//SEARCH# bring up synaps, linux
    break;
  case 2://places or web search ##!!need to cater to OS
    //webSearch();//open web site
    sKey(1, right);//move one to the right for places option
    break;
  case 3://files or web search ##!!need to cater to OS
    //fileSearch();//openlocal file
    sKey(5, right);//move five to the right for web option
    break;
  case 4:
    combo(rctrl, ' ');//close search 
  default:
    count[METAC]=0;
    return;
  }
  count[METAC]++;
}
//#####################################NO
void noCase()
{
  /*if(count[METAC])
   {
   //closeProgram();//alt-f4//meta, no
   combo(lalt, f4);//CLOSE# close window
   }
  /*else if(count[CSENT]==0 && count[LINEC]==0)//nothing case?
   {
   //back();//alt-left//nothing, no..
   combo(lalt, left);//BACK#browser back
   }*/
  //else 
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
    }
  }
  else if(count[CWORD])
  {
    backSpace();//no... forSpaces inWord
  }
  else
  {
    if(count[LWORD] && count[LINEC])
    {
      count[CSENT]-=count[LWORD];
      backlast(LWORD);//..no after aWord backspaced
    }
    else if (count[CSENT] && count[CSENT]<175 && count[LINEC])
    {
      backlast(CSENT);//..no after backword
      //revert to backspace after sentence removal
    }
    else
    {
      count[LSENT]=0;
      backSpace();
    };
  };
  count[NOC]++;
}
//#########################################Auto suggest
void autoSug()
{//makes a suggestion based on typed letters
  if (count[CWORD] && count[CWORD]<4)
  {//given the typed letters meet the list capibilities 
    for(word i=comboIndex[count[CWORD]-1];i<comboIndex[count[CWORD]];i++)
    {//for everthing in the list
      strcpy_P(buffer, (char*)pgm_read_word(&(oneCombos[i])));
      //read the called string number into the buffer
      for(word l=0;l<count[CWORD];l++)
        //for the letters currently typed
      {
        if(letterBuffer[l]==buffer[l])//compare each letter
        {
          if(l==count[CWORD]-1)//if this is the last comparable letter
          {
            //make the suggestion
            Keyboard.print(" ");
            sugSize=1;
            buffPrint();
            sKey(sugSize, left);
            return;//stop looking for suggestions
          }
          else{
            continue;// try the next letter          
          };//redundant?
        }
        else{//no match its not this word
          break;//short-curcuit        
        };
      }
      //no suggestion case here;
      sugSize=0;
    }
  }
}
void autoFill()
{
  sKey(sugSize, right);
  sKey(sugSize+count[LWORD],BACK);
  count[LINEC]+=sugSize;
  count[LINEC]-=count[LWORD];
  count[CSENT]+=sugSize;
  count[CSENT]-=count[LWORD];
  count[LWORD]=sugSize;
  buffPrint();
  Keyboard.write(' ');
}

void buffPrint()
{
  for(byte j=0;j<14;j++)
  {
    if(buffer[j])
    {
      Keyboard.write(buffer[j]);
      sugSize++;
    }
    else
    {
      return;
    };
  }
}

void cleanSug()
{
  buffer[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
  //clear the buffer?
  sKey(sugSize,right);
  sKey(sugSize,BACK);
  //clear the suggestion
  sugSize=0;
}
//###########################################formating functions
void backSpace()
{
  sKey(1,BACK);
  //pronoun case
  if(count[CWORD]==1 && letterBuffer[0]>96)
  {
    Keyboard.write(letterBuffer[0]-32);
    return;
  }
  countChange(-1);
}
void backlast(byte last)
{
  sKey(count[last],BACK);
  count[LINEC]-=count[last];
  while(count[LINEC]>LINESIZE)
  {
    count[LINEC]+=LINESIZE;
  }
  count[last]=0;
}

void punctuation(char mark)
{
  sKey(2,BACK);
  Keyboard.write(mark);
  Keyboard.write(' ');
  count[YESC]++;
}
//############ YES CASES
void centerJust()
{
  count[LSENT]=0;
  cJustify=true;
  explicitMode=true;
  sfill(LINESIZE/2-count[LINEC], ' ');
}
void rightJust()
{
  count[LSENT]=0;
  rJustify=true;
  explicitMode=true;
  sfill(LINESIZE-1-count[LINEC], ' ');
}
void backTxt()
{
  sKey(count[LSENT],left);
  sKey(count[LSENT],BACK);
  sKey(count[LSENT],right);
}
void enter()
{
  if(rJustify)
  {
    count[LSENT]=0;
    rJustify=false;
  }
  if(cJustify)
  {
    sKey(count[LSENT],left);
    sKey(count[LSENT]/2,BACK);
    sKey(count[LSENT],right);
    cJustify=false;
  }
  Keyboard.write(RTN);
  count[LINEC]=0;
  explicitMode=false;
}
void space()
{
  Keyboard.write(' ');
  countChange(1);
  count[LWORD]=count[CWORD];
  count[CWORD]=0;
  if(rJustify || cJustify)
  {
    count[LSENT]++;
  }
}
void period()
{
  sKey(1,BACK);
  Keyboard.print(". ");
  countChange(1);
  count[LWORD]++;
  count[LSENT]=count[CSENT];
  count[CSENT]=0;
}

//########## Meta or Magic functions 
void switchWindow()
{
  Keyboard.press(lalt);
  Keyboard.press(tab);
  delay(REACT);
  Keyboard.press(tab);
  delay(REACT);
  Keyboard.releaseAll();
}
//--------------------------------------------keyboard functions
//---------------------------letter printing
void printLetter(byte letterNum)
{
  if(count[CSENT]==0)
  {
    Keyboard.write(letterNum-32);
    letterBuffer[0]=letterNum-32;
  }
  else
  {
    Keyboard.write(letterNum);
    if(count[CWORD]<14)
    {
      letterBuffer[count[CWORD]]=letterNum;
    }
    count[YESC]=0;
    count[NOC]=0;
    //count[METAC]=0;
  }
  countChange(1);
  if(rJustify || cJustify)
  {
    count[LSENT]++;
  }
}
void countChange(byte increment)
{
  for(byte i=0;i<3;i++)
  {
    count[i]=count[i]+increment;
    //increments 'current' counts
  }
}
//this is possibly kruft at this point
//spacer
void sfill(byte spacing, char Char)
{
  for(int i=0;i<spacing;i++)
  {
    Keyboard.print(Char);
    if(Char==' ')
    {
      count[LINEC]++;
    }
  }
}
//Multi key press
void sKey(int presses, int key)
{
  for(int i=0;i<presses;i++)
  {
    Keyboard.write(key);
  }
}

// Key binding function
void combo(char key1, char key2)
{
  Keyboard.press(key1);
  Keyboard.press(key2);
  delay(REACT);
  Keyboard.releaseAll();
}

void combo(char key1, char key2, char key3)
{
  Keyboard.press(key1);
  Keyboard.press(key2);
  Keyboard.press(key3);
  delay(REACT);
  Keyboard.releaseAll();
}

//---------------------------------macros
void paulsMacro()
{
  combo(lshift, lalt);
  //change layout
  delay(REACT);
  combo(lctrl, ' ');
  //bring up app search
  delay(REACT);
  Keyboard.print("pluma");
  // open mint's text editor
  delay(REACT);
  Keyboard.println(  );
}
//-----------------------------------------------------------------------session related functions
//------------------------------------clear EEPROM
void clearPROM(int start, int finish)
{
  for(int i=start;i<finish;i++)
  {
    EEPROM.write(i, 0);
    delay(6);//again percautionary, not sure is this is nessisary
  }
}

//-----------------------------check for persistent key
boolean session(int address, byte code)
{
  //establishes if source has been run by checkig is a key/code has been written
  if(code==EEPROM.read(address))
  {
    return false;
    // false, ie skip things that would need to be done in an unastablished session
  }
  else
  {
    // write the key to establish the session
    EEPROM.write(address, code);
    return true;
    // true, ie do things unique to an unestablished session 
  };
}
//misc
void wait()
{
  while(digitalRead(buttons[0])==HIGH)
  {
    // waits for input to prompt yes and no
    ;
  }
}









