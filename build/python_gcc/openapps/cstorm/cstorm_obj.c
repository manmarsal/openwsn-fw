/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-24 17:03:43.356782.
*/
#include "opendefs_obj.h"
#include "cstorm_obj.h"
#include "opencoap_obj.h"
#include "opentimers_obj.h"
#include "openqueue_obj.h"
#include "packetfunctions_obj.h"
#include "openserial_obj.h"
#include "openrandom_obj.h"
#include "scheduler_obj.h"
//#include "ADC_Channel.h"
#include "IEEE802154E_obj.h"
#include "idmanager_obj.h"

//=========================== defines =========================================

const uint8_t cstorm_path0[]    = "storm";
const uint8_t cstorm_payload[]  = "OpenWSN";
static const uint8_t dst_addr[]   = {
   0xbb, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
}; 

//=========================== variables =======================================

cstorm_vars_t cstorm_vars;

//=========================== prototypes ======================================

owerror_t cstorm_receive(OpenMote* self, 
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);
void cstorm_timer_cb(OpenMote* self);
void cstorm_task_cb(OpenMote* self);
void cstorm_sendDone(OpenMote* self, OpenQueueEntry_t* msg, owerror_t error);

//=========================== public ==========================================

void cstorm_init(OpenMote* self) {
   
   // register to OpenCoAP module
   cstorm_vars.desc.path0len              = sizeof(cstorm_path0)-1;
   cstorm_vars.desc.path0val              = (uint8_t*)(&cstorm_path0);
   cstorm_vars.desc.path1len              = 0;
   cstorm_vars.desc.path1val              = NULL;
   cstorm_vars.desc.componentID           = COMPONENT_CSTORM;
   cstorm_vars.desc.discoverable          = TRUE;
   cstorm_vars.desc.callbackRx            = &cstorm_receive;
   cstorm_vars.desc.callbackSendDone      = &cstorm_sendDone;
 opencoap_register(self, &cstorm_vars.desc);
   
   /*
   //start a periodic timer
   //comment : not running by default
   cstorm_vars.period           = 6553; 
   
   cstorm_vars.timerId                    = opentimers_start(self, 
      cstorm_vars.period,
      TIMER_PERIODIC,TIME_MS,
      cstorm_timer_cb
   );
   
   //stop 
   // opentimers_stop(self, cstorm_vars.timerId);
   */
}

//=========================== private =========================================

owerror_t cstorm_receive(OpenMote* self, 
      OpenQueueEntry_t* msg,
      coap_header_iht*  coap_header,
      coap_option_iht*  coap_options
   ) {
   owerror_t outcome;
   
   switch (coap_header->Code) {
      
      case COAP_CODE_REQ_GET:
         
         // reset packet payload
         msg->payload             = &(msg->packet[127]);
         msg->length              = 0;
         
         // add CoAP payload
 packetfunctions_reserveHeaderSize(self, msg, 3);
         msg->payload[0]          = COAP_PAYLOAD_MARKER;
         
         // return as big endian
         msg->payload[1]          = (uint8_t)(cstorm_vars.period >> 8);
         msg->payload[2]          = (uint8_t)(cstorm_vars.period & 0xff);
         
         // set the CoAP header
         coap_header->Code        = COAP_CODE_RESP_CONTENT;
         
         outcome                  = E_SUCCESS;
         break;
      
      case COAP_CODE_REQ_PUT:
         
         if (msg->length!=2) {
            outcome               = E_FAIL;
            coap_header->Code     = COAP_CODE_RESP_BADREQ;
         }
         
         // read the new period
         cstorm_vars.period     = 0;
         cstorm_vars.period    |= (msg->payload[0] << 8);
         cstorm_vars.period    |= msg->payload[1];
         
         /*
         // stop and start again only if period > 0
 opentimers_stop(self, cstorm_vars.timerId);
         
         if(cstorm_vars.period > 0) {
 opentimers_setPeriod(self, cstorm_vars.timerId,TIME_MS,cstorm_vars.period);
 opentimers_restart(self, cstorm_vars.timerId);
         }
         */
         
         // reset packet payload
         msg->payload             = &(msg->packet[127]);
         msg->length              = 0;
         
         // set the CoAP header
         coap_header->Code        = COAP_CODE_RESP_CHANGED;
         
         outcome                  = E_SUCCESS;
         break;
      
      default:
         outcome = E_FAIL;
         break;
   }
   
   return outcome;
}

/**
\note timer fired, but we don't want to execute task in ISR mode instead, push
   task to scheduler with CoAP priority, and let scheduler take care of it.
*/
void cstorm_timer_cb(OpenMote* self){
 scheduler_push_task(self, cstorm_task_cb,TASKPRIO_COAP);
}

void cstorm_task_cb(OpenMote* self) {
   OpenQueueEntry_t*    pkt;
   owerror_t            outcome;
   uint8_t              numOptions;
   
   // don't run if not synch
   if ( ieee154e_isSynch(self) == FALSE) return;
   
   // don't run on dagroot
   if ( idmanager_getIsDAGroot(self)) {
 opentimers_stop(self, cstorm_vars.timerId);
      return;
   }
   
   if(cstorm_vars.period == 0) {
      // stop the periodic timer
 opentimers_stop(self, cstorm_vars.timerId);
      return;
   }
   
   // if you get here, send a packet
   
   // get a packet
   pkt = openqueue_getFreePacketBuffer(self, COMPONENT_CSTORM);
   if (pkt==NULL) {
 openserial_printError(self, COMPONENT_CSTORM,ERR_NO_FREE_PACKET_BUFFER,
                            (errorparameter_t)0,
                            (errorparameter_t)0);
 openqueue_freePacketBuffer(self, pkt);
      return;
   }
   
   // take ownership over that packet
   pkt->creator    = COMPONENT_CSTORM;
   pkt->owner      = COMPONENT_CSTORM;
   
   //The contents of the message are written in reverse order : the payload first
   //packetfunctions_reserveHeaderSize moves the index pkt->payload
   
   // add payload
 packetfunctions_reserveHeaderSize(self, pkt,sizeof(cstorm_payload)-1);
   memcpy(&pkt->payload[0],cstorm_payload,sizeof(cstorm_payload)-1);
   
   //set the TKL byte as a counter of Options
   //TODO: This is not conform with RFC7252, but yes with current dissector WS v1.10.6
   numOptions = 0;
   
   //Bigger Options last in message, first in the code (as it is in reverse order) 
   //Deltas are calculated between too consecutive lengthes.
   
   // content-type option
 packetfunctions_reserveHeaderSize(self, pkt,2);
   pkt->payload[0] = (COAP_OPTION_NUM_CONTENTFORMAT-COAP_OPTION_NUM_URIPATH) << 4 | 1; 
   pkt->payload[1] = COAP_MEDTYPE_APPOCTETSTREAM;
   numOptions++;
   
   // location-path option
 packetfunctions_reserveHeaderSize(self, pkt,sizeof(cstorm_path0)-1);
   memcpy(&pkt->payload[0],cstorm_path0,sizeof(cstorm_path0)-1);
 packetfunctions_reserveHeaderSize(self, pkt,1);
   pkt->payload[0] = (COAP_OPTION_NUM_URIPATH-7) << 4 | (sizeof(cstorm_path0)-1);
   numOptions++;
   
   // length of uri-port option added directly by opencoap_send
 packetfunctions_reserveHeaderSize(self, pkt,11);
   numOptions++;
   
   // metadata
   pkt->l4_destination_port = WKP_UDP_COAP;
   pkt->l3_destinationAdd.type = ADDR_128B;
   memcpy(&pkt->l3_destinationAdd.addr_128b[0],&dst_addr,16);
   
   // send
   outcome = opencoap_send(self, 
      pkt,
      COAP_TYPE_NON,
      COAP_CODE_REQ_PUT,
      numOptions,
      &cstorm_vars.desc
   );
   
   // avoid overflowing the queue if fails
   if (outcome==E_FAIL) {
 openqueue_freePacketBuffer(self, pkt);
   }
}

void cstorm_sendDone(OpenMote* self, OpenQueueEntry_t* msg, owerror_t error) {
 openqueue_freePacketBuffer(self, msg);
}

