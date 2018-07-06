#include "7seg.h"
extern void clearbit(volatile unsigned int * x, int n);
extern void setbit(volatile unsigned int * x, int n);
extern int getbit(volatile unsigned int x, int n);
void setup_7seg(void)
{
	GPSEL * Sel0 = &gpio[GPFSEL0/4];
	GPSEL * Sel1 = &gpio[GPFSEL1/4];
	GPSEL * Sel2 = &gpio[GPFSEL2/4];
	Sel0->sel6 = 1;
	Sel1->sel2 = 1;
	Sel1->sel3 = 1;
	Sel1->sel6 = 1;
	Sel1->sel9 = 1;
	Sel2->sel0 = 1;
	Sel2->sel6 = 1;
	clear_7seg();
	printf("%d \n",Sel0 ->sel6 );
	//setup GPIO 6, 12, 13,16, 19, 20, 26
}
void use_7seg(void * args)
{

// change option to output
//
clear_7seg();
int i = 0;
while(1)
    {	
	printf("%d 번째 seg_value = %d\n",i++ , seg_value );
	sleep(1);
	if(seg_value < 4 )
	{
		seg_value = 4;
		show_7seg(seg_value);
	}
	else if(seg_value <= 8)
	{ 
		show_7seg(seg_value);
	}
	else if(seg_value > 8)
	{
		seg_value = 8;
		show_7seg(seg_value);
		
	}

    }	
}
void show_7seg(int seg)
{
	printf("seg = %d\n", seg);
	if(seg == 4)
	{
		/*
		setbit(&gpio[GPSET/4] , 6);
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 19);
		setbit(&gpio[GPSET/4] , 26);


		*/
		printf("gpiobase = %X\n",gpio);
		gpio[GPSET/4] = 0;
		gpio[GPSET/4] |= 1<<6;
		printf("gpset = %X",gpio[GPSET/4]);

	}
	else if(seg == 5)
	{	
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 13);
		setbit(&gpio[GPSET/4] , 16);
		setbit(&gpio[GPSET/4] , 19);
		setbit(&gpio[GPSET/4] , 26);



	}
	else if(seg == 6)
	{
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 16);
		setbit(&gpio[GPSET/4] , 19);
		setbit(&gpio[GPSET/4] , 20);
		setbit(&gpio[GPSET/4] , 26);

	}
	else if(seg == 7)
	{
		setbit(&gpio[GPSET/4] , 6);
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 13);
		setbit(&gpio[GPSET/4] , 19);
		}
	else if(seg == 8)
	{
		setbit(&gpio[GPSET/4] , 6);
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 13);
		setbit(&gpio[GPSET/4] , 16);
		setbit(&gpio[GPSET/4] , 19);
		setbit(&gpio[GPSET/4] , 20);
		setbit(&gpio[GPSET/4] , 26);

	}

}
void clear_7seg(void)
{
	setbit(&gpio[GPCLR/4] , 6);
	setbit(&gpio[GPCLR/4] , 12);
	setbit(&gpio[GPCLR/4] , 13);
	setbit(&gpio[GPCLR/4] , 16);
	setbit(&gpio[GPCLR/4] , 19);
	setbit(&gpio[GPCLR/4] , 20);
	setbit(&gpio[GPCLR/4] , 26);

	clearbit(&gpio[GPCLR/4] , 6);
	clearbit(&gpio[GPCLR/4] , 12);
	clearbit(&gpio[GPCLR/4] , 13);
	clearbit(&gpio[GPCLR/4] , 16);
	clearbit(&gpio[GPCLR/4] , 19);
	clearbit(&gpio[GPCLR/4] , 20);
	clearbit(&gpio[GPCLR/4] , 26);
	usleep(20);
	printf("GPCLR = %d\n",getbit(gpio[GPCLR/4], 6));
}











