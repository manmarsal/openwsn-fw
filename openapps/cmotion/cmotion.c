/**
\brief A CoAP resource which allows an application to GET/SET the state of the
   error LED.
*/

#include <stdio.h>

#include "opendefs.h"
#include "cmotion.h"
#include "opencoap.h"
#include "opentimers.h"
#include "openqueue.h"
#include "packetfunctions.h"
#include "leds.h"
#include "openrandom.h"
#include "scheduler.h"
#include "idmanager.h"
#include "IEEE802154E.h"

//=========================== defines =========================================

/// inter-packet period (in ms)
#define CMOTIONPERIOD  100000
#define PAYLOADLEN      40

//=========================== variables =======================================

cmotion_vars_t cmotion_vars;

//random_vars_t random_vars;

const uint8_t cmotion_path0[]       = "motion";

int min_num, i;
int max_num;
uint16_t resultado, motion, dig0, dig1;
char str[1], int2string[1];
char aChar;

char digits2[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

//=========================== prototypes ======================================

owerror_t cmotion_receive(
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);

void    cmotion_timer_cb(opentimer_id_t id);
void    cmotion_task_cb(void);
int     cmotion_openrandomtemp (void);

void     cmotion_sendDone(
   OpenQueueEntry_t* msg,
   owerror_t error
);

//=========================== public ==========================================

void cmotion_init() {
	
   //if(idmanager_getIsDAGroot()==TRUE) return;
   
   
   //// prepare the resource descriptor for the /cmotion path
   cmotion_vars.desc.path0len            = sizeof(cmotion_path0)-1;
   cmotion_vars.desc.path0val            = (uint8_t*)(&cmotion_path0);
   cmotion_vars.desc.path1len            = 0;
   cmotion_vars.desc.path1val            = NULL;
   cmotion_vars.desc.componentID         = COMPONENT_CMOTION;
   cmotion_vars.desc.discoverable        = TRUE;
   cmotion_vars.desc.callbackRx          = &cmotion_receive;
   cmotion_vars.desc.callbackSendDone    = &cmotion_sendDone;
   
    // job specifically for mote bbbb::1415:92cc:0:3
   if(idmanager_getMyID(ADDR_16B)->addr_16b[1]!=0x03) return;
   
   // register with the CoAP module
   opencoap_register(&cmotion_vars.desc);
   printf ("inicializo cmotion_init\n");
   
   cmotion_vars.time2Id    = opentimers_start(CMOTIONPERIOD,
                                                TIMER_PERIODIC,TIME_MS,
                                                cmotion_timer_cb);

}

//=========================== private =========================================

//timer fired, but we don't want to execute task in ISR mode
//instead, push task to scheduler with COAP priority, and let scheduler take care of it
void cmotion_timer_cb(opentimer_id_t id){
   scheduler_push_task(cmotion_task_cb,TASKPRIO_COAP);
}


void cmotion_task_cb() {

//uint16_t             avg         = 0;

// don't run if not synch
   if (ieee154e_isSynch() == FALSE) return;
   
   // don't run on dagroot
   if (idmanager_getIsDAGroot()) {
      opentimers_stop(cmotion_vars.time2Id);
      return;
   }


//avg = openrandom_get16b();


}


int cmotion_openrandomtemp (){
	
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
owerror_t cmotion_receive(
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
         packetfunctions_reserveHeaderSize(msg,2);
         msg->payload[0]                  = COAP_PAYLOAD_MARKER;

         //if (leds_error_isOn()==1) {
         //   msg->payload[1]               = '4';
         //   msg->payload[1]               = openrandom_get16b();
            
            motion = cmotion_openrandomtemp ();
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
            //leds_error_on();
         //} else if (msg->payload[0]=='2') {
            //leds_error_toggle();
         //} else {
            //leds_error_off();
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
void cmotion_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
   openqueue_freePacketBuffer(msg);
}
