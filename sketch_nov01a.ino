#include "arrpat.h"

#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <EventDelay.h>
#include <Ead.h>
#include <ADSR.h>
#include <tables/sin2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>
#include <LowPassFilter.h>

#define CONTROL_RATE 128

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <TRIANGLE2048_NUM_CELLS, AUDIO_RATE> aTri(TRIANGLE2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw(SAW2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSqu(SQUARE_NO_ALIAS_2048_DATA);

EventDelay kDelay;
EventDelay noteDelay;
Ead eadEnv(CONTROL_RATE);
ADSR <CONTROL_RATE, CONTROL_RATE> envelope;
LowPassFilter lpf;

#define arrpOn 2 //arrp ON
#define looptop 13

//switch assign
#define SW_NUM 3
#define PUSH_SHORT  10
#define PUSH_LIMIT  PUSH_SHORT + 10
const byte s_pin[SW_NUM] = {3,8,7}; //3page, 8octUP, 7octDOWN
int octv = 0;

//cross fade
int input[2] = {0,0};
int curve[2][6];
int sinGain[2];
int triGain[2];
int sawGain[2];
int squGain[2];

//nob assign
//int const nob[5] = {A4,A3,A2,A1,A7};
int nob0 = A1;
int nob1 = A2;
int nob2 = A3;
int nob3 = A4;
int nob4 = A7;

//int const keySwitch = A7;

//page nob
int valNob[4][4];//ページ番号,ノブ番号
int realNob[4][4];

//page Flag
int Flag[4][4] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};

int page = 0;
int pageState[4] = {0,0,0,0};
#define page1 10
#define page2 4
#define page3 5
#define page4 6

int stepNum = 16;
int pattern = 0;
int tmp_bpm = 250;

int scaleNum = 0;
int keyshift = 0;

int bang = 0;

//envelope
int gain;
unsigned int attack = 250;
unsigned int decay = 250;
unsigned int attackR = 200;
unsigned int decayR = 200;
unsigned int sustain, release_ms;


int pushkey(int);

int playNote;

byte cutoff_freq;

//--------------------setup start
void setup() {
  Serial.begin(115200);
  pinMode(arrpOn, INPUT);
  pinMode(looptop, OUTPUT);
  pinMode(page1, OUTPUT);
  pinMode(page2, OUTPUT);
  pinMode(page3, OUTPUT);
  pinMode(page4, OUTPUT);
  randSeed();
  kDelay.start(500);
  for (byte i = 0 ; i < SW_NUM ; i++){
    pinMode(s_pin[i], INPUT);
  }
  cutoff_freq = 255;
  startMozzi(CONTROL_RATE);
/*
  byte attack_level = 255;
  byte decay_level = 255;
  envelope.setADLevels(attack_level,decay_level);
  
Serial.begin(9600);
*/
  //lpf.setResonance(150);
}
//---------------------setup end


//---------------------updateControl start
void updateControl(){

//int keySwitch = mozziAnalogRead(nob[4]);
int keySwitch = mozziAnalogRead(nob4);

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
        pageState[1] = 0;
        pageState[2] = 0;
        pageState[3] = 0;
        digitalWrite(page1, HIGH);
        digitalWrite(page2, LOW);
        digitalWrite(page3, LOW);
        digitalWrite(page4, LOW);
        break;

       case 1:
        pageState[0] = 0;
        pageState[1] = 1;
        pageState[2] = 0;
        pageState[3] = 0;
        digitalWrite(page1, LOW);
        digitalWrite(page2, HIGH);
        digitalWrite(page3, LOW);
        digitalWrite(page4, LOW);
        break;

       case 2:
        pageState[0] = 0;
        pageState[1] = 0;
        pageState[2] = 1;
        pageState[3] = 0;
        digitalWrite(page1, LOW);
        digitalWrite(page2, LOW);
        digitalWrite(page3, HIGH);
        digitalWrite(page4, LOW);
        break;

       case 3:
        pageState[0] = 0;
        pageState[1] = 0;
        pageState[2] = 0;
        pageState[3] = 1;
        digitalWrite(page1, LOW);
        digitalWrite(page2, LOW);
        digitalWrite(page3, LOW);
        digitalWrite(page4, HIGH);
        break;
    }
  
    if((sw2 == 255)&&(octv<3)){
      octv++;
      keyshift = octv*12;
    }else if(octv>3){
      octv = 3;
    }
    if((sw3 == 255)&&(octv>-3)){
      octv--;
      keyshift = octv*12;
    }else if(octv<-3){
      octv = -3;
    }
/*
  for(int i=0; i<4; i++){
    realNob[0][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
    realNob[1][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
    realNob[2][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
    realNob[3][i] = map(mozziAnalogRead(nob[i]),0,1023,0,127);
  }
*/

for(int i=0; i<4; i++){
    realNob[i][0] = map(mozziAnalogRead(nob0),0,1023,0,127);
    realNob[i][1] = map(mozziAnalogRead(nob1),0,1023,0,127);
    realNob[i][2] = map(mozziAnalogRead(nob2),0,1023,0,127);
    realNob[i][3] = map(mozziAnalogRead(nob3),0,1023,0,127);
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
//page1
pattern = map(valNob[0][0], 0, 127, 0, 5);
scaleNum = map(valNob[0][1], 0, 127, 0, 5);
stepNum = map(valNob[0][2], 0, 127, 0, 16);
tmp_bpm = map(valNob[0][3], 0, 127, 10, 2000);

//page2
attackR = map(valNob[1][0], 0, 127, 10, 500);
decayR = map(valNob[1][1], 0, 127, 10, 500);
sustain = map(valNob[1][2], 0, 127, 10, 500);
release_ms = map(valNob[1][3], 0, 127, 10, 500);
envelope.setADLevels(attack,decay);
envelope.setTimes(attackR,decayR,sustain,release_ms);

//page3
//select waveform
  input[0] = valNob[2][0];
  curve[0][0] = map(valNob[2][0], 0, 42, 100, 0);
  curve[0][1] = map(valNob[2][0], 0, 42, 0, 100);
  curve[0][2] = map(valNob[2][0], 43, 84, 100, 0);
  curve[0][3] = map(valNob[2][0], 43, 84, 0, 100);
  curve[0][4] = map(valNob[2][0], 85, 127, 100, 0);
  curve[0][5] = map(valNob[2][0], 85, 127, 0, 100);

  if((0<=input[0])&&(input[0]<42)){
      sinGain[0] = curve[0][0];
  }else{
      sinGain[0] = 0;
  }
  
  if((0<=input[0])&&(input[0]<42)){
      triGain[0] = curve[0][1];
  }
  
  if((43<input[0])&&(input[0]<84)){
      triGain[0] = curve[0][2];
  }else if(input[0]>85){
      triGain[0] = 0;
  }
  
  if((43<input[0])&&(input[0]<84)){
      sawGain[0] = curve[0][3];
  }else if(input[0]<42){
      sawGain[0] = 0;
  }
  
  if((85<input[0])&&(input[0]<=127)){
      sawGain[0] = curve[0][4];
  }
  
  if((85<input[0])&&(input[0]<=127)){
      squGain[0] = curve[0][5];
  }else if(input[0]<84){
      squGain[0] = 0;
  }

  cutoff_freq = map(valNob[2][1], 0, 127, 20, 255);
  byte flt_res = map(valNob[2][2], 0, 127, 0, 255);
  lpf.setResonance(cutoff_freq);
  lpf.setCutoffFreq(flt_res);

//page 4


//push da oscillate
  //if(keySwitch != 1023){
  if((digitalRead(arrpOn) == LOW)&&(keySwitch < 1005)){
      envelope.noteOn();
      //eadEnv.start(attackR,decayR);
      digitalWrite(looptop, LOW);
      playNote = pushkey(keySwitch) + 60 + keyshift;
  		aSin.setFreq(mtof(playNote));
		  aTri.setFreq(mtof(playNote));
		  aSaw.setFreq(mtof(playNote));
		  aSqu.setFreq(mtof(playNote));

     

      //gain = (int) kEnvelope.next();
               
  }else if((digitalRead(arrpOn) == HIGH)&&(keySwitch < 1005)){ //Arrp mode:sometime make liblary
    if(kDelay.ready()){
     if(bang < stepNum){
      switch(bang){
        case 0: //int noiPatt(int patt, int scale, int note, int steps)
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, HIGH);
        
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 1:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 2:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 3:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 4:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 5:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 6:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 7:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 8:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 9:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 10:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 11:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 12:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 13:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 14:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;

        case 15:
        envelope.noteOn();
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aTri.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSaw.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        aSqu.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A7)), bang)));
        digitalWrite(looptop, LOW);
        //envelope.noteOn();
        //kEnvelope.start(attack,decay);
        bang++;
        break;
      }
     envelope.update();
     gain = envelope.next();
     kDelay.start(tmp_bpm + attackR + decayR); 
    }else{
      bang = 0;
      }  
    }
   }else{
    envelope.update();
    gain = envelope.next();
    //gain = (int)eadEnv.next();
    digitalWrite(looptop, HIGH);
    /*
    aSin.setFreq(0);
    aTri.setFreq(0);
    aSaw.setFreq(0);
    aSqu.setFreq(0);
    */
  }

  Serial.print("1:");
  Serial.println(attackR);
  Serial.print("2:");
  Serial.println(decayR);
  Serial.print("3:");
  Serial.println(sustain);
  Serial.print("4:");
  Serial.println(release_ms);
  Serial.print("5:");
  Serial.println(mozziAnalogRead(nob4));
  Serial.print("SW:");
  Serial.println(tmp_bpm);

}
//---------------------updateControl end


//---------------------updateAudio start
int updateAudio(){ 
	char master1 = ((aSin.next()*sinGain[0]>>8)+(aTri.next()*triGain[0]>>8)+(aSaw.next()*sawGain[0]>>8)+(aSqu.next()*squGain[0]>>8));
  char asig = lpf.next(master1);
  return (int) (gain * asig)>>8;
	//return (gain*aSin.next())>>8;
  //return (int) (envelope.next() * lpf.next((aSin.next()*sinGain[0])+(aTri.next()*triGain[0])+(aSaw.next()*sawGain[0])+(aSqu.next()*squGain[0])))>>8;
  //return (int) ((aSin.next()*sinGain[0])+(aTri.next()*triGain[0])+(aSaw.next()*sawGain[0])+(aSqu.next()*squGain[0]))>>8;
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
