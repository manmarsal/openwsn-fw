/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-05-01 21:17:02.847030.
*/
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

#include "Python.h"

#include "opencoap_obj.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       time3Id;
} cair_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void cair_init(OpenMote* self);

/**
\}
\}
*/

#endif
