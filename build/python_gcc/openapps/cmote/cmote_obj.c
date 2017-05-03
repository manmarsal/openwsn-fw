/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-30 21:01:05.970045.
*/
/**
\brief A CoAP resource which allows an application to GET/SET the state of the
   error LED.
*/

#include <stdio.h>

#include "opendefs_obj.h"
#include "cmote_obj.h"
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
#define CMOTEPERIOD  100000
#define PAYLOADLEN      10

//=========================== variables =======================================

cmote_vars_t cmote_vars;

// declaration of global variable _random_vars_ removed during objectification.

const uint8_t cmote_path0[]       = "mote";

int min_num;
int max_num;
int valor, temp, dig0, dig1;
char str[6], int2string;

//=========================== prototypes ======================================

owerror_t cmote_receive(OpenMote* self, 
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);

void cmote_timer_cb(OpenMote* self, opentimer_id_t id);
void cmote_task_cb(OpenMote* self);
int cmote_openrandomtemp(OpenMote* self);

void cmote_sendDone(OpenMote* self, 
   OpenQueueEntry_t* msg,
   owerror_t error
);

//=========================== public ==========================================

void cmote_init(OpenMote* self) {
 
   //// prepare the resource descriptor for the /l path
   cmote_vars.desc.path0len            = sizeof(cmote_path0)-1;
   cmote_vars.desc.path0val            = (uint8_t*)(&cmote_path0);
   cmote_vars.desc.path1len            = 0;
   cmote_vars.desc.path1val            = NULL;
   cmote_vars.desc.componentID         = COMPONENT_CMOTE;
   cmote_vars.desc.discoverable        = TRUE;
   cmote_vars.desc.callbackRx          = &cmote_receive;
   cmote_vars.desc.callbackSendDone    = &cmote_sendDone;
   
   // register with the CoAP module
 opencoap_register(self, &cmote_vars.desc);
   
   cmote_vars.tiempoId    = opentimers_start(self, CMOTEPERIOD,
                                                TIMER_PERIODIC,TIME_MS,
                                                cmote_timer_cb);
   

}

//=========================== private =========================================

//=========================== private =========================================

owerror_t cmote_receive(OpenMote* self, OpenQueueEntry_t* msg,
                      coap_header_iht* coap_header,
                      coap_option_iht* coap_options) {
   return E_FAIL;
}

//timer fired, but we don't want to execute task in ISR mode
//instead, push task to scheduler with COAP priority, and let scheduler take care of it
void cmote_timer_cb(OpenMote* self, opentimer_id_t id){
 scheduler_push_task(self, cmote_task_cb,TASKPRIO_COAP);
}


int cmote_openrandomtemp(OpenMote* self){
	
	int valor = 0, low_num = 0, hi_num = 0;
    min_num=15;
	max_num=25;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    srand(time(NULL));
    valor = (rand() % (hi_num - low_num)) + low_num;
    printf("Min=15 Max=25 TEMP=%d\n",valor);
    return valor;

}



void cmote_task_cb(OpenMote* self) {

//avg = openrandom_get16b(self);

OpenQueueEntry_t*    pkte;
   owerror_t            outcomen;
   uint8_t              i;
   
   uint16_t             x_int       = 0;
   uint16_t             sum         = 0;
   uint16_t             avg         = 0;
   uint8_t              N_avg       = 10;
   
   // don't run if not synch
   if ( ieee154e_isSynch(self) == FALSE) return;
   
   // don't run on dagroot
   if ( idmanager_getIsDAGroot(self)) {
 opentimers_stop(self, cmote_vars.tiempoId);
      return;
   }
   
   // HACE LA MEDIAS DE LAS MEDIDAS DEL ADC
   for (i = 0; i < N_avg; i++) {
      sum += x_int;
   }
   avg = sum/N_avg;
   
   // create a CoAP RD packet
   pkte = openqueue_getFreePacketBuffer(self, COMPONENT_CMOTE);
   
   if (pkte==NULL) {
 openserial_printError(self, 
         COMPONENT_CMOTE,
         ERR_NO_FREE_PACKET_BUFFER,
         (errorparameter_t)0,
         (errorparameter_t)0
      );
 openqueue_freePacketBuffer(self, pkte);
      return;
   }
   
   // take ownership over that packet
   pkte->creator                   = COMPONENT_CMOTE;
   pkte->owner                     = COMPONENT_CMOTE;
   
   // CoAP payload
 packetfunctions_reserveHeaderSize(self, pkte,PAYLOADLEN);
   for (i=0;i<PAYLOADLEN;i++) {
      pkte->payload[i]             = i;
   }
   avg = openrandom_get16b(self);
   
   pkte->payload[0]                = (avg>>8)&0xff;
   pkte->payload[1]                = (avg>>0)&0xff;

 packetfunctions_reserveHeaderSize(self, pkte,1);
   pkte->payload[0] = COAP_PAYLOAD_MARKER;
   
   // content-type option
 packetfunctions_reserveHeaderSize(self, pkte,2);
   pkte->payload[0]                = (COAP_OPTION_NUM_CONTENTFORMAT - COAP_OPTION_NUM_URIPATH) << 4
                                    | 1;
   pkte->payload[1]                = COAP_MEDTYPE_APPOCTETSTREAM;
   // location-path option
 packetfunctions_reserveHeaderSize(self, pkte,sizeof(cmote_path0)-1);
   memcpy(&pkte->payload[0],cmote_path0,sizeof(cmote_path0)-1);
 packetfunctions_reserveHeaderSize(self, pkte,1);
   pkte->payload[0]                = ((COAP_OPTION_NUM_URIPATH) << 4) | (sizeof(cmote_path0)-1);
   
   // metadata
   pkte->l4_destination_port       = WKP_UDP_COAP;
   pkte->l3_destinationAdd.type    = ADDR_128B;
   memcpy(&pkte->l3_destinationAdd.addr_128b[0],&ipAddr_motesEecs,16);
   
   // send
   outcomen = opencoap_send(self, 
      pkte,
      COAP_TYPE_NON,
      COAP_CODE_REQ_PUT,
      1,
      &cmote_vars.desc
   );
   
   // avoid overflowing the queue if fails
   if (outcomen==E_FAIL) {
 openqueue_freePacketBuffer(self, pkte);
   }
   
   return;
}

/**
\brief The stack indicates that the packet was sent.

\param[in] msg The CoAP message just sent.
\param[in] error The outcome of sending it.
*/
void cmote_sendDone(OpenMote* self, OpenQueueEntry_t* msg, owerror_t error) {
 openqueue_freePacketBuffer(self, msg);
}
