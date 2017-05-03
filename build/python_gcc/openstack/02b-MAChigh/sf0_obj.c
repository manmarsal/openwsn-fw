/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-24 16:58:06.473944.
*/
#include "opendefs_obj.h"
#include "sf0_obj.h"
#include "neighbors_obj.h"
#include "sixtop_obj.h"
#include "scheduler_obj.h"
#include "schedule_obj.h"
#include "idmanager_obj.h"
#include "openapps_obj.h"

//=========================== definition =====================================

#define SF0THRESHOLD      2

//=========================== variables =======================================

sf0_vars_t sf0_vars;

//=========================== prototypes ======================================

void sf0_addCell_task(OpenMote* self);
void sf0_removeCell_task(OpenMote* self);
void sf0_bandwidthEstimate_task(OpenMote* self);

//=========================== public ==========================================

void sf0_init(OpenMote* self) {
    memset(&sf0_vars,0,sizeof(sf0_vars_t));
    sf0_vars.numAppPacketsPerSlotFrame = 0;
}

void sf0_notif_addedCell(OpenMote* self) {
 scheduler_push_task(self, sf0_addCell_task,TASKPRIO_SF0);
}

void sf0_notif_removedCell(OpenMote* self) {
 scheduler_push_task(self, sf0_removeCell_task,TASKPRIO_SF0);
}

// this function is called once per slotframe. 
void sf0_notifyNewSlotframe(OpenMote* self) {
 scheduler_push_task(self, sf0_bandwidthEstimate_task,TASKPRIO_SF0);
}

void sf0_setBackoff(OpenMote* self, uint8_t value){
    sf0_vars.backoff = value;
}

//=========================== private =========================================

void sf0_addCell_task(OpenMote* self) {
   open_addr_t          neighbor;
   bool                 foundNeighbor;
   
   // get preferred parent
   foundNeighbor = icmpv6rpl_getPreferredParentEui64(self, &neighbor);
   if (foundNeighbor==FALSE) {
      return;
   }
   
   if ( sixtop_setHandler(self, SIX_HANDLER_SF0)==FALSE){
      // one sixtop transcation is happening, only one instance at one time
      return;
   }
   // call sixtop
 sixtop_request(self, 
      IANA_6TOP_CMD_ADD,
      &neighbor,
      1
   );
}

void sf0_removeCell_task(OpenMote* self) {
   open_addr_t          neighbor;
   bool                 foundNeighbor;
   
   // get preferred parent
   foundNeighbor = icmpv6rpl_getPreferredParentEui64(self, &neighbor);
   if (foundNeighbor==FALSE) {
      return;
   }
   
   if ( sixtop_setHandler(self, SIX_HANDLER_SF0)==FALSE){
      // one sixtop transcation is happening, only one instance at one time
      return;
   }
   // call sixtop
 sixtop_request(self, 
      IANA_6TOP_CMD_DELETE,
      &neighbor,
      1
   );
}

void sf0_bandwidthEstimate_task(OpenMote* self){
    open_addr_t    neighbor;
    bool           foundNeighbor;
    int8_t         bw_outgoing;
    int8_t         bw_incoming;
    int8_t         bw_self;
    
    // do not reserve cells if I'm a DAGroot
    if ( idmanager_getIsDAGroot(self)){
        return;
    }
    
    if (sf0_vars.backoff>0){
        sf0_vars.backoff -= 1;
        return;
    }
    
    // get preferred parent
    foundNeighbor = icmpv6rpl_getPreferredParentEui64(self, &neighbor);
    if (foundNeighbor==FALSE) {
        return;
    }
    
    // get bandwidth of outgoing, incoming and self.
    // Here we just calculate the estimated bandwidth for 
    // the application sending on dedicate cells(TX or Rx).
    bw_outgoing = schedule_getNumOfSlotsByType(self, CELLTYPE_TX);
    bw_incoming = schedule_getNumOfSlotsByType(self, CELLTYPE_RX);
    
    // get self required bandwith, you can design your
    // application and assign bw_self accordingly. 
    // for example:
    //    bw_self = application_getBandwdith(app_name);
    // By default, it's set to zero.
    // bw_self = openapps_getBandwidth(COMPONENT_UINJECT);
    bw_self = sf0_vars.numAppPacketsPerSlotFrame;
    
    // In SF0, scheduledCells = bw_outgoing
    //         requiredCells  = bw_incoming + bw_self
    // when scheduledCells<requiredCells, add one or more cell
    
    if (bw_outgoing <= bw_incoming+bw_self){
        if ( sixtop_setHandler(self, SIX_HANDLER_SF0)==FALSE){
            // one sixtop transcation is happening, only one instance at one time
            return;
        }
 sixtop_request(self, 
            IANA_6TOP_CMD_ADD,
            &neighbor,
            bw_incoming+bw_self-bw_outgoing+1
        );
    } else {
        // remove cell(s)
        if ( (bw_incoming+bw_self) < (bw_outgoing-SF0THRESHOLD)) {
            if ( sixtop_setHandler(self, SIX_HANDLER_SF0)==FALSE){
               // one sixtop transcation is happening, only one instance at one time
               return;
            }
 sixtop_request(self, 
                IANA_6TOP_CMD_DELETE,
                &neighbor,
                SF0THRESHOLD
            );
        } else {
            // nothing to do
        }
    }
}

void sf0_appPktPeriod(uint8_t numAppPacketsPerSlotFrame){
    sf0_vars.numAppPacketsPerSlotFrame = numAppPacketsPerSlotFrame;
}
