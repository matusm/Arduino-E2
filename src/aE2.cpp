/*
 aE2.cpp - E+E E2 bus library for Arduino
 https://github.com/matusm/Arduino-E2
*/

#include "Arduino.h"
#include "aE2.h"

// definitions
#define DELAY_FACTOR 10 // setup clock-frequency - was 50 in original library
#define ACK 1 // define acknowledge
#define NAK 0 // define not-acknowledge
#define TEMPERATURE_ERROR -300
#define HUMIDITY_ERROR -1
#define UNKNOWN_STRING "???"

// global variables
bool errorFlag;	// set by readByteFromSlave() only
unsigned char lowByte;
unsigned char highByte;

// the constructor
E2Device::E2Device(int pinSDA, int pinSCL)
{
  _pinSDA = pinSDA;
  _pinSCL = pinSCL;
}

/* Private Functions */
/* Arduino speed test: mean time to execute: 10µs */
void E2Device::set_SDA(void) // set port-pin (pinSDA)
{
  pinMode(_pinSDA, OUTPUT);
  digitalWrite(_pinSDA, HIGH);
}
/*-------------------------------------------------------------------------*/
void E2Device::clear_SDA(void) // clear port-pin (pinSDA)
{
  pinMode(_pinSDA, OUTPUT);
  digitalWrite(_pinSDA, LOW);
}
/*-------------------------------------------------------------------------*/
int E2Device::read_SDA(void) // read pinSDA status
{
  pinMode(_pinSDA, INPUT_PULLUP);
  return digitalRead(_pinSDA);
}
/*-------------------------------------------------------------------------*/
void E2Device::set_SCL(void) // set port-pin (pinSCL)
{
  pinMode(_pinSCL, OUTPUT);
  digitalWrite(_pinSCL, HIGH);
}
/*-------------------------------------------------------------------------*/
void E2Device::clear_SCL(void) // clear port-pin (pinSCL)
{
  pinMode(_pinSCL, OUTPUT);
  digitalWrite(_pinSCL, LOW);
}
/*-------------------------------------------------------------------------*/
void E2Device::E2Bus_start(void) // send Start condition to E2 Interface
{
  set_SDA();
  set_SCL();
  e2delay(30*DELAY_FACTOR);
  clear_SDA();
  e2delay(30*DELAY_FACTOR);
}
/*-------------------------------------------------------------------------*/
void E2Device::E2Bus_stop(void) // send Stop condition to E2 Interface
{
  clear_SCL();
  e2delay(20*DELAY_FACTOR);
  clear_SDA();
  e2delay(20*DELAY_FACTOR);
  set_SCL();
  e2delay(20*DELAY_FACTOR);
  set_SDA();
  e2delay(20*DELAY_FACTOR);
}
/*-------------------------------------------------------------------------*/
void E2Device::E2Bus_send(unsigned char value) // send Byte to E2 Interface
{
  unsigned char i;
  unsigned char maske = 0x80;
  for (i=8;i>0;i--)
  {
    clear_SCL();
    e2delay(10*DELAY_FACTOR);
    if ((value & maske) != 0)
    {set_SDA();}
    else
    {clear_SDA();}
    e2delay(20*DELAY_FACTOR);
    set_SCL();
    maske >>= 1;
    e2delay(30*DELAY_FACTOR);
    clear_SCL();
  }
  set_SDA();
}
/*-------------------------------------------------------------------------*/
unsigned char E2Device::E2Bus_read(void) // read Byte from E2 Interface
{
  unsigned char data_in = 0x00;
  unsigned char maske = 0x80;
  for (maske=0x80;maske>0;maske >>=1)
  {
    clear_SCL();
    e2delay(30*DELAY_FACTOR);
    set_SCL();
    e2delay(15*DELAY_FACTOR);
    if (read_SDA())
    {data_in |= maske;}
    e2delay(15*DELAY_FACTOR);
    clear_SCL();
  }
  return data_in;
}
/*-------------------------------------------------------------------------*/
char E2Device::check_ack(void) // check for acknowledge
{
  int input;
  e2delay(30*DELAY_FACTOR);
  set_SCL();
  e2delay(15*DELAY_FACTOR);
  input = read_SDA();
  e2delay(15*DELAY_FACTOR);
  if(input == 1)
    return NAK;
  else
    return ACK;
}
/*-------------------------------------------------------------------------*/
void E2Device::send_ack(void) // send acknowledge
{
  clear_SCL();
  e2delay(15*DELAY_FACTOR);
  clear_SDA();
  e2delay(15*DELAY_FACTOR);
  set_SCL();
  e2delay(30*DELAY_FACTOR);
  clear_SCL();
  set_SDA();
}
/*-------------------------------------------------------------------------*/
void E2Device::send_nak(void) // send NOT-acknowledge
{
  clear_SCL();
  e2delay(15*DELAY_FACTOR);
  set_SDA();
  e2delay(15*DELAY_FACTOR);
  set_SCL();
  e2delay(30*DELAY_FACTOR);
  clear_SCL();
  set_SDA();
}
/*-------------------------------------------------------------------------*/
void E2Device::e2delay(unsigned int value) // e2delay- routine
{
  delayMicroseconds(value);
}
/*-------------------------------------------------------------------------*/
unsigned char E2Device::readByteFromSlave(unsigned char command)
{
  E2Bus_start();
  E2Bus_send(command);
  if (check_ack() == ACK)
  {
    unsigned char byteReadFromSlave = E2Bus_read();
    send_ack();
    unsigned char checksum = E2Bus_read();
    send_nak(); // terminate communication
    E2Bus_stop();
    if (((command + byteReadFromSlave) % 256) == checksum) // checksum OK?
    {
      errorFlag = false;
      return byteReadFromSlave;
    }
  }
  errorFlag = true;
  return 0x00;
}
/*-------------------------------------------------------------------------*/
unsigned int E2Device::combineTwoBytes(unsigned char bl, unsigned char bh)
{
  unsigned int temp = (unsigned int)bl + 256 * (unsigned int)bh;
  return temp;
}
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Public Functions */
/*-------------------------------------------------------------------------*/
float E2Device::getTemperature(void)
{
  lowByte = readByteFromSlave(0xA1);
  if (errorFlag) return TEMPERATURE_ERROR;
  highByte = readByteFromSlave(0xB1);
  if (errorFlag) return TEMPERATURE_ERROR;
  float temperature = ((float)combineTwoBytes(lowByte, highByte) / 100.0) - 273.15;
  return temperature;
}
/*-------------------------------------------------------------------------*/
float E2Device::getHumidity(void)
{
  lowByte = readByteFromSlave(0x81);
  if (errorFlag) return HUMIDITY_ERROR;
  highByte = readByteFromSlave(0x91);
  if (errorFlag) return HUMIDITY_ERROR;
  float humidity = ((float)combineTwoBytes(lowByte, highByte) / 100.0);
  return humidity;
}
/*-------------------------------------------------------------------------*/
unsigned char E2Device::getStatus(void)
{
  unsigned char c = readByteFromSlave(0x71);
  if (errorFlag) return 0xFF;
  return c;
}
/*-------------------------------------------------------------------------*/
String E2Device::getSensorType()
{
  String designation = UNKNOWN_STRING;
  unsigned char gLow = readByteFromSlave(0x11);
  if (errorFlag) return designation;
  unsigned char gHigh = readByteFromSlave(0x41);
  if (errorFlag) return designation;
  unsigned char subG = readByteFromSlave(0x21);
  if (errorFlag) return designation;
  if (gHigh == 0x55)
    gHigh = 0x00; // (?)
  unsigned int groupNumber = combineTwoBytes(gLow, gHigh);
  if(groupNumber <= 9)
    designation = "EE0" + String(groupNumber);
  else
    designation = "EE" + String(groupNumber);
  designation += "-";
  designation += String(subG);
  return designation;
}
/*-------------------------------------------------------------------------*/
// undocumented! may only work with EE-07 sensors!
String E2Device::getSerialNumber()
{
  // magic command
  E2Bus_start();
  E2Bus_send(0x40);
  check_ack();
  E2Bus_send(0x00);
  check_ack();
  E2Bus_send(0x84);
  check_ack();
  E2Bus_send(0xC4);
  check_ack();
  E2Bus_stop();
  // reading 14 characters
  String designation = "";
  for (int i = 0; i < 14; i++)
  {
    designation += (char)readByteFromSlave(0x01);
    if (errorFlag) return UNKNOWN_STRING;
  }
  return designation;
}
/*-------------------------------------------------------------------------*/
// undocumented! may only work with EE-07 sensors!
String E2Device::getFirmware()
{
  // magic command
  E2Bus_start();
  E2Bus_send(0x40);
  check_ack();
  E2Bus_send(0x00);
  check_ack();
  E2Bus_send(0x80);
  check_ack();
  E2Bus_send(0xC0);
  check_ack();
  E2Bus_stop();
  // reading 4 characters
  String designation = "";
  for (int i = 0; i < 4; i++)
  {
    designation += (char)readByteFromSlave(0x01);
    if (i == 1)
      designation += '.';
    if (errorFlag) return UNKNOWN_STRING;
  }
  return designation;
}
/*-------------------------------------------------------------------------*/
// uses undocumented functions
String E2Device::getSensorDescription()
{
  String sensorInfo = getSensorType();
  sensorInfo += "|v";
  sensorInfo += getFirmware();
  sensorInfo += "|";
  sensorInfo += getSerialNumber();
  return sensorInfo;
}
/*-------------------------------------------------------------------------*/

