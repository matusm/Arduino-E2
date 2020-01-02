#include <aE2.h>

E2Device ee07(5, 6); //SDA=5 SCL=6

float humidity;
float temperature;
unsigned char state;
String description;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  description = ee07.getSensorDescription();
  humidity = ee07.getHumidity();
  temperature = ee07.getTemperature();
  ee07.getStatus(); // to start a new measurement

  Serial.print("Sensor description: ");
  Serial.println(description);
  Serial.print("Temperature: ");
  Serial.println(temperature, 2);
  Serial.print("Humidity: ");
  Serial.println(humidity, 1);
  Serial.println("-----------------------------------");

  delay(1000); // wait 1 s
}
