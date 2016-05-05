/* 
 * File:   HBRIDGE-PWM.c
 * Author: CraigVella
 *
 * Created on April 13, 2016, 6:59 PM
 */

#include "HBRIDGE-PWM.h"

unsigned char waitingForCommand = 1;
unsigned char commandWaitingFor = 0;
unsigned char recievedDistance = 0;
unsigned char distanceHigh = 0;
unsigned char distanceLow  = 0;
unsigned char recievingDistance = 0;
unsigned char nocCount = 0;

void interrupt ISR() {

    if (PIR1bits.TMR1GIF) {
        distanceHigh = TMR1H;
        distanceLow = TMR1L;
        recievedDistance = 1;
        recievingDistance = 0;
        TMR1H = 0;
        TMR1L = 0;
        PIR1bits.TMR1GIF = 0;
        T1GCONbits.T1GGO_nDONE = 1;
    }

}

void requestDistance() {
    if (recievingDistance) return;
    recievingDistance = 1;
    recievedDistance = 0;
    PORTCbits.RC2 = 1;
    for (unsigned int x = 0; x < 3400; ++x);
    PORTCbits.RC2 = 0;
}

void main() {
    initializeModule();

    unsigned char cBuffer = 0;

    SSPBUF = 0;
    while (1) {
        BFWAIT(); // Wait for Buffer to Be full
        cBuffer = SSPBUF;
        if (waitingForCommand) {
            commandWaitingFor = cBuffer;
            if (commandWaitingFor != COMMAND_NOCOMMAND) nocCount = 0;
            switch (commandWaitingFor) {
                case COMMAND_NOCOMMAND:
                    if (++nocCount == 100) {
                        writeOutSPI(COMMAND_TOOMANYNOC);
                        nocCount = 0;
                    } else {
                        writeOutSPI(COMMAND_CONFIRMED);
                    }
                    break;
                case COMMAND_RETDISTANCEH:
                    writeOutSPI(distanceHigh);
                    break;
                case COMMAND_RETDISTANCEL:
                    writeOutSPI(distanceLow);
                    break;
                case COMMAND_QDISTREADY:
                    if (recievedDistance)
                        writeOutSPI(COMMAND_DISTREADY);
                    else
                        writeOutSPI(COMMAND_DISTNREADY);
                    break;
                case COMMAND_GETDISTANCE:
                    if (recievingDistance) {
                        writeOutSPI(COMMAND_BADSTATE);
                    } else {
                        requestDistance();
                        writeOutSPI(COMMAND_QUERY);
                    }
                    break;
                default:
                    waitingForCommand = 0;
                    writeOutSPI(COMMAND_CONFIRMED);
                    break;
            }
        } else {
            switch (commandWaitingFor) {
                case COMMAND_SETLWHEEL:
                    PWM1DCH = cBuffer & 0b01111111;
                    writeOutSPI(COMMAND_CONFIRMED);
                    commandFinished();
                    break;
                case COMMAND_SETRWHEEL:
                    PWM2DCH = cBuffer & 0b01111111;
                    writeOutSPI(COMMAND_CONFIRMED);
                    commandFinished();
                    break;
                default:
                    writeOutSPI(COMMAND_UNKNOWN);
                    commandFinished();
                    break;
            }
        }
    }
}

void commandFinished() {
    commandWaitingFor = COMMAND_NOCOMMAND;
    waitingForCommand = 1;
}

void writeOutSPI(unsigned char outWrite) {
    SSPBUF = outWrite;
    while (SSPCONbits.WCOL) {
        SSPCONbits.WCOL = 0;
        SSPBUF = outWrite;
    }
}

void initializeModule() {
    OSCCON = 0b01111000; // 16 MHZ and Select From FOSC Config

    // Go All Digital
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;

    TRISCbits.TRISC6 = 1; // Input on SS
    TRISBbits.TRISB6 = 1; // Input on SCK
    TRISBbits.TRISB4 = 1; // Input on SDI
    TRISAbits.TRISA4 = 1; // Input on TG1

        // Timer for PWM Enable
    T2CON = 0b00000100; // Prescaler to 1x
    PR2 = 0x70; // 7 Bit Resolution

    // PWMs Enabled
    PWM1CON = 0b11000000; // Enable PWM1
    PWM1DCH = 0x00; // Duty cycle To 0
    PWM2CON = 0b11000000; // Enable PWM2
    PWM2DCH = 0x00; // Duty cycle To 0

    // SPI Configuration (Slave Mode)
    SSPSTAT = 0b01000000; // Data on idle to Active Clock
    SSPCON1 = 0b00100100; // Slave mode with SS enabled

    // Timer1 Configuration
    TMR1H = 0;
    TMR1L = 0;
    T1GCON = 0b11011000;
    T1CON = 0b00100001;
    PIE1bits.TMR1GIE = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIR1bits.TMR1IF = 0;
    PIR1bits.TMR1GIF = 0;
}

