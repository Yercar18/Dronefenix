# Dronefenix
- Powered by arduino
[![N|Solid](https://www.arduino.cc/en/uploads/Trademark/ArduinoCommunityLogo.png)](https://arduino.cc)

#Library

 - Added library https://github.com/miguel5612/metereologicStationLib

Dronefenix is an open source project that seeks the implementation of a datalogger on a drone for efficient environmental sampling.

  1. Download the github project.
  2. Install the libraries in the arduino software.
  3. Upload the code to your arduino mega card.
  4. Take off and perform your personal measurements.

# New Features!

  - Add gas sensor ccs 811 to allow detection of contaminants (05/02/2018)

You can also:
  - Obtain measurements of temperature, humidity, atmospheric pressure, heat index, GPS coordinates of the point where these data were measured.
  - Measure the degree of contamination. new!! 

Through this implementation you will be able to quickly observe the status of your crops, constantly monitor a large area, among others. 

### Tech

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


License
----

MIT

