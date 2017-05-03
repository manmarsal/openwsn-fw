#ifndef __CCALEF_H
#define __CCALEF_H

/**
\addtogroup AppCoAP
\{
\addtogroup AppUdp
\{
\addtogroup ccalef
\{
*/

#include "opencoap.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       time1Id;
} ccalef_vars_t;

//=========================== prototypes ======================================

void ccalef_init(void);

/**
\}
\}
*/

#endif
