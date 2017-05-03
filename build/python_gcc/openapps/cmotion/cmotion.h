#ifndef __CMOTION_H
#define __CMOTION_H

/**
\addtogroup AppCoAP
\{
\addtogroup AppUdp
\{
\addtogroup cmotion
\{
*/

#include "opencoap.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       time2Id;
} cmotion_vars_t;

//=========================== prototypes ======================================

void cmotion_init(void);

/**
\}
\}
*/

#endif
