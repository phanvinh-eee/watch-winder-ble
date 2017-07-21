#include "lcd_led.h"
#include "main.h"
extern TIM_OCInitTypeDef  TIM1_OCInitStructure;
uint8_t digits[]={0xaf, 0xa0, 0x6d, 0xe9, 0xe2,0xcb,0xcf,0xa1,0xef,0xeb,0x00};
uint8_t area[]={8,4,2,1,0};
uint8_t DIR[]={0,0,0,0,0,0};
uint32_t tim_s[]={0,0,0,0,0,0};
char str[50];
extern uint8_t mLine,mArea,mDir,mMCH;
extern uint16_t mTPD;
extern Motor_Typedef Mo[];
extern uint8_t mNum;
extern MCH_Typedef MCH_Set[];
extern uint16_t TPD_Speed[];
extern uint16_t TPD_Delay[];
extern __IO uint8_t RxCounter;
extern char RxBuffer[];
void printnum(uint8_t num, uint8_t pos_led, uint8_t dp){
  uint8_t add1, add2,hi,lo;
  add1=pos_led;
  add2=add1+4;  
  hi=(digits[num]>>4)&0xf;
  if(dp==1) hi=hi+1;
  lo=(digits[num]>>0)&0xf;
  write_1621(add1,hi);
  write_1621(add2,lo);
}
void print_mch(uint8_t num){
  printnum(num, 48, 1);
}
void print_line(uint8_t num){
  uint8_t num1,num2;
  num1=num/10;
  num2=num%10;
  printnum(num1,64,1);
  printnum(num2,56,0);
}
void print_tpd(uint16_t num, uint8_t dir,uint8_t pow){
  uint16_t num1,num2,num3,num4;
  num4=num/1000;
  if(num4==0)
    printnum(10,40,1);
  else
    printnum(num4,40,1);
  
  num4=num%1000;
  num3=num4/100;
	if(pow==1)
		printnum(num3,32,1);
	else 
		printnum(num3,32,0);
  
  num3=num4%100;
  num2=num3/10;
  if(dir!=1)
    printnum(num2,24,1);
  else 
    printnum(num2,24,0);
  
  num1=num3%10;
  if(dir!=0)
    printnum(num1,16,1);
  else
    printnum(num1,16,0);
}
void print_area(uint8_t num){
  write_1621(4,area[num-1]);
}
void print_screen(uint8_t area,uint8_t line, uint8_t mch, uint16_t tpd, uint8_t dir,uint8_t pow){
  print_area(area);
  print_line(line);
  print_mch(mch);
  print_tpd(tpd,dir,pow);
}
void sendbit_1621(uint8_t data,uint8_t cnt)
{
  unsigned char i;
  
  for (i=0;i<cnt;i++)
  {
    output_low(wr);
    delay_20us(1);
    if ((data&0x80)==0) 
      output_low(dat);
    else 
      output_high(dat);
    
    delay_20us(1);
    output_high(wr);
    delay_20us(1);
    data<<=1;
  }
}

void senddatabit_1621(uint8_t data,uint8_t cnt)
{
  unsigned char i;
  for(i =0; i <cnt; i ++)
  {
    output_low(wr);
    delay_20us(1);
    if((data&0x01)==0) output_low(dat);
    else output_high(dat);
    delay_20us(1);
    output_high(wr);
    delay_20us(1);
    data>>=1;    
  }
}

void lcd1621_init(void)
{  
  output_high(cs);  
  output_high(wr);
  output_high(dat);
}

void sendcmd(uint8_t command)
{
  //  lcd1621_init();
  output_low(cs);
  sendbit_1621(0x80,4);
  sendbit_1621(command,8);
  output_high(cs);
}

void write_1621(uint8_t addr,uint8_t data)
{
  output_low(cs);
  sendbit_1621(0xa0,3);
  sendbit_1621(addr,6);
  senddatabit_1621(data,4);
  output_high(cs);
}

void writeall_1621(uint8_t addr,uint8_t *p,uint8_t cnt )
{
  unsigned char i;
  output_low(cs);
  sendbit_1621(0xa0,3);
  sendbit_1621(addr<<3,6);
  for (i=1;i<cnt;i++,p++)
  {
    senddatabit_1621(*p,8);
  }
  output_high(cs);
}
void lcd_clear(void){
  uint8_t i=0;
  for (i = 1 ; i < 70 ; i++) {
    write_1621(i, 0x0);
  }
}
uint8_t button5(void){
  if(GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_15)==0)
  {
    while(GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_15)==0)
      delay_ms(100);
	return 1;
	}
	else if(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_3)==0)
  {
    while(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_3)==0)
      delay_ms(100);
		return 2;
	}
	else if(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_2)==0)
  {
    while(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_2)==0)
      delay_ms(100);
		return 3;
	}
	else if(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_5)==0)
  {
    while(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_5)==0)
      delay_ms(100);
		return 4;
	}
	else if(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_4)==0)
  {
    while(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_4)==0)
      delay_ms(100);
		return 5;
	}
	return 0;
}
void main_processing(void){
  uint8_t i=0;
	uint8_t val=0;
	val=(uint8_t)button5();
	switch(val){
		case 0:
			delay_ms(80);
			if(RxCounter!=0){
				val=read_buffer();
				USART_SendString(USART3,RxBuffer);
				RxInit();
			}
			break;
		case 1:
			mLine++;		
			if(mLine>23){
				mLine=0;
			}
			mMCH=Mo[mLine].MCH;
			mTPD=Mo[mLine].Speed;		
			mDir=Mo[mLine].Di;	
			mArea=mLine/6;  		
			break;
		case 2:
			mMCH++;    
			if(mMCH>3) mMCH=0;
			if(Mo[mLine].Pow==RUN)
				Mo[mLine].Pow=SLEEP;
			Mo[mLine].MCH=mMCH;
			Mo[mLine].Speed=MCH_Set[mMCH].Speed;
			Mo[mLine].Di=MCH_Set[mMCH].Di;
			if(mLine>5){
				sprintf(str,"mch%02d%d%d%d%d",mLine-6,mMCH,Mo[mLine].Speed,Mo[mLine].Di,SLEEP);
				USART_SendString(USART2,str);
			}
			break;  
		case 3:
			mTPD++;
			if(mTPD>3) mTPD=0;
			if(Mo[mLine].Pow==RUN)
				Mo[mLine].Pow=SLEEP;
			MCH_Set[mMCH].Speed=mTPD;
			sprintf(str,"tpd%02d%d%d%d%d",mLine-6,mMCH,mTPD,mDir,SLEEP);
			USART_SendString(USART2,str);
			for(i=0;i<24;i++)
				if(Mo[i].MCH==mMCH){
					Mo[i].Speed=mTPD;
				}
			break;
			
		case 4:
			mDir++;
			if(mDir>2)mDir=0;
			if(Mo[mLine].Pow==RUN)
				Mo[mLine].Pow=SLEEP;
			MCH_Set[mMCH].Di=mDir; 					
			sprintf(str,"dir%02d%d%d%d%d",mLine-6,mMCH,mTPD,mDir,SLEEP);
			USART_SendString(USART2,str);
			for(i=0;i<24;i++)
				if(Mo[i].MCH==mMCH){
					Mo[i].Di=mDir;		
				}
			break;
  
		case 5:
			if(Mo[mLine].Pow==RUN)
				Mo[mLine].Pow=SLEEP;
			else
			{
				Mo[mLine].Pow=RUN;
				if(mLine<6)
				motor_res(mLine);
			}
			if(mLine>5){
				sprintf(str,"poo%02d%d%d%d%d",mLine-6,mMCH,Mo[mLine].Speed,Mo[mLine].Di,Mo[mLine].Pow);
				USART_SendString(USART2,str);
			}
			break;
		default:
			break;
	}
}
void motor_res(uint8_t mo)
{
	tim_s[mo]=0;
	if(Mo[mo].Di==BOTH)
		DIR[mo]=1-DIR[mo];
	else
		DIR[mo]=Mo[mo].Di;
	motor_run(mo,DIR[mo],CCR4_Val);
}
void motor_run(uint8_t mo, uint8_t dir, uint16_t ccr)
{
	uint16_t ccr1;
	uint16_t ccr2;
	if(dir==1){
		ccr1=(uint16_t)ccr;
		ccr2=(uint16_t)CCR2_Val;
	}
	else{
		ccr1=CCR2_Val;
		ccr2=ccr;		
	}
switch(mo){  
	case 0:		
		TIM3->CCR3=ccr1;
		TIM3->CCR4=ccr2;		
		break;	
	case 1:		
		TIM3->CCR1=ccr1;
		TIM3->CCR2=ccr2;
		break;
	case 2:		
		TIM2->CCR1=ccr1;
		TIM2->CCR2=ccr2;	
		break;
	case 3:
		TIM4->CCR3=ccr1;
		TIM4->CCR4=ccr2;
		break;
	case 4:
		TIM4->CCR1=ccr1;
		TIM4->CCR2=ccr2;
		break;
	case 5:
//		TIM1->CCR1=ccr1;
//		TIM1->CCR2=ccr2;	
	TIM1_OCInitStructure.TIM_Pulse = ccr1;	
	TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
	TIM1_OCInitStructure.TIM_Pulse = ccr2;	
	TIM_OC2Init(TIM1, &TIM1_OCInitStructure);		
		break;
	}
}
