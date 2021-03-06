/**
DO NOT EDIT DIRECTLY!!

This file was 'objectified' by SCons as a pre-processing
step for the building a Python extension module.

This was done on 2017-04-24 17:04:32.800280.
*/
/**
\brief Definition of the "openserial" driver.

\author Fabien Chraim <chraim@eecs.berkeley.edu>, March 2012.
\author Thomas Watteyne <thomas.watteyne@inria.fr>, August 2016.
*/

#include "opendefs_obj.h"
#include "openserial_obj.h"
#include "IEEE802154E_obj.h"
#include "neighbors_obj.h"
#include "sixtop_obj.h"
#include "icmpv6echo_obj.h"
#include "idmanager_obj.h"
#include "openqueue_obj.h"
#include "openbridge_obj.h"
#include "leds_obj.h"
#include "schedule_obj.h"
#include "uart_obj.h"
#include "opentimers_obj.h"
#include "openhdlc_obj.h"
#include "schedule_obj.h"
#include "icmpv6rpl_obj.h"
#include "icmpv6echo_obj.h"
#include "sf0_obj.h"

//=========================== variables =======================================

// declaration of global variable _openserial_vars_ removed during objectification.

//=========================== prototypes ======================================

// printing
owerror_t openserial_printInfoErrorCritical(OpenMote* self, 
    char             severity,
    uint8_t          calling_component,
    uint8_t          error_code,
    errorparameter_t arg1,
    errorparameter_t arg2
);

// command handlers
void openserial_handleEcho(OpenMote* self, uint8_t* but, uint8_t bufLen);
void openserial_handleCommands(OpenMote* self);

// misc
void openserial_board_reset_cb(OpenMote* self, 
    opentimer_id_t id
);

// HDLC output
void outputHdlcOpen(OpenMote* self);
void outputHdlcWrite(OpenMote* self, uint8_t b);
void outputHdlcClose(OpenMote* self);

// HDLC input
void inputHdlcOpen(OpenMote* self);
void inputHdlcWrite(OpenMote* self, uint8_t b);
void inputHdlcClose(OpenMote* self);

//=========================== public ==========================================

//===== admin

void openserial_init(OpenMote* self) {
    uint16_t crc;
    
    // reset variable
    memset(&(self->openserial_vars),0,sizeof(openserial_vars_t));
    
    // admin
    (self->openserial_vars).mode               = MODE_OFF;
    (self->openserial_vars).debugPrintCounter  = 0;
    
    // input
    (self->openserial_vars).reqFrame[0]        = HDLC_FLAG;
    (self->openserial_vars).reqFrame[1]        = SERFRAME_MOTE2PC_REQUEST;
    crc = HDLC_CRCINIT;
    crc = crcIteration(crc,(self->openserial_vars).reqFrame[1]);
    crc = ~crc;
    (self->openserial_vars).reqFrame[2]        = (crc>>0)&0xff;
    (self->openserial_vars).reqFrame[3]        = (crc>>8)&0xff;
    (self->openserial_vars).reqFrame[4]        = HDLC_FLAG;
    (self->openserial_vars).reqFrameIdx        = 0;
    (self->openserial_vars).lastRxByte         = HDLC_FLAG;
    (self->openserial_vars).busyReceiving      = FALSE;
    (self->openserial_vars).inputEscaping      = FALSE;
    (self->openserial_vars).inputBufFill       = 0;
    
    // ouput
    (self->openserial_vars).outputBufFilled    = FALSE;
    (self->openserial_vars).outputBufIdxR      = 0;
    (self->openserial_vars).outputBufIdxW      = 0;
    
    // set callbacks
 uart_setCallbacks(self, 
        isr_openserial_tx,
        isr_openserial_rx
    );
}

void openserial_register(OpenMote* self, openserial_rsvpt* rsvp) {
    // FIXME: register multiple commands (linked list)
    (self->openserial_vars).registeredCmd = rsvp;
}

//===== printing

owerror_t openserial_printStatus(OpenMote* self, 
    uint8_t             statusElement,
    uint8_t*            buffer,
    uint8_t             length
) {
    uint8_t i;
    INTERRUPT_DECLARATION();
    
    DISABLE_INTERRUPTS();
    (self->openserial_vars).outputBufFilled  = TRUE;
 outputHdlcOpen(self);
 outputHdlcWrite(self, SERFRAME_MOTE2PC_STATUS);
 outputHdlcWrite(self, idmanager_getMyID(self, ADDR_16B)->addr_16b[0]);
 outputHdlcWrite(self, idmanager_getMyID(self, ADDR_16B)->addr_16b[1]);
 outputHdlcWrite(self, statusElement);
    for (i=0;i<length;i++){
 outputHdlcWrite(self, buffer[i]);
    }
 outputHdlcClose(self);
    ENABLE_INTERRUPTS();
    
    return E_SUCCESS;
}

owerror_t openserial_printInfo(OpenMote* self, 
    uint8_t             calling_component,
    uint8_t             error_code,
    errorparameter_t    arg1,
    errorparameter_t    arg2
) {
    return openserial_printInfoErrorCritical(self, 
        SERFRAME_MOTE2PC_INFO,
        calling_component,
        error_code,
        arg1,
        arg2
    );
}

owerror_t openserial_printError(OpenMote* self, 
    uint8_t             calling_component,
    uint8_t             error_code,
    errorparameter_t    arg1,
    errorparameter_t    arg2
) {
    // toggle error LED
 leds_error_toggle(self);
    
    return openserial_printInfoErrorCritical(self, 
        SERFRAME_MOTE2PC_ERROR,
        calling_component,
        error_code,
        arg1,
        arg2
    );
}

owerror_t openserial_printCritical(OpenMote* self, 
    uint8_t             calling_component,
    uint8_t             error_code,
    errorparameter_t    arg1,
    errorparameter_t    arg2
) {
    // blink error LED, this is serious
 leds_error_blink(self);
    
    // schedule for the mote to reboot in 10s
 opentimers_start(self, 
        10000,
        TIMER_ONESHOT,
        TIME_MS,
        openserial_board_reset_cb
    );
    
    return openserial_printInfoErrorCritical(self, 
        SERFRAME_MOTE2PC_CRITICAL,
        calling_component,
        error_code,
        arg1,
        arg2
    );
}

owerror_t openserial_printData(OpenMote* self, uint8_t* buffer, uint8_t length) {
    uint8_t  i;
    uint8_t  asn[5];
    INTERRUPT_DECLARATION();
    
    // retrieve ASN
 ieee154e_getAsn(self, asn);
    
    DISABLE_INTERRUPTS();
    (self->openserial_vars).outputBufFilled  = TRUE;
 outputHdlcOpen(self);
 outputHdlcWrite(self, SERFRAME_MOTE2PC_DATA);
 outputHdlcWrite(self, idmanager_getMyID(self, ADDR_16B)->addr_16b[1]);
 outputHdlcWrite(self, idmanager_getMyID(self, ADDR_16B)->addr_16b[0]);
 outputHdlcWrite(self, asn[0]);
 outputHdlcWrite(self, asn[1]);
 outputHdlcWrite(self, asn[2]);
 outputHdlcWrite(self, asn[3]);
 outputHdlcWrite(self, asn[4]);
    for (i=0;i<length;i++){
 outputHdlcWrite(self, buffer[i]);
    }
 outputHdlcClose(self);
    ENABLE_INTERRUPTS();
    
    return E_SUCCESS;
}

owerror_t openserial_printSniffedPacket(OpenMote* self, uint8_t* buffer, uint8_t length, uint8_t channel) {
    uint8_t  i;
    INTERRUPT_DECLARATION();
    
    DISABLE_INTERRUPTS();
    (self->openserial_vars).outputBufFilled  = TRUE;
 outputHdlcOpen(self);
 outputHdlcWrite(self, SERFRAME_MOTE2PC_SNIFFED_PACKET);
 outputHdlcWrite(self, idmanager_getMyID(self, ADDR_16B)->addr_16b[1]);
 outputHdlcWrite(self, idmanager_getMyID(self, ADDR_16B)->addr_16b[0]);
    for (i=0;i<length;i++){
 outputHdlcWrite(self, buffer[i]);
    }
 outputHdlcWrite(self, channel);
 outputHdlcClose(self);
    
    ENABLE_INTERRUPTS();
    
    return E_SUCCESS;
}

//===== retrieving inputBuffer

uint8_t openserial_getInputBufferFilllevel(OpenMote* self) {
    uint8_t inputBufFill;
    INTERRUPT_DECLARATION();
    
    DISABLE_INTERRUPTS();
    inputBufFill = (self->openserial_vars).inputBufFill;
    ENABLE_INTERRUPTS();
    
    return inputBufFill-1; // removing the command byte
}

uint8_t openserial_getInputBuffer(OpenMote* self, uint8_t* bufferToWrite, uint8_t maxNumBytes) {
    uint8_t numBytesWritten;
    uint8_t inputBufFill;
    INTERRUPT_DECLARATION();
    
    DISABLE_INTERRUPTS();
    inputBufFill = (self->openserial_vars).inputBufFill;
    ENABLE_INTERRUPTS();
     
    if (maxNumBytes<inputBufFill-1) {
 openserial_printError(self, 
            COMPONENT_OPENSERIAL,
            ERR_GETDATA_ASKS_TOO_FEW_BYTES,
            (errorparameter_t)maxNumBytes,
            (errorparameter_t)inputBufFill-1
        );
        numBytesWritten = 0;
    } else {
        numBytesWritten = inputBufFill-1;
        memcpy(bufferToWrite,&((self->openserial_vars).inputBuf[1]),numBytesWritten);
    }
    
    return numBytesWritten;
}

//===== scheduling

void openserial_startInput(OpenMote* self) {
    INTERRUPT_DECLARATION();
    
    if ((self->openserial_vars).inputBufFill>0) {
 openserial_printError(self, 
            COMPONENT_OPENSERIAL,
            ERR_INPUTBUFFER_LENGTH,
            (errorparameter_t)(self->openserial_vars).inputBufFill,
            (errorparameter_t)0
        );
        DISABLE_INTERRUPTS();
        (self->openserial_vars).inputBufFill=0;
        ENABLE_INTERRUPTS();
    }
    
 uart_clearTxInterrupts(self);
 uart_clearRxInterrupts(self);      // clear possible pending interrupts
 uart_enableInterrupts(self);       // Enable USCI_A1 TX & RX interrupt
    
    DISABLE_INTERRUPTS();
    (self->openserial_vars).busyReceiving  = FALSE;
    (self->openserial_vars).mode           = MODE_INPUT;
    (self->openserial_vars).reqFrameIdx    = 0;
#ifdef FASTSIM
 uart_writeBufferByLen_FASTSIM(self, 
        (self->openserial_vars).reqFrame,
        sizeof((self->openserial_vars).reqFrame)
    );
    (self->openserial_vars).reqFrameIdx = sizeof((self->openserial_vars).reqFrame);
#else
 uart_writeByte(self, (self->openserial_vars).reqFrame[(self->openserial_vars).reqFrameIdx]);
#endif
    ENABLE_INTERRUPTS();
}

void openserial_startOutput(OpenMote* self) {
    uint8_t debugPrintCounter;
    INTERRUPT_DECLARATION();
    
    //=== made modules print debug information
    
    DISABLE_INTERRUPTS();
    (self->openserial_vars).debugPrintCounter = ((self->openserial_vars).debugPrintCounter+1)%STATUS_MAX;
    debugPrintCounter = (self->openserial_vars).debugPrintCounter;
    ENABLE_INTERRUPTS();
    
    switch (debugPrintCounter) {
        case STATUS_ISSYNC:
            if ( debugPrint_isSync(self)==TRUE) {
                break;
            }
        case STATUS_ID:
            if ( debugPrint_id(self)==TRUE) {
               break;
            }
        case STATUS_DAGRANK:
            if ( debugPrint_myDAGrank(self)==TRUE) {
                break;
            }
        case STATUS_OUTBUFFERINDEXES:
            if ( debugPrint_outBufferIndexes(self)==TRUE) {
                break;
            }
        case STATUS_ASN:
            if ( debugPrint_asn(self)==TRUE) {
                break;
            }
        case STATUS_MACSTATS:
            if ( debugPrint_macStats(self)==TRUE) {
                break;
            }
        case STATUS_SCHEDULE:
            if( debugPrint_schedule(self)==TRUE) {
                break;
            }
        case STATUS_BACKOFF:
            if( debugPrint_backoff(self)==TRUE) {
                break;
            }
        case STATUS_QUEUE:
            if( debugPrint_queue(self)==TRUE) {
                break;
            }
        case STATUS_NEIGHBORS:
            if ( debugPrint_neighbors(self)==TRUE) {
                break;
            }
        case STATUS_KAPERIOD:
            if ( debugPrint_kaPeriod(self)==TRUE) {
                break;
            }
        default:
            DISABLE_INTERRUPTS();
            (self->openserial_vars).debugPrintCounter=0;
            ENABLE_INTERRUPTS();
    }
    
    //=== flush TX buffer
    
 uart_clearTxInterrupts(self);
 uart_clearRxInterrupts(self);          // clear possible pending interrupts
 uart_enableInterrupts(self);           // Enable USCI_A1 TX & RX interrupt
    
    DISABLE_INTERRUPTS();
    (self->openserial_vars).mode=MODE_OUTPUT;
    if ((self->openserial_vars).outputBufFilled) {
#ifdef FASTSIM
 uart_writeCircularBuffer_FASTSIM(self, 
            (self->openserial_vars).outputBuf,
            &(self->openserial_vars).outputBufIdxR,
            &(self->openserial_vars).outputBufIdxW
        );
#else
 uart_writeByte(self, (self->openserial_vars).outputBuf[(self->openserial_vars).outputBufIdxR++]);
#endif
    } else {
 openserial_stop(self);
    }
    ENABLE_INTERRUPTS();
}

void openserial_stop(OpenMote* self) {
    uint8_t inputBufFill;
    uint8_t cmdByte;
    bool    busyReceiving;
    INTERRUPT_DECLARATION();

    DISABLE_INTERRUPTS();
    busyReceiving = (self->openserial_vars).busyReceiving;
    inputBufFill  = (self->openserial_vars).inputBufFill;
    ENABLE_INTERRUPTS();

    // disable UART interrupts
 uart_disableInterrupts(self);

    DISABLE_INTERRUPTS();
    (self->openserial_vars).mode=MODE_OFF;
    ENABLE_INTERRUPTS();
    
    // the inputBuffer has to be reset if it is not reset where the data is read.
    // or the function openserial_getInputBuffer is called (which resets the buffer)
    if (busyReceiving==TRUE) {
 openserial_printError(self, 
            COMPONENT_OPENSERIAL,
            ERR_BUSY_RECEIVING,
            (errorparameter_t)0,
            (errorparameter_t)inputBufFill
        );
    }
    
    if (busyReceiving==FALSE && inputBufFill>0) {
        DISABLE_INTERRUPTS();
        cmdByte = (self->openserial_vars).inputBuf[0];
        ENABLE_INTERRUPTS();
        // call hard-coded commands
        // FIXME: needs to be replaced by registered commands only
        switch (cmdByte) {
            case SERFRAME_PC2MOTE_SETROOT:
 idmanager_triggerAboutRoot(self);
                break;
            case SERFRAME_PC2MOTE_RESET:
 board_reset(self);
                break;
            case SERFRAME_PC2MOTE_DATA:
 openbridge_triggerData(self);
                break;
            case SERFRAME_PC2MOTE_TRIGGERSERIALECHO:
 openserial_handleEcho(self, &(self->openserial_vars).inputBuf[1],inputBufFill-1);
                break;
            case SERFRAME_PC2MOTE_COMMAND:
 openserial_handleCommands(self);
                break;
        }
        // call registered commands
        if ((self->openserial_vars).registeredCmd!=NULL && (self->openserial_vars).registeredCmd->cmdId==cmdByte) {
            
            (self->openserial_vars).registeredCmd->cb(self);
        }
    }
    
    DISABLE_INTERRUPTS();
    (self->openserial_vars).inputBufFill  = 0;
    (self->openserial_vars).busyReceiving = FALSE;
    ENABLE_INTERRUPTS();
}

//===== debugprint

/**
\brief Trigger this module to print status information, over serial.

debugPrint_* functions are used by the openserial module to continuously print
status information about several modules in the OpenWSN stack.

\returns TRUE if this function printed something, FALSE otherwise.
*/
bool debugPrint_outBufferIndexes(OpenMote* self) {
    uint16_t temp_buffer[2];
    INTERRUPT_DECLARATION();
    
    DISABLE_INTERRUPTS();
    temp_buffer[0] = (self->openserial_vars).outputBufIdxW;
    temp_buffer[1] = (self->openserial_vars).outputBufIdxR;
    ENABLE_INTERRUPTS();
    
 openserial_printStatus(self, 
        STATUS_OUTBUFFERINDEXES,
        (uint8_t*)temp_buffer,
        sizeof(temp_buffer)
    );
    
    return TRUE;
}

//=========================== private =========================================

//===== printing

owerror_t openserial_printInfoErrorCritical(OpenMote* self, 
    char             severity,
    uint8_t          calling_component,
    uint8_t          error_code,
    errorparameter_t arg1,
    errorparameter_t arg2
) {
    INTERRUPT_DECLARATION();
    
    DISABLE_INTERRUPTS();
    (self->openserial_vars).outputBufFilled  = TRUE;
 outputHdlcOpen(self);
 outputHdlcWrite(self, severity);
 outputHdlcWrite(self, idmanager_getMyID(self, ADDR_16B)->addr_16b[0]);
 outputHdlcWrite(self, idmanager_getMyID(self, ADDR_16B)->addr_16b[1]);
 outputHdlcWrite(self, calling_component);
 outputHdlcWrite(self, error_code);
 outputHdlcWrite(self, (uint8_t)((arg1 & 0xff00)>>8));
 outputHdlcWrite(self, (uint8_t) (arg1 & 0x00ff));
 outputHdlcWrite(self, (uint8_t)((arg2 & 0xff00)>>8));
 outputHdlcWrite(self, (uint8_t) (arg2 & 0x00ff));
 outputHdlcClose(self);
    ENABLE_INTERRUPTS();
    
    return E_SUCCESS;
}

//===== command handlers

void openserial_handleEcho(OpenMote* self, uint8_t* buf, uint8_t bufLen){
    
    // echo back what you received
 openserial_printData(self, 
        buf,
        bufLen
    );
}

void openserial_handleCommands(OpenMote* self){
   uint8_t  input_buffer[10];
   uint8_t  numDataBytes;
   uint8_t  commandId;
   uint8_t  commandLen;
   uint8_t  comandParam_8;
   uint16_t comandParam_16;
   cellInfo_ht cellList[SCHEDULEIEMAXNUMCELLS];
   uint8_t  i;
   
   open_addr_t neighbor;
   bool        foundNeighbor;
   
   memset(cellList,0,sizeof(cellList));
   
   numDataBytes = openserial_getInputBufferFilllevel(self);
   //copying the buffer
 openserial_getInputBuffer(self, &(input_buffer[0]),numDataBytes);
   commandId  = (self->openserial_vars).inputBuf[1];
   commandLen = (self->openserial_vars).inputBuf[2];
   
   if (commandLen>3) {
       // the max command Len is 2, except ping commands
       return;
   } else {
       if (commandLen == 1) {
           comandParam_8 = (self->openserial_vars).inputBuf[3];
       } else {
           // commandLen == 2
           comandParam_16 = ((self->openserial_vars).inputBuf[3]      & 0x00ff) | \
                            (((self->openserial_vars).inputBuf[4]<<8) & 0xff00); 
       }
   }
   
   switch(commandId) {
       case COMMAND_SET_EBPERIOD:
 sixtop_setEBPeriod(self, comandParam_8); // one byte, in seconds
           break;
       case COMMAND_SET_CHANNEL:
           // set communication channel for protocol stack
 ieee154e_setSingleChannel(self, comandParam_8); // one byte
           // set listenning channel for sniffer
           sniffer_setListeningChannel(comandParam_8); // one byte
           break;
       case COMMAND_SET_KAPERIOD: // two bytes, in slots
 sixtop_setKaPeriod(self, comandParam_16);
           break;
       case COMMAND_SET_DIOPERIOD: // two bytes, in mili-seconds
 icmpv6rpl_setDIOPeriod(self, comandParam_16);
           break;
       case COMMAND_SET_DAOPERIOD: // two bytes, in mili-seconds
 icmpv6rpl_setDAOPeriod(self, comandParam_16);
           break;
       case COMMAND_SET_DAGRANK: // two bytes
 icmpv6rpl_setMyDAGrank(self, comandParam_16);
           break;
       case COMMAND_SET_SECURITY_STATUS: // one byte
           if (comandParam_8 ==1) {
 ieee154e_setIsSecurityEnabled(self, TRUE);
           } else {
               if (comandParam_8 == 0) {
 ieee154e_setIsSecurityEnabled(self, FALSE);
               } else {
                   // security only can be 1 or 0 
                   break;
               }
           }
           break;
       case COMMAND_SET_SLOTFRAMELENGTH: // two bytes
 schedule_setFrameLength(self, comandParam_16);
           break;
       case COMMAND_SET_ACK_STATUS:
           if (comandParam_8 == 1) {
 ieee154e_setIsAckEnabled(self, TRUE);
           } else {
               if (comandParam_8 == 0) {
 ieee154e_setIsAckEnabled(self, FALSE);
               } else {
                   // ack reply
                   break;
               }
           }
           break;
        case COMMAND_SET_6P_ADD:
        case COMMAND_SET_6P_DELETE:
        case COMMAND_SET_6P_COUNT:
        case COMMAND_SET_6P_LIST:
        case COMMAND_SET_6P_CLEAR:
            // get preferred parent
            foundNeighbor = icmpv6rpl_getPreferredParentEui64(self, &neighbor);
            if (foundNeighbor==FALSE) {
                break;
            }
            
            if ( sixtop_setHandler(self, SIX_HANDLER_SF0)==FALSE){
                // one sixtop transcation is happening, only one instance at one time
                return;
            }
            if ( 
                (
                  commandId != COMMAND_SET_6P_ADD &&
                  commandId != COMMAND_SET_6P_DELETE
                ) ||
                (
                    ( 
                      commandId == COMMAND_SET_6P_ADD ||
                      commandId == COMMAND_SET_6P_DELETE
                    ) && 
                    commandLen == 0
                ) 
            ){
                // randomly select cell
 sixtop_request(self, commandId-8,&neighbor,1);
            } else {
                for (i=0;i<commandLen;i++){
                    cellList[i].tsNum           = (self->openserial_vars).inputBuf[3+i];
                    cellList[i].choffset        = DEFAULT_CHANNEL_OFFSET;
                    cellList[i].linkoptions     = CELLTYPE_TX;
                }
 sixtop_addORremoveCellByInfo(self, commandId-8,&neighbor,cellList);
            }
            break;
       case COMMAND_SET_SLOTDURATION:
 ieee154e_setSlotDuration(self, comandParam_16);
            break;
       case COMMAND_SET_6PRESPONSE:
            if (comandParam_8 ==1) {
 sixtop_setIsResponseEnabled(self, TRUE);
            } else {
                if (comandParam_8 == 0) {
 sixtop_setIsResponseEnabled(self, FALSE);
                } else {
                    // security only can be 1 or 0 
                    break;
                }
            }
            break;
       case COMMAND_SET_UINJECTPERIOD:
            sf0_appPktPeriod(comandParam_8);
            break;
       case COMMAND_SET_ECHO_REPLY_STATUS:
            if (comandParam_8 == 1) {
 icmpv6echo_setIsReplyEnabled(self, TRUE);
            } else {
                if (comandParam_8 == 0) {
 icmpv6echo_setIsReplyEnabled(self, FALSE);
                } else {
                    // ack reply
                    break;
                }
            }
            break;
       default:
           // wrong command ID
           break;
   }
}

//===== misc

void openserial_board_reset_cb(OpenMote* self, opentimer_id_t id) {
 board_reset(self);
}

//===== hdlc (output)

/**
\brief Start an HDLC frame in the output buffer.
*/
port_INLINE void outputHdlcOpen(OpenMote* self) {
    // initialize the value of the CRC
    (self->openserial_vars).outputCrc                                        = HDLC_CRCINIT;

    // write the opening HDLC flag
    (self->openserial_vars).outputBuf[(self->openserial_vars).outputBufIdxW++]       = HDLC_FLAG;
}
/**
\brief Add a byte to the outgoing HDLC frame being built.
*/
port_INLINE void outputHdlcWrite(OpenMote* self, uint8_t b) {
    
    // iterate through CRC calculator
    (self->openserial_vars).outputCrc = crcIteration((self->openserial_vars).outputCrc,b);
    
    // add byte to buffer
    if (b==HDLC_FLAG || b==HDLC_ESCAPE) {
        (self->openserial_vars).outputBuf[(self->openserial_vars).outputBufIdxW++]   = HDLC_ESCAPE;
        b                                                            = b^HDLC_ESCAPE_MASK;
    }
    (self->openserial_vars).outputBuf[(self->openserial_vars).outputBufIdxW++]       = b;
}
/**
\brief Finalize the outgoing HDLC frame.
*/
port_INLINE void outputHdlcClose(OpenMote* self) {
    uint16_t   finalCrc;

    // finalize the calculation of the CRC
    finalCrc   = ~(self->openserial_vars).outputCrc;

    // write the CRC value
 outputHdlcWrite(self, (finalCrc>>0)&0xff);
 outputHdlcWrite(self, (finalCrc>>8)&0xff);

    // write the closing HDLC flag
    (self->openserial_vars).outputBuf[(self->openserial_vars).outputBufIdxW++]       = HDLC_FLAG;
}

//===== hdlc (input)

/**
\brief Start an HDLC frame in the input buffer.
*/
port_INLINE void inputHdlcOpen(OpenMote* self) {
    // reset the input buffer index
    (self->openserial_vars).inputBufFill                                     = 0;

    // initialize the value of the CRC
    (self->openserial_vars).inputCrc                                         = HDLC_CRCINIT;
}
/**
\brief Add a byte to the incoming HDLC frame.
*/
port_INLINE void inputHdlcWrite(OpenMote* self, uint8_t b) {
    if (b==HDLC_ESCAPE) {
        (self->openserial_vars).inputEscaping = TRUE;
    } else {
        if ((self->openserial_vars).inputEscaping==TRUE) {
            b                             = b^HDLC_ESCAPE_MASK;
            (self->openserial_vars).inputEscaping = FALSE;
        }
        
        // add byte to input buffer
        (self->openserial_vars).inputBuf[(self->openserial_vars).inputBufFill] = b;
        (self->openserial_vars).inputBufFill++;
        
        // iterate through CRC calculator
        (self->openserial_vars).inputCrc = crcIteration((self->openserial_vars).inputCrc,b);
    }
}
/**
\brief Finalize the incoming HDLC frame.
*/
port_INLINE void inputHdlcClose(OpenMote* self) {
    
    // verify the validity of the frame
    if ((self->openserial_vars).inputCrc==HDLC_CRCGOOD) {
        // the CRC is correct
        
        // remove the CRC from the input buffer
        (self->openserial_vars).inputBufFill    -= 2;
    } else {
        // the CRC is incorrect
        
        // drop the incoming fram
        (self->openserial_vars).inputBufFill     = 0;
    }
}

//=========================== interrupt handlers ==============================

// executed in ISR, called from scheduler.c
void isr_openserial_tx(OpenMote* self) {
    switch ((self->openserial_vars).mode) {
        case MODE_INPUT:
            (self->openserial_vars).reqFrameIdx++;
            if ((self->openserial_vars).reqFrameIdx<sizeof((self->openserial_vars).reqFrame)) {
 uart_writeByte(self, (self->openserial_vars).reqFrame[(self->openserial_vars).reqFrameIdx]);
            }
            break;
        case MODE_OUTPUT:
            if ((self->openserial_vars).outputBufIdxW==(self->openserial_vars).outputBufIdxR) {
                (self->openserial_vars).outputBufFilled = FALSE;
            }
            if ((self->openserial_vars).outputBufFilled) {
 uart_writeByte(self, (self->openserial_vars).outputBuf[(self->openserial_vars).outputBufIdxR++]);
            }
            break;
        case MODE_OFF:
            default:
            break;
    }
}

// executed in ISR, called from scheduler.c
void isr_openserial_rx(OpenMote* self) {
    uint8_t rxbyte;
    uint8_t inputBufFill;

    // stop if I'm not in input mode
    if ((self->openserial_vars).mode!=MODE_INPUT) {
        return;
    }

    // read byte just received
    rxbyte = uart_readByte(self);
    // keep length
    inputBufFill=(self->openserial_vars).inputBufFill;
    
    if (
        (self->openserial_vars).busyReceiving==FALSE  &&
        (self->openserial_vars).lastRxByte==HDLC_FLAG &&
        rxbyte!=HDLC_FLAG
    ) {
        // start of frame

        // I'm now receiving
        (self->openserial_vars).busyReceiving         = TRUE;

        // create the HDLC frame
 inputHdlcOpen(self);

        // add the byte just received
 inputHdlcWrite(self, rxbyte);
    } else if (
        (self->openserial_vars).busyReceiving==TRUE   &&
        rxbyte!=HDLC_FLAG
    ) {
        // middle of frame

        // add the byte just received
 inputHdlcWrite(self, rxbyte);
        if ((self->openserial_vars).inputBufFill+1>SERIAL_INPUT_BUFFER_SIZE){
            // input buffer overflow
 openserial_printError(self, 
                COMPONENT_OPENSERIAL,
                ERR_INPUT_BUFFER_OVERFLOW,
                (errorparameter_t)0,
                (errorparameter_t)0
            );
            (self->openserial_vars).inputBufFill       = 0;
            (self->openserial_vars).busyReceiving      = FALSE;
 openserial_stop(self);
        }
    } else if (
        (self->openserial_vars).busyReceiving==TRUE   &&
        rxbyte==HDLC_FLAG
    ) {
        // end of frame
        
        // finalize the HDLC frame
 inputHdlcClose(self);
        
        if ((self->openserial_vars).inputBufFill==0){
            // invalid HDLC frame
 openserial_printError(self, 
                COMPONENT_OPENSERIAL,
                ERR_WRONG_CRC_INPUT,
                (errorparameter_t)inputBufFill,
                (errorparameter_t)0
            );
        }
         
        (self->openserial_vars).busyReceiving      = FALSE;
 openserial_stop(self);
    }
    
    (self->openserial_vars).lastRxByte = rxbyte;
}
