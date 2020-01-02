/*
 aE2.cpp - E+E E2 bus library for Arduino
 https://github.com/matusm/Arduino-E2
*/

#ifndef AE2_H_INCLUDED
#define AE2_H_INCLUDED
#include"Arduino.h"

class E2Device
{
private:
  int _pinSDA;
  int _pinSCL;
  char check_ack(void);
  void send_ack(void);
  void send_nak(void);
  void E2Bus_start(void); // send start condition
  void E2Bus_stop(void); // send stop condition
  void E2Bus_send(unsigned char);
  void set_SDA(void);
  void clear_SDA(void);
  int read_SDA(void);
  void set_SCL(void);
  void clear_SCL(void);
  unsigned char E2Bus_read(void); // read one byte from E2-Bus
  void e2delay(unsigned int);
  unsigned int combineTwoBytes(unsigned char, unsigned char);
  unsigned char readByteFromSlave(unsigned char);
public:
  E2Device(int, int);
  float getTemperature(void);
  float getHumidity(void);
  unsigned char getStatus(void);
  String getSensorType(void);
  String getSerialNumber(void);
  String getFirmware(void);
  String getSensorDescription(void);
};

#endif
