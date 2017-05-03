/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-29 19:31:23.482204.
*/
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

#include "Python.h"

#include "opencoap_obj.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       tempsId;    //timerId;
} ctempo_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void ctempo_init(OpenMote* self);

/**
\}
\}
*/

#endif
