#include "arrpat.h"

#include <Rotary.h>
#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin2048_int8.h>

#define CONTROL_RATE 64
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
EventDelay kDelay;
ADSR <CONTROL_RATE, AUDIO_RATE> envelope;
/*
Rotary rotary1 = Rotary(3, 4);
Rotary rotary2 = Rotary(5, 6);
Rotary rotary3 = Rotary(7, 8);
*/

int const nob[3] = {A2,A3,A4};

#define switch1 3 //page
#define switch2 2 //arrp ON
#define looptop 12

#define SW_NUM 2
#define PUSH_SHORT  10
#define PUSH_LIMIT  PUSH_SHORT + 10
 
const byte s_pin[SW_NUM] = {10, 11};

int octv = 0;

int valNob[2][3];//ページ番号,ノブ番号
int realNob[2][3];

int Flag[2][3] = {
  {0,0,0},
  {0,0,0}
};

int pageState0 = 0;
int pageState1 = 0;

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
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(looptop, OUTPUT);
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

  for(int i=0; i<3; i++){
    realNob[0][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
    realNob[1][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
  }

//page1 prossece
	if(digitalRead(switch1) == LOW){
    pageState0 = 1;
	}else{
    pageState1 = 1;
	}

//page1 process
  if(pageState0 == 1){
    for(int i=0; i<3; i++){
      if(Flag[0][i] == 0){
      if(valNob[0][i] == realNob[0][i]){
         Flag[0][i] = 1;
        }
      }else{
        valNob[0][i] = realNob[0][i];
      }
    }
    pageState0 = 0;
    for(int i=0; i<3; i++){
      Flag[1][i] = 0;
    }
  }

//page2 process
  if(pageState1 == 1){
    for(int i=0; i<3; i++){
      if(Flag[1][i] == 0){
      if(valNob[1][i] == realNob[1][i]){
         Flag[1][i] = 1;
        }
      }else{
        valNob[1][i] = realNob[1][i];
      }
    }
    pageState1 = 0;
    for(int i=0; i<3; i++){
      Flag[0][i] = 0;
    }
  }

/*
//rotary ver
     unsigned char result3 = rotary3.process(); //step select
     if((result3 == DIR_CW)&&(stepNum<16)){
        stepNum++;
        }else if((result3 == DIR_CCW)&&(stepNum>0)){
          stepNum--;
        }
*/

//asign
pattern = map(valNob[0][0], 0, 127, 0, 5);
scaleNum = map(valNob[0][1], 0, 127, 0, 5);
stepNum = map(valNob[0][2], 0, 127, 0, 16);

attackR = map(valNob[1][0], 0, 127, 1, 1000);
decayR = map(valNob[1][1], 0, 127, 1, 1000);
tmp_bpm = map(valNob[1][2], 0, 127, 1, 2000);
envelope.setTimes(attackR, 10, 10, decayR);

//push da oscillate
  if(digitalRead(switch2) == LOW){
    if(mozziAnalogRead(A5) != 1023){
      envelope.noteOn();
      playNote = pushkey(mozziAnalogRead(A5)) + 60 + keyshift;
  		aSin.setFreq(mtof(playNote));
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
        digitalWrite(12, HIGH);
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
  return (int) (envelope.next() * aSin.next())>>8;
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
