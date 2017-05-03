/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-05-01 11:36:55.255612.
*/
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

#include "Python.h"

#include "opencoap_obj.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       time1Id;
} ccalef_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void ccalef_init(OpenMote* self);

/**
\}
\}
*/

#endif
