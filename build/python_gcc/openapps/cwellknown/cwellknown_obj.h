/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-24 16:52:38.437724.
*/
#ifndef __CWELLKNOWN_H
#define __CWELLKNOWN_H

/**
\addtogroup AppCoAP
\{
\addtogroup cwellknown
\{
*/

#include "Python.h"

#include "opencoap_obj.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
} cwellknown_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void cwellknown_init(OpenMote* self);

/**
\}
\}
*/

#endif
