#ifndef SEG_H
#define SEG_H

#ifdef __cplusplus
extern "C"{
#endif

void setup_7seg(void);
/** setup GPIO 6, 12, 13, 16 ,19, 20, 26
  * for using 7segment
  */
void * use_7seg(void * args);
/** use this function for thread function. 
  * repeat this function for control 7seg
  */
void show_7seg(int seg);
// * show percentage that you've set
   
void clear_7seg(GPSEL * Sel0, GPSEL * Sel1, GPSEL * Sel2);
// * Output Mode -> In Mode


#ifdef __cplusplus
}
#endif

#endif
