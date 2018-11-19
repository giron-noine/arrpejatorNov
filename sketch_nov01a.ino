extern "C" {
  #include "arrpat.h"
}
#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <EventDelay.h>
#include <tables/sin2048_int8.h>

#define CONTROL_RATE 64
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
EventDelay kDelay;

int const nob[3] = {A0,A1,A2};

#define switch1 10 //page
#define switch2 2 //arrp ON
#define looptop 12

#define octUP 4
#define octDOWN 5
int octv = 0;

int valNob[2][3]; //ページ番号,ノブ番号

int pageState0 = 0;
int pageState1 = 0;

int stepNum = 16;
int pattern = 1;
int tmp_bpm = 250;

int scaleNum;
int dechune;
int keyshift = 0;

int bang = 0;


int pushkey(int);

int playNote;

//---------setup start
void setup() 
{
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(looptop, OUTPUT);
  pinMode(octUP, INPUT);
  pinMode(octDOWN, INPUT);
  startMozzi(CONTROL_RATE);
  randSeed();
  kDelay.start(500);
  //Serial.begin(9600);
}
//---------setup end


//---------updateControl start
void updateControl(){

  tmp_bpm = map(mozziAnalogRead(A4),0,1023,0,500);
  
	if(digitalRead(switch1) == LOW){
    	pageState0 = 1;
  	}else{
    	pageState1 = 1;
  	}

//page1 prossece
  if(pageState0 == 1){
    for(int i=0; i<3; i++){
    	if(valNob[1][i] != mozziAnalogRead(nob[i])){
    		valNob[0][i] = mozziAnalogRead(nob[i]);
        }
    }
    pageState0 = 0;
  }  
//page2 prossece
  if(pageState1 == 1){
    
    for(int i=0; i<3; i++){
    	if(valNob[0][i] != mozziAnalogRead(nob[i])){
    		valNob[1][i] = mozziAnalogRead(nob[i]);
    	}
    }
    pageState1 = 0;
  }
  
//octave control
  if((digitalRead(5) == 1)&&(octv<3)){
    octv++;
    keyshift = octv*12;
  }
  if((digitalRead(4) == 1)&&(octv>-3)){
    octv--;
    keyshift = octv*12;
  }
  
//push da oscillate
  if(digitalRead(switch2) == LOW){
    if(mozziAnalogRead(A5) != 1023){
      playNote = pushkey(mozziAnalogRead(A5)) + 60 + keyshift;
  		aSin.setFreq(mtof(playNote));
    }else{
    	aSin.setFreq(0);
    }
  }else if((digitalRead(switch2) == HIGH)&&(mozziAnalogRead(A5) != 1023)){ //Arrp mode:sometime make liblary
    if(kDelay.ready()){
      switch(bang){
        case 0:
        aSin.setFreq(mtof(noiPatt(map((valNob[0][0]), 0, 1023, 0, 3), map((valNob[0][1]), 0, 1023, 0, 5), pushkey(mozziAnalogRead(A5)), bang))+ keyshift);
        digitalWrite(12, HIGH);
        bang++;
        break;

        case 1:
        aSin.setFreq(mtof(noiPatt(map((valNob[0][0]), 0, 1023, 0, 3), map((valNob[0][1]), 0, 1023, 0, 5), pushkey(mozziAnalogRead(A5)), bang))+ keyshift);
        digitalWrite(12, LOW);
        bang++;
        break;

        case 2:
        aSin.setFreq(mtof(noiPatt(map((valNob[0][0]), 0, 1023, 0, 3), map((valNob[0][1]), 0, 1023, 0, 5), pushkey(mozziAnalogRead(A5)), bang))+ keyshift);
        digitalWrite(12, LOW);
        bang++;
        break;

        case 3:
        aSin.setFreq(mtof(noiPatt(map((valNob[0][0]), 0, 1023, 0, 3), map((valNob[0][1]), 0, 1023, 0, 5), pushkey(mozziAnalogRead(A5)), bang))+ keyshift);
        digitalWrite(12, LOW);
        bang++;
        break;

        case 4:
        aSin.setFreq(mtof(noiPatt(map((valNob[0][0]), 0, 1023, 0, 3), map((valNob[0][1]), 0, 1023, 0, 5), pushkey(mozziAnalogRead(A5)), bang))+ keyshift);
        digitalWrite(12, LOW);
        bang++;
        break;

        case 5:
        aSin.setFreq(mtof(noiPatt(map((valNob[0][0]), 0, 1023, 0, 3), map((valNob[0][1]), 0, 1023, 0, 5), pushkey(mozziAnalogRead(A5)), bang))+ keyshift);
        digitalWrite(12, LOW);
        bang++;
        break;

        case 6:
        aSin.setFreq(mtof(noiPatt(map((valNob[0][0]), 0, 1023, 0, 3), map((valNob[0][1]), 0, 1023, 0, 5), pushkey(mozziAnalogRead(A5)), bang))+ keyshift);
        digitalWrite(12, LOW);
        bang = 0;
        break;
      }
      
    }
    kDelay.start(tmp_bpm);
  }else{
    aSin.setFreq(0);
  }

/* 
  Serial.print("voltage");
  Serial.println(pushkey(mozziAnalogRead(A5)));
  Serial.print("octaveU");
  Serial.println(digitalRead(octUP));
  Serial.print("octaveD");
  Serial.println(digitalRead(octDOWN));
  Serial.print("octave");
  Serial.println(keyshift);
  Serial.print("dial1");
  Serial.println(mozziAnalogRead(A0));
  Serial.print("note");
  Serial.println(playNote);
*/

  
}
//---------updateControl end


//---------updateAudio start
int updateAudio(){
	return aSin.next();
}
//---------updateAudio end


//---------loop start
void loop(){
  audioHook();
}
//---------loop end
