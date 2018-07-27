#include "base.h"
#include "seg.h"

void setup_7seg(void)
{
	gpio[GPSET/4] |= (1 << 6) + (1 << 12) + (1 << 13) + (1 << 16) + (1 << 19) + (1 << 20) + (1 << 26);
}
void * use_7seg(void * args)
{

// change option to output
//
//clear_7seg();
int i = 0;
while(1)
  {
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
	
	GPSEL * Sel0 = &gpio[GPFSEL0/4];
	GPSEL * Sel1 = &gpio[GPFSEL1/4];
	GPSEL * Sel2 = &gpio[GPFSEL2/4];

	clear_7seg(Sel0, Sel1, Sel2);

	if(seg == 4)
	{
		Sel0->sel6 = 1;
		Sel1->sel2 = 1;
		Sel1->sel9 = 1;
		Sel2->sel6 = 1;
	
	}
	else if(seg == 5)
	{	
		Sel1->sel2 = 1;
		Sel1->sel3 = 1;
		Sel1->sel6 = 1;
		Sel1->sel9 = 1;
		Sel2->sel6 = 1;
		
	}
	else if(seg == 6)
	{
		Sel1->sel2 = 1;
		Sel1->sel6 = 1;
		Sel1->sel9 = 1;
		Sel2->sel0 = 1;
		Sel2->sel6 = 1;
	}
	else if(seg == 7)
	{
		Sel0->sel6 = 1;
		Sel1->sel2 = 1;
		Sel1->sel3 = 1;
		Sel1->sel9 = 1;
	}
	else if(seg == 8)
	{
		Sel0->sel6 = 1;
		Sel1->sel2 = 1;
		Sel1->sel3 = 1;
		Sel1->sel6 = 1;
		Sel1->sel9 = 1;
		Sel2->sel0 = 1;
		Sel2->sel6 = 1;
	
	}

}
void clear_7seg(GPSEL * Sel0, GPSEL * Sel1, GPSEL * Sel2)
{
	Sel0->sel6 = 0;
	Sel1->sel2 = 0;
	Sel1->sel3 = 0;
	Sel1->sel6 = 0;
	Sel1->sel9 = 0;
	Sel2->sel0 = 0;
	Sel2->sel6 = 0;
}

