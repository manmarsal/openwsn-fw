/**
\brief A CoAP resource which allows an application to GET/SET the state of the
   error LED.
*/

#include <stdio.h>

#include "opendefs.h"
#include "chumidity.h"
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

///// inter-packet period (in ms)
#define CTEMPOPERIOD  1000
#define PAYLOADLEN      40

//=========================== variables =======================================

chumidity_vars_t chumidity_vars;

//random_vars_t random_vars;

const uint8_t chumidity_path0[]       = "humedad";

int min_num, i;
int max_num;
uint16_t resultado, humedad, dig0, dig1;
char str[1], int2string[1];
char aChar;

char digitos[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };


//=========================== prototypes ======================================

owerror_t chumidity_receive(
   OpenQueueEntry_t* msg,
   coap_header_iht*  coap_header,
   coap_option_iht*  coap_options
);

//void    chumidity_timer_cb(opentimer_id_t id);
//void    chumidity_task_cb(void);
int     chumidity_openrandomtemp (void);

void     chumidity_sendDone(
   OpenQueueEntry_t* msg,
   owerror_t error
);

//=========================== public ==========================================

void chumidity_init() {
	   
   if(idmanager_getIsDAGroot()==TRUE) return;
   //printf ("antes del init\n");
   
   //resultado = idmanager_getMyID(ADDR_16B)->addr_16b[1];
   // job specifically for mote bbbb::1415:92cc:0:2
   
   //if(idmanager_getMyID(ADDR_16B)->addr_16b[1]!=0x02) return;
   
   //// prepare the resource descriptor for the /humidity path
   chumidity_vars.desc.path0len            = sizeof(chumidity_path0)-1;
   chumidity_vars.desc.path0val            = (uint8_t*)(&chumidity_path0);
   chumidity_vars.desc.path1len            = 0;
   chumidity_vars.desc.path1val            = NULL;
   chumidity_vars.desc.componentID         = COMPONENT_CHUMIDITY;
   chumidity_vars.desc.discoverable       = TRUE;
   chumidity_vars.desc.callbackRx          = &chumidity_receive;
   chumidity_vars.desc.callbackSendDone    = &chumidity_sendDone;
   
      ////// job specifically for mote bbbb::1415:92cc:0:2
   //if(idmanager_getMyID(ADDR_16B)->addr_16b[1]!=0x02) {
	   
	   //chumidity_vars.desc.discoverable        = FALSE;
   //}
   //else {
	   //chumidity_vars.desc.discoverable       = TRUE;
   //}

   // register with the CoAP module
   opencoap_register(&chumidity_vars.desc);
   
   //printf ("inicializo chumidity_init\n");
   //printf ("inicializo chumidity_init, dir = %x\n", resultado);
   
   //chumidity_vars.tiempoId    = opentimers_start(CTEMPOPERIOD,
                                                //TIMER_PERIODIC,TIME_MS,
                                                //chumidity_timer_cb);

}

//=========================== private =========================================

//timer fired, but we don't want to execute task in ISR mode
//instead, push task to scheduler with COAP priority, and let scheduler take care of it
//void chumidity_timer_cb(opentimer_id_t id){
//   scheduler_push_task(chumidity_task_cb,TASKPRIO_COAP);
//}


////void chumidity_task_cb() {




//// don't run if not synch
//if (ieee154e_isSynch() == FALSE) return;
   
   ////printf ("soy un capullo");
   
   //////// job specifically for mote bbbb::1415:92cc:0:2
////if(idmanager_getMyID(ADDR_16B)->addr_16b[1]!=0x02) {
	   
	   ////chumidity_vars.desc.discoverable        = FALSE;
	   ////opencoap_register(&chumidity_vars.desc);

     
   
   ////// don't run on dagroot
   ////if (idmanager_getIsDAGroot()) {
      ////opentimers_stop(chumidity_vars.tiempoId);
      ////return;
   ////}

//}



int chumidity_openrandomtemp (){
	
	int resultado = 0, low_num = 0, hi_num = 0;
    min_num=55;
	max_num=95;

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
    printf("Min=55 Max=95 HUMEDAD=%d\n",resultado);
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
owerror_t chumidity_receive(
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
         packetfunctions_reserveHeaderSize(msg,3);
         msg->payload[0]                  = COAP_PAYLOAD_MARKER;

         //if (leds_error_isOn()==1) {
         //   msg->payload[1]               = '4';
         //   msg->payload[1]               = openrandom_get16b();
            
            humedad = chumidity_openrandomtemp ();
            //int2string = sprintf(str,"%d", temp);
            //printf("TEMP=%d\n",temp);
            
            dig0 = humedad % 10;
            //printf("dig0=%d\n",dig0);
            humedad = humedad/10;
            //printf("TEMP=%d\n",temp);
            dig1 = humedad % 10;
            //printf("dig1=%d\n",dig1);
            //int2string = sprintf(str,"%d", dig0);
            //msg->payload[1]               = dig0;
            aChar = digitos[dig1];
            msg->payload[1]               = aChar;
                      
            //printf("msg->payload[1]=%s\n",str);
            //int2string = sprintf(str,"%d", dig1);
            aChar = digitos[dig0];
            msg->payload[2]               = aChar;
            
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
void chumidity_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
   openqueue_freePacketBuffer(msg);
}
