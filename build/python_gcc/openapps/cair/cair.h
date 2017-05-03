#ifndef __CAIR_H
#define __CAIR_H

/**
\addtogroup AppCoAP
\{
\addtogroup AppUdp
\{
\addtogroup cair
\{
*/

#include "opencoap.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       time3Id;
} cair_vars_t;

//=========================== prototypes ======================================

void cair_init(void);

/**
\}
\}
*/

#endif
