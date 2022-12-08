/* 
 * File:   cmpe322_project
 * Contributors:
 *
 *   - London Johnson
 *   - Jan Molina
 *   - Kyle Smith
 *
 *  Description:
 *
 *    This is the main code for a smart clock that uses the ESP8266.
 *
 *  Hardware Requirements:
 * 
 *    ESP8266, LOLIN WEMOS D1 mini pro (V1)
 *    Adafruit 7-Segment Display with I2C Backpack
 *    DS3231 RTC Module
 * 
 *  Pin connections:
 * 
 *    D1 - SCL
 *    D2 - SDA
 *    D3 - Status LED 
 *    D4 - Buzzer
 *    D5 - button - Decrement
 *    D6 - button - Increment
 *    D7 - button - Mode/snooze
 *
 * Created on September 27, 2022, 8:52 PM
 */
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiManager.h>  
#include <Wire.h>  //SCL = D1, SDA = D2
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <DS3231.h>
#include <Ticker.h>


#define nop() for (int __n=0;__n<100;__n++)

// Uncomment this line for serial output messages
#define DEBUG_MODE

// Uncomment this line to test WiFi Manager
//#define TEST_WIFI

/**
 * HTTP API information
 */
#define API_URL "http://ipwho.is/"
//#define API_URL "https://api.ipgeolocation.io/timezone?apiKey=1d82ac1c3b24485ea23b3b0a00373af9"

/**
 * Function declarations
 */
void setupWiFi(void);                // WiFi Setup (Hard-Coded)
void setTimeFromAPI(void);
bool runWiFiManager(void);           // WiFiManager Setup
void setup7Segment(void);            // 7-Segment Setup
void ICACHE_RAM_ATTR  isrIncrement();
void ICACHE_RAM_ATTR  isrDecrement();


// Constants
#define BUZZER D4
#define BTN_DECREMENT D5
#define BTN_INCREMENT D6
#define BTN_MODE D7
#define LED_WIFI D3

#define MY_SSID "asd"
#define MY_PASSWORD "asd"


enum { MODE_DEFAULT, MODE_TIMESET };

// Global Variables
WiFiManager         wifiManager;       // WiFiManager
HTTPClient          httpClient;        // http client that talks to API
Adafruit_7segment   display = Adafruit_7segment();
Ticker              ticker1;           // 0.50 seconds -> tick_050
Ticker              ticker2;           // 0.25 seconds -> tick_025
Ticker              ticker3;           // 3.00 seconds -> tick_300
unsigned int        alarmTime = 730;   // Time when the alarm goes off
unsigned int        integerTime = 0;   // Time right now

char              mode = MODE_DEFAULT;
bool              alarmShouldPlay = false;
bool              alarmEnabled = true;
bool              alarmSnoozed = false;
bool              wifiConnected = false;
bool              ledWifiStatus = false;


// Setup
void setup(void) {
  #ifdef DEBUG_MODE
  Serial.begin(9600);
  Serial.println();
  Serial.println("Booted.");
  #endif
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER , OUTPUT);
  pinMode(BTN_INCREMENT, INPUT_PULLUP);  // increment button
  pinMode(BTN_DECREMENT, INPUT_PULLUP);  // decrement button
  pinMode(BTN_MODE, INPUT_PULLUP);       // mode button
  pinMode(LED_WIFI, OUTPUT);      // LED for wifi status


  // Attach interrupt to BTN_Increment
  attachInterrupt(digitalPinToInterrupt(BTN_INCREMENT), isrIncrement, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_DECREMENT), isrDecrement, FALLING);


  // Setup the 7-segment display
  setup7Segment();

  // Setup ticker interrupts
  ticker1.attach(0.5, tick_050);  // Run every 0.5 seconds
  ticker2.attach(0.25, tick_025); // Run every 0.25 seconds
  ticker3.attach(3.0, tick_300);  // Run every 3 seconds

  // Run WiFi setup after everything else
  if (runWiFiManager() == true) 
  {
    setTimeFromAPI();
  }

}


void loop(void) 
{

  if (alarmShouldPlay) {
    playAlarm();
  }

  digitalWrite(LED_WIFI, ledWifiStatus);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setTimeFromAPI()
{
  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, API_URL)) {

    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {

        String payload = http.getString();                           // get the JSON
        char *key = "current_time";
        char value[64];
        char payloadBytes[payload.length() + 1];
        payload.toCharArray(payloadBytes, payload.length());         // Convert the JSON to char array
        int success = getJsonValue(payloadBytes, key, value);        // Get time value from the JSON string
        if (success == 0) {
          unsigned int currentTime = iso8601ToInt(value);            // Convert the ISO 8601 time to a single integer
          Serial.printf("API_SUCCESS: %s = %d\n", key, currentTime);
          setRTCTimeFromInt(currentTime);                            // Set RTC's time to the integer value
        } else {
          Serial.println("API_FAILURE: JSON value not found");
        }

      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
}

// Checks if the current time is equal to the alarm time
void checkAlarm(uint32_t currentTime)
{
  bool shouldResetAlarm = (currentTime != alarmTime) && alarmSnoozed;
  if (shouldResetAlarm) {
    alarmSnoozed = false;
  }
  
  bool shouldAlarmPlay = (currentTime == alarmTime) && alarmEnabled && !alarmSnoozed;
  if (shouldAlarmPlay) {
    alarmShouldPlay = true;
  } else {
    alarmShouldPlay = false;
  }
}


/**
 * Sets the time from a 6-digit number
 * Example: 203239
 *    Time: 20:32:39  or 8:32:39
 */
void setRTCTimeFromInt(uint32_t integerTime)
{
  uint32_t second = (integerTime) % 100;
  uint32_t minute = (integerTime / 100) % 100;
  uint32_t hour = (integerTime / 10000) % 100;

  
  if (hour > 12 )
  {
    hour -= 12;
  }
  else
  {
    hour = hour;
  }

  DS3231 rtc;
  rtc.setHour(hour);
  rtc.setMinute(minute);
  rtc.setSecond(second);
}


// Sets the RTC's current time using 3 integers
void setRTCTime(int8_t hour, int8_t minute, int8_t second)
{
  DS3231 rtc;
  rtc.setHour(hour);
  rtc.setMinute(minute);
  rtc.setSecond(second);
}

// Retrieves the RTC's current time in 3 integers
void getRTCTime(int8_t *hour, int8_t *minute, int8_t *second) 
{
  DS3231 rtc;
  bool h12, PM;
  *hour = rtc.getHour(h12, PM);
  *minute = rtc.getMinute();
  *second = rtc.getSecond();
}

void displayTime12Hour()
{
  int8_t hour, minute, second;
  uint32_t currentTime = getRTCTime12Hour();
  display.println(currentTime , DEC);
  display.writeDigitRaw(2, 0x02);  // Draw colon
  display.writeDisplay();
}


void toggleMode()
{
  if (mode == MODE_DEFAULT) {
    mode = MODE_TIMESET;
  } else if (mode == MODE_TIMESET) {
    mode = MODE_DEFAULT;
  }
}


// Returns the RTC's current time as a single integer
uint32_t getRTCTime12Hour() 
{
  DS3231 rtc;
  bool h12, PM;
  int8_t hour =  rtc.getHour(h12, PM) % 12;
  if (hour == 0) {
    hour = 12;
  }
  int8_t minute = rtc.getMinute();
  return hour * 100 + minute;
}


// WiFi Setup
void setupWiFi(void) {
  #ifdef DEBUG_MODE
  Serial.print("\nConnecting to WiFi");
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(MY_SSID, MY_PASSWORD);

  while(WiFi.status() != WL_CONNECTED) {
    #ifdef DEBUG_MODE
    Serial.print(".");
    #endif
    delay(500);
  }

  #ifdef DEBUG_MODE
  Serial.println();
  Serial.print("Done. Local IP: ");
  Serial.println(WiFi.localIP());
  #endif
}

// WiFi Manager
bool runWiFiManager(void) {
  #ifdef DEBUG_MODE
  Serial.println("\nStarting WiFi Manager");
  #endif
  
  #ifdef TEST_WIFI
  wifiManager.resetSettings();
  #endif
 
  wifiManager.setConfigPortalTimeout(120);      // Set portal timeout

  bool connected = wifiManager.autoConnect("JKL-ESP8266-AP");
  if (!connected) {
    Serial.println("Failed to connect.");
    wifiConnected = false;
  } else {
    Serial.println("Connected.");
    wifiConnected = true;
  }
  return connected;
}

void setup7Segment(void) {
  #ifdef DEBUG_MODE
  Serial.print("\nConfiguring 7-Segment Display");
  #endif
  display.begin(0x70);
  display.blinkRate(0);   
  display.println("0000");
  display.writeDisplay();
  delay(300);
  String ourNames = "Jan Kyle London";
  for(int i = 0; i < ourNames.length(); i++)
  {
    char buffer[4 + 1];
    sprintf(buffer, "%4d", ourNames[i]);
    display.println(buffer);
    display.writeDisplay();

    #ifdef DEBUG_MODE
    Serial.print(".");
    #endif
    delay(100);
  }
  display.println("0000");
  display.writeDigitRaw(2, 0x02); // Draw colon
  display.writeDisplay();

  delay(300);
  #ifdef DEBUG_MODE
  Serial.println("Done.");
  #endif
}
