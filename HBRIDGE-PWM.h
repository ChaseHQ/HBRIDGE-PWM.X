/* 
 * File:   HBRIDGE-PWM.h
 * Author: CraigVella
 *
 * Created on April 13, 2016, 6:58 PM
 */

#ifndef HBRIDGE_PWM_H
#define	HBRIDGE_PWM_H

#define COMMAND_NOCOMMAND    0b10000000
#define COMMAND_SETLWHEEL    0b10000001
#define COMMAND_SETRWHEEL    0b10000010
#define COMMAND_GETDISTANCE  0b10000011
#define COMMAND_QDISTREADY   0b10000100
#define COMMAND_RETDISTANCEH 0b10000101
#define COMMAND_RETDISTANCEL 0b10000110

#define COMMAND_CONFIRMED    0b11111111
#define COMMAND_UNKNOWN      0b11111110
#define COMMAND_QUERY        0b11111101
#define COMMAND_DISTREADY    0b11111100
#define COMMAND_DISTNREADY   0b11111011
#define COMMAND_BADSTATE     0b11111010
#define COMMAND_TOOMANYNOC   0b11111001

#ifdef	__cplusplus
extern "C" {
#endif

#include "ConfigBits.h"

#define BFWAIT() while (!SSPSTATbits.BF)

void initializeModule();
void requestDistance();
void commandFinished();
void writeOutSPI(unsigned char outWrite);

#ifdef	__cplusplus
}
#endif

#endif	/* HBRIDGE_PWM_H */

