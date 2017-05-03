/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-05-01 11:18:59.617950.
*/
/**
\brief A CoAP resource which allows an application to GET/SET the state of the
   error LED.
*/

#include <stdio.h>

#include "opendefs_obj.h"
#include "ccalefaccio_obj.h"
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
#define CTEMPOPERIOD  100000
#define PAYLOADLEN      40

//=========================== variables =======================================

ccalefaccio_vars_t ccalefaccio_vars;

// declaration of global variable _random_vars_ removed during objectification.

const uint8_t ccalefaccio_path0[]       = "Actuador_Calefaccio";

int min_num, i;
int max_num;
uint16_t resultado, estado, dig0, dig1;
char str[1], int2string[1];
char aChar;

char digitos1[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

//=========================== prototypes ======================================

owerror_t ccalefaccio_receive(
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);

void    ccalefaccio_timer_cb(opentimer_id_t id);
void    ccalefaccio_task_cb(void);
//int     ccalefaccio_openrandomtemp (void);

void     ccalefaccio_sendDone(
   OpenQueueEntry_t* msg,
   owerror_t error
);

//=========================== public ==========================================

void ccalefaccio_init() {
 
   //// prepare the resource descriptor for the /Actuador_Calefaccio path
   ccalefaccio_vars.desc.path0len            = sizeof(ccalefaccio_path0)-1;
   ccalefaccio_vars.desc.path0val            = (uint8_t*)(&ccalefaccio_path0);
   ccalefaccio_vars.desc.path1len            = 0;
   ccalefaccio_vars.desc.path1val            = NULL;
   ccalefaccio_vars.desc.componentID         = COMPONENT_CCALEFACCIO;
   ccalefaccio_vars.desc.discoverable        = TRUE;
   ccalefaccio_vars.desc.callbackRx          = &ccalefaccio_receive;
   ccalefaccio_vars.desc.callbackSendDone    = &ccalefaccio_sendDone;
   
   // register with the CoAP module
 opencoap_register(self, &ccalefaccio_vars.desc);
   
   ccalefaccio_vars.time1Id    = opentimers_start(self, CTEMPOPERIOD,
                                                TIMER_PERIODIC,TIME_MS,
                                                ccalefaccio_timer_cb);
   

}

//=========================== private =========================================

//timer fired, but we don't want to execute task in ISR mode
//instead, push task to scheduler with COAP priority, and let scheduler take care of it
void ccalefaccio_timer_cb(opentimer_id_t id){
 scheduler_push_task(self, ccalefaccio_task_cb,TASKPRIO_COAP);
}


void ccalefaccio_task_cb() {

//uint16_t             avg         = 0;

// don't run if not synch
   if ( ieee154e_isSynch(self) == FALSE) return;
   
   // don't run on dagroot
   if ( idmanager_getIsDAGroot(self)) {
 opentimers_stop(self, ccalefaccio_vars.time1Id);
      return;
   }


//avg = openrandom_get16b(self);


}


//int ccalefaccio_openrandomtemp (){
	
	//int resultado = 0, low_num = 0, hi_num = 0;
    //min_num=0;
	//max_num=1;

    //if (min_num < max_num)
    //{
        //low_num = min_num;
        //hi_num = max_num + 1; // include max_num in output
    //} else {
        //low_num = max_num + 1; // include max_num in output
        //hi_num = min_num;
    //}

    //srand(time(NULL));
    //resultado = (rand() % (hi_num - low_num)) + low_num;
    //printf("Min=0 Max=1 TEMP=%d\n",resultado);
    //return resultado;

//}


/**
\brief Called when a CoAP message is received for this resource.

\param[in] msg          The received message. CoAP header and options already
   parsed.
\param[in] coap_header  The CoAP header contained in the message.
\param[in] coap_options The CoAP options contained in the message.

\return Whether the response is prepared successfully.
*/
owerror_t ccalefaccio_receive(
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
 packetfunctions_reserveHeaderSize(self, msg,2);
         msg->payload[0]                  = COAP_PAYLOAD_MARKER;

         if ( leds_error_isOn(self)==1) {
            msg->payload[1]               = '1';
         } else {
            msg->payload[1]               = '0';
         }
            
         // set the CoAP header
         coap_header->Code                = COAP_CODE_RESP_CONTENT;
         
         outcome                          = E_SUCCESS;
         break;
      
      case COAP_CODE_REQ_PUT:
      
         // change the LED's state
         if (msg->payload[0]=='1') {
 leds_error_on(self);
         } else if (msg->payload[0]=='2') {
 leds_error_toggle(self);
         } else {
 leds_error_off(self);
         }
         
         // reset packet payload
         msg->payload                     = &(msg->packet[127]);
         msg->length                      = 0;
         
         // set the CoAP header
         coap_header->Code                = COAP_CODE_RESP_CHANGED;
         
         outcome                          = E_SUCCESS;
         break;
         
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
void ccalefaccio_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
 openqueue_freePacketBuffer(self, msg);
}
