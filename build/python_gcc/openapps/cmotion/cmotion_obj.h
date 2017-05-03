/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-05-01 10:36:46.725583.
*/
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

#include "Python.h"

#include "opencoap_obj.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       time2Id;
} cmotion_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void cmotion_init(OpenMote* self);

/**
\}
\}
*/

#endif
