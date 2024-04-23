/* Author: Chris Schorn
   Version: 1.0.0
   Description: Arduino library example for the Arduino Uno to interface with an 
                sx1280, or 2.4GHz LoRa Module, called the DLP-RFS1280.
*/

#include <SPI.h>
#include <sx1280OverSpi.h>

uint32_t i = 0; // iterator

// Arrays for passing data to and receiving data from sx1280 setup, rx, and tx functions
// Sized to 255 because an sx1280 Message Buffer is 255 bytes
uint8_t sx1280Data[ 255 ];

uint8_t antselPin = 9; // Setting variable for DLP-RFS1280 antenna select pin

uint8_t cssPin = 10;
uint8_t busyPin = 8;
uint8_t resetPin = 7;

sx1280OverSpi sx1280_1( cssPin,
                        busyPin,
                        resetPin );

void setup( ) {
    Serial.begin(115200);

    pinMode( LED_BUILTIN, OUTPUT);

    pinMode( antselPin, OUTPUT );
    digitalWrite( antselPin, LOW ); // Setting Antenna Select to onboard one

    SPI.begin( );

    sx1280_1.begin( );
}

void loop() {

    digitalWrite( LED_BUILTIN, HIGH );
    delay( 50 );
    digitalWrite( LED_BUILTIN, LOW);

    /* Giving writeData an arbitrary size of 255 for payloadLength in sx1280Setup
       Payload length does not matter for messages with headers */
    for( i = 0; i < 255; i++){
        sx1280Data[ i ] = 0xFF;
    }
    sx1280Data[ 254 ] = 0x00; // Null Terminating the ascii array

    sx1280_1.sx1280Setup( 0x00,          /* uint8_t standbyMode              */
                          0x01,          /* uint8_t packetType               */
                          0xB8,          /* uint8_t rfFrequency[23:16]       */
                          0x9D,          /* uint8_t rfFrequency[15:8]        */
                          0x89,          /* uint8_t rfFrequency[7:0]         */
                          0x70,          /* uint8_t spreadingFactor          */
                          0x0A,          /* uint8_t bandwidth                */
                          0x01,          /* uint8_t codingRate               */
                          0x0C,          /* uint8_t preambleLength           */
                          0x00,          /* uint8_t headerType               */
                          0x20,          /* uint8_t cyclicalRedundancyCheck  */
                          0x40,          /* uint8_t chirpInvert              */
                          sx1280Data );  /* uint8_t outboundMessage[ ]      */

    sx1280_1.sx1280Rx( 0x40,         /* uint8_t rxIrq158                 */
                       0x7E,         /* uint8_t rxIrq70                  */
                       0x02,         /* uint8_t rxPeriodBase             */
                       0xFF,         /* uint8_t rxPeriodBaseCount[15:8]  */
                       0xFF,         /* uint8_t rxPeriodBaseCount[7:0]   */
                       sx1280Data ); /* uint8_t inboundMessage[ ]      */

    /* Checking message for "hi", "h"=0x68 & "i"=0x69, in hexadecimal ascii in the first three bytes */
    if( sx1280Data[ 0 ] == 0xFF && sx1280Data[ 1 ] == 0xFF &&sx1280Data[ 253 ] == 0xFF ){
            Serial.println("No Inbound Message");
    }
    else{
        for( i = 0; i < 255; i++ ){
            Serial.print( "Inbound Message: 0x");
            Serial.print( sx1280Data[ i ], HEX );

            /* Uncomment to shorten printed message */
            /* if( (sx1280Data[ i ] == 0xFF && sx1280Data[ i - 1 ] == 0xFF && sx1280Data[ i + 1 ] == 0xFF ) || sx1280Data[ i ] == 0x00 ){
                break;
            } */
        }
    }

    sx1280Data[ 0 ] = 0x68;  /* "h"          */
    sx1280Data[ 1 ] = 0x69;  /* "i"          */
    sx1280Data[ 2 ] = 0x00;  /* "\0" or NULL */
    for( i = 3; i < 255; i++ ){
        sx1280Data[ i ] = 0x00;
    }

    sx1280_1.sx1280Setup( 0x00,          /* uint8_t standbyMode              */
                          0x01,          /* uint8_t packetType               */
                          0xB8,          /* uint8_t rfFrequency[23:16]       */
                          0x9D,          /* uint8_t rfFrequency[15:8]        */
                          0x89,          /* uint8_t rfFrequency[7:0]         */
                          0x70,          /* uint8_t spreadingFactor          */
                          0x0A,          /* uint8_t bandwidth                */
                          0x01,          /* uint8_t codingRate               */
                          0x0C,          /* uint8_t preambleLength           */
                          0x00,          /* uint8_t headerType               */
                          0x20,          /* uint8_t cyclicalRedundancyCheck  */
                          0x40,          /* uint8_t chirpInvert              */
                          sx1280Data );  /* uint8_t outboundMessage[ ]      */

    sx1280_1.sx1280Tx( 0x1F,         /* uint8_t power                    */
                       0xE0,         /* uint8_t rampTime                 */
                       sx1280Data,   /* uint8_t outboundMessage[ ]       */
                       0x40,         /* uint8_t txIrq[15:8]              */
                       0x01,         /* uint8_t txIrq[7:0]               */
                       0x02,         /* uint8_t txPeriodBase             */
                       0x01,         /* uint8_t txPeriodBaseCount158     */
                       0xF4 );       /* uint8_t txPeriodBaseCount70      */
}


