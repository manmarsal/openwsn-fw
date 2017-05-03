/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-24 17:01:08.832937.
*/
#include "opendefs_obj.h"
#include "icmpv6rpl_obj.h"
#include "icmpv6_obj.h"
#include "openserial_obj.h"
#include "openqueue_obj.h"
#include "neighbors_obj.h"
#include "packetfunctions_obj.h"
#include "openrandom_obj.h"
#include "scheduler_obj.h"
#include "idmanager_obj.h"
#include "opentimers_obj.h"
#include "IEEE802154E_obj.h"

//=========================== variables =======================================

// declaration of global variable _icmpv6rpl_vars_ removed during objectification.

//=========================== prototypes ======================================

// DIO-related
void icmpv6rpl_timer_DIO_cb(OpenMote* self, opentimer_id_t id);
void icmpv6rpl_timer_DIO_task(OpenMote* self);
void sendDIO(OpenMote* self);
// DAO-related
void icmpv6rpl_timer_DAO_cb(OpenMote* self, opentimer_id_t id);
void icmpv6rpl_timer_DAO_task(OpenMote* self);
void sendDAO(OpenMote* self);

//=========================== public ==========================================

/**
\brief Initialize this module.
*/
void icmpv6rpl_init(OpenMote* self) {
   uint8_t         dodagid[16];
   uint32_t        dioPeriod;
   uint32_t        daoPeriod;
   
   // retrieve my prefix and EUI64
   memcpy(&dodagid[0], idmanager_getMyID(self, ADDR_PREFIX)->prefix,8); // prefix
   memcpy(&dodagid[8], idmanager_getMyID(self, ADDR_64B)->addr_64b,8);  // eui64
   
   //===== reset local variables
   memset(&(self->icmpv6rpl_vars),0,sizeof(icmpv6rpl_vars_t));
   
   //=== routing
   (self->icmpv6rpl_vars).haveParent=FALSE;
   if ( idmanager_getIsDAGroot(self)==TRUE) {
      (self->icmpv6rpl_vars).myDAGrank=MINHOPRANKINCREASE;
   } else {
      (self->icmpv6rpl_vars).myDAGrank=DEFAULTDAGRANK;
   }

   //=== admin
   
   (self->icmpv6rpl_vars).busySendingDIO            = FALSE;
   (self->icmpv6rpl_vars).busySendingDAO            = FALSE;
   (self->icmpv6rpl_vars).fDodagidWritten           = 0;
   
   //=== DIO
   
   (self->icmpv6rpl_vars).dio.rplinstanceId         = 0x00;        ///< TODO: put correct value
   (self->icmpv6rpl_vars).dio.verNumb               = 0x00;        ///< TODO: put correct value
   // rank: to be populated upon TX
   (self->icmpv6rpl_vars).dio.rplOptions            = MOP_DIO_A | \
                                              MOP_DIO_B | \
                                              MOP_DIO_C | \
                                              PRF_DIO_A | \
                                              PRF_DIO_B | \
                                              PRF_DIO_C | \
                                              G_DIO ;
   (self->icmpv6rpl_vars).dio.DTSN                  = 0x33;        ///< TODO: put correct value
   (self->icmpv6rpl_vars).dio.flags                 = 0x00;
   (self->icmpv6rpl_vars).dio.reserved              = 0x00;
   memcpy(
      &((self->icmpv6rpl_vars).dio.DODAGID[0]),
      dodagid,
      sizeof((self->icmpv6rpl_vars).dio.DODAGID)
   ); // can be replaced later
   
   (self->icmpv6rpl_vars).dioDestination.type = ADDR_128B;
   memcpy(&(self->icmpv6rpl_vars).dioDestination.addr_128b[0],all_routers_multicast,sizeof(all_routers_multicast));
   
   (self->icmpv6rpl_vars).dioPeriod                 = TIMER_DIO_TIMEOUT;
   dioPeriod                                = (self->icmpv6rpl_vars).dioPeriod - 0x80 + ( openrandom_get16b(self)&0xff);
   (self->icmpv6rpl_vars).timerIdDIO                = opentimers_start(self, 
                                                dioPeriod,
                                                TIMER_PERIODIC,
                                                TIME_MS,
                                                icmpv6rpl_timer_DIO_cb
                                             );
   
   //=== DAO
   
   (self->icmpv6rpl_vars).dao.rplinstanceId         = 0x00;        ///< TODO: put correct value
   (self->icmpv6rpl_vars).dao.K_D_flags             = FLAG_DAO_A   | \
                                              FLAG_DAO_B   | \
                                              FLAG_DAO_C   | \
                                              FLAG_DAO_D   | \
                                              FLAG_DAO_E   | \
                                              PRF_DIO_C    | \
                                              FLAG_DAO_F   | \
                                              D_DAO        |
                                              K_DAO;
   (self->icmpv6rpl_vars).dao.reserved              = 0x00;
   (self->icmpv6rpl_vars).dao.DAOSequence           = 0x00;
   memcpy(
      &((self->icmpv6rpl_vars).dao.DODAGID[0]),
      dodagid,
      sizeof((self->icmpv6rpl_vars).dao.DODAGID)
   );  // can be replaced later
   
   (self->icmpv6rpl_vars).dao_transit.type          = OPTION_TRANSIT_INFORMATION_TYPE;
   // optionLength: to be populated upon TX
   (self->icmpv6rpl_vars).dao_transit.E_flags       = E_DAO_Transit_Info;
   (self->icmpv6rpl_vars).dao_transit.PathControl   = PC1_A_DAO_Transit_Info | \
                                              PC1_B_DAO_Transit_Info | \
                                              PC2_A_DAO_Transit_Info | \
                                              PC2_B_DAO_Transit_Info | \
                                              PC3_A_DAO_Transit_Info | \
                                              PC3_B_DAO_Transit_Info | \
                                              PC4_A_DAO_Transit_Info | \
                                              PC4_B_DAO_Transit_Info;  
   (self->icmpv6rpl_vars).dao_transit.PathSequence  = 0x00; // to be incremented at each TX
   (self->icmpv6rpl_vars).dao_transit.PathLifetime  = 0xAA;
   //target information
   (self->icmpv6rpl_vars).dao_target.type  = OPTION_TARGET_INFORMATION_TYPE;
   (self->icmpv6rpl_vars).dao_target.optionLength  = 0;
   (self->icmpv6rpl_vars).dao_target.flags  = 0;
   (self->icmpv6rpl_vars).dao_target.prefixLength = 0;
   
   (self->icmpv6rpl_vars).daoPeriod                 = TIMER_DAO_TIMEOUT;
   daoPeriod                                = (self->icmpv6rpl_vars).daoPeriod - 0x80 + ( openrandom_get16b(self)&0xff);
   (self->icmpv6rpl_vars).timerIdDAO                = opentimers_start(self, 
                                                daoPeriod,
                                                TIMER_PERIODIC,
                                                TIME_MS,
                                                icmpv6rpl_timer_DAO_cb
                                             );
   
}

void icmpv6rpl_writeDODAGid(OpenMote* self, uint8_t* dodagid) {
   
   // write DODAGID to DIO/DAO
   memcpy(
      &((self->icmpv6rpl_vars).dio.DODAGID[0]),
      dodagid,
      sizeof((self->icmpv6rpl_vars).dio.DODAGID)
   );
   memcpy(
      &((self->icmpv6rpl_vars).dao.DODAGID[0]),
      dodagid,
      sizeof((self->icmpv6rpl_vars).dao.DODAGID)
   );
   
   // remember I got a DODAGID
   (self->icmpv6rpl_vars).fDodagidWritten = 1;
}

uint8_t icmpv6rpl_getRPLIntanceID(OpenMote* self){
   return (self->icmpv6rpl_vars).dao.rplinstanceId;
}
                                                
void icmpv6rpl_getRPLDODAGid(OpenMote* self, uint8_t* address_128b){
    memcpy(address_128b,(self->icmpv6rpl_vars).dao.DODAGID,16);
}

/**
\brief Called when DIO/DAO was sent.

\param[in] msg   Pointer to the message just sent.
\param[in] error Outcome of the sending.
*/
void icmpv6rpl_sendDone(OpenMote* self, OpenQueueEntry_t* msg, owerror_t error) {
   
   // take ownership over that packet
   msg->owner = COMPONENT_ICMPv6RPL;
   
   // make sure I created it
   if (msg->creator!=COMPONENT_ICMPv6RPL) {
 openserial_printError(self, COMPONENT_ICMPv6RPL,ERR_UNEXPECTED_SENDDONE,
                            (errorparameter_t)0,
                            (errorparameter_t)0);
   }
   
   // I'm not busy sending DIO/DAO anymore
   if ( packetfunctions_isBroadcastMulticast(self, &(msg->l2_nextORpreviousHop))){
        (self->icmpv6rpl_vars).busySendingDIO = FALSE;
   } else {
        (self->icmpv6rpl_vars).busySendingDAO = FALSE;
   }
   
   // free packet
 openqueue_freePacketBuffer(self, msg);
}

/**
\brief Called when RPL message received.

\param[in] msg   Pointer to the received message.
*/
void icmpv6rpl_receive(OpenMote* self, OpenQueueEntry_t* msg) {
   uint8_t      icmpv6code;
   open_addr_t  myPrefix;
   
   // take ownership
   msg->owner      = COMPONENT_ICMPv6RPL;
   
   // retrieve ICMPv6 code
   icmpv6code      = (((ICMPv6_ht*)(msg->payload))->code);
   
   // toss ICMPv6 header
 packetfunctions_tossHeader(self, msg,sizeof(ICMPv6_ht));
   
   // handle message
   switch (icmpv6code) {
      case IANA_ICMPv6_RPL_DIS:
 icmpv6rpl_timer_DIO_task(self);
         break;
      case IANA_ICMPv6_RPL_DIO:
         if ( idmanager_getIsDAGroot(self)==TRUE) {
            // stop here if I'm in the DAG root
            break; // break, don't return
         }
                  
         memcpy(
            &((self->icmpv6rpl_vars).dio),
            (icmpv6rpl_dio_ht*)(msg->payload),
            sizeof(icmpv6rpl_dio_ht)
         );
         
         // write DODAGID in DIO and DAO
 icmpv6rpl_writeDODAGid(self, &(((icmpv6rpl_dio_ht*)(msg->payload))->DODAGID[0]));
         
         // update my prefix // looks like we adopt the prefix from any DIO without a question about this node being our parent??
         myPrefix.type = ADDR_PREFIX;
         memcpy(
            myPrefix.prefix,
            &((icmpv6rpl_dio_ht*)(msg->payload))->DODAGID[0],
            sizeof(myPrefix.prefix)
         );
 idmanager_setMyID(self, &myPrefix);
         
         // update routing info for that neighbor
 icmpv6rpl_indicateRxDIO(self, msg);
         
         break;
      
      case IANA_ICMPv6_RPL_DAO:
         // this should never happen
 openserial_printError(self, COMPONENT_ICMPv6RPL,ERR_UNEXPECTED_DAO,
                               (errorparameter_t)0,
                               (errorparameter_t)0);
         break;
      default:
         // this should never happen
 openserial_printError(self, COMPONENT_ICMPv6RPL,ERR_MSG_UNKNOWN_TYPE,
                               (errorparameter_t)icmpv6code,
                               (errorparameter_t)0);
         break;
      
   }
   
   // free message
 openqueue_freePacketBuffer(self, msg);
}

/**
\brief Retrieve this mote's parent index in neighbor table.

\returns TRUE and index of parent if have one, FALSE if no parent
*/
bool icmpv6rpl_getPreferredParentIndex(OpenMote* self, uint8_t* indexptr) {
   *indexptr = (self->icmpv6rpl_vars).ParentIndex;
   return (self->icmpv6rpl_vars).haveParent;
}

/**
\brief Retrieve my preferred parent's EUI64 address.
\param[out] addressToWrite Where to copy the preferred parent's address to.
*/
bool icmpv6rpl_getPreferredParentEui64(OpenMote* self, open_addr_t* addressToWrite) {
    if (
        (self->icmpv6rpl_vars).haveParent && 
 neighbors_getNeighborNoResource(self, (self->icmpv6rpl_vars).ParentIndex)==FALSE
    ){
        return neighbors_getNeighborEui64(self, addressToWrite,ADDR_64B,(self->icmpv6rpl_vars).ParentIndex);
    } else {
        return FALSE;
    }
}

/**
\brief Indicate whether some neighbor is the routing parent.

\param[in] address The EUI64 address of the neighbor.

\returns TRUE if that neighbor is preferred parent, FALSE otherwise.
*/
bool icmpv6rpl_isPreferredParent(OpenMote* self, open_addr_t* address) {
   open_addr_t  temp;
   // do we currently have a parent?
   if ((self->icmpv6rpl_vars).haveParent==FALSE) {
      return FALSE;
   }
   
   //compare parent address to the one presented.
   switch (address->type) {
      case ADDR_64B:
 neighbors_getNeighborEui64(self, &temp,ADDR_64B,(self->icmpv6rpl_vars).ParentIndex);
         return packetfunctions_sameAddress(self, address,&temp);
      default:
 openserial_printCritical(self, COMPONENT_NEIGHBORS,ERR_WRONG_ADDR_TYPE,
                               (errorparameter_t)address->type,
                               (errorparameter_t)3);
         return FALSE;
   }
}

/**
\brief Retrieve this mote's current DAG rank.

\returns This mote's current DAG rank.
*/
dagrank_t icmpv6rpl_getMyDAGrank(OpenMote* self) {
   return (self->icmpv6rpl_vars).myDAGrank;
}

/**
\brief Direct intervention to set the value of DAG rank in the data structure

Meant for direct control from command on serial port or from test application,
bypassing the routing protocol!
*/
void icmpv6rpl_setMyDAGrank(OpenMote* self, dagrank_t rank){
    (self->icmpv6rpl_vars).myDAGrank = rank;
}

/**
\brief Routing algorithm
*/
void icmpv6rpl_updateMyDAGrankAndParentSelection(OpenMote* self) {
    uint8_t   i;
    uint16_t  previousDAGrank;
    uint16_t  prevRankIncrease;
    uint8_t   prevParentIndex;
    bool      prevHadParent;
    bool      foundBetterParent;
    // temporaries
    uint16_t  rankIncrease;
    dagrank_t neighborRank;
    uint32_t  tentativeDAGrank;
   
    // if I'm a DAGroot, my DAGrank is always MINHOPRANKINCREASE
    if (( idmanager_getIsDAGroot(self))==TRUE) {
        // the dagrank is not set through setting command, set rank to MINHOPRANKINCREASE here 
        if ((self->icmpv6rpl_vars).myDAGrank!=MINHOPRANKINCREASE) { // test for change so as not to report unchanged value when root
            (self->icmpv6rpl_vars).myDAGrank=MINHOPRANKINCREASE;
            return;
        }
    }
    
    // prep for loop, remember state before neighbor table scanning
    prevParentIndex      = (self->icmpv6rpl_vars).ParentIndex;
    prevHadParent        = (self->icmpv6rpl_vars).haveParent;
    prevRankIncrease     = (self->icmpv6rpl_vars).rankIncrease;
    // update my rank to current parent first
    if ((self->icmpv6rpl_vars).haveParent==TRUE){
        rankIncrease     = neighbors_getLinkMetric(self, (self->icmpv6rpl_vars).ParentIndex);
        neighborRank     = neighbors_getNeighborRank(self, (self->icmpv6rpl_vars).ParentIndex);
        tentativeDAGrank = (uint32_t)neighborRank+rankIncrease;
        if (tentativeDAGrank>65535) {
            (self->icmpv6rpl_vars).myDAGrank = 65535;
        } else {
            (self->icmpv6rpl_vars).myDAGrank = (uint16_t)tentativeDAGrank;
        }
    }
    previousDAGrank      = (self->icmpv6rpl_vars).myDAGrank;
    foundBetterParent    = FALSE;
    (self->icmpv6rpl_vars).haveParent = FALSE;
    
    // loop through neighbor table, update myDAGrank
    for (i=0;i<MAXNUMNEIGHBORS;i++) {
        if ( neighbors_isStableNeighborByIndex(self, i)) { // in use and link is stable
            // neighbor marked as NORES can't be parent
            if ( neighbors_getNeighborNoResource(self, i)==TRUE) {
                continue;
            }
            // get link cost to this neighbor
            rankIncrease= neighbors_getLinkMetric(self, i);
            // get this neighbor's advertized rank
            neighborRank= neighbors_getNeighborRank(self, i);
            // if this neighbor has unknown/infinite rank, pass on it
            if (neighborRank==DEFAULTDAGRANK) continue;
            // compute tentative cost of full path to root through this neighbor
            tentativeDAGrank = (uint32_t)neighborRank+rankIncrease;
            if (tentativeDAGrank > 65535) {tentativeDAGrank = 65535;}
            // if not low enough to justify switch, pass (i.e. hysterisis)
            if (
                (previousDAGrank<tentativeDAGrank) ||
                (previousDAGrank-tentativeDAGrank < 2*MINHOPRANKINCREASE)
            ) {
                  continue;
            }
            // remember that we have at least one valid candidate parent
            foundBetterParent=TRUE;
            // select best candidate so far
            if ((self->icmpv6rpl_vars).myDAGrank>tentativeDAGrank) {
                (self->icmpv6rpl_vars).myDAGrank    = (uint16_t)tentativeDAGrank;
                (self->icmpv6rpl_vars).ParentIndex  = i;
                (self->icmpv6rpl_vars).rankIncrease = rankIncrease;
            }
        }
    }
   
   if (foundBetterParent) {
      (self->icmpv6rpl_vars).haveParent=TRUE;
      if (!prevHadParent) {
         // in case preParent is killed before calling this function, clear the preferredParent flag
 neighbors_setPreferredParent(self, prevParentIndex, FALSE);
         // set neighbors as preferred parent
 neighbors_setPreferredParent(self, (self->icmpv6rpl_vars).ParentIndex, TRUE);
      } else {
         if ((self->icmpv6rpl_vars).ParentIndex==prevParentIndex) {
             // report on the rank change if any, not on the deletion/creation of parent
             if ((self->icmpv6rpl_vars).myDAGrank!=previousDAGrank) {
             } else {
                 // same parent, same rank, nothing to report about 
             }
         } else {
             // clear neighbors preferredParent flag
 neighbors_setPreferredParent(self, prevParentIndex, FALSE);
             // set neighbors as preferred parent
 neighbors_setPreferredParent(self, (self->icmpv6rpl_vars).ParentIndex, TRUE);
         }
      }
   } else {
      // restore routing table as we found it on entry
      (self->icmpv6rpl_vars).myDAGrank   = previousDAGrank;
      (self->icmpv6rpl_vars).ParentIndex = prevParentIndex;
      (self->icmpv6rpl_vars).haveParent  = prevHadParent;
      (self->icmpv6rpl_vars).rankIncrease= prevRankIncrease;
      // no change to report on
   }
}

/**
\brief Indicate I just received a RPL DIO from a neighbor.

This function should be called for each received a DIO is received so neighbor
routing information in the neighbor table can be updated.

The fields which are updated are:
- DAGrank

\param[in] msg The received message with msg->payload pointing to the DIO
   header.
*/
void icmpv6rpl_indicateRxDIO(OpenMote* self, OpenQueueEntry_t* msg) {
   uint8_t          i;
   uint8_t          temp_8b;
   dagrank_t        neighborRank;
   open_addr_t      NeighborAddress;
  
   // take ownership over the packet
   msg->owner = COMPONENT_NEIGHBORS;
   
   // save pointer to incoming DIO header in global structure for simplfying debug.
   (self->icmpv6rpl_vars).incomingDio = (icmpv6rpl_dio_ht*)(msg->payload);
   // quick fix: rank is two bytes in network order: need to swap bytes
   temp_8b            = *(msg->payload+2);
   (self->icmpv6rpl_vars).incomingDio->rank = (temp_8b << 8) + *(msg->payload+3);
   // update rank of that neighbor in table
   for (i=0;i<MAXNUMNEIGHBORS;i++) {
      if ( neighbors_getNeighborEui64(self, &NeighborAddress, ADDR_64B, i)) { // this neighbor entry is in use
         if ( packetfunctions_sameAddress(self, &(msg->l2_nextORpreviousHop),&NeighborAddress)) { // matching address
            neighborRank= neighbors_getNeighborRank(self, i);
            if (
              ((self->icmpv6rpl_vars).incomingDio->rank > neighborRank) &&
              ((self->icmpv6rpl_vars).incomingDio->rank - neighborRank) > (DEFAULTLINKCOST*2*MINHOPRANKINCREASE)
            ) {
               // the new DAGrank looks suspiciously high, only increment a bit
 neighbors_setNeighborRank(self, i,neighborRank + (DEFAULTLINKCOST*2*MINHOPRANKINCREASE));
 openserial_printError(self, COMPONENT_NEIGHBORS,ERR_LARGE_DAGRANK,
                               (errorparameter_t)(self->icmpv6rpl_vars).incomingDio->rank,
                               (errorparameter_t)neighborRank);
            } else {
 neighbors_setNeighborRank(self, i,(self->icmpv6rpl_vars).incomingDio->rank);
            }
            // since changes were made to neighbors DAG rank, run the routing algorithm again
 icmpv6rpl_updateMyDAGrankAndParentSelection(self); 
            break; // there should be only one matching entry, no need to loop further
         }
      }
   } 
}

void icmpv6rpl_killPreferredParent(OpenMote* self) {
    (self->icmpv6rpl_vars).haveParent=FALSE;
    if ( idmanager_getIsDAGroot(self)==TRUE) {
       (self->icmpv6rpl_vars).myDAGrank=MINHOPRANKINCREASE;
    } else {
       (self->icmpv6rpl_vars).myDAGrank=DEFAULTDAGRANK;
    }
}

//=========================== private =========================================

//===== DIO-related

/**
\brief DIO timer callback function.

\note This function is executed in interrupt context, and should only push a 
   task.
*/
void icmpv6rpl_timer_DIO_cb(OpenMote* self, opentimer_id_t id) {
 scheduler_push_task(self, icmpv6rpl_timer_DIO_task,TASKPRIO_RPL);
}

/**
\brief Handler for DIO timer event.

\note This function is executed in task context, called by the scheduler.
*/
void icmpv6rpl_timer_DIO_task(OpenMote* self) {
   uint32_t        dioPeriod;
   // send DIO
 sendDIO(self);
   
   // arm the DIO timer with this new value
   dioPeriod = (self->icmpv6rpl_vars).dioPeriod - 0x80 + ( openrandom_get16b(self)&0xff);
 opentimers_setPeriod(self, 
      (self->icmpv6rpl_vars).timerIdDIO,
      TIME_MS,
      dioPeriod
   );
}

/**
\brief Prepare and a send a RPL DIO.
*/
void sendDIO(OpenMote* self) {
   OpenQueueEntry_t*    msg;
   
   // stop if I'm not sync'ed
   if ( ieee154e_isSynch(self)==FALSE) {
      
      // remove packets genereted by this module (DIO and DAO) from openqueue
 openqueue_removeAllCreatedBy(self, COMPONENT_ICMPv6RPL);
      
      // I'm not busy sending a DIO/DAO
      (self->icmpv6rpl_vars).busySendingDIO  = FALSE;
      (self->icmpv6rpl_vars).busySendingDAO  = FALSE;
      
      // stop here
      return;
   }
   
   // do not send DIO if I have the default DAG rank
   if ( icmpv6rpl_getMyDAGrank(self)==DEFAULTDAGRANK) {
      return;
   }
   
   // do not send DIO if I'm already busy sending
   if ((self->icmpv6rpl_vars).busySendingDIO==TRUE) {
      return;
   }
   
   // if you get here, all good to send a DIO
   
   // reserve a free packet buffer for DIO
   msg = openqueue_getFreePacketBuffer(self, COMPONENT_ICMPv6RPL);
   if (msg==NULL) {
 openserial_printError(self, COMPONENT_ICMPv6RPL,ERR_NO_FREE_PACKET_BUFFER,
                            (errorparameter_t)0,
                            (errorparameter_t)0);
      
      return;
   }
   
   // take ownership
   msg->creator                             = COMPONENT_ICMPv6RPL;
   msg->owner                               = COMPONENT_ICMPv6RPL;
   
   // set transport information
   msg->l4_protocol                         = IANA_ICMPv6;
   msg->l4_protocol_compressed              = FALSE;
   msg->l4_sourcePortORicmpv6Type           = IANA_ICMPv6_RPL;
   
   // set DIO destination
   memcpy(&(msg->l3_destinationAdd),&(self->icmpv6rpl_vars).dioDestination,sizeof(open_addr_t));
   
   //===== DIO payload
   // note: DIO is already mostly populated
   (self->icmpv6rpl_vars).dio.rank                  = icmpv6rpl_getMyDAGrank(self);
 packetfunctions_reserveHeaderSize(self, msg,sizeof(icmpv6rpl_dio_ht));
   memcpy(
      ((icmpv6rpl_dio_ht*)(msg->payload)),
      &((self->icmpv6rpl_vars).dio),
      sizeof(icmpv6rpl_dio_ht)
   );
   
   // reverse the rank bytes order in Big Endian
   *(msg->payload+2) = ((self->icmpv6rpl_vars).dio.rank >> 8) & 0xFF;
   *(msg->payload+3) = (self->icmpv6rpl_vars).dio.rank        & 0xFF;
   
   //===== ICMPv6 header
 packetfunctions_reserveHeaderSize(self, msg,sizeof(ICMPv6_ht));
   ((ICMPv6_ht*)(msg->payload))->type       = msg->l4_sourcePortORicmpv6Type;
   ((ICMPv6_ht*)(msg->payload))->code       = IANA_ICMPv6_RPL_DIO;
 packetfunctions_calculateChecksum(self, msg,(uint8_t*)&(((ICMPv6_ht*)(msg->payload))->checksum));//call last
   
    //send
    if ( icmpv6_send(self, msg)==E_SUCCESS) {
        (self->icmpv6rpl_vars).busySendingDIO = TRUE; 
    } else {
 openqueue_freePacketBuffer(self, msg);
    }
}

//===== DAO-related

/**
\brief DAO timer callback function.

\note This function is executed in interrupt context, and should only push a
   task.
*/
void icmpv6rpl_timer_DAO_cb(OpenMote* self, opentimer_id_t id) {
 scheduler_push_task(self, icmpv6rpl_timer_DAO_task,TASKPRIO_RPL);
}

/**
\brief Handler for DAO timer event.

\note This function is executed in task context, called by the scheduler.
*/
void icmpv6rpl_timer_DAO_task(OpenMote* self) {
   uint32_t        daoPeriod;
   
   // send DAO
 sendDAO(self);
   
   // arm the DAO timer with this new value
   daoPeriod = (self->icmpv6rpl_vars).daoPeriod - 0x80 + ( openrandom_get16b(self)&0xff);
 opentimers_setPeriod(self, 
      (self->icmpv6rpl_vars).timerIdDAO,
      TIME_MS,
      daoPeriod
   );
}

/**
\brief Prepare and a send a RPL DAO.
*/
void sendDAO(OpenMote* self) {
   OpenQueueEntry_t*    msg;                // pointer to DAO messages
   uint8_t              nbrIdx;             // running neighbor index
   uint8_t              numTransitParents,numTargetParents;  // the number of parents indicated in transit option
   open_addr_t         address;
   open_addr_t*        prefix;
   
   if ( ieee154e_isSynch(self)==FALSE) {
      // I'm not sync'ed 
      
      // delete packets genereted by this module (DIO and DAO) from openqueue
 openqueue_removeAllCreatedBy(self, COMPONENT_ICMPv6RPL);
      
      // I'm not busy sending a DIO/DAO
      (self->icmpv6rpl_vars).busySendingDAO = FALSE;
      (self->icmpv6rpl_vars).busySendingDIO = FALSE;
      
      // stop here
      return;
   }
   
   // dont' send a DAO if you're the DAG root
   if ( idmanager_getIsDAGroot(self)==TRUE) {
      return;
   }
   
   // dont' send a DAO if you did not acquire a DAGrank
   if ( icmpv6rpl_getMyDAGrank(self)==DEFAULTDAGRANK) {
       return;
   }
   
   // dont' send a DAO if you're still busy sending the previous one
   if ((self->icmpv6rpl_vars).busySendingDAO==TRUE) {
      return;
   }
   
   // if you get here, you start construct DAO
   
   // reserve a free packet buffer for DAO
   msg = openqueue_getFreePacketBuffer(self, COMPONENT_ICMPv6RPL);
   if (msg==NULL) {
 openserial_printError(self, COMPONENT_ICMPv6RPL,ERR_NO_FREE_PACKET_BUFFER,
                            (errorparameter_t)0,
                            (errorparameter_t)0);
      return;
   }
   
   // take ownership
   msg->creator                             = COMPONENT_ICMPv6RPL;
   msg->owner                               = COMPONENT_ICMPv6RPL;
   
   // set transport information
   msg->l4_protocol                         = IANA_ICMPv6;
   msg->l4_sourcePortORicmpv6Type           = IANA_ICMPv6_RPL;
   
   // set DAO destination
   msg->l3_destinationAdd.type=ADDR_128B;
   memcpy(msg->l3_destinationAdd.addr_128b,(self->icmpv6rpl_vars).dio.DODAGID,sizeof((self->icmpv6rpl_vars).dio.DODAGID));
   
   //===== fill in packet
   
   //NOTE: limit to preferrred parent only the number of DAO transit addresses to send
   
   //=== transit option -- from RFC 6550, page 55 - 1 transit information header per parent is required. 
   //getting only preferred parent as transit
   numTransitParents=0;
 icmpv6rpl_getPreferredParentEui64(self, &address);
 packetfunctions_writeAddress(self, msg,&address,OW_BIG_ENDIAN);
   prefix= idmanager_getMyID(self, ADDR_PREFIX);
 packetfunctions_writeAddress(self, msg,prefix,OW_BIG_ENDIAN);
   // update transit info fields
   // from rfc6550 p.55 -- Variable, depending on whether or not the DODAG ParentAddress subfield is present.
   // poipoi xv: it is not very clear if this includes all fields in the header. or as target info 2 bytes are removed.
   // using the same pattern as in target information.
   (self->icmpv6rpl_vars).dao_transit.optionLength  = LENGTH_ADDR128b + sizeof(icmpv6rpl_dao_transit_ht)-2;
   (self->icmpv6rpl_vars).dao_transit.PathControl=0; //todo. this is to set the preference of this parent.      
   (self->icmpv6rpl_vars).dao_transit.type=OPTION_TRANSIT_INFORMATION_TYPE;
           
   // write transit info in packet
 packetfunctions_reserveHeaderSize(self, msg,sizeof(icmpv6rpl_dao_transit_ht));
   memcpy(
          ((icmpv6rpl_dao_transit_ht*)(msg->payload)),
          &((self->icmpv6rpl_vars).dao_transit),
          sizeof(icmpv6rpl_dao_transit_ht)
   );
   numTransitParents++;
   
   //target information is required. RFC 6550 page 55.
   /*
   One or more Transit Information options MUST be preceded by one or
   more RPL Target options.   
   */
    numTargetParents                        = 0;
    for (nbrIdx=0;nbrIdx<MAXNUMNEIGHBORS;nbrIdx++) {
      if (( neighbors_isNeighborWithHigherDAGrank(self, nbrIdx))==TRUE) {
         // this neighbor is of higher DAGrank as I am. so it is my child
         
         // write it's address in DAO RFC6550 page 80 check point 1.
 neighbors_getNeighborEui64(self, &address,ADDR_64B,nbrIdx); 
 packetfunctions_writeAddress(self, msg,&address,OW_BIG_ENDIAN);
         prefix= idmanager_getMyID(self, ADDR_PREFIX);
 packetfunctions_writeAddress(self, msg,prefix,OW_BIG_ENDIAN);
        
         // update target info fields 
         // from rfc6550 p.55 -- Variable, length of the option in octets excluding the Type and Length fields.
         // poipoi xv: assuming that type and length fields refer to the 2 first bytes of the header
         (self->icmpv6rpl_vars).dao_target.optionLength  = LENGTH_ADDR128b +sizeof(icmpv6rpl_dao_target_ht) - 2; //no header type and length
         (self->icmpv6rpl_vars).dao_target.type  = OPTION_TARGET_INFORMATION_TYPE;
         (self->icmpv6rpl_vars).dao_target.flags  = 0;       //must be 0
         (self->icmpv6rpl_vars).dao_target.prefixLength = 128; //128 leading bits  -- full address.
         
         // write transit info in packet
 packetfunctions_reserveHeaderSize(self, msg,sizeof(icmpv6rpl_dao_target_ht));
         memcpy(
               ((icmpv6rpl_dao_target_ht*)(msg->payload)),
               &((self->icmpv6rpl_vars).dao_target),
               sizeof(icmpv6rpl_dao_target_ht)
         );
         
         // remember I found it
         numTargetParents++;
      }  
      //limit to MAX_TARGET_PARENTS the number of DAO target addresses to send
      //section 8.2.1 pag 67 RFC6550 -- using a subset
      // poipoi TODO base selection on ETX rather than first X.
      if (numTargetParents>=MAX_TARGET_PARENTS) break;
   }
   
   
   // stop here if no parents found
   if (numTransitParents==0) {
 openqueue_freePacketBuffer(self, msg);
      return;
   }
   
   (self->icmpv6rpl_vars).dao_transit.PathSequence++; //increment path sequence.
   // if you get here, you will send a DAO
   
   
   //=== DAO header
 packetfunctions_reserveHeaderSize(self, msg,sizeof(icmpv6rpl_dao_ht));
   memcpy(
      ((icmpv6rpl_dao_ht*)(msg->payload)),
      &((self->icmpv6rpl_vars).dao),
      sizeof(icmpv6rpl_dao_ht)
   );
   
   //=== ICMPv6 header
 packetfunctions_reserveHeaderSize(self, msg,sizeof(ICMPv6_ht));
   ((ICMPv6_ht*)(msg->payload))->type       = msg->l4_sourcePortORicmpv6Type;
   ((ICMPv6_ht*)(msg->payload))->code       = IANA_ICMPv6_RPL_DAO;
 packetfunctions_calculateChecksum(self, msg,(uint8_t*)&(((ICMPv6_ht*)(msg->payload))->checksum)); //call last
   
   //===== send
   if ( icmpv6_send(self, msg)==E_SUCCESS) {
      (self->icmpv6rpl_vars).busySendingDAO = TRUE;
   } else {
 openqueue_freePacketBuffer(self, msg);
   }
}

void icmpv6rpl_setDIOPeriod(OpenMote* self, uint16_t dioPeriod){
   uint32_t        dioPeriodRandom;
   
   (self->icmpv6rpl_vars).dioPeriod = dioPeriod;
   dioPeriodRandom = (self->icmpv6rpl_vars).dioPeriod - 0x80 + ( openrandom_get16b(self)&0xff);
 opentimers_setPeriod(self, 
       (self->icmpv6rpl_vars).timerIdDIO,
       TIME_MS,
       dioPeriodRandom
   );
}

void icmpv6rpl_setDAOPeriod(OpenMote* self, uint16_t daoPeriod){
   uint32_t        daoPeriodRandom;
   
   (self->icmpv6rpl_vars).daoPeriod = daoPeriod;
   daoPeriodRandom = (self->icmpv6rpl_vars).daoPeriod - 0x80 + ( openrandom_get16b(self)&0xff);
 opentimers_setPeriod(self, 
       (self->icmpv6rpl_vars).timerIdDAO,
       TIME_MS,
       daoPeriodRandom
   );
}
