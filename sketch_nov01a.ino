#include "arrpat.h"

#include <Rotary.h>
#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>
#include <LowPassFilter.h>

#define CONTROL_RATE 64

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <TRIANGLE2048_NUM_CELLS, AUDIO_RATE> aTri(TRIANGLE2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw(SAW2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSqu(SQUARE_NO_ALIAS_2048_DATA);

EventDelay kDelay;
ADSR <CONTROL_RATE, AUDIO_RATE> envelope;
/*
Rotary rotary1 = Rotary(3, 4);
Rotary rotary2 = Rotary(5, 6);
Rotary rotary3 = Rotary(7, 8);
*/

int const nob[4] = {A1,A2,A3,A4};

//#define switch1 3 //page
#define switch2 2 //arrp ON
#define looptop 12

#define SW_NUM 3
#define PUSH_SHORT  10
#define PUSH_LIMIT  PUSH_SHORT + 10
 
const byte s_pin[SW_NUM] = {3,10,11}; //3page, 10octUP, 11octDOWN

int octv = 0;

//cross fade
int input[2] = {0,0};
int curve[2][6];
int sinGain[2];
int triGain[2];
int sawGain[2];
int squGain[2];

int valNob[4][4];//ページ番号,ノブ番号
int realNob[4][4];

int Flag[4][4] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};

int page = 0;
int pageState[4] = {0,0,0,0};
#define page1 6
#define page2 7
#define page3 8
#define page4 9

int stepNum = 16;
int pattern = 0;
int tmp_bpm = 250;

int scaleNum = 0;
int keyshift = 0;

int bang = 0;
//envelope
int gain;
unsigned int attack = 10;
unsigned int decay = 1000;
unsigned int attackR = 10;
unsigned int decayR = 1000;

int pushkey(int);

int playNote;

//--------------------setup start
void setup() 
{
  //pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(looptop, OUTPUT);
  pinMode(page1, OUTPUT);
  pinMode(page2, OUTPUT);
  pinMode(page3, OUTPUT);
  pinMode(page4, OUTPUT);
  startMozzi(CONTROL_RATE);
  randSeed();
  kDelay.start(500);
  for (byte i = 0 ; i < SW_NUM ; i++){
    pinMode(s_pin[i], INPUT);

  byte attack_level = 255;
  byte decay_level = 255;
  envelope.setADLevels(attack_level,decay_level);

  }
}
//---------------------setup end


//---------------------updateControl start
void updateControl(){

//esc chataring
    byte sw1 = BUTTON(0);
    byte sw2 = BUTTON(1);
    byte sw3 = BUTTON(2);
    if((sw1 == 255)&&(page<4)){
      page++;
    }else if(page>3){
      page = 0;
    }
    switch(page){
      case 0:
        pageState[0] = 1;
        digitalWrite(page1, HIGH);
        digitalWrite(page2, LOW);
        digitalWrite(page3, LOW);
        digitalWrite(page4, LOW);
        break;

       case 1:
        pageState[1] = 1;
        digitalWrite(page1, LOW);
        digitalWrite(page2, HIGH);
        digitalWrite(page3, LOW);
        digitalWrite(page4, LOW);
        break;

       case 2:
        pageState[2] = 1;
        digitalWrite(page1, LOW);
        digitalWrite(page2, LOW);
        digitalWrite(page3, HIGH);
        digitalWrite(page4, LOW);
        break;

       case 3:
        pageState[3] = 1;
        digitalWrite(page1, LOW);
        digitalWrite(page2, LOW);
        digitalWrite(page3, LOW);
        digitalWrite(page4, HIGH);
        break;
    }
  
    if((sw1 == 255)&&(octv<3)){
      octv++;
      keyshift = octv*12;
    }else if(octv>3){
      octv = 3;
    }
    if((sw2 == 255)&&(octv>-3)){
      octv--;
      keyshift = octv*12;
    }else if(octv<-3){
      octv = -3;
    }

  for(int i=0; i<4; i++){
    realNob[0][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
    realNob[1][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
    realNob[2][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
    realNob[3][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
  }

//page1 process
  if(pageState[0] == 1){
    for(int i=0; i<4; i++){
      if(Flag[0][i] == 0){
      if(valNob[0][i] == realNob[0][i]){
         Flag[0][i] = 1;
        }
      }else{
        valNob[0][i] = realNob[0][i];
      }
    }
    pageState[0] = 0;
    for(int i=0; i<4; i++){
      Flag[1][i] = 0;
      Flag[2][i] = 0;
      Flag[3][i] = 0;
    }
  }

//page2 process
  if(pageState[1] == 1){
    for(int i=0; i<4; i++){
      if(Flag[1][i] == 0){
      if(valNob[1][i] == realNob[1][i]){
         Flag[1][i] = 1;
        }
      }else{
        valNob[1][i] = realNob[1][i];
      }
    }
    pageState[1] = 0;
    for(int i=0; i<4; i++){
      Flag[0][i] = 0;
      Flag[2][i] = 0;
      Flag[3][i] = 0;
    }
  }

//page3 process
  if(pageState[2] == 1){
    for(int i=0; i<4; i++){
      if(Flag[2][i] == 0){
      if(valNob[2][i] == realNob[2][i]){
         Flag[2][i] = 1;
        }
      }else{
        valNob[2][i] = realNob[2][i];
      }
    }
    pageState[2] = 0;
    for(int i=0; i<4; i++){
      Flag[0][i] = 0;
      Flag[1][i] = 0;
      Flag[3][i] = 0;
    }
  }

//page4 process
  if(pageState[3] == 1){
    for(int i=0; i<4; i++){
      if(Flag[3][i] == 0){
      if(valNob[3][i] == realNob[3][i]){
         Flag[3][i] = 1;
        }
      }else{
        valNob[3][i] = realNob[3][i];
      }
    }
    pageState[3] = 0;
    for(int i=0; i<4; i++){
      Flag[0][i] = 0;
      Flag[1][i] = 0;
      Flag[2][i] = 0;
    }
  }


//asign
pattern = map(valNob[0][0], 0, 127, 0, 5);
scaleNum = map(valNob[0][1], 0, 127, 0, 5);
stepNum = map(valNob[0][2], 0, 127, 0, 16);

attackR = map(valNob[1][0], 0, 127, 1, 1000);
decayR = map(valNob[1][1], 0, 127, 1, 1000);
tmp_bpm = map(valNob[1][2], 0, 127, 1, 2000);
envelope.setTimes(attackR, 10, 10, decayR);

//select waveform
  input[0] = mozziAnalogRead(nob[3]);
  curve[0][0] = map(mozziAnalogRead(nob[3]), 0, 341, 100, 0);
  curve[0][1] = map(mozziAnalogRead(nob[3]), 0, 341, 0, 100);
  curve[0][2] = map(mozziAnalogRead(nob[3]), 342, 682, 100, 0);
  curve[0][3] = map(mozziAnalogRead(nob[3]), 342, 682, 0, 100);
  curve[0][4] = map(mozziAnalogRead(nob[3]), 683, 1023, 100, 0);
  curve[0][5] = map(mozziAnalogRead(nob[3]), 683, 1023, 0, 100);

  if((0<=input[0])&&(input[0]<342)){
      sinGain[0] = curve[0][0];
  }else{
      sinGain[0] = 0;
  }
  
  if((0<=input[0])&&(input[0]<342)){
      triGain[0] = curve[0][1];
  }
  
  if((341<input[0])&&(input[0]<683)){
      triGain[0] = curve[0][2];
  }else if(input[0]>683){
      triGain[0] = 0;
  }
  
  if((341<input[0])&&(input[0]<683)){
      sawGain[0] = curve[0][3];
  }else if(input[0]<341){
      sawGain[0] = 0;
  }
  
  if((682<input[0])&&(input[0]<=1023)){
      sawGain[0] = curve[0][4];
  }
  
  if((682<input[0])&&(input[0]<=1023)){
      squGain[0] = curve[0][5];
  }else if(input[0]<682){
      squGain[0] = 0;
  }

//push da oscillate
  if(digitalRead(switch2) == LOW){
    if(mozziAnalogRead(A5) != 1023){
      envelope.noteOn();
      playNote = pushkey(mozziAnalogRead(A5)) + 60 + keyshift;
  		aSin.setFreq(mtof(playNote));
		aTri.setFreq(mtof(playNote));
		aSaw.setFreq(mtof(playNote));
		aSqu.setFreq(mtof(playNote));
      //gain = (int) kEnvelope.next();
    }else{
      envelope.noteOff();
      //aSin.setFreq(0.f);
    }
  }else if((digitalRead(switch2) == HIGH)&&(mozziAnalogRead(A5) != 1023)){ //Arrp mode:sometime make liblary
    if(kDelay.ready()){
     if(bang < stepNum){        
      switch(bang){
        case 0: //int noiPatt(int patt, int scale, int note, int steps)
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(looptop, HIGH);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 1:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 2:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 3:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 4:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 5:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 6:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 7:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 8:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 9:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 10:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 11:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 12:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 13:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 14:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 15:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;
      }
     envelope.noteOff();
     kDelay.start(tmp_bpm+attackR+decayR); 
    }else{
      bang = 0;
      }  
    }
   }else{
    //gain = (int) kEnvelope.next();
    aSin.setFreq(0.f);
  }
  
}
//---------------------updateControl end


//---------------------updateAudio start
int updateAudio(){
	//return (gain*aSin.next())>>8;
  return (int) (envelope.next() * (aSin.next()*sinGain[0])+(aTri.next()*triGain[0])+(aSaw.next()*sawGain[0])+(aSqu.next()*squGain[0]))>>9;
  //return aSin.next();
}
//---------------------updateAudio end


//---------------------loop start
void loop(){
  audioHook();
}
//---------------------loop end


byte BUTTON(byte pin_num) {
  static unsigned short gauge[SW_NUM];
 
  byte sw_status = 0;
 
  //check switch status on or off
  if (!digitalRead(s_pin[pin_num]))
  {
    gauge[pin_num]++;
    gauge[pin_num] = min(gauge[pin_num], PUSH_LIMIT);
  } else {
    if (gauge[pin_num] >= PUSH_SHORT) sw_status = 255;
    gauge[pin_num] = 0;
  }
 
  //return value of switch status
  if (gauge[pin_num] >= PUSH_SHORT) sw_status = 1;
 
  return sw_status;
}
