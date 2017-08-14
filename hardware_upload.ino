#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_NeoPixel.h>
#include <ThingSpeak.h>
#include <EthernetClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Tunable global variables
char ssid[] = "449Group1"; //  Your network SSID (name)
char pass[] = "wedemboyz"; // Your network password
const unsigned long postingInterval = 30L * 1000L; // Post data every 100 seconds (don't go over 100)
const unsigned long updateInterval = 3L * 1000L; // Update once every 15 seconds (don't go below 10)
const int checkTalkBackInterval = 10 * 1000;    // Time interval in milliseconds to check TalkBack (number of seconds * 1000 = interval)
const int submergeTime = 180 * 1000; // Time for device to be submerged in water before starting readings
int sensorA = 13;  // digital pin 13 powers sensor A
int sensorB = 15;  // digital pin 15 powers sensor B
int LEDpin = 12;   // digital pin 12 powers the LED

// Initialize 16-bit ADC
Adafruit_ADS1115 ads;

// Initialize the jsonBuffer to hold data
char jsonBuffer[500] = "[";

// Initialize clock variables
unsigned long lastConnectionTime = 0; // Track the last connection time to ThingSpeak channel
unsigned long lastUpdateTime = 0; // Track the last update time

// ThingSpeak Server
char server[] = "api.thingspeak.com";
String thingSpeakAPI = "api.thingspeak.com";

// Initialize sensors & variables
int SensorActivePin = sensorA; // Activate sensor A to begin with
boolean switchSensor = true; // True when first 2 data points need to be skipped
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
uint16_t r, g, b, c, lux = 0;
int16_t voltageReading;
float multiplier = 0.1875F; // ADS1115  @ +/- 6.144V gain (16-bit results) //

// Initialize LED
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1,LEDpin);

void setup() {
  Serial.begin(115200);

  // Start LED
  strip.begin();
  strip.setPixelColor(0,255,255,255,0);
  strip.show();

  // Start ADC
  ads.begin();

  // Start RGB sensors
  pinMode(sensorA, OUTPUT);  // set pin to power sensorA
  pinMode(sensorB, OUTPUT);  // set pin to power sensorB
  digitalWrite(sensorB, LOW);
  digitalWrite(sensorA, HIGH);

  // Attempt to connect to wifi
  attemptWifi();

  // Check TalkBack for start signal
  checkTalkBack();
}

void loop() {
  // If halfway through updates, switch sensors
  if (((millis()- lastConnectionTime) >=  postingInterval/2) && (SensorActivePin == sensorA)) {
    sensorSwitch();
  }

  // If update time has reached 15 seconds, then update the jsonBuffer
  if (millis() - lastUpdateTime >=  updateInterval) {
    updatesJson(jsonBuffer);
  }
}

// Updates the jsonBuffer with data
void updatesJson(char* jsonBuffer) {
  /* JSON format for updates paramter in the API
   *  This example uses the relative timestamp as it uses the "delta_t". If your device has a real-time clock, you can provide the absolute timestamp using the "created_at" parameter
   *  instead of "delta_t".
   *  "[{\"delta_t\":0,\"field1\":-70},{\"delta_t\":15,\"field1\":-66}]"
   */

  // Reinitialize sensor every time function is run
  Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

  // If reading from the sensor for the first time since a switch, skip the first two readings
  if (switchSensor == true) {
    for(int i = 0; i < 2; i++){
      tcs.getRawData(&r, &g, &b, &c);
      delay(200);
    }
    switchSensor == false;
  }

  // Get sensor readings
  tcs.getRawData(&r, &g, &b, &c);
  lux = tcs.calculateLux(r, g, b);

  Serial.print(lux, DEC); Serial.print(" ");
  Serial.print(r, DEC); Serial.print(" ");
  Serial.print(g, DEC); Serial.print(" ");
  Serial.print(b, DEC); Serial.println(" ");

  // Get voltage reading
  voltageReading = ads.readADC_Differential_0_1();
  int voltage = voltageReading * multiplier;
  Serial.println(voltage);

  // Initialize variables for jsonBuffer
  char temp[4];
  unsigned long deltaT = (millis() - lastUpdateTime)/1000;
  size_t lengthT = String(deltaT).length();

  // If sensorA is active, add to fields 1-4,8
  if (SensorActivePin == sensorA) {
    strcat(jsonBuffer,"{\"delta_t\":");
    String(deltaT).toCharArray(temp,lengthT+1);
    strcat(jsonBuffer,temp);

//    //Add R to JsonBuffer
//    strcat(jsonBuffer,",");
//    strcat(jsonBuffer, "\"field1\":");
//    lengthT = String(r).length();
//    String(r).toCharArray(temp,lengthT+1);
//    strcat(jsonBuffer,temp);
//
//    //Add G to JsonBuffer
//    strcat(jsonBuffer,",");
//    strcat(jsonBuffer, "\"field2\":");
//    lengthT = String(g).length();
//    String(g).toCharArray(temp,lengthT+1);
//    strcat(jsonBuffer,temp);
//
//    //Add B to JsonBuffer
//    strcat(jsonBuffer,",");
//    strcat(jsonBuffer, "\"field3\":");
//    lengthT = String(b).length();
//    String(b).toCharArray(temp,lengthT+1);
//    strcat(jsonBuffer,temp);

    //Add Lux to JsonBuffer
    strcat(jsonBuffer,",");
    strcat(jsonBuffer, "\"field4\":");
    lengthT = String(lux).length();
    String(lux).toCharArray(temp,lengthT+1);
    strcat(jsonBuffer,temp);

    //Add Voltage to JsonBuffer
    strcat(jsonBuffer,",");
    strcat(jsonBuffer, "\"field8\":");
    lengthT = String(voltage).length();
    String(voltage).toCharArray(temp,lengthT+1);
    strcat(jsonBuffer,temp);

    strcat(jsonBuffer,"},");
  }
  // If sensorB is active, add to fields 5-8
  else {
    strcat(jsonBuffer,"{\"delta_t\":");
    String(deltaT).toCharArray(temp,lengthT+1);
    strcat(jsonBuffer,temp);

//    //Add R2 to JsonBuffer
//    strcat(jsonBuffer,",");
//    strcat(jsonBuffer, "\"field5\":");
//    lengthT = String(r).length();
//    String(r).toCharArray(temp,lengthT+1);
//    strcat(jsonBuffer,temp);
//
//    //Add G2 to JsonBuffer
//    strcat(jsonBuffer,",");
//    strcat(jsonBuffer, "\"field6\":");
//    lengthT = String(g).length();
//    String(g).toCharArray(temp,lengthT+1);
//    strcat(jsonBuffer,temp);
//
//    //Add B2 to JsonBuffer
//    strcat(jsonBuffer,",");
//    strcat(jsonBuffer, "\"field7\":");
//    lengthT = String(b).length();
//    String(b).toCharArray(temp,lengthT+1);
//    strcat(jsonBuffer,temp);

    //Add Voltage to JsonBuffer
    strcat(jsonBuffer,",");
    strcat(jsonBuffer, "\"field8\":");
    lengthT = String(voltage).length();
    String(voltage).toCharArray(temp,lengthT+1);
    strcat(jsonBuffer,temp);

    strcat(jsonBuffer,"},");
  }

  // Update the last update time
  lastUpdateTime = millis();

  // If 100 seconds has passed, get ready to send data and reset
  if ((millis() - lastConnectionTime) >=  postingInterval) {
    attemptWifi(); // Connect to WiFi
    size_t len = strlen(jsonBuffer);
    jsonBuffer[len-1] = ']';
    httpRequest(jsonBuffer); // Send data to ThingSpeak
    sensorSwitch(); // Switch active sensor back to sensorA
    checkTalkBack(); // Wait for next start signal
  }
}

// Switch active sensor
void sensorSwitch(){
  Serial.println("Sensor switch");
  switchSensor = true;
  digitalWrite(SensorActivePin, LOW);
  if (SensorActivePin == sensorA) {
    SensorActivePin = sensorB;
  }
  else {
    SensorActivePin = sensorA;
  }
  digitalWrite(SensorActivePin, HIGH);
  delay(5000);
}

// Check TalkBack for start signal
void checkTalkBack() {
  HTTPClient http;
  String talkBackCommand;

  // Continue running infinitely until signal is received
  while (1) {
    char charIn;
    String talkBackURL =  "http://" + thingSpeakAPI + "/talkbacks/" + 14958 + "/commands/execute?api_key=" + "61QGI8MFYN17D3VY";

    // Make a HTTP GET request to the TalkBack API:
    http.begin(talkBackURL);
    int httpCode = http.GET();
    Serial.print("Waiting for user input, ");
    Serial.print("httpCode: ");
    Serial.println(httpCode>0);

    talkBackCommand = http.getString();
    http.end();

    // Start script if triggered
    if (talkBackCommand.equals("Start")) {
      Serial.println(talkBackCommand);
      delay(submergeTime);

      // Restart clock variables
      lastUpdateTime = millis();
      lastConnectionTime = millis();
      return;
    }
    delay(checkTalkBackInterval);
  }
}

// Attempt to connect to WiFi network
void attemptWifi(){
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
    delay(5000);  // Wait 5 seconds to connect
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Failed connecting to wifi");
    }
    else {
      Serial.println("Connected to wifi");
    }
  }
  printWiFiStatus(); // Print WiFi connection information
}

// Updates the ThingSpeakchannel with data
void httpRequest(char* jsonBuffer) {
  /* JSON format for data buffer in the API
   *  This example uses the relative timestamp as it uses the "delta_t". If your device has a real-time clock, you can also provide the absolute timestamp using the "created_at" parameter
   *  instead of "delta_t".
   *   "{\"write_api_key\":\"YOUR-CHANNEL-WRITEAPIKEY\",\"updates\":[{\"delta_t\":0,\"field1\":-60},{\"delta_t\":15,\"field1\":200},{\"delta_t\":15,\"field1\":-66}]
   */
  // Format the data buffer as noted above
  char data[500] = "{\"write_api_key\":\"M370S66LE099HJPD\",\"updates\":"; //Replace YOUR-CHANNEL-WRITEAPIKEY with your ThingSpeak channel write API key
  strcat(data,jsonBuffer);
  strcat(data,"}");

  // Initialize the WiFi client library
  WiFiClient client;

  String data_length = String(strlen(data)+1); //Compute the data buffer length
  // POST data to ThingSpeak
  if (client.connect(server, 80)) {
    client.println("POST /channels/244820/bulk_update.json HTTP/1.1"); //Replace YOUR-CHANNEL-ID with your ThingSpeak channel ID
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: mw.doc.bulk-update (Arduino ESP8266)");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("Content-Length: "+data_length);
    client.println();
    client.println(data);
  }
  else {
    Serial.println("Failure: Failed to connect to ThingSpeak");
    delay(5000);
    httpRequest(jsonBuffer);
  }
  delay(250); //Wait to receive the response
  client.parseFloat();
  String resp = String(client.parseInt());
  Serial.println("Response code:"+resp); // Print the response code. 202 indicates that the server has accepted the response
  if (!resp.equals("202")) {
    delay(10000);
    httpRequest(jsonBuffer);
  }
  jsonBuffer[0] = '['; // Reinitialize the jsonBuffer for next batch of data
  jsonBuffer[1] = '\0';
}

void printWiFiStatus() {
  // Print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print your device IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
