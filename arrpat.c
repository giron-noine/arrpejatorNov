#include "arrpat.h"
#include "arduino.h"

//#include <mozzi_rand.h>

/*
int scaleMap[6][7] = {
  {60, 62, 64, 65, 67, 69, 71}, //major
  {60, 62, 63, 65, 67, 69, 70}, //dorian
  {60, 62, 64, 67, 69, 72, 74}, //penta
  {60, 63, 65, 67, 70, 72, 75}, //minor penta 
  {60, 62, 63, 65, 67, 68, 70}, //minor
  {60, 61, 64, 65, 67, 68, 71} //gypsy
};

int scaleMap[6][30] = {
  {38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88}, //major
  {38, 39, 41, 43, 45, 46, 48, 50, 51, 53, 55, 57, 58, 60, 62, 63, 65, 67, 69, 70, 72, 74, 75, 77, 79, 81, 82, 84, 86, 87}, //dorian
  {26, 28, 31, 33, 36, 38, 40, 43, 45, 48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72, 74, 76, 79, 81, 84, 86, 88, 91, 93, 96}, //penta
  {27, 29, 31, 34, 36, 39, 41, 43, 46, 48, 51, 53, 55, 58, 60, 63, 65, 67, 70, 72, 75, 77, 79, 82, 84, 87, 89, 91, 94, 96}, //minor penta
  {60, 62, 63, 65, 67, 68, 70}, //minor
  {60, 61, 64, 65, 67, 68, 71} //gypsy
};
*/

//int noteNum[13] = {60,61,62,63,64,65,66,67,68,69,70,71,72};

int noteNum;

int scaleMap[6][7] = {
	{0, 2, 4, 5, 7, 9, 11}, //major
	{0, 2, 3, 5, 7, 9, 10}, //dorian
	{0, 2, 4, 7, 9, 12, 14}, //penta
	{0, 2, 3, 5, 7, 8, 10}, //minor
	{0, 2, 3, 5, 7, 9, 11}, //melo minor
	{0, 1, 4, 5, 7, 8, 11}, //gypsy
};

int noiPatt(int patt, int scale, int note, int steps){

 	if(patt == 0){ //patt1 is step up
		noteNum = note + scaleMap[scale][steps] + 60; //60 is 0 to midi note C
		return noteNum;
	
	}else if(patt == 1){ //patt2 is step down
		switch(steps){
			case 0:
				noteNum = note + scaleMap[scale][6] + 60;
				break;
			case 1:
				noteNum = note + scaleMap[scale][5] + 60;
				break;
			case 2:
				noteNum = note + scaleMap[scale][4] + 60;
				break;
			case 3:
				noteNum = note + scaleMap[scale][3] + 60;
				break;
			case 4:
				noteNum = note + scaleMap[scale][2] + 60;
				break;
			case 5:
				noteNum = note + scaleMap[scale][1] + 60;
				break;
			case 6:
				noteNum = note + scaleMap[scale][0] + 60;
				break;
		}
		return noteNum;

	}else if(patt == 2){ //patt3 is skip up
		switch(steps){
			case 0:
				noteNum = note + scaleMap[scale][0] + 60;
				break;
			case 1:
				noteNum = note + scaleMap[scale][3] + 60;
				break;
			case 2:
				noteNum = note + scaleMap[scale][1] + 60;
				break;
			case 3:
				noteNum = note + scaleMap[scale][4] + 60;
				break;
			case 4:
				noteNum = note + scaleMap[scale][2] + 60;
				break;
			case 5:
				noteNum = note + scaleMap[scale][6] + 60;
				break;
			case 6:
				noteNum = note + scaleMap[scale][5] + 60;
				break;
		}
		return noteNum;
	}else if(patt == 3){ //patt3 is skip down
		switch(steps){
			case 0:
				noteNum = note + scaleMap[scale][6] + 60;
				break;
			case 1:
				noteNum = note + scaleMap[scale][3] + 60;
				break;
			case 2:
				noteNum = note + scaleMap[scale][5] + 60;
				break;
			case 3:
				noteNum = note + scaleMap[scale][2] + 60;
				break;
			case 4:
				noteNum = note + scaleMap[scale][4] + 60;
				break;
			case 5:
				noteNum = note + scaleMap[scale][0] + 60;
				break;
			case 6:
				noteNum = note + scaleMap[scale][1] + 60;
				break;
		}
		return noteNum;
	} /*
	else if(patt == 4){ //patt4 is random
			switch(steps){
			case 0:
				noteNum = note + scaleMap[scale][rand(7)] + 60;
				break;
			case 1:
				noteNum = note + scaleMap[scale][rand(7)] + 60;
				break;
			case 2:
				noteNum = note + scaleMap[scale][rand(7)] + 60;
				break;
			case 3:
				noteNum = note + scaleMap[scale][rand(7)] + 60;
				break;
			case 4:
				noteNum = note + scaleMap[scale][rand(7)] + 60;
				break;
			case 5:
				noteNum = note + scaleMap[scale][rand(7)] + 60;
				break;
			case 6:
				noteNum = note + scaleMap[scale][rand(7)] + 60;
				break;
		}
		return noteNum; 

  } */ 
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
