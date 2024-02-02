/* Author: Chris Schorn
   Version: 1.0.0
   Description: Arduino library example for the Raspberry Pi Pico to interface with an 
                sx1280, or 2.4GHz LoRa Module. The module I have chosen is a
                DLP-RFS1280 from DLP-Design.

   Useful Links: https://arduino-pico.readthedocs.io/en/latest/spi.html
                 https://www.arduino.cc/reference/en/language/functions/communication/spi/
*/

#include <sx1280OverSpi.h>
#include <SPI.h>

sx1280OverSpi sx1280_1( 13,   // uint8_t cssPin
                        22,   // uint8_t busyPin
                        21 ); // uint8_t resetPin 

// Arrays for passing data to and receiving data from sx1280 setup, rx, and tx functions
uint8_t writeData[ 255 ];
uint8_t readData[ 255 ]

uint8_t LED_PIN = 25; // Setting variable for the Rpi Pico onboard LED

uint8_t antselPin = 28; // Setting variable for DLP-RFS1280 antenna select pin

uint32_t i = 0; // iterator

void setup( ) {

    pinMode( LED_PIN, OUTPUT);

    pinMode( antselPin, OUTPUT );
    digitalWrite( antselPin, LOW );

    // bool setCS(pin_size_t pin); choosing to handle the CS pin in the library
    SPI.setSCK( 10 ); // bool setSCK(pin_size_t pin);
    SPI.setTX( 11 );  // bool setTX(pin_size_t pin);
    SPI.setRX( 12 );  // bool setRX(pin_size_t pin);
    SPI.begin( );

    sx1280_1.begin( );
}

void loop() {

    digitalWrite( 25, HIGH );
    delay( 50 );
    digitalWrite( 25, LOW);

    /* Giving writeData an arbitrary size of 255 for payloadLength in sx1280Setup
       Payload length does not matter for messages with headers */
    for( i = 0; i < 255; i++){
        *( writeData + i ) = 0xFF;
    }
    writeData[ 254 ] = 0x00; // Ending the array with a NULL for a NULL terminated string

    for( i = 0; i < 255; i++){
        *( readData + i ) = 0xFF;
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

    /* Checking message for "hi" in hexadecimal ascii in the first three bytes */
    if( readData[ 0 ] == 0x68 && readData[ 1 ] == 0x69 ){

        for( uint32_t i = 0; i <= 2; i++ ){
            Serial.print( "Inbound Message: 0x");
            Serial.println( readData[ i ], HEX );
        }

    }
    else if( readData[ 0 ] != 0 && ( readData[ 3 ] != 0x68 && readData[ 4 ] != 0x69 ) ){

        for( uint8_t i = 0; readData[ i ] != 0x00; i++ ){
            Serial.print( "Inbound Message: 0x");
            Serial.println( readData[ i ], HEX );
        }

    }
    else if( readData[ 0 ] == 0 ){
        Serial.println("No Inbound Message");
    }

    writeData[ 0 ] = 0x68;      /* "h"          */
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


