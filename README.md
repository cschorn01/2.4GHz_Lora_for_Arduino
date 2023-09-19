# 2.4GHz_LoRa_for_Arduino

[![By Chris Schorn](https://img.shields.io/badge/Author-Chris_Schorn-000000?style=for-the-badge)](https://github.com/cschorn01)
![Status](https://img.shields.io/badge/Status-Untested-000000?style=for-the-badge)
[![Download Repository Zip](https://img.shields.io/badge/Download_Repository_Zip-000000?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/archive/refs/heads/main.zip)

![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
<!-- [![MIT License](https://img.shields.io/badge/License-MIT-A31B34?style=for-the-badge)](https://mit-license.org/)
[![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)
[![Raspberry Pi](https://img.shields.io/badge/-RaspberryPi-C51A4A?style=for-the-badge&logo=Raspberry-Pi)](https://www.raspberrypi.com/products/raspberry-pi-pico/)
[![Semtech LoRa](https://img.shields.io/badge/LoRa-1CAEED?style=for-the-badge)](https://www.semtech.com/lora)
[![FreeRTOS](https://img.shields.io/badge/FreeRTOS-5CBA5B?style=for-the-badge)](https://www.freertos.org/) -->

[![Description](https://img.shields.io/badge/Description-000000?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main#description)
[![Functionality](https://img.shields.io/badge/Functionality-000000?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main#functionality)
[![File Structure](https://img.shields.io/badge/file_structure-000000?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main#file-structure)
[![How to Use](https://img.shields.io/badge/how_to_use-000000?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/tree/main#how-to-use)
[![Fixed Issues](https://img.shields.io/badge/fixed_issues-000000?style=for-the-badge)](https://github.com/cschorn01/2.4GHz_Lora_for_Arduino/blob/main/README.md#fixed-issues)

[![Stargazers repo roster for @cschorn01/4GHz_Lora_for_Arduino](https://reporoster.com/stars/cschorn01/2.4GHz_Lora_for_Arduino)](https://github.com/cschorn01/4GHz_Lora_for_Arduino/stargazers)

<!-- ![GitHub Contributors Image](https://contrib.rocks/image?repo=cschorn01/rpi_pico_lora_template) -->

<!-- [![Top Langs](https://github-readme-stats.vercel.app/api/top-langs/?username=cschorn01&layout=compact&theme=dark)](https://github.com/cschorn01/Lora_Pico_Driver) -->

## Description
This is a library meant for the Arduino IDE. It is based on the code from my [rpi_pico_lora_template](https://github.com/cschorn01/rpi_pico_lora_template/blob/main/README.md), which uses FreeRTOS on the Raspberry Pi Pico to allow for more expandability. However, this library is aimed at hobbyists using either the Pi Pico or any of the other boards supported by the Arduino IDE to expand their long range networking capabilities. The LoRa Modem of choice is the [Semtech sx1280](https://www.semtech.com/products/wireless-rf/lora-connect/sx1280). This gives access to the 2.4GHz spectrum which is useable without a license worldwide, sharing the frequency with [Bluetooth and WiFi](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/44000000MDcO/Ll4bon.4HPwcyXv9fegcfcgbpvLYd7Lx_aZLMzYNLIQ).

## Functionality

[LoRa PHY](https://www.semtech.com/lora/what-is-lora) layer functionality is included in this library for ease of point to point messaging. [Click here to download the sx1280 datasheet.](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/2R000000HoCW/8EVYKPLcthcKCB_cKzApAc6Xf6tAHtn9.UKcOh7SNmg)

## File Structure


## How To Use


## Fixed Issues

<!-- ![Error](https://img.shields.io/badge/Error-A31B34?style=for-the-badge) `arm-none-eabi-gcc: fatal error: cannot read spec file 'nosys.specs': No such file or directory`

![Solution](https://img.shields.io/badge/Solution-5CBA5B?style=for-the-badge) Go to `/usr/bin/local/` delete all files beginning in `arm-none-eabi-` then reinstall the arm toolchain. -->


<div align="center" dir="auto">
  <a href="https://github.com/cschorn01/2.4GHz_Lora_for_Arduino">
    <img src="https://img.shields.io/badge/Back_To_Top-FFFFFF?style=for-the-badge" alt="Back To Top">
  </a>
</div>
