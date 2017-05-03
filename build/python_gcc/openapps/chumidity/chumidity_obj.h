/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-05-01 01:33:15.781938.
*/
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

#include "Python.h"

#include "opencoap_obj.h"

//=========================== define ==========================================

//=========================== typedef =========================================

//=========================== variables =======================================

typedef struct {
   coap_resource_desc_t desc;
   opentimer_id_t       tiempoId;    //timerId;
} chumidity_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void chumidity_init(OpenMote* self);

/**
\}
\}
*/

#endif
