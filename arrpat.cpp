#include "arrpat.h"
#include "arduino.h"

#include <mozzi_rand.h>


int noteNum;

int scaleMap[6][16] = {
	{0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 23, 24, 26, 28}, //major
	{0, 2, 3, 5, 7, 9, 10, 12, 14, 15, 17, 19, 21, 22, 24, 26}, //dorian
	{0, 2, 4, 7, 9, 12, 14, 16, 19, 21, 24, 26, 28, 31, 33, 36}, //penta
	{0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26}, //minor
	{0, 2, 3, 5, 7, 9, 11, 12, 14, 15, 17, 19, 21, 23, 24, 26}, //melo minor
	{0, 1, 4, 5, 7, 8, 11, 12, 13, 16, 17, 19, 20, 24, 24, 25}, //gypsy
};

int noiPatt(int patt, int scale, int note, int steps){

 	if(patt == 0){ //patt1 is step up
		noteNum = note + scaleMap[scale][steps] + 60; //60 is 0 to midi note C
		return noteNum;
	
	}else if(patt == 1){ //patt2 is step down
		switch(steps){
			case 0:
				noteNum = note + scaleMap[scale][15] + 60;
				break;
			case 1:
				noteNum = note + scaleMap[scale][14] + 60;
				break;
			case 2:
				noteNum = note + scaleMap[scale][13] + 60;
				break;
			case 3:
				noteNum = note + scaleMap[scale][12] + 60;
				break;
			case 4:
				noteNum = note + scaleMap[scale][11] + 60;
				break;
			case 5:
				noteNum = note + scaleMap[scale][10] + 60;
				break;
			case 6:
				noteNum = note + scaleMap[scale][9] + 60;
				break;
      		case 7:
      			noteNum = note + scaleMap[scale][8] + 60;
        		break;
      		case 8:
        		noteNum = note + scaleMap[scale][7] + 60;
        		break;
      		case 9:
        		noteNum = note + scaleMap[scale][6] + 60;
        		break;
      		case 10:
        		noteNum = note + scaleMap[scale][5] + 60;
        		break;
     		case 11:
        		noteNum = note + scaleMap[scale][4] + 60;
        		break;
      		case 12:
        		noteNum = note + scaleMap[scale][3] + 60;
        		break;
      		case 13:
        		noteNum = note + scaleMap[scale][2] + 60;
        		break;
      		case 14:
        		noteNum = note + scaleMap[scale][1] + 60;
        		break;
      		case 15:
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
				noteNum = note + scaleMap[scale][5] + 60;
				break;
			case 6:
				noteNum = note + scaleMap[scale][3] + 60;
				break;
      		case 7:
       			noteNum = note + scaleMap[scale][6] + 60;
        		break;
      		case 8:
        		noteNum = note + scaleMap[scale][4] + 60;
        		break;
      		case 9:
       			noteNum = note + scaleMap[scale][7] + 60;
        		break;
      		case 10:
        		noteNum = note + scaleMap[scale][5] + 60;
        		break;
      		case 11:
        		noteNum = note + scaleMap[scale][8] + 60;
        		break;
      		case 12:
        		noteNum = note + scaleMap[scale][6] + 60;
        		break;
      		case 13:
        		noteNum = note + scaleMap[scale][9] + 60;
        		break;
      		case 14:
        		noteNum = note + scaleMap[scale][7] + 60;
        		break;
      		case 15:
        		noteNum = note + scaleMap[scale][10] + 60;
        		break;
			}
		return noteNum;

	}else if(patt == 3){ //patt3 is skip center
		switch(steps){
			case 0:
				noteNum = note + scaleMap[scale][0] + 60;
				break;
			case 1:
				noteNum = note + scaleMap[scale][15] + 60;
				break;
			case 2:
				noteNum = note + scaleMap[scale][1] + 60;
				break;
			case 3:
				noteNum = note + scaleMap[scale][14] + 60;
				break;
			case 4:
				noteNum = note + scaleMap[scale][3] + 60;
				break;
			case 5:
				noteNum = note + scaleMap[scale][13] + 60;
				break;
			case 6:
				noteNum = note + scaleMap[scale][4] + 60;
				break;
      		case 7:
        		noteNum = note + scaleMap[scale][12] + 60;
        		break;
      		case 8:
        		noteNum = note + scaleMap[scale][5] + 60;
        		break;
      		case 9:
        		noteNum = note + scaleMap[scale][11] + 60;
        		break;
      		case 10:
        		noteNum = note + scaleMap[scale][6] + 60;
        		break;
      		case 11:
        		noteNum = note + scaleMap[scale][10] + 60;
        		break;
      		case 12:
        		noteNum = note + scaleMap[scale][7] + 60;
        		break;
      		case 13:
        		noteNum = note + scaleMap[scale][9] + 60;
        		break;
      		case 14:
        		noteNum = note + scaleMap[scale][8] + 60;
        		break;
      		case 15:
        		noteNum = note + scaleMap[scale][9] + 60;
        		break;
			}
		return noteNum;
	}
	else if(patt == 4){ //patt4 is random
			switch(steps){
			case 0:
				noteNum = note + scaleMap[scale][rand(16)] + 60;
				break;
			case 1:
				noteNum = note + scaleMap[scale][rand(16)] + 60;
				break;
			case 2:
				noteNum = note + scaleMap[scale][rand(16)] + 60;
				break;
			case 3:
				noteNum = note + scaleMap[scale][rand(16)] + 60;
				break;
			case 4:
				noteNum = note + scaleMap[scale][rand(16)] + 60;
				break;
			case 5:
				noteNum = note + scaleMap[scale][rand(16)] + 60;
				break;
			case 6:
				noteNum = note + scaleMap[scale][rand(16)] + 60;
				break;
      		case 7:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
      		case 8:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
     		case 9:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
      		case 10:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
      		case 11:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
      		case 12:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
      		case 13:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
      		case 14:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
      		case 15:
        		noteNum = note + scaleMap[scale][rand(16)] + 60;
        		break;
			}
		return noteNum; 
  } 
}


int pushkey(int pushSwitch){
  if(pushSwitch < 50){
    return 0; //0 10kohm
  }else if(pushSwitch < 150){
    return 1; //78 820ohm
  }else if(pushSwitch < 300){
    return 2; //158 1kohm
  }else if(pushSwitch < 400){
    return 3; //237 1.2kohm
  }else if(pushSwitch < 500){
    return 4; //318 1.5kohm
  }else if(pushSwitch < 600){
    return 5; //404 2kohm
  }else if(pushSwitch < 660){
    return 6; //477 2.2kohm
  }else if(pushSwitch < 730){
  return 7; //552 3kohm
  }else if(pushSwitch < 800){
  return 8; //636 4.7kohm
  }else if(pushSwitch < 850){
    return 9; //715 6.8kohm
  }else if(pushSwitch < 900){
    return 10; //797 12kohm
  }else if(pushSwitch < 950){
    return 11; //871 22kohm
  }else if(pushSwitch < 1000){
    return 12; //947 68kohm
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
