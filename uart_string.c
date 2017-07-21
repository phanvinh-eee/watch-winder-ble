#include "uart_string.h"
#include "main.h"
extern uint8_t RxBuffer[];
extern __IO uint8_t RxCounter; 
extern uint8_t NbrOfDataToRead;
const char line[]="lin";
const char mch[]="mch";
const char tpd[]="tpd";
const char dir[]="dir";
const char power[]="pow";
const char reset[]="reset";
uint8_t p_char1=0;
uint8_t p_char2=0;
uint8_t line_str[2];
extern uint8_t mLine,mArea,mDir,mMCH;
extern uint8_t mTPD;
uint8_t mPow;
extern Motor_Typedef Mo[];
extern MCH_Typedef MCH_Set[];
extern char str[50];
uint8_t i=0;
uint8_t gsmchr(uint8_t str[],uint8_t size,uint8_t ch, uint8_t num){
	uint8_t t=0,i=0;
	for(i=0;i<size;i++){
		if(str[i]==ch)t++;
		if(t==num)return i;
	}
	return 0;
}

void RxInit(){
//	uint8_t i=0;
	RxCounter=0;
	RxBuffer[0]=0;
}
uint8_t char2num(char ch){
	uint8_t num=0;
	switch(ch){
		case '0':num=0;
		break;
		case '1':num=1;
		break;
		case '2':num=2;
		break;
		case '3':num=3;
		break;
		case '4':num=4;
		break;
		case '5':num=5;
		break;
		case '6':num=6;
		break;
		case '7':num=7;
		break;
		case '8':num=8;
		break;
		case '9':num=9;
		break;
	}
	return num;
}
uint8_t read_buffer(){ 	
	if(strstr((char*)RxBuffer,line)!=0){
		return 1;
	}
	else if(strstr((char*)RxBuffer,mch)!=0){
		return 2;
	}
	else if(strstr((char*)RxBuffer,tpd)!=0){
		return 3;
	}
	else if(strstr((char*)RxBuffer,dir)!=0){
		return 4;
	}
	else if(strstr((char*)RxBuffer,power)!=0){
		return 5;
	}
	else if(strstr((char*)RxBuffer,reset)!=0){
		Motor_Init();
	}
return 0;
}
