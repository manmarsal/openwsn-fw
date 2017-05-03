#ifndef __CHUMIDITY_H
#define __CHUMIDITY_H

/**
\addtogroup AppCoAP
\{
\addtogroup AppUdp
\{
\addtogroup chumidity
\{
*/

#include "opencoap.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       tiempoId;    //timerId;
} chumidity_vars_t;

//=========================== prototypes ======================================

void chumidity_init(void);

/**
\}
\}
*/

#endif
