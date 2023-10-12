#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "TimeLib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
TaskHandle_t TaskHandle_2;
int ledweb = 18;
int ledlight = 19;
int ledgas = 5;
int gas = 35;
int buzzer = 26;
int light = 34;
int fire = 32;
int rain = 33;
int valueGas;
int valueLight;
int valueFire;
int valueRain;
#include <FirebaseESP32.h>
#include <DHT.h>

const int DHTPIN = 23;
const int DHTTYPE = DHT11;
DHT dht(DHTPIN, DHTTYPE);

#define WIFI_SSID "Hello"
#define WIFI_PASSWORD "honghai147"

#define FIREBASE_HOST "smarthome-9e7cc-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "SS8XbicV6rgjAJ96TrQhMjz6a7KKQ0tDYYtibbqb"
FirebaseData firebaseData;
//----------------------------------------
//----------------------------------------OLED screen size configuration
#define SCREEN_WIDTH 128 //--> OLED display width, in pixels
#define SCREEN_HEIGHT 64 //--> OLED display height, in pixels
//----------------------------------------

//----------------------------------------Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//----------------------------------------

//----------------------------------------Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//----------------------------------------

//----------------------------------------SSID and Password of your WiFi router
const char* ssid = "Hello";  //--> Your wifi name
const char* password = "honghai147"; //--> Your wifi password
//----------------------------------------

//----------------------------------------TimeLib configuration
struct timelib_tm tinfo; //--> Structure that holds human readable time information;

timelib_t now, initialt;
unsigned long previousMillisGetTimeDate = 0; //--> will store last time was updated
const long intervalGetTimeDate = 1000; //--> The interval for updating the time and date
//----------------------------------------

//----------------------------------------Declaration of time and date variables
//Week Days
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String weekDays3Dgt[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

//Month names
String months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

unsigned long epochTime;
int currentHour, currentMinute, currentSecond;
String weekDay;
int monthDay, currentMonth;
String currentMonthName;
int currentYear, currentYearforSet;
String _DateTime;
//----------------------------------------

//----------------------------------------Variable declarations to hold weather data
String current_weather_Description;
String current_weather;
int current_temperature, current_pressure, current_humidity, current_wind_deg, current_visibility;
float current_wind_speed, current_feels_like, current_uv, current_dew_point;
String current_weather_sym;

String forecast_weather[4];
String forecast_weather_sym[4];
float forecast_temp_min[4];
float forecast_temp_max[4];
//----------------------------------------

//----------------------------------------openweathermap API configuration
String openWeatherMapApiKey = "4e7d478283b69a7c9bdc8faf92c84550";

// Check subroutine "void Get_Weather_Data ()"
// If using a city name : String current_serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=" + units + "&APPID=" + openWeatherMapApiKey;
// If using a city ID : String current_serverPath = "http://api.openweathermap.org/data/2.5/weather?id=" + city + "&units=" + units + "&APPID=" + openWeatherMapApiKey;
// Please choose one.
String city = "VINH LONG";
String countryCode = "VN";
String units = "metric";
//----------------------------------------

//----------------------------------------Variable declaration for the json data and defining the ArduinoJson(DynamicJsonBuffer)
String strjsonBuffer;
String strjsonBufferCF;
DynamicJsonBuffer jsonBuffer;
//----------------------------------------

//----------------------------------------Variable declaration for the weather data update indicator
int Upadate_Trig = 1;
//----------------------------------------

//----------------------------------------Variable declarations for weather and loading symbols / icons
// 'Process1_Sym', 8x8px
const unsigned char Process1_Sym [] PROGMEM = {
  0x00, 0x18, 0x00, 0x01, 0x99, 0x80, 0x01, 0x81, 0x80, 0x08, 0x00, 0x10, 0x18, 0x00, 0x18, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x06, 0x60, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x06,
  0x60, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x08, 0x00, 0x10, 0x01,
  0x81, 0x80, 0x01, 0x99, 0x80, 0x00, 0x18, 0x00
};

// 'Process2_Sym', 8x8px
const unsigned char Process2_Sym [] PROGMEM = {
  0x00, 0x66, 0x00, 0x00, 0x66, 0x00, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60, 0x00, 0x00, 0x00, 0x30,
  0x00, 0x0c, 0x30, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x03, 0xc0, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x30, 0x00, 0x0c, 0x30, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x06,
  0x00, 0x60, 0x00, 0x66, 0x00, 0x00, 0x66, 0x00
};

// 'Clear_Daylight_Sym', 24x24px
const unsigned char Clear_Daylight_Sym [] PROGMEM = {
  0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x10, 0x18, 0x08, 0x38, 0x00, 0x1c, 0x1c, 0x3c, 0x38, 0x08,
  0xff, 0x10, 0x01, 0xc1, 0x80, 0x03, 0x00, 0xc0, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60, 0x0c, 0x00,
  0x30, 0xec, 0x00, 0x37, 0xec, 0x00, 0x37, 0x0c, 0x00, 0x30, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60,
  0x03, 0x00, 0xc0, 0x01, 0xc3, 0x80, 0x08, 0xff, 0x10, 0x1c, 0x3c, 0x38, 0x38, 0x00, 0x1c, 0x10,
  0x18, 0x08, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00
};

// 'Clear_Night_Sym', 24x24px
const unsigned char Clear_Night_Sym [] PROGMEM = {
  0x08, 0x08, 0x80, 0x08, 0x1c, 0xc0, 0x1c, 0x08, 0xe0, 0x3e, 0x00, 0xf0, 0xff, 0x80, 0xd8, 0x3e,
  0x00, 0xcc, 0x1c, 0x00, 0xc6, 0x08, 0x01, 0x86, 0x08, 0x01, 0x83, 0x00, 0x03, 0x03, 0x00, 0x03,
  0x03, 0x40, 0x06, 0x03, 0xe0, 0x06, 0x03, 0x40, 0x1c, 0x03, 0x00, 0x78, 0x03, 0x01, 0xe0, 0x03,
  0xff, 0x80, 0x06, 0x7e, 0x00, 0x06, 0x30, 0x00, 0x0c, 0x18, 0x00, 0x18, 0x0c, 0x00, 0x30, 0x07,
  0x00, 0xe0, 0x03, 0xff, 0xc0, 0x00, 0xff, 0x00
};

// 'Thunderstorm', 24x24px
const unsigned char Thunderstorm_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0xf8, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60,
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff,
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x10, 0x1e, 0x02, 0x20, 0x24, 0x04, 0x40, 0x48, 0x08,
  0xf8, 0x9f, 0x9f, 0x11, 0x00, 0x82, 0x21, 0xfd, 0x04, 0x40, 0x0a, 0x08, 0x80, 0x14, 0x10, 0x00,
  0x28, 0x00, 0x00, 0x50, 0x00, 0x00, 0x60, 0x00
};

// 'Drizzle', 24x24px
const unsigned char Drizzle_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0x18, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60,
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff,
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xc3, 0x0c, 0x30, 0xc3, 0x0c,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x18, 0x61, 0x86, 0x18, 0x61, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x30, 0xc3, 0x0c, 0x30, 0xc3, 0x0c
};

// 'Rain', 24x24px
const unsigned char Rain_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0xf8, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60,
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff,
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x21, 0x21, 0x08, 0x42,
  0x42, 0x10, 0x84, 0x84, 0x21, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x84, 0x21, 0x21,
  0x08, 0x42, 0x42, 0x10, 0x84, 0x84, 0x21, 0x08
};

// 'Snow', 24x24px
const unsigned char Snow_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0x18, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60,
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff,
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0xa0, 0x50, 0x20, 0x40, 0x20, 0xa8, 0xa0, 0x50, 0x70,
  0x00, 0x01, 0xfc, 0x04, 0x00, 0x70, 0x15, 0x00, 0xa8, 0x0e, 0x00, 0x20, 0x3f, 0x80, 0x00, 0x0e,
  0x0a, 0x05, 0x15, 0x04, 0x02, 0x04, 0x0a, 0x05
};

// 'Mist', 24x24px
const unsigned char Mist_Sym [] PROGMEM = {
  0x01, 0xf0, 0x00, 0x03, 0x18, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c, 0x38, 0x03, 0x06, 0x60,
  0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x7f, 0xff,
  0xfe, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xc3, 0x0c, 0x79, 0xe7, 0x9e,
  0xcf, 0x3c, 0xf3, 0x86, 0x18, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xc3, 0x0c, 0x79,
  0xe7, 0x9e, 0xcf, 0x3c, 0xf3, 0x86, 0x18, 0x61
};

// 'Clouds_Daylight', 24x24px
const unsigned char Clouds_Daylight_Sym [] PROGMEM = {
  0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x10, 0x18, 0x08, 0x38, 0x00, 0x1c, 0x1c, 0x3c, 0x38, 0x08,
  0xff, 0x10, 0x01, 0xc3, 0x80, 0x03, 0x00, 0xc0, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60, 0x0c, 0x00,
  0x30, 0xec, 0x00, 0x37, 0xed, 0xf0, 0x37, 0x0f, 0xf8, 0x70, 0x06, 0x0c, 0xf8, 0x1c, 0x07, 0x8c,
  0x38, 0x03, 0x06, 0x60, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xe0,
  0x00, 0x07, 0x7f, 0xff, 0xfe, 0x3f, 0xff, 0xfc
};

// 'Clouds_Night_Sym', 24x24px
const unsigned char Clouds_Night_Sym [] PROGMEM = {
  0x00, 0x00, 0x02, 0x00, 0x01, 0x07, 0x04, 0x01, 0xc2, 0x04, 0x01, 0xe0, 0x0e, 0x01, 0xb0, 0x3f,
  0x81, 0x98, 0x0e, 0x01, 0x8c, 0x04, 0x03, 0x0c, 0x04, 0x03, 0x06, 0x00, 0x03, 0x06, 0x00, 0x06,
  0x03, 0x00, 0x06, 0x03, 0x00, 0x0c, 0x03, 0x00, 0x38, 0x03, 0x01, 0xf0, 0x06, 0x7f, 0xc0, 0x06,
  0x3e, 0x00, 0x0c, 0x30, 0x00, 0x0c, 0x18, 0x00, 0x18, 0x0c, 0x00, 0x30, 0x07, 0x00, 0xe0, 0x43,
  0xc3, 0xc2, 0xe0, 0xff, 0x07, 0x40, 0x3c, 0x02
};

// 'Cloudy_Sym', 24x24px
const unsigned char Cloudy_Sym [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x01, 0xf0, 0xe0, 0x03, 0x19, 0xf0, 0x0e,
  0x0f, 0x1c, 0x1c, 0x06, 0x0e, 0x30, 0x00, 0x03, 0x60, 0x00, 0x03, 0x61, 0xc0, 0x03, 0x63, 0xe1,
  0xc3, 0x36, 0x33, 0xe6, 0x3c, 0x1e, 0x3c, 0x78, 0x0c, 0x1c, 0xe0, 0x00, 0x06, 0xc0, 0x00, 0x06,
  0xc0, 0x00, 0x06, 0xc0, 0x00, 0x06, 0xe0, 0x00, 0x0c, 0x7f, 0xff, 0xf8, 0x3f, 0xff, 0xf0, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

char *mist[] = {"Mist", "Smoke", "Haze", "Dust", "Fog", "Sand", "Dust", "Ash", "Squall", "Tornado"};
//----------------------------------------

//----------------------------------------Variable declaration for calculating the connection time
int cnt_con = 0;
//----------------------------------------

int timezone_offset;

//=====================================================================================Subroutines for the connection process
void Connect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  //  Serial.println("");
  //  Serial.print("Connecting");
  //
  //  while (WiFi.status() != WL_CONNECTED) {
  //    Serial.print(".");
  //
  //    display.clearDisplay(); //--> for Clearing the display
  //    display.setTextSize(1);
  //    display.setTextColor(WHITE);
  //    display.setCursor(20, 15);
  //    display.println("Connect to WiFi");
  //    display.drawBitmap(52, 25, Process1_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  //    display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
  //    display.display();
  //    delay(250);
  //
  //
  //
  //    display.clearDisplay(); //--> for Clearing the display
  //    display.setTextSize(1);
  //    display.setTextColor(WHITE);
  //    display.setCursor(20, 15);
  //    display.println("Connect to WiFi");
  //    display.drawBitmap(52, 25, Process2_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  //    display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
  //    display.display();
  //    delay(250);
  //
  //    cnt_con++;
  //    if (cnt_con > 59) {
  //      cnt_con = 0;
  //      Serial.println("Failed to connect !");
  //      display.clearDisplay(); //--> for Clearing the display
  //      display.setTextSize(1);
  //      display.setTextColor(WHITE);
  //      display.setCursor(0, 28);
  //      display.println(" Failed to connect !");
  //      display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
  //      display.display();
  //      delay(5000);
  //      Waiting_to_be_connected_again();
  //    }
  //    //----------------------------------------
  //  }
  //  //----------------------------------------
  //  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  //  Serial.println("");
  //  Serial.print("Successfully connected to : ");
  //  Serial.println(ssid);
  //  Serial.print("IP address: ");
  //  Serial.println(WiFi.localIP());
  //  Serial.println();
  //
  //  display.clearDisplay(); //--> for Clearing the display
  //  display.setTextSize(1);
  //  display.setTextColor(WHITE);
  //  display.setCursor(0, 24);
  //  display.println("     Connection");
  //  display.setCursor(0, 33);
  //  display.println("     Successful");
  //  display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
  //  display.display();
  //  delay(2000);
  //----------------------------------------
}
//=====================================================================================

//=====================================================================================Subroutine if connection is lost

//=====================================================================================

//=====================================================================================Subroutines waiting to be reconnected

//=====================================================================================

//=====================================================================================setup()
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); //--> Don't proceed, loop forever
  }
  display.display();
  delay(2000);
  Connect();
  dht.begin();
  pinMode(ledweb, OUTPUT);
  pinMode(ledlight, OUTPUT);
  pinMode(ledgas, OUTPUT);
  pinMode(gas, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(light, INPUT);
  pinMode(fire, INPUT);
  pinMode(rain, INPUT);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 1);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  timeClient.begin(); //--> Initialize a NTPClient to get time
  display.clearDisplay(); //--> for Clearing the display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(3, 19);
  display.println("  Get weather data");
  display.setCursor(3, 28);
  display.println("        from");
  display.setCursor(3, 37);
  display.println("   openweathermap");
  display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
  display.display();
  delay(1000);
}
void loop() {
  thoitiet();
}
void thoitiet() {
  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      web();
      //__________________________________________________________________________________________________Millis to display time, date and weather data
      unsigned long currentMillisGetTimeDate = millis();
      if (currentMillisGetTimeDate - previousMillisGetTimeDate >= intervalGetTimeDate) {
        previousMillisGetTimeDate = currentMillisGetTimeDate;

        now = timelib_get(); //--> Get the timestamp from the library

        timelib_break(now, &tinfo); //--> Convert to human readable format

        //----------------------------------------Processing time and date data
        char timeProcess[1000];
        sprintf(timeProcess, "%02u:%02u:%02u", tinfo.tm_hour, tinfo.tm_min, tinfo.tm_sec);
        Serial.println(timeProcess);
        char _time[1000];
        strcpy(_time, timeProcess);

        char dateDayProcess[1000];
        sprintf(dateDayProcess, "%02u", tinfo.tm_mday);
        char _dateDay[100];
        strcpy(_dateDay, dateDayProcess);

        _DateTime = _dateDay;
        _DateTime += "-" + currentMonthName + "-" + String(currentYear) + " " + _time;
        //----------------------------------------

        //----------------------------------------Conditions for updating weather data, time and date.
        if (Upadate_Trig == 1 && tinfo.tm_min % 15 == 0) {
          web();
          display.clearDisplay(); //--> for Clearing the display
    
          display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame

          Display_current_weather_data();

          display.drawRect(0, 0, 128, 64, WHITE);
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(4, 52); //--> (x position, y position)
          display.println("  Updating Data...");

          display.drawLine(0, 47, 127, 47, WHITE);

          display.display();

          Get_Weather_Data();

          Serial.println("Set the Date and Time from the Internet...");
          Set_Time_and_Date();

          Upadate_Trig = 0;
        }

        if (tinfo.tm_min % 15 != 0) {
          Upadate_Trig = 1;
          web();
        }
        //----------------------------------------

        //----------------------------------------Conditions for displaying more detailed weather data

        if (tinfo.tm_sec > 20 && tinfo.tm_sec < 26) {
          Display_weather_description();
          web();
        } else if (tinfo.tm_sec > 25 && tinfo.tm_sec < 36) {
          Display_other_information_1();
          web();
        } else if (tinfo.tm_sec > 35 && tinfo.tm_sec < 46) {
          Display_other_information_2();
          web();
        }

        //      } else if(tinfo.tm_sec > 45 && tinfo.tm_sec < 56) {
        //        Display_weather_forecast_data();
        else {
          web();
          display.clearDisplay(); //--> for Clearing the display

          display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame

          Display_current_weather_data();

          //Display_current_DateTime();

          display.drawLine(0, 47, 127, 47, WHITE);

          display.display();
        }
        //----------------------------------------
      }
      //__________________________________________________________________________________________________
    }
  }
}
void Get_Weather_Data() {
  if (WiFi.status() == WL_CONNECTED) {
    //__________________________________________________________________________________________________Get longitude, latitude and timezone offset
    //----------------------------------------If using a city name
    String current_serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=" + units + "&APPID=" + openWeatherMapApiKey;
    //----------------------------------------

    //----------------------------------------If using a city ID
    //String current_serverPath = "http://api.openweathermap.org/data/2.5/weather?id=" + city + "&units=" + units + "&APPID=" + openWeatherMapApiKey;
    //----------------------------------------

    Serial.println("--------------------");
    Serial.println("Get weather data from openweathermap...");
    Serial.println();
    strjsonBuffer = httpGETRequest(current_serverPath.c_str());
    //Serial.println(strjsonBuffer);
    JsonObject& root = jsonBuffer.parseObject(strjsonBuffer);

    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
      display.clearDisplay(); //--> for Clearing the display
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 19);
      display.println("    Failed to get");
      display.setCursor(0, 28);
      display.println("    weather data !");
      display.setCursor(4, 37);
      display.println("parseObject() failed");
      display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
      display.display();
      delay(5000);
      return;
    }

    String lon = root["coord"]["lon"];
    String lat = root["coord"]["lat"];
    timezone_offset = root["timezone"];

    Serial.println();
    Serial.print("Lon : ");
    Serial.println(lon);
    Serial.print("Lat : ");
    Serial.println(lat);
    Serial.print("Timezone offset : ");
    Serial.println(timezone_offset);
    Serial.println();

    jsonBuffer.clear();

    Serial.println("Set the Date and Time from the Internet...");
    Set_Time_and_Date();
    //__________________________________________________________________________________________________

    //__________________________________________________________________________________________________Get current weather data and weather forecast data for the following days
    //----------------------------------------Get current weather data
    String current_forecast_serverPath = "https://api.openweathermap.org/data/2.5/weather?lat=10.5&lon=105.9667&appid=4e7d478283b69a7c9bdc8faf92c84550";

    strjsonBufferCF = httpGETRequest(current_forecast_serverPath.c_str());
    //Serial.println(strjsonBuffer);
    JsonObject& rootCF = jsonBuffer.parseObject(strjsonBufferCF);

    // Test if parsing succeeds.
    if (!rootCF.success()) {
      Serial.println("parseObject() failed");
      display.clearDisplay(); //--> for Clearing the display
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 19);
      display.println("    Failed to get");
      display.setCursor(0, 28);
      display.println("    weather data !");
      display.setCursor(4, 37);
      display.println("parseObject() failed");
      display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
      display.display();
      delay(5000);
      return;
    }

    String str_current_weather = rootCF["weather"][0]["main"];
    current_weather = str_current_weather;

    String str_current_weather_Description = rootCF["weather"][0]["description"];
    current_weather_Description = str_current_weather_Description;

    String str_current_weather_sym = rootCF["weather"][0]["icon"];
    current_weather_sym = str_current_weather_sym;

    current_temperature = rootCF["main"]["temp"];
    current_feels_like = rootCF["main"]["feels_like"];
    current_uv = rootCF["main"]["grnd_level"];
    current_dew_point = 0;
    current_pressure = rootCF["main"]["pressure"];
    current_humidity = rootCF["main"]["humidity"];
    current_visibility = rootCF["visibility"];
    current_wind_speed = rootCF["wind"]["speed"];
    current_wind_deg = rootCF["wind"]["deg"];
    //----------------------------------------

    //----------------------------------------Get weather forecast data for the following days
    String str_forecast_weather1 = rootCF["weather"][0]["main"];
    forecast_weather[0] = str_forecast_weather1;
    String str_forecast_weather_sym1 = rootCF["weather"][0]["icon"];
    forecast_weather_sym[0] = str_forecast_weather_sym1;
    forecast_temp_max[0] = rootCF["main"]["temp_max"];
    forecast_temp_min[0] = rootCF["main"]["temp_min"];

    String str_forecast_weather2 = rootCF["weather"][0]["main"];
    forecast_weather[1] = str_forecast_weather1;
    String str_forecast_weather_sym2 = rootCF["weather"][0]["icon"];
    forecast_weather_sym[1] = str_forecast_weather_sym2;
    forecast_temp_max[1] = rootCF["main"]["temp_max"];
    forecast_temp_min[1] = rootCF["main"]["temp_min"];

    String str_forecast_weather3 = rootCF["weather"][0]["main"];
    forecast_weather[2] = str_forecast_weather1;
    String str_forecast_weather_sym3 = rootCF["weather"][0]["icon"];
    forecast_weather_sym[2] = str_forecast_weather_sym3;
    forecast_temp_max[2] = rootCF["main"]["temp_max"];
    forecast_temp_min[2] = rootCF["main"]["temp_min"];

    String str_forecast_weather4 = rootCF["weather"][0]["main"];
    forecast_weather[3] = str_forecast_weather1;
    String str_forecast_weather_sym4 = rootCF["weather"][0]["icon"];
    forecast_weather_sym[3] = str_forecast_weather_sym4;
    forecast_temp_max[3] = rootCF["main"]["temp_max"];
    forecast_temp_min[3] = rootCF["main"]["temp_min"];
    //----------------------------------------

    //----------------------------------------

    float current_visibility_to_km = current_visibility / 1000;
    //----------------------------------------

    //----------------------------------------
    int wds[4] = {timeClient.getDay(), timeClient.getDay() + 1, timeClient.getDay() + 2, timeClient.getDay() + 3};

    for (int i = 0; i < 4; i++) {
      if (wds[i] == 7) {
        wds[i] = 0;
      }
      if (wds[i] == 8) {
        wds[i] = 1;
      }
      if (wds[i] == 9) {
        wds[i] = 2;
      }
    }

    String wd;
    //----------------------------------------

    //----------------------------------------
    wd = weekDays[wds[0]];
    wd = weekDays[wds[1]];
    wd = weekDays[wds[2]];
    wd = weekDays[wds[3]];
    jsonBuffer.clear();

    Serial.println("Set the Date and Time again from the Internet...");
    Set_Time_and_Date();
    //__________________________________________________________________________________________________
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}
//=====================================================================================

//=====================================================================================httpGETRequest
String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode == 200) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);

    display.clearDisplay(); //--> for Clearing the display
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 19);
    display.println("    Failed to get");
    display.setCursor(0, 28);
    display.println("    weather data !");
    display.setCursor(0, 37);
    display.print("  HTTP Response: ");
    display.println(httpResponseCode);
    display.drawRect(0, 0, 128, 64, WHITE); //--> Display frame
    display.display();
    delay(5000);
  }
  // Free resources
  http.end();

  return payload;
}
//=====================================================================================

//=====================================================================================Subroutines for setting the time and date from the internet
void Set_Time_and_Date() {
  timeClient.setTimeOffset(timezone_offset);

  // Send an HTTP GET request
  timeClient.update();

  epochTime = timeClient.getEpochTime();
  currentHour = timeClient.getHours();
  currentMinute = timeClient.getMinutes();
  currentSecond = timeClient.getSeconds();
  weekDay = weekDays[timeClient.getDay()];
  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  monthDay = ptm->tm_mday;
  currentMonth = ptm->tm_mon + 1;
  currentMonthName = months[currentMonth - 1];
  currentYear = ptm->tm_year + 1900;
  currentYearforSet = currentYear - 2000;
  // Set time manually to 13:55:30 Jan 1st 2014
  // YOU CAN SET THE TIME FOR THIS EXAMPLE HERE
  tinfo.tm_year = currentYearforSet;
  tinfo.tm_mon = currentMonth;
  tinfo.tm_mday = monthDay;
  tinfo.tm_hour = currentHour;
  tinfo.tm_min = currentMinute;
  tinfo.tm_sec = currentSecond;

  // Convert time structure to timestamp
  initialt = timelib_make(&tinfo);

  // Set system time counter
  timelib_set(initialt);

  // Configure the library to update / sync every day (for hardware RTC)
  //  timelib_set_provider(time_provider, TIMELIB_SECS_PER_DAY);
  Serial.println("Setting Date and Time from Internet is successful.");
}
//=====================================================================================

//=====================================================================================Subroutines to display time and date
void Display_current_DateTime() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(4, 52); //--> (x position, y position)
  display.println(_DateTime);
}
//=====================================================================================

//=====================================================================================Subroutines for displaying weather data
void Display_current_weather_data() {
  //----------------------------------------Weather icon selection conditions
  if (current_weather == "Thunderstorm") {
    display.drawBitmap(10, 5, Thunderstorm_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  }
  else if (current_weather == "Drizzle") {
    display.drawBitmap(10, 5, Drizzle_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  }
  else if (current_weather == "Rain") {
    display.drawBitmap(10, 5, Rain_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  }
  else if (current_weather == "Snow") {
    display.drawBitmap(10, 5, Snow_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
  }
  else if (current_weather == "Clear") {
    if (current_weather_sym == "01d") {
      display.drawBitmap(10, 5, Clear_Daylight_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    } else if (current_weather_sym == "01n") {
      display.drawBitmap(10, 5, Clear_Night_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
  }
  else if (current_weather == "Clouds") {
    if (current_weather_sym == "02d") {
      display.drawBitmap(10, 5, Clouds_Daylight_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    } else if (current_weather_sym == "02d") {
      display.drawBitmap(10, 5, Clouds_Night_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    } else {
      display.drawBitmap(10, 5, Cloudy_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
  }

  for (int i = 0; i < 10; i++) {
    if (current_weather == mist[i]) {
      display.drawBitmap(10, 5, Mist_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
  }
  //----------------------------------------

  //----------------------------------------Displays weather data, temperature, humidity and wind speed
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(50, 5); //--> (x position, y position)
  display.println(current_weather);

  display.setCursor(50, 15); //--> (x position, y position)
  String Hum = "H:" + String(current_humidity) + "%";
  display.println(Hum);

  display.setCursor(50, 25); //--> (x position, y position)
  String WS = "WS:" + String(current_wind_speed) + "m/s";
  display.println(WS);

  display.setCursor(50, 35); //--> (x position, y position)
  display.println(weekDay);

  display.setCursor(4, 35); //--> (x position, y position)
  display.print("T:");
  display.print(current_temperature);
  display.print((char)247); //--> ASCII degree symbol
  display.print("C");
  //----------------------------------------
}
//=====================================================================================

//=====================================================================================Subroutines to display data for the next few days
void Display_weather_forecast_data() {
  int forecast_X1[] = {20, 90};
  int forecast_X2[] = {17, 87};
  int forecast_X3[] = {8, 79};
  int wds[4] = {timeClient.getDay(), timeClient.getDay() + 1, timeClient.getDay() + 2, timeClient.getDay() + 3};
  int round_forecast_temp_max[2];
  int round_forecast_temp_min[2];

  for (int i = 0; i < 4; i++) {
    if (wds[i] == 7) {
      wds[i] = 0;
    }
    if (wds[i] == 8) {
      wds[i] = 1;
    }
    if (wds[i] == 9) {
      wds[i] = 2;
    }
  }

  String forecast_weather_Prcs[2];
  String forecast_weather_sym_Prcs[2];
  int wds_Prcs[2];

  if (tinfo.tm_sec > 45 && tinfo.tm_sec < 51) {
    forecast_weather_Prcs[0] = forecast_weather[0];
    forecast_weather_Prcs[1] = forecast_weather[1];
    forecast_weather_sym_Prcs[0] = forecast_weather_sym[0];
    forecast_weather_sym_Prcs[1] = forecast_weather_sym[1];
    wds_Prcs[0] = wds[0];
    wds_Prcs[1] = wds[1];
    round_forecast_temp_max[0] = round(forecast_temp_max[0]);
    round_forecast_temp_min[0] = round(forecast_temp_min[0]);
    round_forecast_temp_max[1] = round(forecast_temp_max[1]);
    round_forecast_temp_min[1] = round(forecast_temp_min[1]);
  } else {
    //    forecast_weather_Prcs[0] = forecast_weather[2];
    //    forecast_weather_Prcs[1] = forecast_weather[3];
    //    forecast_weather_sym_Prcs[0] = forecast_weather_sym[2];
    //    forecast_weather_sym_Prcs[1] = forecast_weather_sym[3];
    //    wds_Prcs[0] = wds[2];
    //    wds_Prcs[1] = wds[3];
    //    round_forecast_temp_max[0] = round(forecast_temp_max[2]);
    //    round_forecast_temp_min[0] = round(forecast_temp_min[2]);
    //    round_forecast_temp_max[1] = round(forecast_temp_max[3]);
    //    round_forecast_temp_min[1] = round(forecast_temp_min[3]);
  }

  String wd;
  String WF, WSF;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  //----------------------------------------Process data for display
  for (int i = 0; i < 2; i++) {
    wd = weekDays3Dgt[wds_Prcs[i]];
    display.setCursor(forecast_X1[i], 0);
    display.print(wd);

    WF = forecast_weather_Prcs[i];
    WSF = forecast_weather_sym_Prcs[i];

    if (WF == "Thunderstorm") {
      display.drawBitmap(forecast_X2[i], 9, Thunderstorm_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
    else if (WF == "Drizzle") {
      display.drawBitmap(forecast_X2[i], 9, Drizzle_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
    else if (WF == "Rain") {
      display.drawBitmap(forecast_X2[i], 9, Rain_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
    else if (WF == "Snow") {
      display.drawBitmap(forecast_X2[i], 9, Snow_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
    }
    else if (WF == "Clear") {
      if (WSF == "01d") {
        display.drawBitmap(forecast_X2[i], 9, Clear_Daylight_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      } else if (WSF == "01n") {
        display.drawBitmap(forecast_X2[i], 9, Clear_Night_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      }
    }
    else if (WF == "Clouds") {
      if (WSF == "02d") {
        display.drawBitmap(forecast_X2[i], 9, Clouds_Daylight_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      } else if (WSF == "02d") {
        display.drawBitmap(forecast_X2[i], 9, Clouds_Night_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      } else {
        display.drawBitmap(forecast_X2[i], 9, Cloudy_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      }
    }
    for (int i = 0; i < 10; i++) {
      if (WF == mist[i]) {
        display.drawBitmap(forecast_X2[i], 9, Mist_Sym, 24, 24, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color);
      }
    }
    String forecast_temp_max_min = String(round_forecast_temp_max[i]) + "/" + round_forecast_temp_min[i];
    if (forecast_temp_max_min.length() == 6) {
      forecast_X3[i] += 3;
    } else if (forecast_temp_max_min.length() == 5) {
      forecast_X3[i] += 6;
    } else if (forecast_temp_max_min.length() == 4) {
      forecast_X3[i] += 9;
    } else if (forecast_temp_max_min.length() == 3) {
      forecast_X3[i] += 12;
    }

    display.setCursor(forecast_X3[i], 36);
    display.print(forecast_temp_max_min);
  }
  //----------------------------------------

  display.drawRect(0, 47, 128, 17, WHITE); //--> Display frame

  //Display_current_DateTime();

  display.display();
}
//=====================================================================================

//=====================================================================================Show_weather_description()
void Display_weather_description() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0); //--> (x position, y position)
  display.println("Weather description :");

  display.setCursor(0, 10); //--> (x position, y position)
  display.println(current_weather_Description);

  display.drawRect(0, 47, 128, 17, WHITE); //--> Display frame

  //Display_current_DateTime();

  display.display();
}
//=====================================================================================

//=====================================================================================Displays_other_information_1()
void Display_other_information_1() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  int round_current_feels_like = round(current_feels_like);
  String Show_current_feels_like = "Feels Like :" + String(round_current_feels_like);
  display.setCursor(0, 0); //--> (x position, y position)
  display.print(Show_current_feels_like);
  display.print((char)247); //--> ASCII degree symbol
  display.print("C");

  int round_current_uv = round(current_uv);
  String Show_current_uv = "UV         :" + String(round_current_uv);
  display.setCursor(0, 10); //--> (x position, y position)
  display.println(Show_current_uv);

  int round_current_dew_point = round(current_dew_point);
  String Show_current_dew_point = "Dew Point  :" + String(round_current_dew_point);
  display.setCursor(0, 20); //--> (x position, y position)
  display.print(Show_current_dew_point);
  display.print((char)247); //--> ASCII degree symbol
  display.print("C");

  display.drawRect(0, 47, 128, 17, WHITE); //--> Display frame

  //Display_current_DateTime();

  display.display();
}
//=====================================================================================

//=====================================================================================Displays_other_information_2()
void Display_other_information_2() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  String Show_current_pressure = "Pressure    :" + String(current_pressure) + "hPa";
  display.setCursor(0, 0); //--> (x position, y position)
  display.println(Show_current_pressure);

  float current_visibility_to_km = current_visibility / 1000;
  String Show_current_visibility = "visibility  :" + String(current_visibility_to_km) + "km";
  display.setCursor(0, 10); //--> (x position, y position)
  display.println(Show_current_visibility);

  String Cardinal_Direction;
  if (current_wind_deg > 347 || current_wind_deg < 12) {
    Cardinal_Direction = "N";
  } else if (current_wind_deg > 11 && current_wind_deg < 34) {
    Cardinal_Direction = "NNE";
  } else if (current_wind_deg > 33 && current_wind_deg < 57) {
    Cardinal_Direction = "NE";
  } else if (current_wind_deg > 56 && current_wind_deg < 79) {
    Cardinal_Direction = "ENE";
  } else if (current_wind_deg > 78 && current_wind_deg < 102) {
    Cardinal_Direction = "E";
  } else if (current_wind_deg > 101 && current_wind_deg < 124) {
    Cardinal_Direction = "ESE";
  } else if (current_wind_deg > 123 && current_wind_deg < 147) {
    Cardinal_Direction = "SE";
  } else if (current_wind_deg > 146 && current_wind_deg < 169) {
    Cardinal_Direction = "SSE";
  } else if (current_wind_deg > 168 && current_wind_deg < 192) {
    Cardinal_Direction = "S";
  } else if (current_wind_deg > 191 && current_wind_deg < 214) {
    Cardinal_Direction = "SSW";
  } else if (current_wind_deg > 213 && current_wind_deg < 237) {
    Cardinal_Direction = "SW";
  } else if (current_wind_deg > 236 && current_wind_deg < 259) {
    Cardinal_Direction = "WSW";
  } else if (current_wind_deg > 258 && current_wind_deg < 282) {
    Cardinal_Direction = "W";
  } else if (current_wind_deg > 281 && current_wind_deg < 304) {
    Cardinal_Direction = "WNW";
  } else if (current_wind_deg > 303 && current_wind_deg < 327) {
    Cardinal_Direction = "NW";
  } else if (current_wind_deg > 326 && current_wind_deg < 347) {
    Cardinal_Direction = "NNW";
  }

  display.setCursor(0, 20); //--> (x position, y position)
  display.print("Wind Degree :");
  display.print(current_wind_deg);
  display.print((char)247); //--> ASCII degree symbol
  display.println(Cardinal_Direction);

  display.drawRect(0, 47, 128, 17, WHITE); //--> Display frame

  //Display_current_DateTime();

  display.display();
}
void nhietdodoam() {
  float doam = dht.readHumidity();
  float doC = dht.readTemperature();
  Firebase.setInt(firebaseData, "/DuLieu/doam", doam);
  Firebase.setInt(firebaseData, "/DuLieu/doc", doC);
}
void Gas() {
  valueGas = analogRead(gas);
  if (valueGas > 90) {
    Firebase.setInt(firebaseData, "/DuLieu/Gas", 1);
    digitalWrite(ledgas, HIGH);
  } else {
    Firebase.setInt(firebaseData, "/DuLieu/Gas", 0);
    digitalWrite(ledgas, LOW);
  }
}
void Rain() {
  valueRain = digitalRead(rain);
  if (valueRain == 1) {
    Firebase.setInt(firebaseData, "/DuLieu/Mua", 0);
  } else {
    Firebase.setInt(firebaseData, "/DuLieu/Mua", 1);
  }
}
void Light() {
  valueLight = analogRead(light);
  Serial.println(valueLight);
  if (valueLight > 700) {
    digitalWrite(ledlight, HIGH);
    Firebase.setString(firebaseData, "TrangThai/Light", 1);
  } else if (valueLight < 400) {
    digitalWrite(ledlight, LOW);
    Firebase.setString(firebaseData, "TrangThai/Light", 0);
  }
}
void Fire() {
  valueFire = analogRead(fire);
  if (valueFire > 1900) {
    Firebase.setInt(firebaseData, "/DuLieu/Fire", 0);
    digitalWrite(buzzer, LOW);
  } else {
    Firebase.setInt(firebaseData, "/DuLieu/Fire", 1);
    digitalWrite(buzzer, HIGH);
  }
}
void web() {
  Gas();
  Rain();
  Light();
  Fire();
  nhietdodoam();
  if (Firebase.getInt(firebaseData, "TrangThai/Led") == true) {
    int St = firebaseData.to<int>();
    if (St == 1) {
      digitalWrite(ledweb, HIGH);
    } else {
      digitalWrite(ledweb, LOW);
    }
  }
}
