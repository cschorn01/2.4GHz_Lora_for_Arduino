/* sx1280OverSpi.cpp - Library interacting with and sending messages through a 2.4Ghz Lora modem
   Created by Christopher Schorn, August 16, 2023.
   Released into the public domain.

*/

#include "Arduino.h"
#include "sx1280OverSpi.h"

sx1280OverSpi::sx1280OverSpi( uint8_t cssPin,   
                              uint8_t busyPin,
                              uint8_t resetPin ){
    sx1280CssPin = cssPin;   
    sx1280BusyPin = busyPin;
    sx1280ResetPin = resetPin;
    gpio_put( sx1280ResetPin, 0 ); /* Resetting sx1280 during startup */
    asm volatile ("nop \n nop \n nop");
    gpio_put( sx1280ResetPin, 1 );

}

void sx1280OverSpi::begin( ){
}

/*  Driving the chip select pin low 
    Transactions with sx1280 start with chip select low */
void sx1280OverSpi::sx1280Select(){

    asm volatile ("nop \n nop \n nop");/* Find out what it does */
    gpio_put( sx1280CssPin, 0 );
    asm volatile ("nop \n nop \n nop");
}

/*  Driving the chip select pin high 
    Transactions with sx1280 end with chip select high */
void sx1280OverSpi::sx1280Deselect(){

     asm volatile ("nop \n nop \n nop");
     gpio_put( sx1280CssPin, 1 );
     asm volatile ("nop \n nop \n nop");
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
                                 uint8_t *outboundMessage ){

    /* Format For SPI Communication with sx1280 */

    /* Allocate memory for a dynamic array to 
            send data to sx1280 (writeData) */
    /* Allocate memory for a dynamic array to 
            receive data from sx1280 (readData) */
    /* Add Opcode to first memory byte in writeData */
    /* Add data in hexadecimal to second, third, etc. 
            memory byte in writeData for sets, NOP for gets  */
    /* int spi_write_blocking( spi_inst_t *spi, 
                               const uint8_t *src,
                               size_t len ) */
    /* Deallocate memory for writeData and/or readData */

    uint8_t *setupWriteData;
    uint8_t *setupReadData;
    uint32_t payloadLength = 0;

    /* Driving pin 21 low to reset the sx1280 */
    gpio_put( 21, 0 );
    asm volatile ("nop \n nop \n nop");
    gpio_put( 21, 1 );

    /* Waiting till the busy pin is driven low 
       So the sx1280 is not sent a command while busy
            Because it wont receive the command */
    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after reset");
    }

    /* Setting sx1280 Standby mode */
    setupWriteData = ( uint8_t * ) malloc( 2*sizeof( uint8_t ) );
    *( setupWriteData ) = SETSTANDBY;
    *( setupWriteData + 1 ) = standbyMode; /* Setting STDBY_RC Mode 0x01, STDBY_XOSC */
    sx1280Select();
    SPI.transfer( setupWriteData, 2*sizeof( uint8_t ) );
    sx1280Deselect();
    free( setupWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after SETSTANDBY");
    }

    /* Setting sx1280 Packet Type */
    setupWriteData = ( uint8_t * ) malloc( 2*sizeof( uint8_t ) );
    *( setupWriteData ) = SETPACKETTYPE;
    *( setupWriteData + 1 ) = packetType;
    sx1280Select();
    SPI.transfer( setupWriteData, 2*sizeof( uint8_t ) );
    sx1280Deselect();
    free( setupWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after SETPACKETTYPE");
    }

    /* Setting RF Frequency */
    setupWriteData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
    *( setupWriteData ) = SETRFFREQUENCY;
    *( setupWriteData + 1 ) = rfFrequency2316; /* rfFrequency[23:16] */
    *( setupWriteData + 2 ) = rfFrequency158; /* rfFrequency[15:8] */
    *( setupWriteData + 3 ) = rfFrequency70; /* rfFrequency[7:0] */
    sx1280Select();
    SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    free( setupWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after SETRFFREQUENCY");
    }

    /* Setting Tx and Rx Buffer Base Addresses
       Putting both at 0 since messages can be size of buffer */
    setupWriteData = ( uint8_t * ) malloc( 3*sizeof( uint8_t ) );
    *( setupWriteData ) = SETBUFFERBASEADDRESS;
    *( setupWriteData + 1 ) = 0x00;
    *( setupWriteData + 2 ) = 0x00;
    sx1280Select();
    SPI.transfer( setupWriteData, 3*sizeof( uint8_t ) );
    sx1280Deselect();
    free( setupWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after SETBUFFERBASEADDRESS");
    }

    /* Setting the Modulation Params */
    setupWriteData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
    *( setupWriteData ) = SETMODULATIONPARAMS;
    *( setupWriteData + 1 ) = spreadingFactor; /* Spreading Factor */
    *( setupWriteData + 2 ) = bandwidth; /* Bandwidth */
    *( setupWriteData + 3 ) = codingRate; /* Coding Rate */
    sx1280Select();
    SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    free( setupWriteData );
    /* 0x1E Must be written to register 0x0925 for SF5 or SF6 */
    if( spreadingFactor == 0x50 || 
        spreadingFactor == 0x60 ){

        setupWriteData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
        *( setupWriteData ) = WRITEREGISTER;
        *( setupWriteData + 1 ) = 0x09;
        *( setupWriteData + 2 ) = 0x25;
        *( setupWriteData + 3 ) = 0x1E;
        sx1280Select();
        SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
        sx1280Deselect();
        free( setupWriteData );
    }
    /* 0x37 Must be written to register 0x0925 for SF7 or SF8 */
    else if( spreadingFactor == 0x70 || 
             spreadingFactor == 0x80 ){

        setupWriteData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
        *( setupWriteData ) = WRITEREGISTER;
        *( setupWriteData + 1 ) = 0x09;
        *( setupWriteData + 2 ) = 0x25;
        *( setupWriteData + 3 ) = 0x37;
        sx1280Select();
        SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
        sx1280Deselect();
        free( setupWriteData );
    }
    /* 0x32 Must be written to register 0x0925 for SF9, SF10, SF11, or SF12 */
    else if( spreadingFactor == 0x90 || 
             spreadingFactor == 0xA0 || 
             spreadingFactor == 0xB0 || 
             spreadingFactor == 0xC0 ){
        
        setupWriteData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
        *( setupWriteData ) = WRITEREGISTER;
        *( setupWriteData + 1 ) = 0x09;
        *( setupWriteData + 2 ) = 0x25;
        *( setupWriteData + 3 ) = 0x32;
        sx1280Select();
        SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
        sx1280Deselect();
        free( setupWriteData );
    }
    /* 0x01 must be written to register 0x093C */
    setupWriteData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
    *( setupWriteData ) = WRITEREGISTER;
    *( setupWriteData + 1 ) = 0x09;
    *( setupWriteData + 2 ) = 0x3C;
    *( setupWriteData + 3 ) = 0x01;
    sx1280Select();
    SPI.transfer( setupWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    free( setupWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after SETMODULATIONPARAMS");
    }

    /* Setting Packet Params */
    while( *( outboundMessage + payloadLength ) != 0x00 ){
        /* Maximum payloadLength on sx1280 is 255, PRESET TO MAXIMUM PAYLOAD ALWAYS */
        if( payloadLength > 255 ){
            payloadLength = 255;
            break;
        }
        payloadLength = payloadLength + 1;
    }
    setupWriteData = ( uint8_t * ) malloc( 8*sizeof( uint8_t ) );
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
    free( setupWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after SETPACKETPARAMS");
    }

    /* Testing connecting from pico to sx1280 by writing to and reading from buffer
       Working output should be "status status FF" */

    /* writeData = ( uint8_t * ) malloc( 3*sizeof( uint8_t ) );
    *( writeData ) = WRITEBUFFER;
    *( writeData + 1 ) = 0x00;
    *( writeData + 2 ) = 0xFF;
    sx1280Select();
    SPI.transfer( setupWriteData, 3*sizeof( uint8_t ) );
    sx1280Deselect();
    free( writeData ); */

    /* Must use two NOP's for reads because data is
            returned beginning on the second NOP */
    /*setupReadData = ( uint8_t * ) malloc( 5*sizeof( uint8_t ) );
    setupWriteData = ( uint8_t * ) malloc( 5*sizeof( uint8_t ) );
    *( writeData ) = READBUFFER;
    *( writeData + 1 ) = 0x00;
    *( writeData + 2 ) = 0x00;
    *( writeData + 3 ) = 0x00;
    *( writeData + 4 ) = 0x00;
    sx1280Select();
    setupReadData = SPI.transfer( setupWriteData, 5*sizeof( uint8_t ) );
    sx1280Deselect(); 
    Serial.print( *( readData ) ); 
    Serial.print( *( readData + 1 ) );
    Serial.print( *( readData + 2 ) );
    Serial.print( *( readData + 3 ) );
    Serial.print( *( readData + 4 ) );
    free( writeData );
    free( readData ); */

}


/* Function setting up and running tx operation on an sx1280, taking 255 byte message packets */
void sx1280OverSpi::sx1280Tx( uint8_t power, 
                              uint8_t rampTime, 
                              uint8_t *outboundMessage,
                              uint8_t txIrq158, 
                              uint8_t txIrq70, 
                              uint8_t txPeriodBase,
                              uint8_t txPeriodBaseCount158, 
                              uint8_t txPeriodBaseCount70 ){

    uint8_t *txWriteData = 0;
    uint8_t *txReadData = 0;
    uint32_t txPayloadLength = 0;
    /* Iterators */
    uint32_t i = 0;

    /* Setting the tx parameters necessary for sending a message */
    txWriteData = ( uint8_t * ) malloc( 3*sizeof( uint8_t ) );
    *( txWriteData ) = SETTXPARAMS;
    *( txWriteData + 1 ) = power;    /* power       */
    *( txWriteData + 2 ) = rampTime; /* rampTime    */
    sx1280Select();
    SPI.transfer( txWriteData, 3*sizeof( uint8_t ) );
    sx1280Deselect();
    free( txWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after SETTXPARAMS");
    }

    /* Writing a message to the sx1280 Tx message buffer */
    while( *( outboundMessage + txPayloadLength ) != 0x00 ){
        /* Getting size of a single outbound message, storing it in a holder variable */
        txPayloadLength = txPayloadLength + 1;
    }
    /* Allocating txPayloadLength+3 bytes to writeData, payloadLength is indexed from zero
            and space is needed for the WRITEBUFFER command and nop */
    txWriteData = ( uint8_t * )malloc( ( txPayloadLength+3 )*sizeof( uint8_t ) );
    *( txWriteData ) = WRITEBUFFER;
    *( txWriteData + 1 ) = 0x00;
    /* Looping payloadLength times, writing outboundMessage data to WRITEBUFFER command */
    for( i = 0; i <= txPayloadLength; i++ ){
        *( txWriteData + i + 2 ) = *( outboundMessage + i );
        Serial.print("Outbound Message: 0x");
        Serial.println( *( outboundMessage + i ), HEX );
    }
    sx1280Select();
    SPI.transfer( txWriteData, ( txPayloadLength+3 )*sizeof( uint8_t ) );
    sx1280Deselect();
    free( txWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after tx WRITEBUFFER");
    }

    /* setting IRQ parameters for the outgoing message, looping SPI not DIO pins to check*/
    txWriteData = ( uint8_t * ) malloc( 9*sizeof( uint8_t ) );
    *( txWriteData ) = SETDIOIRQPARAMS;
    *( txWriteData + 1 ) = txIrq158; /* IRQ Mask for bits 15:8 of IRQ register */
    *( txWriteData + 2 ) = txIrq70; /* IRQ Mask for bits 7:0 of IRQ register */
    *( txWriteData + 3 ) = 0x00; /* setting DIO 1 Mask bits 15:8 to 0 */
    *( txWriteData + 4 ) = 0x00; /* setting DIO 1 Mask bits 7:0 to 0 */
    *( txWriteData + 5 ) = 0x00; /* setting DIO 2 Mask bits 15:8 to 0 */
    *( txWriteData + 6 ) = 0x00; /* setting DIO 2 Mask bits 7:0 to 0 */
    *( txWriteData + 7 ) = 0x00; /* setting DIO 3 Mask bits 15:8 to 0 */
    *( txWriteData + 8 ) = 0x00; /* setting DIO 3 Mask bits 7:0 to 0 */
    sx1280Select();
    SPI.transfer( txWriteData, 9*sizeof( uint8_t ) );
    sx1280Deselect();
    free( txWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after tx SETDIOIRQPARAMS");
    }

    /* Putting sx1280 in transmit mode to send the message in sx1280's message buffer 
       Timeout is periodBase * periodBaseCount */
    txWriteData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
    *( txWriteData ) = SETTX;
    *( txWriteData + 1 ) = txPeriodBase; /* setting periodBase, RTC step */
    *( txWriteData + 2 ) = txPeriodBaseCount158; /* setting periodBaseCount[15:8] */
    *( txWriteData + 3 ) = txPeriodBaseCount70; /* setting periodBaseCount[8:0] */
    sx1280Select();
    SPI.transfer( txWriteData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    free( txWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after tx SETTX");
    }

    /* Looping over GETIRQSTATUS using SPI, till TxDone bit is high */
    for( i = 0; i <= 100; i++){

        free( txReadData );
        delay( 50 );

        txWriteData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
        txReadData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
        *( txWriteData ) = GETIRQSTATUS;
        *( txWriteData + 1 ) = 0x00;
        *( txWriteData + 2 ) = 0x00;
        *( txWriteData + 3 ) = 0x00;
        sx1280Select();
        txReadData = SPI.transfer( txWriteData, 4*sizeof( uint8_t ) );
        sx1280Deselect();
        free( txWriteData );
 
        while( gpio_get( sx1280BusyPin ) == 1 ){
            delay( 10 );
            Serial.print("Busy after tx GETIRQSTATUS");
        }

        Serial.print("IRQ Check: 0x", 
        Serial.print( *( txReadData + 3 ), HEX );
        Serial.print(" ");
        Serial.println( i, DEC );

        /* Checking bits [7:0] to see if the TxDone bit in the IRQ register is high
           Doing bitwise 'and' operation with 0x01 to mask the rest of the bits in 
                the IRQ register, giving a clear indication that a message has been sent
            Bits [15:8] would be in  *( readData + 4 ) */
        if( *( txReadData + 3 ) != 0x00 ){ /* GETIRQSTATUS TxDone == 1 */

            Serial.print("IRQ: 0x  ", 
            Serial.print( *( txReadData + 3 ), HEX);
            Serial.print(" ");
            Serial.println( i, DEC );
            free( txReadData );
            break;
        }

    }

    /* Clearing the IRQ register, reseting IRQ Mask bits to 0 */
    txWriteData = ( uint8_t * ) malloc( 3*sizeof( uint8_t ) );
    *( txWriteData ) = CLRIRQSTATUS;
    *( txWriteData + 1 ) = 0xFF; /* clearing bits 15:8 of IRQ mask */
    *( txWriteData + 2 ) = 0xFF; /* clearing bits 7:0 of IRQ mask */
    sx1280Select();
    SPI.transfer( txWriteData,  3*sizeof( uint8_t ) );
    sx1280Deselect();
    free( txWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after tx CLRIRQSTATUS");
    }

    /* Tx SETSANDBY */
    txWriteData = ( uint8_t * ) malloc( 2*sizeof( uint8_t ) );
    *( txWriteData ) = SETSTANDBY;
    *( txWriteData + 1 ) = 0x00;
    sx1280Select();
    SPI.transfer( txWriteData,  2*sizeof( uint8_t ) );
    sx1280Deselect();
    free( txWriteData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after tx SETSTANDBY");
    }
}


/* Function setting up and running rx operation on an sx1280, 2.4Ghz LORA Modem*/
void sx1280OverSpi::sx1280Rx( uint8_t rxIrq158, 
                              uint8_t rxIrq70, 
                              uint8_t rxPeriodBase,
                              uint8_t rxPeriodBaseCount158, 
                              uint8_t rxPeriodBaseCount70,
                              uint8_t *inboundMessage ){

    uint8_t *writeData;
    uint8_t *readData;
    uint32_t totalSizeOfMessage = 0;
    uint32_t sizeOfMessageInBuffer = 0;
    /* Iterators */
    uint32_t i = 0;
    uint32_t j = 0;
    

    /* setting IRQ parameters for Rx mode */
    writeData = ( uint8_t * ) malloc( 9*sizeof( uint8_t ) );
    *( writeData ) = SETDIOIRQPARAMS;
    *( writeData + 1 ) = rxIrq158; /* IRQ Mask for bits 15:8 of IRQ register */
    *( writeData + 2 ) = rxIrq70; /* IRQ Mask for bits 7:0 of IRQ register */ 
    *( writeData + 3 ) = 0x00; /* setting DIO 1 Mask bits 15:8 to 0 */
    *( writeData + 4 ) = 0x00; /* setting DIO 1 Mask bits 7:0 to 0 */
    *( writeData + 5 ) = 0x00; /* setting DIO 2 Mask bits 15:8 to 0 */
    *( writeData + 6 ) = 0x00; /* setting DIO 2 Mask bits 7:0 to 0 */
    *( writeData + 7 ) = 0x00; /* setting DIO 3 Mask bits 15:8 to 0 */
    *( writeData + 8 ) = 0x00; /* setting DIO 3 Mask bits 7:0 to 0 */
    sx1280Select();
    SPI.transfer( writeData, 9*sizeof( uint8_t ) );
    sx1280Deselect();
    free( writeData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after rx SETDIOIRQPARAMS");
    }

    /* setting sx1280 to Rx mode
       Setting Rx mode to continuous, so multiple messages can be received */
    writeData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
    *( writeData ) = SETRX;
    *( writeData + 1 ) = rxPeriodBase; /* Setting the RTC step */
    *( writeData + 2 ) = rxPeriodBaseCount158; /* perdiodBase[15:8] for rx */
    *( writeData + 3 ) = rxPeriodBaseCount70; /* perdiodBase[7:0] for rx */
    sx1280Select();
    SPI.transfer( writeData, 4*sizeof( uint8_t ) );
    sx1280Deselect();
    free( writeData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after SETRX");
    }

    /* Loop polling 100 times over rx mode
       Each loop has a 50 clock-tick delay allowing other tasks to run */
    for( i = 0; i <= 100; i++ ){ 

        Serial.print("Listening: ");
        Serial.println( i, DEC );
        sizeOfMessageInBuffer = 0;

        /* Using GETIRQSTATUS to check if there is a new message in the rx buffer */
        writeData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
        readData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
        *( writeData ) = GETIRQSTATUS;
        *( writeData + 1 ) = 0x00;
        *( writeData + 2 ) = 0x00;
        *( writeData + 3 ) = 0x00;
        sx1280Select();
        readData = SPI.transfer( writeData, 4*sizeof( uint8_t ) );
        sx1280Deselect();
        free( writeData );

        /* Checking to see if the RxDone bit in the IRQ register is high, with 0x02 bitmask */
        if( ( *( readData + 3 ) & 0x02 ) == 0x02 ){ /* GETIRQSTATUS RxDone == 1 */
 
            free( readData );

            while( gpio_get( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.print("Busy after rx GETIRQSTATUS");
            }

            /* see what the message is and decide what to do with it */

            /* using GETPACKETSTATUS which returns rssiSync, and Signal to Noise Ratio ( SNR )
               Not currently using but it's in sx1280 Documentation for Rx operation
                    pretty sure it's used to see if the received message is useable or not */
            writeData = ( uint8_t * ) malloc( 7*sizeof( uint8_t ) );
            readData = ( uint8_t * ) malloc( 7*sizeof( uint8_t ) );
            *( writeData ) = GETPACKETSTATUS;
            *( writeData + 1 ) = 0x00;
            *( writeData + 2 ) = 0x00;
            *( writeData + 3 ) = 0x00;
            *( writeData + 4 ) = 0x00;
            *( writeData + 5 ) = 0x00;
            *( writeData + 6 ) = 0x00;
            sx1280Select();
            readData = SPI.transfer( writeData, 4*sizeof( uint8_t ) );
            sx1280Deselect();
            free( writeData );
            free( readData );

            while( gpio_get( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.print("Busy after rx GETPACKETSTATUS");
            }

            /* Clearing the IRQ register on the sx1280
               Not sure why it's done here in the rx operation in sx1280 documentation */
            writeData = ( uint8_t * ) malloc( 3*sizeof( uint8_t ) );
            *( writeData ) = CLRIRQSTATUS;
            *( writeData + 1 ) = 0xFF;
            *( writeData + 2 ) = 0xFF;
            sx1280Select();
            SPI.transfer( writeData, 3*sizeof( uint8_t ) );
            sx1280Deselect();
            free( writeData );

            while( gpio_get( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.print("Busy after rx CLRIRQSTATUS");
            }

            /* Getting the length of the newly received message
               GETRXBUFFERSTATUS only works for LORA messages with headers, 
                    otherwise read register 0x0901 */
            writeData = ( uint8_t * ) malloc( 4*sizeof( uint8_t ) );
            readData = ( uint8_t * ) malloc( 4*sizeof(uint8_t ) );
            *( writeData ) = GETRXBUFFERSTATUS; 
            *( writeData + 1 ) = 0x00;
            *( writeData + 2 ) = 0x00;
            *( writeData + 3 ) = 0x00;
            sx1280Select();
            readData = SPI.transfer( writeData, 4*sizeof( uint8_t ) );
            sx1280Deselect();
            /* Grabbing message size for correct memory allocation for incoming message */
            sizeOfMessageInBuffer = *( readData + 2 );
            free( writeData );
            free( readData );

            while( gpio_get( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.print("Busy after rx READREGISTER");
            }

            /* Reading message buffer of sx1280
               Allocating the size of the message in the sx1280 buffer plus 3 because over 
                    spi you must send an opcode, the buffer offset, and a nop to receive the
                    payload on the buffer */
            writeData = ( uint8_t * ) malloc((sizeOfMessageInBuffer + 3)*sizeof(uint8_t));
            readData = ( uint8_t * ) malloc( (sizeOfMessageInBuffer + 3)*sizeof(uint8_t));
            *( writeData ) = READBUFFER;
            *( writeData + 1 ) = 0x00; /* sx1280 message buffer offset */
            *( writeData + 2 ) = 0x00; /* sending first nop */
            /* Looping through rest of writeData to add nops, i begins at *( writeData + 3 ) */
            Serial.print("Final Address = 0x"); 
            Serial.println( *( writeData + sizeOfMessageInBuffer + 3 ), HEX );
            for( j = 0; j <= sizeOfMessageInBuffer; j++){
                *( writeData + j + 3 ) = 0x00;
                /* Serial.print("writeData + j = 0x j =  ", ( writeData + j + 3 ), j ); */
            }
            sx1280Select();
            readData = SPI.transfer( writeData, 
                                     ( sizeOfMessageInBuffer + 3 )*sizeof( uint8_t ) );
            sx1280Deselect();
            free( writeData );
            /* Passing newly received message pointer to vSx1280Task */
            inboundMessage = readData; 

            while( gpio_get( sx1280BusyPin ) == 1 ){
                delay( 10 );
                Serial.print("Busy after rx READBUFFER");
            }
        delay( 50 ); 
    }

    /* Rx SETSANDBY */
    writeData = ( uint8_t * ) malloc( 2*sizeof( uint8_t ) );
    *( writeData ) = SETSTANDBY;
    *( writeData + 1 ) = 0x00;
    sx1280Select();
    SPI.transfer( writeData, 2*sizeof( uint8_t ) );
    sx1280Deselect();
    free( writeData );

    while( gpio_get( sx1280BusyPin ) == 1 ){
        delay( 10 );
        Serial.print("Busy after rx SETSTANDBY");
    }
}
