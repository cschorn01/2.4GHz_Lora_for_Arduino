#include <SPI.h>
#include <sx1280OverSpi.h>

sx1280OverSpi sx1280_1( 10,  // uint8_t cssPin
                        8,   // uint8_t busyPin
                        7 ); // uint8_t resetPin 

// Arrays for passing data to and receiving data from sx1280 setup, rx, and tx functions
uint8_t writeData[ 255 ];
uint8_t readData[ 255 ];

uint8_t antselPin = 9; // Setting variable for DLP-RFS1280 antenna select pin

uint32_t i = 0; // iterator

void setup( ) {
    Serial.begin(115200);

    pinMode( LED_BUILTIN, OUTPUT);

    pinMode( antselPin, OUTPUT );
    digitalWrite( antselPin, LOW );

    SPI.begin( );

    sx1280_1.begin( );
}

void loop() {

    digitalWrite( LED_BUILTIN, HIGH );
    delay( 50 );
    digitalWrite( LED_BUILTIN, LOW);

    /* Listening for a packet over the air with the DLP-RFS1280 */

    /* Giving writeData an arbitrary size of 255 for payloadLength in sx1280Setup
       Payload length does not matter for messages with headers */
    for( i = 0; i < 255; i++){
        writeData[ i ] = 0xFF;
    }
    writeData[ 254 ] = 0x00;

    for( i = 0; i < 255; i++){
        readData[ i ] = 0xFF;
    }

    sx1280_1.sx1280Setup( 0x00,          /* uint8_t standbyMode              */
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
                          writeData );   /* uint8_t outboundMessage[ ]       */

    sx1280_1.sx1280Rx( 0x40,         /* uint8_t rxIrq158                 */
                       0x7E,         /* uint8_t rxIrq70                  */
                       0x02,         /* uint8_t rxPeriodBase             */
                       0xFF,         /* uint8_t rxPeriodBaseCount158     */
                       0xFF,         /* uint8_t rxPeriodBaseCount70      */
                       readData );   /* uint8_t inboundMessage[ ]        */

    /* Checking message for "hi", "h"=0x68 & "i"=0x69, in hexadecimal ascii */
    if( readData[ 0 ] == 0x68 && readData[ 1 ] == 0x69 ){

        for( uint32_t i = 0; i <= 2; i++ ){
            Serial.print( "Inbound Message: 0x");
            Serial.print( readData[ i ], HEX );
            Serial.print( " " );
            Serial.println( readData[ i ] );
        }

    }
    else if( readData[ 0 ] == 0xFF ){
        Serial.println("No Inbound Message");
    }

    /* Sending a packet over the air with the DLP-RFS1280 */

    writeData[ 0 ] = 0x68;  /* "h"          */
    writeData[ 1 ] = 0x69;  /* "i"          */
    writeData[ 2 ] = 0x00;  /* "\0" or NULL */
    for( i = 3; i < 255; i++ ){
        writeData[ i ] = 0x00;
    }

    sx1280_1.sx1280Setup( 0x00,          /* uint8_t standbyMode              */
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
                          writeData );   /* uint8_t outboundMessage[ ]       */

    sx1280_1.sx1280Tx( 0x1F,         /* uint8_t power                    */
                       0xE0,         /* uint8_t rampTime                 */
                       writeData,    /* uint8_t outboundMessage[ ]       */
                       0x40,         /* uint8_t txIrq158                 */
                       0x01,         /* uint8_t txIrq70                  */
                       0x02,         /* uint8_t txPeriodBase             */
                       0x01,         /* uint8_t txPeriodBaseCount158     */
                       0xF4 );       /* uint8_t txPeriodBaseCount70      */
}




