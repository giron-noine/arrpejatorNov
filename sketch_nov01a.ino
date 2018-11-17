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

int scaleMap[6][7] = {
  {60, 62, 64, 65, 67, 69, 71}, //major
  {60, 62, 63, 65, 67, 69, 70}, //dorian
  {60, 62, 64, 67, 69, 72, 74}, //penta
  {60, 63, 65, 67, 70, 72, 75}, //minor penta 
  {60, 62, 63, 65, 67, 68, 70}, //minor
  {60, 61, 64, 65, 67, 68, 71} //gypsy
};

int noteNum[13] = {60,61,62,63,64,65,66,67,68,69,70,71,72};
int notefreq[13] = {261,277,293,311,329,349,370,392,415,440,466,493,523};

int pushkey(int);

void setup()
{
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(looptop, OUTPUT);
  pinMode(octUP, INPUT);
  pinMode(octDOWN, INPUT);
  Serial.begin(9600);
}

void loop()
{
  if(digitalRead(switch1) == LOW){
    pageState0 = 1;
  }else{
    pageState1 = 1;
  }
//page1 prossece
  if(pageState0 == 1){
    for(int i=0; i<3; i++){
    	if(valNob[1][i] != analogRead(nob[i])){
    		valNob[0][i] = analogRead(nob[i]);
        }
    }
    pageState0 = 0;
  }  
//page2 prossece
  if(pageState1 == 1){
    
    for(int i=0; i<3; i++){
    	if(valNob[0][i] != analogRead(nob[i])){
    		valNob[1][i] = analogRead(nob[i]);
    	}
    }
    pageState1 = 0;
  }
  
//octave control
  if((digitalRead(octUP) == HIGH)&&(octv<3)){
    octv++;
    keyshift = octv*12;
  }
  if((digitalRead(octDOWN) == HIGH)&&(octv>-3)){
    octv--;
    keyshift = octv*12;
  }
  
//push da oscillate
  if(digitalRead(switch2) == LOW){
    if(analogRead(A5) != 1023){
  	tone(9,noteNum[pushkey(analogRead(A5))]);
    }else{
      noTone(9);
    }
  }else{ //Arrp mode:sometime make liblary
    
  }
  
  Serial.print("voltage");
  Serial.println(pushkey(analogRead(A5)));
  Serial.print("octaveU");
  Serial.println(digitalRead(octUP));
  Serial.print("octaveD");
  Serial.println(digitalRead(octDOWN));
  Serial.print("octave");
  Serial.println(keyshift);
  Serial.print("dial1");
  Serial.println(analogRead(A0));
  delay(500);
}

int pushkey(int pushSwitch){
  if(pushSwitch < 50){
    return 0; //0
  }else if(pushSwitch < 100){
    return 1; //78
  }else if(pushSwitch < 200){
    return 2; //158
  }else if(pushSwitch < 300){
    return 3; //237
  }else if(pushSwitch < 350){
    return 4; //318
  }else if(pushSwitch < 420){
    return 5; //404
  }else if(pushSwitch < 500){
  	return 6; //477
  }else if(pushSwitch < 600){
	return 7; //552
  }else if(pushSwitch < 700){
 	return 8; //636
  }else if(pushSwitch < 740){
    return 9; //715
  }else if(pushSwitch < 840){
    return 10; //797
  }else if(pushSwitch < 900){
    return 11; //871
  }else if(pushSwitch < 1000){
    return 12; //947
  }
}

/*
0
78
158
237
318 
396 404
471 477
548 552
627 636
709 715
788 792
867 868
943 943
*/