#ifndef __CMOTE_H
#define __CMOTE_H

/**
\addtogroup AppCoAP
\{
\addtogroup AppUdp
\{
\addtogroup cmote
\{
*/

#include "opencoap.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       tiempoId;    //timerId;
} cmote_vars_t;

//=========================== prototypes ======================================

void cmote_init(void);

/**
\}
\}
*/

#endif
