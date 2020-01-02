aE2 (Arduino-E2)
=============

A simple Arduino E2 bus library.

## E2Device
This library allows you to communicate with sensor devices using the proprietary E2 bus protocol. This protocol is used by sensors from the manufacturer [E + E Elektronik Ges.m.b.H](https://www.epluse.com) and is quite similar to the I2C bus. It is possible to control devices with two general-purpose I/O pins.

This bit-banging library is based on a similar software by <romain.bazile@gromain.me> hosted at <https://github.com/ubiant/E2>. The key differences are subsumized in the notes below. The original library was  never released.

## Key features

* Clean API
* Sensor identification possible 
* No additional dependencies (except Arduino.h)

### Constructor

* `E2Device(int SDA, int SCL)`
Creates a new instance of this class taking numbers of two GPIO pins (for data and clock line, respectively) as parameters.

### Methods (Getters)

* `getTemperature()`
  Returns the temperature reading in °C as float. On error `-300` is returned.

* `getHumidity()`
  Returns the relative humidity in % as float. On error  `-1` is returned.

* `getStatus()`
  Returns the status of the sensor as a byte. On error `0xFF`  is returned. 

* `getSensorType()`
  Returns the type designation of the sensor as String. On error `"???"` is returned.

* `getSerialNumber()`
  Returns the serial number of the sensor as String. On error `"???"` is returned.

* `getFirmware()`
  Returns the firmware version of the sensor as String. On error `"???"` is returned.

* `getSensorDescription()`
  Returns a full description of the sensor. This function is a combination of the three preceeding getters.

### Notes

The library was tested with different versions of the E+E EE-07 temperature and humidity sensor. Some methods make use of undocumented calls and my not work with each sensor type.

Support of CO2 sensors and on-sensor memory modification was removed from this version.

### Usage

Consult the provided example which show the use of this class.

