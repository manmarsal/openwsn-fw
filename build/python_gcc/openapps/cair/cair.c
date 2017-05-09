/**
\brief A CoAP resource which allows an application to GET/SET the state of the
   error LED.
*/

#include <stdio.h>

#include "opendefs.h"
#include "cair.h"
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
#define CAIRPERIOD  100000
#define PAYLOADLEN      40

//=========================== variables =======================================

cair_vars_t cair_vars;

const uint8_t cair_path0[]       = "Aire";

//int min_num, i;
//int max_num;
//uint16_t resultado, estado, dig0, dig1;
//char str[1], int2string[1];
//char aChar;

//char digitos3[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

//=========================== prototypes ======================================

owerror_t cair_receive(
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);

void    cair_timer_cb(opentimer_id_t id);
void    cair_task_cb(void);

void     cair_sendDone(
   OpenQueueEntry_t* msg,
   owerror_t error
);

//=========================== public ==========================================

void cair_init() {
   
   //if(idmanager_getIsDAGroot()==TRUE) return; 
 
   //// prepare the resource descriptor for the /AC path
   cair_vars.desc.path0len            = sizeof(cair_path0)-1;
   cair_vars.desc.path0val            = (uint8_t*)(&cair_path0);
   cair_vars.desc.path1len            = 0;
   cair_vars.desc.path1val            = NULL;
   cair_vars.desc.componentID         = COMPONENT_CAIR;
   cair_vars.desc.discoverable        = TRUE;
   cair_vars.desc.callbackRx          = &cair_receive;
   cair_vars.desc.callbackSendDone    = &cair_sendDone;
   
   // register with the CoAP module
   opencoap_register(&cair_vars.desc);
   
   cair_vars.time3Id    = opentimers_start(CAIRPERIOD,
                                                TIMER_PERIODIC,TIME_MS,
                                                cair_timer_cb);
   

}

//=========================== private =========================================

//timer fired, but we don't want to execute task in ISR mode
//instead, push task to scheduler with COAP priority, and let scheduler take care of it
void cair_timer_cb(opentimer_id_t id){
   scheduler_push_task(cair_task_cb,TASKPRIO_COAP);
}


void cair_task_cb() {

//uint16_t             avg         = 0;

// don't run if not synch
   if (ieee154e_isSynch() == FALSE) return;
   
   // don't run on dagroot
   if (idmanager_getIsDAGroot()) {
      opentimers_stop(cair_vars.time3Id);
      return;
   }

}

/**
\brief Called when a CoAP message is received for this resource.

\param[in] msg          The received message. CoAP header and options already
   parsed.
\param[in] coap_header  The CoAP header contained in the message.
\param[in] coap_options The CoAP options contained in the message.

\return Whether the response is prepared successfully.
*/
owerror_t cair_receive(
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
         packetfunctions_reserveHeaderSize(msg,2);
         msg->payload[0]                  = COAP_PAYLOAD_MARKER;

         if (leds_error_isOn()==1) {
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
            leds_error_on();
         } else if (msg->payload[0]=='2') {
            leds_error_toggle();
         } else {
            leds_error_off();
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
void cair_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
   openqueue_freePacketBuffer(msg);
}
