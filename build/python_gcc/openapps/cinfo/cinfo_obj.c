/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-05-07 18:36:24.679357.
*/
/**
\brief A CoAP resource which indicates the board its running on.
*/

#include "opendefs_obj.h"
#include "cinfo_obj.h"
#include "opencoap_obj.h"
#include "openqueue_obj.h"
#include "packetfunctions_obj.h"
#include "openserial_obj.h"
#include "openrandom_obj.h"
#include "board_obj.h"
#include "idmanager_obj.h"

//=========================== defines =========================================

const uint8_t cinfo_path0[] = "info";

//=========================== variables =======================================

cinfo_vars_t cinfo_vars;

//=========================== prototypes ======================================

owerror_t cinfo_receive(OpenMote* self, 
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);
void cinfo_sendDone(OpenMote* self, 
   OpenQueueEntry_t* msg,
   owerror_t error
);

//=========================== public ==========================================

/**
\brief Initialize this module.
*/
void cinfo_init(OpenMote* self) {
   // do not run if DAGroot
   if( idmanager_getIsDAGroot(self)==TRUE) return; 
   
   // prepare the resource descriptor for the /i path
   cinfo_vars.desc.path0len             = sizeof(cinfo_path0)-1;
   cinfo_vars.desc.path0val             = (uint8_t*)(&cinfo_path0);
   cinfo_vars.desc.path1len             = 0;
   cinfo_vars.desc.path1val             = NULL;
   cinfo_vars.desc.componentID          = COMPONENT_CINFO;
   cinfo_vars.desc.discoverable         = TRUE;
   cinfo_vars.desc.callbackRx           = &cinfo_receive;
   cinfo_vars.desc.callbackSendDone     = &cinfo_sendDone;
   
   // register with the CoAP module
 opencoap_register(self, &cinfo_vars.desc);
}

//=========================== private =========================================

/**
\brief Called when a CoAP message is received for this resource.

\param[in] msg          The received message. CoAP header and options already
   parsed.
\param[in] coap_header  The CoAP header contained in the message.
\param[in] coap_options The CoAP options contained in the message.

\return Whether the response is prepared successfully.
*/
owerror_t cinfo_receive(OpenMote* self, 
      OpenQueueEntry_t* msg,
      coap_header_iht* coap_header,
      coap_option_iht* coap_options
   ) {
   
   owerror_t outcome;
   
   switch (coap_header->Code) {
      case COAP_CODE_REQ_GET:
         
         //=== reset packet payload (we will reuse this packetBuffer)
         msg->payload                     = &(msg->packet[127]);
         msg->length                      = 0;
         
         //=== prepare  CoAP response
         
         // radio name
 packetfunctions_reserveHeaderSize(self, msg,sizeof(infoRadioName)-1);
         memcpy(&msg->payload[0],&infoRadioName,sizeof(infoRadioName)-1);
         
         // uC name
 packetfunctions_reserveHeaderSize(self, msg,1);
         msg->payload[0] = '\n';
 packetfunctions_reserveHeaderSize(self, msg,sizeof(infouCName)-1);
         memcpy(&msg->payload[0],&infouCName,sizeof(infouCName)-1);
         
         // board name
 packetfunctions_reserveHeaderSize(self, msg,1);
         msg->payload[0] = '\n';
 packetfunctions_reserveHeaderSize(self, msg,sizeof(infoBoardname)-1);
         memcpy(&msg->payload[0],&infoBoardname,sizeof(infoBoardname)-1);
         
         // stack name and version
 packetfunctions_reserveHeaderSize(self, msg,1);
         msg->payload[0] = '\n';
 packetfunctions_reserveHeaderSize(self, msg,sizeof(infoStackName)-1+5);
         memcpy(&msg->payload[0],&infoStackName,sizeof(infoStackName)-1);
         msg->payload[sizeof(infoStackName)-1+5-5] = '0'+OPENWSN_VERSION_MAJOR;
         msg->payload[sizeof(infoStackName)-1+5-4] = '.';
         msg->payload[sizeof(infoStackName)-1+5-3] = '0'+OPENWSN_VERSION_MINOR;
         msg->payload[sizeof(infoStackName)-1+5-2] = '.';
         msg->payload[sizeof(infoStackName)-1+5-1] = '0'+OPENWSN_VERSION_PATCH;
         
         // payload marker
 packetfunctions_reserveHeaderSize(self, msg,1);
         msg->payload[0] = COAP_PAYLOAD_MARKER;
         
         // set the CoAP header
         coap_header->Code                = COAP_CODE_RESP_CONTENT;
         
         outcome                          = E_SUCCESS;
         break;
      default:
         // return an error message
         outcome = E_FAIL;
   }
   
   return outcome;
}

/**
\brief The stack indicates that the packet was sent.

\param[in] msg The CoAP message just sent.
\param[in] error The outcome of sending it.
*/
void cinfo_sendDone(OpenMote* self, OpenQueueEntry_t* msg, owerror_t error) {
 openqueue_freePacketBuffer(self, msg);
}
