#include "arrpat.h"

#include <Rotary.h>
#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <EventDelay.h>
#include <Ead.h>
#include <tables/sin2048_int8.h>

#define CONTROL_RATE 64
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
EventDelay kDelay;
Ead kEnvelope(CONTROL_RATE);

Rotary rotary1 = Rotary(3, 4);
Rotary rotary2 = Rotary(5, 6);
Rotary rotary3 = Rotary(7, 8);


//int const nob[3] = {A0,A1,A2};

#define switch1 1 //page
#define switch2 2 //arrp ON
#define looptop 12

#define SW_NUM 2
#define PUSH_SHORT  10
#define PUSH_LIMIT  PUSH_SHORT + 10
 
const byte s_pin[SW_NUM] = {10, 11};

int octv = 0;

int valNob[2][3]; //ページ番号,ノブ番号

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
unsigned int decay = 100;

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
  }
}
//---------------------setup end


//---------------------updateControl start
void updateControl(){

  tmp_bpm = map(mozziAnalogRead(A4),0,1023,0,5000);
  //stepNum = map(valNob[0][0],0,1023,0,15);

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
/*
//octave control
if((digitalRead(7) == 1)&&(octv<3)){
    octv++;
    keyshift = octv*12;
  }
  if((digitalRead(8) == 1)&&(octv>-3)){
    octv--;
    keyshift = octv*12;
  }
*/

  
	if(digitalRead(switch1) == LOW){
    	pageState0 = 1;
  	}else{
    	pageState1 = 1;
  	}

//page1 prossece
	if(pageState0 == 1){
		unsigned char result1 = rotary1.process(); //pattern select
			if((result1 == DIR_CW)&&(pattern<4)){
 				pattern++;
  			}else if((result1 == DIR_CCW)&&(pattern>0)){
  				pattern--;
  			}

		unsigned char result2 = rotary2.process(); //scale select
			if((result2 == DIR_CW)&&(scaleNum<5)){
 				scaleNum++;
  			}else if((result2 == DIR_CCW)&&(scaleNum>0)){
  				scaleNum--;
  			}

     unsigned char result3 = rotary3.process(); //scale select
     if((result3 == DIR_CW)&&(stepNum<15)){
        stepNum++;
        }else if((result3 == DIR_CCW)&&(stepNum>0)){
          stepNum--;
        }
		pageState0 = 0;
	}

//page2 prossece
	if(pageState1 == 1){
		unsigned char result1 = rotary1.process(); //attack select
			if((result1 == DIR_CW)&&(attack<2000)){
 				  attack = attack++ * 10;
  			}else if((result1 == DIR_CCW)&&(attack>0)){
  			  attack = attack-- * 10;
  			}

		unsigned char result2 = rotary2.process(); //decay select
			if((result2 == DIR_CW)&&(decay<2000)){
 				decay++;
  			}else if((result2 == DIR_CCW)&&(decay>0)){
  				decay--;
  			}
		pageState1 = 0;
	}



/*
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
*/  

//push da oscillate
  if(digitalRead(switch2) == LOW){
    if(mozziAnalogRead(A5) != 1023){
      playNote = pushkey(mozziAnalogRead(A5)) + 60 + keyshift;
  		aSin.setFreq(mtof(playNote));
		kEnvelope.start(attack,decay); //エンベロープスタート
    }else{
		//kDelay.start(100); //トリガー
		gain = (int) kEnvelope.next(); 
    //aSin.setFreq(0.f);
    }
  }else if((digitalRead(switch2) == HIGH)&&(mozziAnalogRead(A5) != 1023)){ //Arrp mode:sometime make liblary
    if(kDelay.ready()){
     if(bang < stepNum){        
      switch(bang){
        case 0: //int noiPatt(int patt, int scale, int note, int steps)
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, HIGH);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 1:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 2:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 3:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 4:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 5:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 6:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 7:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 8:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 9:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 10:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 11:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 12:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 13:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 14:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;

        case 15:
        aSin.setFreq(mtof(keyshift + noiPatt(pattern, scaleNum, pushkey(mozziAnalogRead(A5)), bang)));
        digitalWrite(12, LOW);
        kEnvelope.start(attack,decay);
        bang++;
        break;
      }
     kDelay.start(tmp_bpm); 
    }else{
      bang = 0;
      }  
    }
   }else{
    gain = (int) kEnvelope.next();
    //aSin.setFreq(0.f);
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
//---------------------updateControl end


//---------------------updateAudio start
int updateAudio(){
	//return (gain*aSin.next())>>8;
  return gain*aSin.next();
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
