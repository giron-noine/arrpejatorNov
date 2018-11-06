#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <mozzi_midi.h>
#include <mozzi_rand.h>
#include <Ead.h>
#include <tables/sin2048_int8.h> // sine table for oscillator

#define CONTROL_RATE 64
#define shiftKey1 5
//#define shiftKey2 6
#define looptop 12

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

/* Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> bSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> cSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> dSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> eSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> fSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> gSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> hSin(SIN2048_DATA);
*/

EventDelay kDelay;
//Ead kEnvelope(CONTROL_RATE);  //エンベロープ用インスタンス

int shiftposi1 = 0;
//int shiftposi2 = 0;

int nob[3];

int scaleNum = 0;
int stepNum = 16;
int dechune = 0;
float keyshift = 1;
int bang = 0;
int tempo = 2000;
int octave = 1;
int notechange;
int randomnote;

int gain;

//note mapping 1=c
int notemap[] = {
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71
};

//scale 2d array　MIDIノートとして格納
int scaleMap[6][7] = {
  {60, 62, 64, 65, 67, 69, 71}, //major
  {60, 62, 63, 65, 67, 69, 70}, //dorian
  {60, 62, 64, 67, 69, 72, 74}, //penta
  {60, 63, 65, 67, 70, 72, 75}, //minor penta 
  {60, 62, 63, 65, 67, 68, 70}, //minor
  {60, 61, 64, 65, 67, 68, 71} //gypsy
};

void setup() {
  // put your setup code here, to run once:
randSeed();
startMozzi(CONTROL_RATE);
kDelay.start(1000);

pinMode(shiftKey1, INPUT);
//pinMode(shiftKey2 INPUT);
pinMode(looptop, OUTPUT);
}

void updateControl(){
shiftposi1 = digitalRead(shiftKey1);//スイッチON、OFFの格納
//shiftposi2 = digitalRead(shiftKey2);

for(int i=0; i<3; i++){
  nob[i] = mozziAnalogRead(i); //フェーダーの読み取りと格納
}


 if(shiftposi1 == 0){
  scaleNum = map(nob[0], 0, 1023, 0, 5); //スケールの種類を選択
  dechune = map(nob[1], 0, 1023, 0, 12); //デチューン
  keyshift = map(nob[2], 0, 1023, 0.0, 2.0);
  octave = 12 * keyshift;
  notechange = dechune + octave;
 }
 else if(shiftposi1 == 1){
  stepNum = map(nob[0], 0, 1023, 1, 16); //ステップ数
  tempo = map(nob[1], 0, 1023, 50, 5000); //ステップのスピード
  kDelay.set(tempo);
 }
  

//設定した時間経つとreadyがTrueになってステップが進む
 if(kDelay.ready()){
  if(bang < stepNum){
    switch(bang){
      case 0:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange])); //scaleMapの２次元配列から取り出してmtofで周波数へ
        digitalWrite(looptop,HIGH);
        //kDelay.start(tempo);
        break;

      case 1:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 2:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 3:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 4:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 5:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 6:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 7:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 8:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 9:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 10:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 11:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 12:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 13:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 14:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;

      case 15:
        //aSin.setFreq(mtof(scaleMap[scaleNum][rand(6) + notechange]));
        //kDelay.start(tempo);
        digitalWrite(looptop,LOW);
        break;
    }
    randomnote = rand(6);
    aSin.setFreq(mtof(scaleMap[scaleNum][randomnote + notechange])); //scaleMapの２次元配列から取り出してmtofで周波数へ
    kDelay.start(tempo);
    bang++;
  }else {
    bang = 0;
  }
  
}
}

int updateAudio(){
  return aSin.next();
}

void loop() {
  // put your main code here, to run repeatedly:
  audioHook();
}
