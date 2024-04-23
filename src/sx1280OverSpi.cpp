/* sx1280OverSpi.cpp - Library interacting with and sending messages through a 2.4Ghz Lora modem
   Created by Christopher Schorn, August 16, 2023.
   Released into the public domain.

   Library Spec Link: https://arduino.github.io/arduino-cli/0.34/library-specification/
*/

#include "Arduino.h"
#include "SPI.h"
#include "sx1280OverSpi.h"

sx1280OverSpi::sx1280OverSpi( uint8_t cssPin,   
                              uint8_t busyPin,
                              uint8_t resetPin ){
    sx1280CssPin = cssPin;   
    sx1280BusyPin = busyPin;
    sx1280ResetPin = resetPin;
}

void sx1280OverSpi::begin( ){

    pinMode( sx1280CssPin, OUTPUT );
    pinMode( sx1280BusyPin, INPUT );
    pinMode( sx1280ResetPin, OUTPUT );

}

/*  Driving the chip select pin low 
    Transactions with sx1280 start with chip select low */
void sx1280OverSpi::sx1280Select(){

    asm volatile ("nop \n nop \n nop");
    digitalWrite( sx1280CssPin, 0 );
    asm volatile ("nop \n nop \n nop");
}

/*  Driving the chip select pin high 
    Transactions with sx1280 end with chip select high */
void sx1280OverSpi::sx1280Deselect(){

     asm volatile ("nop \n nop \n nop");
     digitalWrite( sx1280CssPin, 1 );
     asm volatile ("nop \n nop \n nop");
}

void sx1280OverSpi::sx1280Reset( ){

    digitalWrite( sx1280ResetPin, 0 ); /* Resetting sx1280 during startup */
    asm volatile ("nop \n nop \n nop");
    digitalWrite( sx1280ResetPin, 1 );
}

void sx1280OverSpi::zeroingAnArray( uint8_t arrayToZero[], 
                                    uint16_t arrayLength ){

    for( uint16_t i = 0; i < arrayLength; i ++ ){
        arrayToZero[ i ] = 0;
    }
}

/* Function sending common transciever settings to sx1280 */ 
void sx1280OverSpi::sx1280Setup( uint8_t standbyMode, 
                                 uint8_t packetType, 
                                 uint8_t rfFrequency2316,
                                 uint8_t rfFrequency158, 
                                 uint8_t rfFrequency70, 
                                 uint8_t spreadingFactor,
                                 uint8_t bandwidth, 
                                 uint8_t codingRate, 
                                 uint8_t preambleLength, 
                                 uint8_t headerType, 
                                 uint8_t cyclicalRedundancyCheck, 
                                 uint8_t chirpInvert, 
                                 uint8_t outboundMessage[ ] ){

    uint8_t setupWriteData[ 10 ] = { 0 };
    uint16_t payloadLength = 0;

    /* Iterator */
    uint16_t i = 0;

    sx1280Reset( );

    /* Waiting till the busy pin is driven low 
       So the sx1280 is not sent a command while busy
            Because it wont receive the command */
    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after reset"));
    }

    /* Setting sx1280 Standby mode */
    *( setupWriteData ) = SETSTANDBY;
    *( setupWriteData + 1 ) = standbyMode; /* Setting STDBY_RC Mode 0x01, STDBY_XOSC */
    sx1280Select();
    SPI.transfer( setupWriteData, 2*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                    10 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after SETSTANDBY"));
    }

    /* Setting sx1280 Packet Type */
    *( setupWriteData ) = SETPACKETTYPE;
    *( setupWriteData + 1 ) = packetType;
    sx1280Select();
    SPI.transfer( setupWriteData, 2*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                    10 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after SETPACKETTYPE"));
    }

    /* Setting RF Frequency */
    *( setupWriteData ) = SETRFFREQUENCY;
    *( setupWriteData + 1 ) = rfFrequency2316; /* rfFrequency[23:16] */
    *( setupWriteData + 2 ) = rfFrequency158; /* rfFrequency[15:8] */
    *( setupWriteData + 3 ) = rfFrequency70; /* rfFrequency[7:0] */
    sx1280Select();
    SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                    10 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after SETRFFREQUENCY"));
    }

    /* Setting Tx and Rx Buffer Base Addresses
       Putting both at 0 since messages can be size of buffer */
    *( setupWriteData ) = SETBUFFERBASEADDRESS;
    *( setupWriteData + 1 ) = 0x00;
    *( setupWriteData + 2 ) = 0x00;
    sx1280Select();
    SPI.transfer( setupWriteData, 3*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                    10 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after SETBUFFERBASEADDRESS"));
    }

    /* Setting the Modulation Params */
    *( setupWriteData ) = SETMODULATIONPARAMS;
    *( setupWriteData + 1 ) = spreadingFactor; /* Spreading Factor */
    *( setupWriteData + 2 ) = bandwidth; /* Bandwidth */
    *( setupWriteData + 3 ) = codingRate; /* Coding Rate */
    sx1280Select();
    SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                    10 );

    /* 0x1E Must be written to register 0x0925 for SF5 or SF6 */
    if( spreadingFactor == 0x50 || spreadingFactor == 0x60 ){

        *( setupWriteData ) = WRITEREGISTER;
        *( setupWriteData + 1 ) = 0x09;
        *( setupWriteData + 2 ) = 0x25;
        *( setupWriteData + 3 ) = 0x1E;
        sx1280Select();
        SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
        sx1280Deselect();
        zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                        10 );
    }
    /* 0x37 Must be written to register 0x0925 for SF7 or SF8 */
    else if( spreadingFactor == 0x70 || spreadingFactor == 0x80 ){

        *( setupWriteData ) = WRITEREGISTER;
        *( setupWriteData + 1 ) = 0x09;
        *( setupWriteData + 2 ) = 0x25;
        *( setupWriteData + 3 ) = 0x37;
        sx1280Select();
        SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
        sx1280Deselect();
        zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                        10 );

    }
    /* 0x32 Must be written to register 0x0925 for SF9, SF10, SF11, or SF12 */
    else if( spreadingFactor == 0x90 || spreadingFactor == 0xA0 || spreadingFactor == 0xB0 || spreadingFactor == 0xC0 ){
        
        *( setupWriteData ) = WRITEREGISTER;
        *( setupWriteData + 1 ) = 0x09;
        *( setupWriteData + 2 ) = 0x25;
        *( setupWriteData + 3 ) = 0x32;
        sx1280Select();
        SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
        sx1280Deselect();
        zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                        10 );

    }
    /* 0x01 must be written to register 0x093C */
    *( setupWriteData ) = WRITEREGISTER;
    *( setupWriteData + 1 ) = 0x09;
    *( setupWriteData + 2 ) = 0x3C;
    *( setupWriteData + 3 ) = 0x01;
    sx1280Select();
    SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                    10 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after SETMODULATIONPARAMS"));
    }

    /* Setting Packet Params */
    for( i = 0; *( outboundMessage + i ) != 0x00; i++ ){
        /* Maximum payloadLength on sx1280 is 255, PRESET TO MAXIMUM PAYLOAD ALWAYS */
        if( payloadLength > 255 ){
            payloadLength = 255;
            break;
        }
        payloadLength = payloadLength + 1;
    }
    *( setupWriteData ) = SETPACKETPARAMS;
    *( setupWriteData + 1 ) = preambleLength; /* Preamble Length */
    *( setupWriteData + 2 ) = headerType; /* Header Type */
    *( setupWriteData + 3 ) = payloadLength; /* Payload Length */
    *( setupWriteData + 4 ) = cyclicalRedundancyCheck; /* Cyclical Redundancy Check */
    *( setupWriteData + 5 ) = chirpInvert; /* Invert IQ/chirp invert */
    *( setupWriteData + 6 ) = 0x00; /* Not Used */
    *( setupWriteData + 7 ) = 0x00; /* Not Used */
    sx1280Select();
    SPI.transfer( setupWriteData, 8*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( setupWriteData, /* Reassigning all array values to 0 */
                    10 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after SETPACKETPARAMS"));
    }

    /* Testing connecting from pico to sx1280 by writing to and reading from buffer
       Working output should be "status status FF" */

    /* for( i = 0; i < 255; i++ ){
        setupReadData[i] = 0;
    }

    *( writeData ) = WRITEBUFFER;
    *( writeData + 1 ) = 0x00;
    *( writeData + 2 ) = 0xFF;
    sx1280Select();
    SPI.transfer( setupWriteData, 3*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( setupWriteData, // Reassigning all array values to 0
                    10 ); */

    /* Must use two NOP's for reads because data is
            returned beginning on the second NOP */
    /* *( writeData ) = READBUFFER;
    *( writeData + 1 ) = 0x00;
    *( writeData + 2 ) = 0x00;
    *( writeData + 3 ) = 0x00;
    *( writeData + 4 ) = 0x00;
    sx1280Select();
    SPI.transfer( setupWriteData, 5*sizeof( uint8_t ) );
    sx1280Deselect(); 
    Serial.print( *( readData ) ); 
    Serial.print( *( readData + 1 ) );
    Serial.print( *( readData + 2 ) );
    Serial.print( *( readData + 3 ) );
    Serial.print( *( readData + 4 ) );
    zeroingAnArray( setupWriteData, 
                    10 );
*/

}


/* Function setting up and running tx operation on an sx1280, taking 255 byte message packets */
void sx1280OverSpi::sx1280Tx( uint8_t power, 
                              uint8_t rampTime, 
                              uint8_t outboundMessage[ ],
                              uint8_t txIrq158, 
                              uint8_t txIrq70, 
                              uint8_t txPeriodBase,
                              uint8_t txPeriodBaseCount158, 
                              uint8_t txPeriodBaseCount70 ){

    uint8_t txWriteData[ 259 ] = { 0 }; /* Size is 258 cause message buffer + opcode + offset */
    uint16_t txPayloadLength = 0;

    /* Iterators */
    uint16_t i = 0;

    /* Setting the tx parameters necessary for sending a message */
    *( txWriteData ) = SETTXPARAMS;
    *( txWriteData + 1 ) = power;    /* power       */
    *( txWriteData + 2 ) = rampTime; /* rampTime    */
    sx1280Select();
    SPI.transfer( txWriteData, 3*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( txWriteData, /* Reassigning all array values to 0 */
                    258 );


    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after SETTXPARAMS"));
    }

    /* Writing a message to the sx1280 Tx message buffer */
    while( *( outboundMessage + txPayloadLength ) != 0x00 ){

        /* Getting size of a single outbound message, storing it in a holder variable */
        txPayloadLength = txPayloadLength + 1;

        if( txPayloadLength > 255 ){
            txPayloadLength = 255;
            *( outboundMessage + txPayloadLength ) = 0x00;

        }
    }
    /* Allocating txPayloadLength+3 bytes to writeData, payloadLength is indexed from zero
            and space is needed for the WRITEBUFFER command and nop
    txWriteData = ( uint8_t * )malloc( ( txPayloadLength+3 )*sizeof( uint8_t ) ); */
    *( txWriteData ) = WRITEBUFFER;
    *( txWriteData + 1 ) = 0x00;
    /* Looping payloadLength times, writing outboundMessage data to WRITEBUFFER command */
    for( i = 0; i <= txPayloadLength; i++ ){

        *( txWriteData + i + 2 ) = *( outboundMessage + i );
        Serial.print(F("Outbound Message: 0x"));
        Serial.println( *( outboundMessage + i ), HEX );
    }
    sx1280Select();
    SPI.transfer( txWriteData, ( txPayloadLength+3 )*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( txWriteData, /* Reassigning all array values to 0 */
                    258 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after tx WRITEBUFFER"));
    }

    /* setting IRQ parameters for the outgoing message, looping SPI not DIO pins to check*/
    *( txWriteData ) = SETDIOIRQPARAMS;
    *( txWriteData + 1 ) = txIrq158;    /* IRQ Mask for bits 15:8 of IRQ register   */
    *( txWriteData + 2 ) = txIrq70;     /* IRQ Mask for bits 7:0 of IRQ register    */
    *( txWriteData + 3 ) = 0x00;        /* setting DIO 1 Mask bits 15:8 to 0        */
    *( txWriteData + 4 ) = 0x00;        /* setting DIO 1 Mask bits 7:0 to 0         */
    *( txWriteData + 5 ) = 0x00;        /* setting DIO 2 Mask bits 15:8 to 0        */
    *( txWriteData + 6 ) = 0x00;        /* setting DIO 2 Mask bits 7:0 to 0         */
    *( txWriteData + 7 ) = 0x00;        /* setting DIO 3 Mask bits 15:8 to 0        */
    *( txWriteData + 8 ) = 0x00;        /* setting DIO 3 Mask bits 7:0 to 0         */
    sx1280Select();
    SPI.transfer( txWriteData, 9*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( txWriteData, /* Reassigning all array values to 0 */
                    258 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after tx SETDIOIRQPARAMS"));
    }

    /* Putting sx1280 in transmit mode to send the message in sx1280's message buffer 
       Timeout is periodBase * periodBaseCount */
    *( txWriteData ) = SETTX;
    *( txWriteData + 1 ) = txPeriodBase;            /* setting periodBase, RTC step         */
    *( txWriteData + 2 ) = txPeriodBaseCount158;    /* setting periodBaseCount bits 15 to 8 */
    *( txWriteData + 3 ) = txPeriodBaseCount70;     /* setting periodBaseCount bits 8 to 0  */
    sx1280Select();
    SPI.transfer( txWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( txWriteData, /* Reassigning all array values to 0 */
                    258 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after tx SETTX"));
    }

    /* Looping over GETIRQSTATUS using SPI, till TxDone bit is high */
    for( i = 0; i <= 100; i++){

        delay( 50 );

        *( txWriteData ) = GETIRQSTATUS;
        *( txWriteData + 1 ) = 0x00;
        *( txWriteData + 2 ) = 0x00;
        *( txWriteData + 3 ) = 0x00;
        sx1280Select();
        /* Arduino SPI replaces txWriteData with incoming data when size is specified */
        SPI.transfer( txWriteData, 4*sizeof( uint8_t ) ); 
        sx1280Deselect();
 
        while( digitalRead( sx1280BusyPin ) == 1 ){
            delay( 10 );
            Serial.println(F("Busy after tx GETIRQSTATUS"));
        }

        Serial.print(F("Outbound IRQ Check: 0x"));
        Serial.print( *( txWriteData + 3 ), HEX );
        Serial.print(F(" "));
        Serial.println( i, DEC );

        /* Checking bits [7:0] to see if the TxDone bit in the IRQ register is high
           Doing bitwise 'and' operation with 0x01 to mask the rest of the bits in 
                the IRQ register, giving a clear indication that a message has been sent
            Bits [15:8] would be in  *( readData + 4 ) */
        if( *( txWriteData + 3 ) != 0x00 ){ /* GETIRQSTATUS TxDone == 1 */

            Serial.print(F("Outbound IRQ: 0x"));
            Serial.print( *( txWriteData + 3 ), HEX);
            Serial.print(F(" "));
            Serial.println( i, DEC );
            break;
        }

        zeroingAnArray( txWriteData, /* Reassigning all array values to 0 */
                        258 );
    }


    /* Clearing the IRQ register, reseting IRQ Mask bits to 0 */
    *( txWriteData ) = CLRIRQSTATUS;
    *( txWriteData + 1 ) = 0xFF; /* clearing bits 15:8 of IRQ mask */
    *( txWriteData + 2 ) = 0xFF; /* clearing bits 7:0 of IRQ mask */
    sx1280Select();
    SPI.transfer( txWriteData,  3*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( txWriteData, /* Reassigning all array values to 0 */
                    258 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after tx CLRIRQSTATUS"));
    }

    /* Tx SETSANDBY */
    *( txWriteData ) = SETSTANDBY;
    *( txWriteData + 1 ) = 0x00;
    sx1280Select();
    SPI.transfer( txWriteData,  2*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( txWriteData, /* Reassigning all array values to 0 */
                    258 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after tx SETSTANDBY"));
    }
}


/* Function setting up and running rx operation on an sx1280, 2.4Ghz LORA Modem*/
void sx1280OverSpi::sx1280Rx( uint8_t rxIrq158, 
                              uint8_t rxIrq70, 
                              uint8_t rxPeriodBase,
                              uint8_t rxPeriodBaseCount158, 
                              uint8_t rxPeriodBaseCount70,
                              uint8_t inboundMessage[ 255 ] ){

    /* Iterators */
    uint16_t i = 0;
    uint16_t j = 0;

    uint8_t rxWriteData[ 258 ] = { 0 };

    uint16_t totalSizeOfMessage = 0;
    uint16_t sizeOfMessageInBuffer = 0;

    /* setting IRQ parameters for Rx mode */
    *( rxWriteData ) = SETDIOIRQPARAMS;
    *( rxWriteData + 1 ) = rxIrq158;  /* IRQ Mask for bits 15:8 of IRQ register */
    *( rxWriteData + 2 ) = rxIrq70;   /* IRQ Mask for bits 7:0 of IRQ register */ 
    *( rxWriteData + 3 ) = 0x00;      /* setting DIO 1 Mask bits 15:8 to 0 */
    *( rxWriteData + 4 ) = 0x00;      /* setting DIO 1 Mask bits 7:0 to 0 */
    *( rxWriteData + 5 ) = 0x00;      /* setting DIO 2 Mask bits 15:8 to 0 */
    *( rxWriteData + 6 ) = 0x00;      /* setting DIO 2 Mask bits 7:0 to 0 */
    *( rxWriteData + 7 ) = 0x00;      /* setting DIO 3 Mask bits 15:8 to 0 */
    *( rxWriteData + 8 ) = 0x00;      /* setting DIO 3 Mask bits 7:0 to 0 */
    sx1280Select();
    SPI.transfer( rxWriteData, 9*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( rxWriteData, /* Reassigning all array values to 0 */
                    258 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after rx SETDIOIRQPARAMS"));
    }

    /* setting sx1280 to Rx mode
       Setting Rx mode to continuous, so multiple messages can be received */
    *( rxWriteData ) = SETRX;
    *( rxWriteData + 1 ) = rxPeriodBase;          /* Setting the RTC step         */
    *( rxWriteData + 2 ) = rxPeriodBaseCount158;  /* perdiodBase[15:8] for rx     */
    *( rxWriteData + 3 ) = rxPeriodBaseCount70;   /* perdiodBase[7:0] for rx      */
    sx1280Select();
    SPI.transfer( rxWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( rxWriteData, /* Reassigning all array values to 0 */
                    258 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after SETRX"));
    }

    /* Loop polling 25 times over rx mode
       Each loop has a 50 millisecond delay allowing other tasks to run */
    for( i = 0; i <= 25; i++ ){ 

        Serial.print(F("Listening: "));
        Serial.println( i, DEC );
        sizeOfMessageInBuffer = 0;

        /* Using GETIRQSTATUS to check if there is a new message in the rx buffer */
        *( rxWriteData ) = GETIRQSTATUS;
        *( rxWriteData + 1 ) = 0x00;
        *( rxWriteData + 2 ) = 0x00;  /* IRQ bits 15 to 8 returned    */
        *( rxWriteData + 3 ) = 0x00;  /* IRQ bits 7 to 0 returned     */
        sx1280Select();
        SPI.transfer( rxWriteData, 4*sizeof( uint8_t ) );
        sx1280Deselect();

        /* Checking to see if the RxDone bit in the IRQ register is high, with 0x02 bitmask */
        if( ( *( rxWriteData + 3 ) & 0x02 ) == 0x02 ){ /* GETIRQSTATUS RxDone == 1 */

            zeroingAnArray( rxWriteData, /* Reassigning all array values to 0 */
                            258 );

            while( digitalRead( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.println(F("Busy after rx GETIRQSTATUS"));
            }

            /* using GETPACKETSTATUS which returns rssiSync, and Signal to Noise Ratio ( SNR )
               Not currently using but it's in sx1280 Documentation for Rx operation
                    pretty sure it's used to see if the received message is useable or not */
            *( rxWriteData ) = GETPACKETSTATUS;
            *( rxWriteData + 1 ) = 0x00;
            *( rxWriteData + 2 ) = 0x00;
            *( rxWriteData + 3 ) = 0x00;
            *( rxWriteData + 4 ) = 0x00;
            *( rxWriteData + 5 ) = 0x00;
            *( rxWriteData + 6 ) = 0x00;
            sx1280Select();
            SPI.transfer( rxWriteData, 7*sizeof( uint8_t ) );
            sx1280Deselect();
            zeroingAnArray( rxWriteData, /* Reassigning all array values to 0 */
                            258 );

            while( digitalRead( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.println(F("Busy after rx GETPACKETSTATUS"));
            }

            /* Clearing the IRQ register on the sx1280
               Not sure why it's done here in the rx operation in sx1280 documentation */
            *( rxWriteData ) = CLRIRQSTATUS;
            *( rxWriteData + 1 ) = 0xFF;
            *( rxWriteData + 2 ) = 0xFF;
            sx1280Select();
            SPI.transfer( rxWriteData, 3*sizeof( uint8_t ) );
            sx1280Deselect();
            zeroingAnArray( rxWriteData, /* Reassigning all array values to 0 */
                            258 );

            while( digitalRead( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.println(F("Busy after rx CLRIRQSTATUS"));
            }

            /* Getting the length of the newly received message
               GETRXBUFFERSTATUS only works for LORA messages with headers, 
                    otherwise read register 0x0901 */
            *( rxWriteData ) = GETRXBUFFERSTATUS; 
            *( rxWriteData + 1 ) = 0x00;
            *( rxWriteData + 2 ) = 0x00;
            *( rxWriteData + 3 ) = 0x00;
            sx1280Select();
            SPI.transfer( rxWriteData, 4*sizeof( uint8_t ) );
            sx1280Deselect();
            /* Grabbing message size for correct memory allocation for incoming message */
            sizeOfMessageInBuffer = *( rxWriteData + 2 );
            zeroingAnArray( rxWriteData, /* Reassigning all array values to 0 */
                            258 );

            while( digitalRead( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.println(F("Busy after rx READREGISTER"));
            }

            /* Reading message buffer of sx1280
               Allocating the size of the message in the sx1280 buffer plus 3 because over 
                    spi you must send an opcode, the buffer offset, and a nop to receive the
                    payload on the buffer */
            // rxWriteData = ( uint8_t * ) malloc( (sizeOfMessageInBuffer + 3)*sizeof(uint8_t));
            *( rxWriteData ) = READBUFFER;
            *( rxWriteData + 1 ) = 0x00; /* sx1280 message buffer offset */
            *( rxWriteData + 2 ) = 0x00; /* sending first nop */
            /* Looping through rxWriteData to add nops, i begins at *( rxWriteData + 3 ) */
            /* Serial.print("Final Address = 0x"); 
            Serial.println( *( rxWriteData + sizeOfMessageInBuffer + 3 ), HEX ); */
            for( j = 3; j <= sizeOfMessageInBuffer; j++){
                *( rxWriteData + j ) = 0x00;
                /* Serial.print("rxWriteData + j = 0x j =  ", ( rxWriteData + j ), j ); */
            }
            sx1280Select();
            SPI.transfer( rxWriteData, ( sizeOfMessageInBuffer + 3 )*sizeof( uint8_t ) );
            sx1280Deselect();

            /* Passing newly received message pointer to vSx1280Task */
            for( j = 0; j <= sizeOfMessageInBuffer; j++ ){
                inboundMessage[ j ] = *( rxWriteData + j + 3 );
            }
            zeroingAnArray( rxWriteData, /* Reassigning all array values to 0 */
                            258 );

            while( digitalRead( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.println(F("Busy after rx READBUFFER"));
            }
        }
        delay( 50 ); 
    }

    /* Rx SETSANDBY */
    *( rxWriteData ) = SETSTANDBY;
    *( rxWriteData + 1 ) = 0x00;
    sx1280Select();
    SPI.transfer( rxWriteData, 2*sizeof( uint8_t ) );
    sx1280Deselect();
    zeroingAnArray( rxWriteData, /* Reassigning all array values to 0 */
                    258 );

    while( digitalRead( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.println(F("Busy after rx SETSTANDBY"));
    }
}
