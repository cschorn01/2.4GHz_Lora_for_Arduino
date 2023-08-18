#include <sx1280OverSPI.h>
#include <SPI.h>

sx1280OverSpi sx1280_1( 13,   // uint8_t cssPin
                        22,   // uint8_t busyPin
                        21 ); // uint8_t resetPin 

void setup( ) {

    /* Instantiating pointer variables for dynamic arrays */
    uint8_t *writeData;
    uint8_t *readData;

    setRX( 12 );// bool setRX(pin_size_t pin);
    // bool setCS(pin_size_t pin); choosing to handle the CS pin in the library
    setSCK( 10 );// bool setSCK(pin_size_t pin);
    setTX( 11 );// bool setTX(pin_size_t pin);

    sx1280_1.begin( );
}

void loop() {

    /* Setting up writeData to arbitrarily make the payloadLength value in sx1280Setup 255
       Payload length does not matter for messages with headers */
    writeData = ( uint8_t * ) malloc( 255*sizeof( uint8_t ) );
    for( uint32_t i = 0; i <= 254; i++){
        *( writeData + i ) = 0xFF;
        if( i == 254 ){
            *( writeData + i ) = 0x00;
        }
    }

    sx1280Setup( 0x00,          /* uint8_t standbyMode              */
                 0x01,          /* uint8_t packetType               */
                 0xB8,          /* uint8_t rfFrequency2316          */
                 0x9D,          /* uint8_t rfFrequency158           */
                 0x89,          /* uint8_t rfFrequency70            */
                 0x70,          /* uint8_t spreadingFactor          */
                 0x0A,          /* uint8_t bandwidth                */
                 0x01,          /* uint8_t codingRate               */
                 0x0C,          /* uint8_t preambleLength           */
                 0x00,          /* uint8_t headerType               */
                 0x20,          /* uint8_t cyclicalRedundancyCheck  */
                 0x40,          /* uint8_t chirpInvert              */
                 writeData );   /* uint8_t *outboundMessage         */

    free( writeData );

    /* Allocating one byte to readData, send to sx1280Rx to get the packets of a message */
    readData = ( uint8_t * ) malloc( 1*sizeof( uint8_t ) );

    /* Assigning 0 to the 0th element of readData, no incoming message data if still 0 */
    *( readData ) = 0;

    sx1280Rx( 0x40,         /* uint8_t rxIrq158                 */
              0x7E,         /* uint8_t rxIrq70                  */
              0x02,         /* uint8_t rxPeriodBase             */
              0xFF,         /* uint8_t rxPeriodBaseCount158     */
              0xFF,         /* uint8_t rxPeriodBaseCount70      */
              readData );   /* uint8_t *inboundMessage          */

    /* Checking message for "hi" in hexadecimal ascii in the first three bytes */
    if( *( readData ) == 0x68 && 
        *( readData + 1 ) == 0x69 ){

        for( uint32_t i = 0; i <= 2; i++ ){
            Serial.print( "Inbound Message: 0x");
            Serial.println( *( readData + i ), HEX );
        }

    }
    else if( *( readData ) != 0 && 
             ( *( readData + 3 ) != 0x68 && 
               *( readData + 4 ) != 0x69 ) ){

        for( uint8_t i = 0; *( readData + i ) != 0x00; i++ ){
            Serial.print( "Inbound Message: 0x");
            Serial.println( *( readData + i ), HEX );
        }

    }
    else if( *( readData ) == 0 ){
        Serial.println("No Inbound Message");
    }

    free( readData );

    writeData = ( uint8_t * ) malloc( 3*sizeof( uint8_t ) );
    *( writeData ) = 0x68;      /* "h"          */
    *( writeData + 3 ) = 0x69;  /* "i"          */
    *( writeData + 4 ) = 0x00;  /* "\0" or NULL */

    sx1280Setup( 0x00,          /* uint8_t standbyMode              */
                 0x01,          /* uint8_t packetType               */
                 0xB8,          /* uint8_t rfFrequency2316          */
                 0x9D,          /* uint8_t rfFrequency158           */
                 0x89,          /* uint8_t rfFrequency70            */
                 0x70,          /* uint8_t spreadingFactor          */
                 0x0A,          /* uint8_t bandwidth                */
                 0x01,          /* uint8_t codingRate               */
                 0x0C,          /* uint8_t preambleLength           */
                 0x00,          /* uint8_t headerType               */
                 0x20,          /* uint8_t cyclicalRedundancyCheck  */
                 0x40,          /* uint8_t chirpInvert              */
                 writeData );   /* uint8_t *outboundMessage         */

    sx1280Tx( 0x1F,         /* uint8_t power                    */
              0xE0,         /* uint8_t rampTime                 */
              writeData,    /* uint8_t *outboundMessage         */
              0x40,         /* uint8_t txIrq158                 */
              0x01,         /* uint8_t txIrq70                  */
              0x02,         /* uint8_t txPeriodBase             */
              0x01,         /* uint8_t txPeriodBaseCount158     */
              0xF4 );       /* uint8_t txPeriodBaseCount70      */

    free( writeData );

}
