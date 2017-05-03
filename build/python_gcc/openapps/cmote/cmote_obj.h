/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-30 11:04:12.503411.
*/
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

#include "Python.h"

#include "opencoap_obj.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       tiempoId;    //timerId;
} cmote_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void cmote_init(OpenMote* self);

/**
\}
\}
*/

#endif
