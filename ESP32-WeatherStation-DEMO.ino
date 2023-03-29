//Origional code found here: 
//https://duino4projects.com/mini-tv-weather-station-with-the-esp32/amp/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_GFX.h>

//uint16_t time = millis();

const char* ssid = "XXXXXXXXX";//enter the name of your Wi-Fi network
const char* password = "XXXXXXXXXX";//enter the password to your network

const int buttonPin = 15;
int buttonState = 0;

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "XXXXXXXXXXXXXXXXXXXXXXX";//your open weather map API key

String city = "Boston";//Your city name, New York is an example I dont actually live there
String state = "MA";//enter your state abbreviation. Thx paul159 for figuring this out!
String countryCode = "US";// Your country code

String jsonBuffer;

///\/OLED SETUP\///////////////
// Screen dimensions
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 135 // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins 
#define SCLK_PIN 18
#define MOSI_PIN 4
#define DC_PIN   22
#define CS_PIN   21
#define RST_PIN  23

// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

///////////////////////////////

void setup() {
  Serial.begin(115200);
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  

  pinMode(buttonPin, INPUT);

  WiFi.begin(ssid, password);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Weather");
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }
  tft.println("");
  tft.print("Connected to WiFi network with IP Address: ");
  tft.println(WiFi.localIP());
  tft.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    tft.print(".");
  }
  tft.println("");
  tft.print("Connected to WiFi network with IP Address: ");
  tft.println(WiFi.localIP());
  delay(2000);
  tft.fillScreen(ST77XX_BLACK);

    tft.fillRect(0, 0, 18, 128, WHITE );
    tft.fillRect(18, 0, 37, 128, YELLOW);
    tft.fillRect(37, 0, 56, 128, CYAN);
    tft.fillRect(56, 0, 75, 128, GREEN);
    tft.fillRect(75, 0, 94, 128, MAGENTA);
    tft.fillRect(94, 0, 113, 128, RED);
    tft.fillRect(113, 0, 128, 114, BLUE);
    tft.fillRect(0, 114, 128, 14, BLACK);
    //////////////
    //lcdTestPattern();
    delay(2000);

}

void loop() {
  //buttonState = digitalRead(buttonPin);
  // Send an HTTP GET request
  //  if (buttonState == HIGH) {
    //tft.println("Button was pressed");
    //////////////
    /*
    tft.fillRect(0, 0, 18, 128, WHITE );
    tft.fillRect(18, 0, 37, 128, YELLOW);
    tft.fillRect(37, 0, 56, 128, CYAN);
    tft.fillRect(56, 0, 75, 128, GREEN);
    tft.fillRect(75, 0, 94, 128, MAGENTA);
    tft.fillRect(94, 0, 113, 128, RED);
    tft.fillRect(113, 0, 128, 114, BLUE);
    tft.fillRect(0, 114, 128, 14, BLACK);
    //////////////
    //lcdTestPattern();
    delay(1200);
    */
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + state + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

      int Kelvin = myObject["main"]["temp"];
      int FTemp = (((Kelvin - 273.15) * 9) / 5) + 32;

      int KelvinMIN = myObject["main"]["temp_min"];
      int FTemp_MIN = (((KelvinMIN - 273.15) * 9) / 5) + 32;

      int KelvinMAX = myObject["main"]["temp_max"];
      int FTemp_MAX = (((KelvinMAX - 273.15) * 9) / 5) + 32;
      int Pressure = myObject["main"]["pressure"];
      int Humidity = myObject["main"]["humidity"];
      int WindSpeed = myObject["wind"]["speed"];
      //char test = myObject["weather"];
     
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.println(JSON.typeof(myObject));
      Serial.print("Temperature: ");
      Serial.println(FTemp);
      Serial.print("Temperature MIN: ");
      Serial.println(FTemp_MIN);
      Serial.print("Temperature MAX: ");
      Serial.println(FTemp_MAX);
      Serial.print("Pressure: ");
      Serial.println(Pressure);
      Serial.print("Humidity: ");
      Serial.println(Humidity);
      Serial.print("Wind Speed: ");
      Serial.println(WindSpeed);
      Serial.print("Weather description: ");
      Serial.println(myObject["weather"]);

      String SFTemp = String(FTemp);
      String SFTemp_MIN = String(FTemp_MIN);
      String SFTemp_MAX = String(FTemp_MAX);
      String SPressure = String(Pressure);
      String SHumidity = String(Humidity);
      String SWindSpeed = String(WindSpeed);
      //probably should have made a function for this, but guess what, I didnt, so deal with it.
      tft.fillScreen(BLACK);
      tft.setTextSize(3);
      tft.setTextColor(GREEN);
      tft.setCursor(30, 0);
      tft.print("Boston, MA");
      tft.setCursor(100, 24);
      tft.setTextSize(3);
      tft.setTextColor(YELLOW);
      tft.print(SFTemp);
      tft.setTextSize(2);
      tft.setCursor(134, 21);
      tft.print("o");      
      tft.setCursor(0,50);
      tft.setTextSize(2);
      tft.setTextColor(BLUE);
      tft.print(" Minimum Temp: " + SFTemp_MIN);
      tft.setCursor(0,66);
      tft.setTextSize(2);
      tft.setTextColor(BLUE);
      tft.print(" Maximum Temp: " + SFTemp_MAX);
      tft.setCursor(0,82);
      tft.setTextSize(2);
      tft.setTextColor(BLUE);
      tft.print(" Pressure: " + SPressure + "hPa");
      tft.setCursor(0, 98);
      tft.setTextSize(2);
      tft.setTextColor(BLUE);
      tft.print(" Humidity: " + SHumidity + "%");
      tft.setCursor(0, 114);
      tft.setTextSize(2);
      tft.setTextColor(BLUE);
      tft.print(" Wind Speed: " + SWindSpeed + "mph");
      

    }
    else {
      tft.println("WiFi Disconnected");
    }
    
  }
//}

String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;

  delay(1000);
}