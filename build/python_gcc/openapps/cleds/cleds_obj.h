/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-24 16:52:36.130539.
*/
#ifndef __CLEDS_H
#define __CLEDS_H

/**
\addtogroup AppCoAP
\{
\addtogroup cleds
\{
*/

#include "Python.h"

#include "opencoap_obj.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
} cleds_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void cleds__init(OpenMote* self);

/**
\}
\}
*/

#endif
