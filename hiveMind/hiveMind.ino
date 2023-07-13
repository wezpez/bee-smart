#include <OneWire.h>
#include <DallasTemperature.h>
#include "HX711.h"

// define temperature sensor pins 
#define TEMP1_PIN 2
#define TEMP2_PIN 3

// define weight sensor pins and calibration factor
#define LOADCELL_DOUT_PIN 4
#define LOADCELL_SCK_PIN 5
#define calibration_factor -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

// Setup oneWire and DallasTemperature instances for each temperature sensor
OneWire oneWire1(TEMP1_PIN);
DallasTemperature tempSensor1(&oneWire1);

OneWire oneWire2(TEMP2_PIN);
DallasTemperature tempSensor2(&oneWire2);

// global variables
float temperature1 = 0;
float temperature2 = 0;
float currentWeight = 0;
HX711 scale;

void setup(void)
{
  // start temp sensors
  tempSensor1.begin();
  tempSensor2.begin();

  //start weight sensor
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0


  Serial.begin(9600);
}

void loop(void)
{ 
  // get data from sensors 
  getTemperatureData();
  getWeightData();

  //print data to serial monitor for debugging
  Serial.print("Temperature 1: ");
  Serial.print(temperature1);
  Serial.print((char)176);//shows degrees character
  Serial.print("C  |  ");
  Serial.print("\n");

  Serial.print("Temperature 2: ");
  Serial.print(temperature2);
  Serial.print((char)176);//shows degrees character
  Serial.print("C  |  ");
  Serial.print("\n");

  Serial.print("Weight: ");
  Serial.print(currentWeight);
  Serial.print(" kg");
  Serial.print("\n");
  
  delay(5000);
}

void getTemperatureData() {
  tempSensor1.requestTemperatures();
  temperature1 = tempSensor1.getTempCByIndex(0);

  tempSensor2.requestTemperatures();
  temperature2 = tempSensor2.getTempCByIndex(0);
}

void getWeightData() {
  currentWeight = 0.454 * scale.get_units(); // convert lbs to kgs
}
