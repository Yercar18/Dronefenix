<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
![Build Status][build-url]
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

# Dronefenix
- Powered by arduino
[![N|Solid](https://www.arduino.cc/en/uploads/Trademark/ArduinoCommunityLogo.png)](https://arduino.cc)


<!-- TABLE OF CONTENTS -->
## Table of Contents

* [Getting Started](#Getting-Started)
* [List of Materials](#Materials)
* [Authors](#TEAM)
* [Partners](#Partners)

# Getting Started

The steps to do this project is:

1. Adquire the materials (Includes the PCB or breadboard and wires).

2. Assembly the station [view schematic here](https://easyeda.com/miguelangelcu/dron_fenix) or [PCB]().

4. Install the [libraries](https://github.com/Yercar18/Dronefenix/tree/master/Librerias) [tutorial](https://www.youtube.com/watch?v=jMSic83Prs8).

3. Program the [arduino pro mini](https://github.com/Yercar18/Dronefenix/tree/master/airQ_Arduino) and [wemos d1](https://github.com/Yercar18/Dronefenix/tree/master/AirQ_Wemos).

5. Create the account on the [onmotica site](www.onmotica.com).

6. Have Fun, give us a start and share this project on your social networks, thanks!.

# Materials
| Name | Provider | Pricing (COP) |
| ------------- | ------------- |  ------------- |
| Arduino Nano V3 | [vistronica](https://www.vistronica.com/board-de-desarrollo/arduino/board/arduino-pro-mini-328-5v-16mhz-detail.html) | $ 10.782 |
| Wemos D1 Mini | [didacticas electronicas](https://www.didacticaselectronicas.com/index.php/comunicaciones/wi-fi/tarjeta-wemos-d1-mini-wifi-esp8266-wemosd1mini-wifi-internet-iot-detail) | $ 17.400 |
| SD Card Shield for Wemos | [didacticas electronicas](https://www.didacticaselectronicas.com/index.php/suiches-y-conectores/holders/shield-micro-sd-card-para-wemos-mini-d1-usd-wemos-sh-usd-holder-detail) | $ 7.000 |
| ASM1117 - 5V | [mercadolibre](https://articulo.mercadolibre.com.co/MCO-452253545-regulador-voltaje-5voltios-ams1117-superficial-arduino-_JM#position=12&type=item&tracking_id=e6f385fa-6939-45f6-9d87-42d85eede402) | $ 2.000 |
| ASM1117 - 3V3 | [mercadolibre](https://articulo.mercadolibre.com.co/MCO-523150391-10x-regulador-ams1117-33v-smd-_JM?quantity=1#position=4&type=item&tracking_id=08078b22-9c8c-4f64-8ddc-8c076cf0a255) | $ 6.900 |
| Capacitor 470 uF-16V | [vistronica](https://www.vistronica.com/componentes-pasivos/capacitores/capacitor-electrolitico-de-aluminio-470uf-16v-8x12-mm-detail.html) | $ 193 |
| SMD LED | [vistronica](https://articulo.mercadolibre.com.co/MCO-533066032-diodos-led-smd-3535-6v-lg-_JM?quantity=1#position=1&type=item&tracking_id=b9116a7f-46ef-42b5-8bf0-341ca223466c) | $ 1.500 |
| SMD Resistor | [vistronica](https://www.vistronica.com/componentes-pasivos/resistencias/resistencias-4-7-kohm-smd-1206-14w-5-x10-detail.html) | $ 340 |
| Serial Level Shifter | [vistronica](https://www.vistronica.com/comunicaciones/serial/modulo-conversor-de-nivel-logico-de-5v-a-3-3v-3-3v-a-5v-detail.html) | $ 2.056 |
| I2C Level Shifter | [vistronica](https://www.didacticaselectronicas.com/index.php/semiconductores/drivers-y-conversores/convertidor-de-nivel-l%C3%B3gico-bidireccional-pca9306-conversor-convertidor-conversor-de-voltaje-nivel-l%C3%B3gico-bidireccional-pca9306-sparkfun-detail) | $ 30.200 |
| CCS 811 | [vistronica](https://www.didacticaselectronicas.com/index.php/sensores/ambientales/sensor-de-calidad-del-aire-sensor-calidad-de-aire,-ccs811,-ntc-co2-eco2-tvoc-monoxiodo-de-carbono-interiores-detail) | $58.000 |
| GPS Neo 6M | [vistronica](https://www.vistronica.com/comunicaciones/modulo-gps-gy-neo6mv2-con-memoria-eeprom-detail.html) | $ 39.000 |
| DHT 11 | [vistronica](https://www.vistronica.com/sensores/modulo-sensor-de-temperatura-y-humedad-dht11-detail.html) | $ 5.454 |
| MQ-135 | [vistronica](https://www.didacticaselectronicas.com/index.php/sensores/gases/modulo-sensor-de-gas-mq-135-sensor-de-gas-gases-benzeno-amoniaco-dioxido-de-carbono-nh3-co2-mq135-mq-135-detail) | $ 9.000 |
| MQ-4 | [vistronica](https://www.didacticaselectronicas.com/index.php/sensores/gases/modulo-sensor-para-gas-metano-cng-mq-4-sensor-gas-gases-metano-mq4-mq-4-seeed-studio-detail) | $ 7.800 |
| BMP-180 | [vistronica](https://www.didacticaselectronicas.com/index.php/sensores/presion-atm/sensor-de-presion-atmosferica-bmp180-presion-relativa-barometro-bmp180-detail) | $ 9.600 |


## library required before install 
- [Adrafruit sensor](https://github.com/adafruit/Adafruit_Sensor)
- [Adafruit_CCS811](https://github.com/adafruit/Adafruit_CCS811)
- [SFE_BMP180](https://github.com/sparkfun/BMP180_Breakout)
- [TinyGPS](https://github.com/mikalhart/TinyGPS)
- [DHT](https://github.com/adafruit/DHT-sensor-library)
Note: 
3.0 version has all libs in library folder.


Dronefenix is an open source project that seeks the implementation of a datalogger on a drone for efficient environmental sampling.

  1. Download the github project.
  2. Install the libraries in the arduino software.
  3. Upload the code to your arduino mega card.
  4. Take off and perform your personal measurements.

# New Features!

  - Updated, now are using MQSensor lib 2.0.1

  - Add gas sensor ccs 811 to allow detection of contaminants (05/02/2018)

You can also:
  - Obtain measurements of temperature, humidity, atmospheric pressure, heat index, GPS coordinates of the point where these data were measured.
  - Measure the degree of contamination. new!! 

Through this implementation you will be able to quickly observe the status of your crops, constantly monitor a large area, among others. 

### Third-party librarys

Dronefenix uses a number of open source projects to work properly:

* [TinyGPS] - Excelent for gps Apps!
* [DHT-sensor-library] - From adafruit.com to read DHT11.
* [BMP180_Breakout] - A great librarie to read BMP180 Sensor.
* [Adafruit_CCS811] - Best librarie for CCS811 Sensor.

And of course Dronefenix itself is open source with a https://github.com/Yercar18/Dronefenix/

### Installation

Dronefenix requires [Arduino ide](https://arduino.cc/)  to run.

Install the libraries.


To install a new library into your Arduino IDE you can use the Library Manager (available from IDE version 1.6.2).
Open the IDE and click to the "Sketch" menu and then Include Library > Manage Libraries.

# Autors
* **Miguel A. Califa U.** - [*GitHub*](https://github.com/miguel5612) - [CV](https://scienti.colciencias.gov.co/cvlac/visualizador/generarCurriculoCv.do?cod_rh=0000050477)
* **Yersson Ramiro Carrillo Amado** - [*GitHub*](https://github.com/yercar18) - [CV](https://scienti.colciencias.gov.co/cvlac/visualizador/generarCurriculoCv.do?cod_rh=0001637655)


# License
----

MIT

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/Yercar18/Dronefenix.svg?style=flat-square
[contributors-url]: https://github.com/Yercar18/Dronefenix/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/Yercar18/Dronefenix.svg?style=flat-square
[forks-url]: https://github.com/Yercar18/Dronefenix/network/members
[stars-shield]: https://img.shields.io/github/stars/Yercar18/Dronefenix.svg?style=flat-square
[stars-url]: https://github.com/Yercar18/Dronefenix/stargazers
[issues-shield]: https://img.shields.io/github/issues/Yercar18/Dronefenix.svg?style=flat-square
[issues-url]: https://github.com/Yercar18/Dronefenix/issues
[license-shield]: https://img.shields.io/github/license/Yercar18/Dronefenix.svg?style=flat-square
[license-url]: https://github.com/Yercar18/Dronefenix/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[build-url]: https://travis-ci.org/dwyl/esta.svg?branch=master
[linkedin-url]: https://www.linkedin.com/in/miguel5612
[product-screenshot]: images/screenshot.png
