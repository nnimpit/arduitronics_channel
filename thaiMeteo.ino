// ตัวอย่างการใช้งาน Arduino MKR 1010 Wifi ดึงข้อมูลจาก website แบบ SSL
// โดย Arduitronics.com  :  Mountain A
// Data from https://www.tmd.go.th/en/province.php?StationNumber=48455

/*______Import Libraries_______*/
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
/*______End of Libraries_______*/

/*__Pin definitions for the Arduino MKR__*/
#define TFT_CS   5
#define TFT_DC   2

/*_______End of definitions______*/
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, 8, 9, 6, 7);


/*____Calibrate Touchscreen_____*/
#define MINPRESSURE 10      // minimum required force for touch event
#define TS_MINX 370
#define TS_MINY 470
#define TS_MAXX 3700
#define TS_MAXY 3600
/*______End of Calibration______*/


/*____Wifi _____________________*/
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "GT TECH_2.4G";        // your network SSID (name)
char pass[] = "48044993255";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 2000; 
/*______End of Wifi______*/


int status = WL_IDLE_STATUS;
int infected=0;
int recovered=0;
int deaths=0;


WiFiSSLClient client;
HttpClient http(client,"www.tmd.go.th", 443); 


void setup() {
  //Initialize serial and wait for port to open:
  delay(1000);
  Serial.begin(9600);
 
  // init GPIOs
  // pinMode(TFT_LED, OUTPUT); // define as output for backlight control

  // initialize the TFT
  Serial.println("Init TFT ...");
  tft.begin();          
  tft.setRotation(3);   // landscape mode  
  tft.fillScreen(ILI9341_BLACK);// clear screen 

  tft.setCursor(70,110);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Connecting...");
  //digitalWrite(TFT_LED, LOW);    // LOW to turn backlight on; 
  delay(3000);
  tft.fillScreen(ILI9341_BLACK);// clear screen 
  tft.setCursor(70,90);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextSize(2);
  tft.print("by ARDUITRONICS");
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(30,120);
  tft.print("data by www.tmd.go.th");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");

}
// <TR><TD>RH</TD><TD>
// strokeme'>
//Dew Point</TD><TD>

//<td class='PRH'>&nbsp;

void loop() {
 tft.fillScreen(ILI9341_BLACK);
 
 tft.setCursor(10,10);
 tft.setTextColor(ILI9341_WHITE);
 tft.setTextSize(2);
 tft.print("Measured Time:");
 check_meteo("","<td class='PRH'>&nbsp;",30);  // Date and time

 check_meteo("","<a href='province.php?StationNumber=48455'>",60); // Location
 //delay(2000);
 check_meteo("Temp","strokeme'>",90);
 //delay(2000);
 check_meteo("Dew Point","Dew Point</TD><TD>",120);
 //delay(2000);
 check_meteo("RH","<TR><TD>RH</TD><TD>",150);
 //delay(2000);
 check_meteo("Wind","Wind</TD><TD>",180);
 delay(2000);
 tft.fillScreen(ILI9341_BLACK);
 check_meteo("Cloud","Cloud</TD><TD>",20);
 //delay(2000);
 check_meteo("Visibility","Visibility</TD><TD>",60);
 //delay(2000);
 check_meteo("Pressure","Pressure</TD><TD>",100);
 //delay(2000);
 check_meteo("Rain 3 hr","Rain 3 hr</TD><TD>",140);
 // delay(2000);
}

/*
void draw_country_screen(String sCountry){
  tft.fillScreen(ILI9341_BLACK);// clear screen

  // headline
  tft.setCursor(5,10);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.print(sCountry + ":");

  // infected
  tft.setCursor(5,70);
  tft.setTextColor(ILI9341_RED);
  tft.print("Infected:");
  tft.setCursor(190,70);
  tft.print(infected);

  // recovered
  tft.setCursor(5,130);
  tft.setTextColor(ILI9341_GREEN);
  tft.print("Recovered:");
  tft.setCursor(190,130);
  tft.print(recovered);

  // deaths
  tft.setCursor(5,190);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.print("Deaths:");
  tft.setCursor(190,190);
  tft.print(deaths);
}
*/

void check_meteo(String readv, String sCountry, int row) {
  int err =0;
  int readcounter = 0;
  int read_value_step = 0;
  String s1 = "";
  String s2 = "";
  String s3 = "";
  
  int stringl = sCountry.length();
  
  err = http.get("/en/province.php?id=37");
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      Serial.print("Request data for: ");
      Serial.println(readv);
    
      // Now we've got to the body, so we can print it out
      unsigned long timeoutStart = millis();
      char c;
      // Whilst we haven't timed out & haven't reached the end of the body
      while ( (http.connected() || http.available()) &&
             (!http.endOfBodyReached()) &&
             ((millis() - timeoutStart) < kNetworkTimeout) )
      {
          if (http.available())
          {
              c = http.read();
              s2 = s2 + c;
              if (readcounter < 300) {
                readcounter++;
              } else {
                readcounter = 0;
                String tempString = "";
                tempString.concat(s1);
                tempString.concat(s2);
                // check infected first 
                if (read_value_step == 0) {                               
                  int place = tempString.indexOf(sCountry);
                  if ((place != -1) && (place < 350)) { 
                    read_value_step = 1;
                    //s2 = tempString.substring(place + 10,place+15);  
                    s2 = tempString.substring(place + stringl,place+stringl+25);
                    int place2 = s2.indexOf("</");
                    s3 = s2.substring(0,place2);
                    s3.replace("&deg;C"," 'C");
                    s3.replace("</","");
                    
                    Serial.println();
                    Serial.println("-------------------");
                    Serial.println(place);
                    Serial.print(readv+ ": ");
                    //Serial.println(s2);
                    Serial.println(s3);
                    Serial.println("-------------------");
                    
                    //tft.fillScreen(ILI9341_BLACK);
                    tft.setCursor(5,10+row);
                    tft.setTextColor(ILI9341_GREEN);
                    tft.setTextSize(2);
                    tft.print(readv + ":" + s3);
                    delay(2000);
  
                    read_value_step++;
                  }
                }
                s1 = s2;
                s2 = ""; 
                s3 = "";
              }              
              
              // We read something, reset the timeout counter
              timeoutStart = millis();
          }
          else
          {
              // We haven't got any data, so let's pause to allow some to
              // arrive
              delay(kNetworkDelay);
          }
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
