# 2.4GHz_LoRa_for_Arduino
## Curently Untested because I don't own a working Arduino lol, will get there

This is a library meant for the Arduino IDE. It is based on the code from my [Lora_Pico_Driver](https://github.com/cschorn01/Lora_Pico_Driver), which uses FreeRTOS on the Raspberry Pi Pico to allow for more expandability. However, this library is aimed at hobbyists using either the Pi Pico or any of the other boards supported by the Arduino IDE to expand their long range networking capabilities. The LoRa Modem of choice is the [Semtech sx1280](https://www.semtech.com/products/wireless-rf/lora-connect/sx1280). This gives access to the 2.4GHz spectrum which is useable without a license worldwide, sharing the frequency with [Bluetooth and WiFi](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/44000000MDcO/Ll4bon.4HPwcyXv9fegcfcgbpvLYd7Lx_aZLMzYNLIQ).

##Features
[LoRa PHY](https://www.semtech.com/lora/what-is-lora) layer functionality is included in this library for ease of point to point messaging. [Click here to download the sx1280 datasheet.](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/2R000000HoCW/8EVYKPLcthcKCB_cKzApAc6Xf6tAHtn9.UKcOh7SNmg)
