/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-05-01 10:47:05.308704.
*/
/**
\brief A CoAP resource which allows an application to GET/SET the state of the
   error LED.
*/

#include <stdio.h>

#include "opendefs_obj.h"
#include "cmotion_obj.h"
#include "opencoap_obj.h"
#include "opentimers_obj.h"
#include "openqueue_obj.h"
#include "packetfunctions_obj.h"
#include "leds_obj.h"
#include "openrandom_obj.h"
#include "scheduler_obj.h"
#include "idmanager_obj.h"
#include "IEEE802154E_obj.h"

//=========================== defines =========================================

/// inter-packet period (in ms)
#define CMOTIONPERIOD  100000
#define PAYLOADLEN      40

//=========================== variables =======================================

cmotion_vars_t cmotion_vars;

// declaration of global variable _random_vars_ removed during objectification.

const uint8_t cmotion_path0[]       = "motion";

int min_num, i;
int max_num;
uint16_t resultado, motion, dig0, dig1;
char str[1], int2string[1];
char aChar;

char digits2[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

//=========================== prototypes ======================================

owerror_t cmotion_receive(OpenMote* self, 
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);

void cmotion_timer_cb(OpenMote* self, opentimer_id_t id);
void cmotion_task_cb(OpenMote* self);
int cmotion_openrandomtemp(OpenMote* self);

void cmotion_sendDone(OpenMote* self, 
   OpenQueueEntry_t* msg,
   owerror_t error
);

//=========================== public ==========================================

void cmotion_init(OpenMote* self) {
 
   //// prepare the resource descriptor for the /l path
   cmotion_vars.desc.path0len            = sizeof(cmotion_path0)-1;
   cmotion_vars.desc.path0val            = (uint8_t*)(&cmotion_path0);
   cmotion_vars.desc.path1len            = 0;
   cmotion_vars.desc.path1val            = NULL;
   cmotion_vars.desc.componentID         = COMPONENT_CMOTION;
   cmotion_vars.desc.discoverable        = TRUE;
   cmotion_vars.desc.callbackRx          = &cmotion_receive;
   cmotion_vars.desc.callbackSendDone    = &cmotion_sendDone;
   
   // register with the CoAP module
 opencoap_register(self, &cmotion_vars.desc);
   
   cmotion_vars.time2Id    = opentimers_start(self, CMOTIONPERIOD,
                                                TIMER_PERIODIC,TIME_MS,
                                                cmotion_timer_cb);
   

}

//=========================== private =========================================

//timer fired, but we don't want to execute task in ISR mode
//instead, push task to scheduler with COAP priority, and let scheduler take care of it
void cmotion_timer_cb(OpenMote* self, opentimer_id_t id){
 scheduler_push_task(self, cmotion_task_cb,TASKPRIO_COAP);
}


void cmotion_task_cb(OpenMote* self) {

//uint16_t             avg         = 0;

// don't run if not synch
   if ( ieee154e_isSynch(self) == FALSE) return;
   
   // don't run on dagroot
   if ( idmanager_getIsDAGroot(self)) {
 opentimers_stop(self, cmotion_vars.time2Id);
      return;
   }


//avg = openrandom_get16b(self);


}


int cmotion_openrandomtemp(OpenMote* self){
	
	int resultado = 0, low_num = 0, hi_num = 0;
    min_num=0;
	max_num=1;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    srand(time(NULL));
    resultado = (rand() % (hi_num - low_num)) + low_num;
    printf("Min=0 Max=1 MOTION=%d\n",resultado);
    return resultado;

}


/**
\brief Called when a CoAP message is received for this resource.

\param[in] msg          The received message. CoAP header and options already
   parsed.
\param[in] coap_header  The CoAP header contained in the message.
\param[in] coap_options The CoAP options contained in the message.

\return Whether the response is prepared successfully.
*/
owerror_t cmotion_receive(OpenMote* self, 
      OpenQueueEntry_t* msg,
      coap_header_iht*  coap_header,
      coap_option_iht*  coap_options
   ) {
   owerror_t outcome;
   
   switch (coap_header->Code) {
      case COAP_CODE_REQ_GET:
         // reset packet payload
         msg->payload                     = &(msg->packet[127]);
         msg->length                      = 0;
         
         // add CoAP payload
         // se define el tamaño del mensaje util (payload)
 packetfunctions_reserveHeaderSize(self, msg,2);
         msg->payload[0]                  = COAP_PAYLOAD_MARKER;

         //if ( leds_error_isOn(self)==1) {
         //   msg->payload[1]               = '4';
         //   msg->payload[1]               = openrandom_get16b(self);
            
            motion = cmotion_openrandomtemp(self);
            //int2string = sprintf(str,"%d", temp);
            //printf("TEMP=%d\n",temp);
            
            //dig0 =  % 10;
            //printf("dig0=%d\n",dig0);
            //temp = temp/10;
            //printf("TEMP=%d\n",temp);
            //dig1 = temp % 10;
            //printf("dig1=%d\n",dig1);
            //int2string = sprintf(str,"%d", dig0);
            //msg->payload[1]               = dig0;
            aChar = digits2[motion];
            msg->payload[1]               = aChar;
                      
            //printf("msg->payload[1]=%s\n",str);
            //int2string = sprintf(str,"%d", dig1);
            //aChar = digits[dig0];
            //msg->payload[2]               = aChar;
            
            //printf("msg->payload[2]=%s\n",str);  
         //} else {
         //   msg->payload[1]               = '7';
         //}
            
         // set the CoAP header
         coap_header->Code                = COAP_CODE_RESP_CONTENT;
         
         outcome                          = E_SUCCESS;
         break;
      
      //case COAP_CODE_REQ_PUT:
      
         //// change the LED's state
         //if (msg->payload[0]=='1') {
            // leds_error_on(self);
         //} else if (msg->payload[0]=='2') {
            // leds_error_toggle(self);
         //} else {
            // leds_error_off(self);
         //}
         
         //// reset packet payload
         //msg->payload                     = &(msg->packet[127]);
         //msg->length                      = 0;
         
         //// set the CoAP header
         //coap_header->Code                = COAP_CODE_RESP_CHANGED;
         
         //outcome                          = E_SUCCESS;
         //break;
         
      default:
         outcome                          = E_FAIL;
         break;
   }
   
   return outcome;
}

/**
\brief The stack indicates that the packet was sent.

\param[in] msg The CoAP message just sent.
\param[in] error The outcome of sending it.
*/
void cmotion_sendDone(OpenMote* self, OpenQueueEntry_t* msg, owerror_t error) {
 openqueue_freePacketBuffer(self, msg);
}
