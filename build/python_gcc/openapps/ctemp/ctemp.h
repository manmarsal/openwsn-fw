#ifndef __CTEMPO_H
#define __CTEMPO_H

/**
\addtogroup AppCoAP
\{
\addtogroup AppUdp
\{
\addtogroup cleds
\{
\addtogroup ctempo
\{
*/

#include "opencoap.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       tempsId;    //timerId;
} ctempo_vars_t;

//=========================== prototypes ======================================

void ctempo_init(void);

/**
\}
\}
*/

#endif
