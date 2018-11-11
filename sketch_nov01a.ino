#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <mozzi_midi.h>
#include <mozzi_rand.h>
#include <Ead.h>
#include <tables/sin2048_int8.h> // sine table for oscillator
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

int const nob[3] = {A5,A4,A3};

#define switch1 7
#define switch2 10

#define looptop 12

#define CONTROL_RATE 64

EventDelay kDelay;
char pageState0;
char pageState1;
int valNob[2][3];
int stepNum = 16;
int pattern = 1;
int tmp_bpm = 250;

int scaleNum;
int dechune;
int keyshift;

int bang = 0;

int scaleMap[6][7] = {
  {60, 62, 64, 65, 67, 69, 71}, //major
  {60, 62, 63, 65, 67, 69, 70}, //dorian
  {60, 62, 64, 67, 69, 72, 74}, //penta
  {60, 63, 65, 67, 70, 72, 75}, //minor penta 
  {60, 62, 63, 65, 67, 68, 70}, //minor
  {60, 61, 64, 65, 67, 68, 71} //gypsy
};



void setup(){
  startMozzi(CONTROL_RATE);
  kDelay.start(1000);
  pinMode(switch1, INPUT);
  pinMode(looptop, OUTPUT);
  //pinMode(switch2 INPUT);
  randSeed();
}

void updateControl(){

  if(digitalRead(switch1) == LOW){
    pageState0 = 1;
  }else{
    pageState1 = 1;
  }

  if(pageState0 == 1){
    for(int i=0; i<3; i++){
      if(valNob[1][i] != analogRead(nob[i])){
        valNob[0][i] = analogRead(nob[i]);
        }
    }
    pageState0 = 0;
  }
  
  if(pageState1 == 1){ 
    for(int i=0; i<3; i++){
      if(valNob[0][i] != analogRead(nob[i])){
        valNob[1][i] = analogRead(nob[i]);
      }
    }
    pageState1 = 0;
  }

  tmp_bpm = map(valNob[0][0], 0, 1023, 10, 2000);
  stepNum = map(valNob[0][1], 0, 1023, 0, 15);
  pattern =map(valNob[0][2], 0, 1023, 0, 5);
  scaleNum = map(valNob[1][0], 0, 1023, 0, 5);
  dechune = map(valNob[1][1], 0, 1023, 0, 11);
  keyshift = map(valNob[1][2], 0, 1023, -2, 2) * 12;

  //Play Sound
  if(kDelay.ready()){
    if(bang < stepNum){
    switch(bang){
      case 0:
        digitalWrite(looptop,HIGH);
        break;
      default:
        digitalWrite(looptop,LOW);
        break;
    }
    aSin.setFreq(mtof(scaleMap[scaleNum][bang + dechune + keyshift])); //scaleMapの２次元配列から取り出してmtofで周波数へ
    kDelay.start(tmp_bpm);
    bang++;
  }else {
    bang = 0;
  }
  
}

}

int updateAudio(){
  return aSin.next();
}

void loop(){
  audioHook();
}
