#include <OneWire.h>
#include <DallasTemperature.h>
#include "HX711.h"
#include <WiFiNINA.h>
#include <WiFiClient.h>
#include <ArduinoHttpClient.h>
#include <utility/wifi_drv.h>
#include "secrets.h"

// API host
const char* host = "api.thingspeak.com";

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
// wifi variables
WiFiClient wifi;
HttpClient client = HttpClient(wifi, host, 80);

void setupWiFi()
{
  // Set LED to red to indicate it has not connected to WIFI yet
  WiFiDrv::analogWrite(25, 255);
  WiFiDrv::analogWrite(26, 0);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set LED to green to indicate it has successfully connected to WIFI
  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 255);
}

void setup(void)
{
  Serial.begin(9600);

  // Set up on board LEDs
  WiFiDrv::pinMode(25, OUTPUT); //define red pin
  WiFiDrv::pinMode(26, OUTPUT); //define green pin
  WiFiDrv::pinMode(27, OUTPUT); //define blue pin

  // Connect to WiFi
  setupWiFi();

  // start temp sensors
  tempSensor1.begin();
  tempSensor2.begin();

  //start weight sensor
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
}

void loop(void)
{ 
  // get data from sensors 
  getTemperatureData();
  getWeightData();

  //print data to serial monitor for debugging
  Serial.print("Temperature 1: ");
  Serial.print(temperature1);
  Serial.print("C");
  Serial.print("\n");

  Serial.print("Temperature 2: ");
  Serial.print(temperature2);
  Serial.print("C");
  Serial.print("\n");

  Serial.print("Weight: ");
  Serial.print(currentWeight);
  Serial.print(" kg");
  Serial.print("\n");

  sendSensorData();
  //check that the Wifi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    // set LED to RED to indicate that some intervention is needed
    WiFiDrv::analogWrite(25, 255);
    WiFiDrv::analogWrite(26, 0);
  }
  
  // we only need to run the loop every 5 min
  delay(1000*60*5);
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

void sendSensorData() {
  // send data to Matlab API
  String url = String("/update?api_key=") + apiKey +
                      "&field1=" + String(temperature1, 3) +
                      "&field2=" + String(temperature2, 3) +
                      "&field3=" + String(currentWeight, 3);
  client.get(url);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  // if api status code is not good then flash red to indicate data was not sent
  if (statusCode != 200) {
    WiFiDrv::analogWrite(25, 255);
    WiFiDrv::analogWrite(26, 0);
    delay(1000);
    WiFiDrv::analogWrite(25, 0);
    WiFiDrv::analogWrite(26, 255);
  }
}
