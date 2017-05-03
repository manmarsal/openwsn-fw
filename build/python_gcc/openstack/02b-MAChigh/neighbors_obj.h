/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-24 16:52:49.986827.
*/
#ifndef __NEIGHBORS_H
#define __NEIGHBORS_H

/**
\addtogroup MAChigh
\{
\addtogroup Neighbors
\{
*/
#include "Python.h"

#include "opendefs_obj.h"
#include "icmpv6rpl_obj.h"

//=========================== define ==========================================

#define MAXPREFERENCE             2
#define BADNEIGHBORMAXRSSI        -80 //dBm
#define GOODNEIGHBORMINRSSI       -90 //dBm
#define SWITCHSTABILITYTHRESHOLD  3
#define DEFAULTLINKCOST           15

#define MAXDAGRANK                0xffff
#define DEFAULTDAGRANK            MAXDAGRANK
#define MINHOPRANKINCREASE        256  //default value in RPL and Minimal 6TiSCH draft


//=========================== typedef =========================================

BEGIN_PACK
typedef struct {
   uint8_t         row;
   neighborRow_t   neighborEntry;
} debugNeighborEntry_t;
END_PACK

BEGIN_PACK
typedef struct {
   uint8_t         last_addr_byte;   // last byte of the neighbor's address
   int8_t          rssi;
   uint8_t         parentPreference;
   dagrank_t       DAGrank;
   uint16_t        asn; 
} netDebugNeigborEntry_t;
END_PACK

//=========================== module variables ================================
   
typedef struct {
   neighborRow_t        neighbors[MAXNUMNEIGHBORS];
   dagrank_t            myDAGrank;
   uint8_t              debugRow;
} neighbors_vars_t;

#include "openwsnmodule_obj.h"
typedef struct OpenMote OpenMote;

//=========================== prototypes ======================================

void neighbors_init(OpenMote* self);

// getters
dagrank_t neighbors_getNeighborRank(OpenMote* self, uint8_t index);
uint8_t neighbors_getNumNeighbors(OpenMote* self);
uint16_t neighbors_getLinkMetric(OpenMote* self, uint8_t index);
open_addr_t* neighbors_getKANeighbor(OpenMote* self, uint16_t kaPeriod);
bool neighbors_getNeighborNoResource(OpenMote* self, uint8_t index);
// setters
void neighbors_setNeighborRank(OpenMote* self, uint8_t index, dagrank_t rank);
void neighbors_setNeighborNoResource(OpenMote* self, open_addr_t* address);
void neighbors_setPreferredParent(OpenMote* self, uint8_t index, bool isPreferred);

// interrogators
bool neighbors_isStableNeighbor(OpenMote* self, open_addr_t* address);
bool neighbors_isStableNeighborByIndex(OpenMote* self, uint8_t index);
bool neighbors_isNeighborWithLowerDAGrank(OpenMote* self, uint8_t index);
bool neighbors_isNeighborWithHigherDAGrank(OpenMote* self, uint8_t index);

// updating neighbor information
void neighbors_indicateRx(OpenMote* self, 
   open_addr_t*         l2_src,
   int8_t               rssi,
   asn_t*               asnTimestamp,
   bool                 joinPrioPresent,
   uint8_t              joinPrio
);
void neighbors_indicateTx(OpenMote* self, 
   open_addr_t*         dest,
   uint8_t              numTxAttempts,
   bool                 was_finally_acked,
   asn_t*               asnTimestamp
);

// get addresses
bool neighbors_getNeighborEui64(OpenMote* self, open_addr_t* address,uint8_t addr_type,uint8_t index);
// maintenance
void neighbors_removeOld(OpenMote* self);
// debug
bool debugPrint_neighbors(OpenMote* self);

/**
\}
\}
*/

#endif
