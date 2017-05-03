/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-30 00:40:10.691054.
*/
#include "opendefs_obj.h"
#include "openrandom_obj.h"
#include "idmanager_obj.h"

//=========================== variables =======================================

// declaration of global variable _random_vars_ removed during objectification.

//=========================== prototypes ======================================

//=========================== public ==========================================

void openrandom_init(OpenMote* self) {
   // seed the random number generator with the last 2 bytes of the MAC address
   (self->random_vars).shift_reg  = 0;
   (self->random_vars).shift_reg += idmanager_getMyID(self, ADDR_16B)->addr_16b[0]*256;
   (self->random_vars).shift_reg += idmanager_getMyID(self, ADDR_16B)->addr_16b[1];
}

uint16_t openrandom_get16b(OpenMote* self) {
   uint8_t  i;
   uint16_t random_value;
   random_value = 0;
   for(i=0;i<16;i++) {
      // Galois shift register
      // taps: 16 14 13 11
      // characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1
      random_value          |= ((self->random_vars).shift_reg & 0x01)<<i;
      (self->random_vars).shift_reg  = ((self->random_vars).shift_reg>>1)^(-(int16_t)((self->random_vars).shift_reg & 1)&0xb400);
   }
   return random_value;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// manel

//#include <stdio.h>

//int min_num;
//int max_num;
//int random_value;


//int main(void)
//{
    //printf("Min : 1 Max : 30 %d\n", random_number(0,5));
    //printf("Min : 100 Max : 1000 %d\n",random_number(100,1000));
    //return 0;
//}

//min_num=15;
	//max_num=25;

    //int result = 0, low_num = 0, hi_num = 0;

    //if (min_num < max_num)
    //{
        //low_num = min_num;
        //hi_num = max_num + 1; // include max_num in output
    //} else {
        //low_num = max_num + 1; // include max_num in output
        //hi_num = min_num;
    //}

    //srand(time(NULL));
    //random_value = (rand() % (hi_num - low_num)) + low_num;
    //random_value=23;

////}



//=========================== private =========================================
