# 2.4GHz_LoRa_for_Arduino

[![By Chris Schorn](https://img.shields.io/badge/Author-Chris_Schorn-FFFFFF?style=for-the-badge)](https://github.com/cschorn01)
![Status](https://img.shields.io/badge/Status-Working-FFFFFF?style=for-the-badge)
[![Download Repository Zip](https://img.shields.io/badge/Download_Repository_Zip-FFFFFF?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/archive/refs/heads/main.zip)

[![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)](https://www.arduino.cc/)

[![Description](https://img.shields.io/badge/Description-FFFFFF?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main#description)
[![Functionality](https://img.shields.io/badge/Functionality-FFFFFF?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main#functionality)
[![File Structure](https://img.shields.io/badge/file_structure-FFFFFF?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main#file-structure)
[![How to Use](https://img.shields.io/badge/how_to_use-FFFFFF?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main#how-to-use)
[![Issues](https://img.shields.io/badge/issues-FFFFFF?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/blob/main/README.md#issues)

[![Stargazers repo roster for @cschorn01/4GHz_Lora_for_Arduino](https://reporoster.com/stars/cschorn01/2.4GHz_Lora_for_Arduino)](https://github.com/cschorn01/4GHz_Lora_for_Arduino/stargazers)

<!-- ![GitHub Contributors Image](https://contrib.rocks/image?repo=cschorn01/rpi_pico_lora_template) -->

<!-- [![Top Langs](https://github-readme-stats.vercel.app/api/top-langs/?username=cschorn01&layout=compact&theme=dark)](https://github.com/cschorn01/Lora_Pico_Driver) -->

## Description
<!-- This is a library meant for the Arduino IDE. It is based on the code from my [rpi_pico_lora_template](https://github.com/cschorn01/rpi_pico_lora_template/blob/main/README.md), which uses FreeRTOS on the Raspberry Pi Pico to allow for more expandability. However, this library is aimed at hobbyists using either the Pi Pico or any of the other boards supported by the Arduino IDE to expand their long range networking capabilities. The LoRa Modem of choice is the [Semtech sx1280](https://www.semtech.com/products/wireless-rf/lora-connect/sx1280). This gives access to the 2.4GHz spectrum which is useable without a license worldwide, sharing the frequency with [Bluetooth and WiFi](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/44000000MDcO/Ll4bon.4HPwcyXv9fegcfcgbpvLYd7Lx_aZLMzYNLIQ). -->

This Arduino library alows for streamlined use of the [Semtech sx1280 LoRa Radio](https://www.semtech.com/products/wireless-rf/lora-connect/sx1280) over SPI communication for point to point messaging. Meant to enable hobbyists worldwide by allowing long range radio transmission without license on the 2.4 GHz frequency.

## Functionality

Three functions are made available to use:

1. `sx1280Setup` prepares the sx1280 Lora module for message transmission or reception by setting the message and buffer parameters. 
2. `sx1280Tx` sets the sx1280 module parameters for  message transmission, and the IRQ, which will alert the Arduino when the message is sent. Once a message is transmitted the IRQ will read `0x01`.
3. `sx1280Rx` sets the sx1280 module parameters for message reception, and the IRQ, which will alert the Arduino if a message is received. If a message is received this function will fill the array passed to it with the message data.

<!-- [Click here to download the sx1280 datasheet.](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/2R000000HoCW/8EVYKPLcthcKCB_cKzApAc6Xf6tAHtn9.UKcOh7SNmg) -->

## File Structure

- :file_folder: [examples](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main/examples)
- :file_folder: [src](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main/src) 
  - :page_facing_up: [sx1280OverSpi.cpp](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/blob/main/src/sx1280OverSpi.cpp)  
  - :page_facing_up: [sx1280OverSpi.h](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/blob/main/src/sx1280OverSpi.h)
- :page_facing_up: [README.md](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/blob/main/README.md)  
- :page_facing_up: [keywords.txt](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/blob/main/keywords.txt)
- :page_facing_up: [library.properties](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/blob/main/library.properties)

## How To Use

Each of the functions contain print outs for the serial monitor. In `sx1280Setup` you will only see the busy status of the sx1280. `sx1280Rx` prints its *Listening* loop iteration, then breaks and fills the arrays passed in with any received message data. If there was any message data received then it is printed in `void loop`.  `sx1280Tx` prints the IRQ, which if it equals `0x01` the message is sent and the function is exited. If you want to input your own messages you can change the lines which fill the writeData array with 'hi', to input over the serial monitor, which will be added in the examples soon.

## Issues

<!-- ![Error](https://img.shields.io/badge/Error-A31B34?style=for-the-badge) `arm-none-eabi-gcc: fatal error: cannot read spec file 'nosys.specs': No such file or directory`

![Solution](https://img.shields.io/badge/Solution-5CBA5B?style=for-the-badge) Go to `/usr/bin/local/` delete all files beginning in `arm-none-eabi-` then reinstall the arm toolchain. -->


<div align="center" dir="auto">
  <a href="https://github.com/cschorn01/2.4GHz_Lora_for_Arduino">
    <img src="https://img.shields.io/badge/Back_To_Top-FFFFFF?style=for-the-badge" alt="Back To Top">
  </a>
</div>
