/*
        A part for this code was created from the information of D L Bird 2019
                          See more at http://dsbird.org.uk
           https://www.youtube.com/watch?v=RTKQMuWPL5A&ab_channel=G6EJD-David

          Some routines were created from the information of Andreas Spiess
                        https://www.sensorsiot.org/
                  https://www.youtube.com/c/AndreasSpiess/

                         --- Audio information ---
                   >>> Download: Game_Audio Library <<<
   http://www.buildlog.net/blog/wp-content/uploads/2018/02/Game_Audio.zip
     https://www.buildlog.net/blog/2018/02/game-audio-for-the-esp32/

                  >>> Download: XT_DAC_Audio Library <<<
     https://www.xtronical.com/wp-content/uploads/2018/03/XT_DAC_Audio.zip
 http://www.xtronical.com/basics/audio/digitised-speech-sound-esp32-playing-wavs/
        https://www.xtronical.com/basics/audio/playing-wavs-updated/

                                 --- oOo ---
                                
                              Modified by: J_RPM
                               http://j-rpm.com/
                        https://www.youtube.com/c/JRPM
                (v1.44) Two time zones, 2 alarms, voice prompts
                UTF-8 Spanish characters and Pac-Man animations
                              >>> March of 2021 <<<
                  
               An OLED display is added to show the Time an Date,
                adding some new routines and modifying others.

                              >>> HARDWARE <<<
                  LIVE D1 mini ESP32 ESP-32 WiFi + Bluetooth
                https://es.aliexpress.com/item/32816065152.html
                
            HW-699 0.66 inch OLED display module, for D1 Mini (64x48)
               https://es.aliexpress.com/item/4000504485892.html

             4x Led Display Control Modules 8x8 MAX7219 (ROTATE 0)
                https://es.aliexpress.com/item/33038259447.html

            8x8 MAX7219 Led Control Module, 4 in one screen  (ROTATE 90)
               https://es.aliexpress.com/item/4001296969309.html

                           >>> IDE Arduino <<<
                        Model: WEMOS MINI D1 ESP32
       Add URLs: https://dl.espressif.com/dl/package_esp32_index.json
                  https://github.com/espressif/arduino-esp32
                                
 ____________________________________________________________________________________
*/
///////////////////////////////////////////////////////////
// ROTATE 90 >>> Two time zones, 2 alarms, voice prompts //
//    UTF-8 Spanish characters and Pac-Man animations     //
///////////////////////////////////////////////////////////
String HWversion = "(v1.44)"; 
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <time.h>
#include "Game_Audio.h";
#include "SoundData.h";

Game_Audio_Class GameAudio(26,0); 
Game_Audio_Wav_Class waw0(w0); 
Game_Audio_Wav_Class waw1(w1); 
Game_Audio_Wav_Class waw2(w2); 
Game_Audio_Wav_Class waw3(w3); 
Game_Audio_Wav_Class waw4(w4); 
Game_Audio_Wav_Class waw5(w5); 
Game_Audio_Wav_Class waw6(w6); 
Game_Audio_Wav_Class waw7(w7); 
Game_Audio_Wav_Class waw8(w8); 
Game_Audio_Wav_Class waw9(w9); 
Game_Audio_Wav_Class wawMin(Minutos); 
Game_Audio_Wav_Class wawHor(Horas); 
Game_Audio_Wav_Class wawSon(sonLas); 
Game_Audio_Wav_Class wawTone(Tone); 
Game_Audio_Wav_Class pmEat(Eating); 
Game_Audio_Wav_Class pmDeath(pacmanDeath);
Game_Audio_Wav_Class wawIP(wIP);
Game_Audio_Wav_Class wawPunto(wPunto);

#include <DNSServer.h>
#include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager
#include <Adafruit_GFX.h>
//*************************************************IMPORTANT******************************************************************
#include "Adafruit_SSD1306.h" // Copy the supplied version of Adafruit_SSD1306.cpp and Adafruit_ssd1306.h to the sketch folder
#define  OLED_RESET 0         // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

String CurrentTime, CurrentDate, nDay, webpage = "";
bool display_EU = true;
String zone1= "España";
String zone2= "Japan";
bool T_Zone2 = false;
bool display_msg = false;
bool on_txt = false;
bool pac = false;
bool sTime = false;
int matrix_speed = 25;

// Alarm #1
int alarm_H = 0;
int alarm_M = 0;
int alarm_R = 0;
String al_H = "00";
String al_M = "00";
String al_R = "0";
bool chgA1 = false;

// Alarm #2
int alarm_H2 = 0;
int alarm_M2 = 0;
int alarm_R2 = 0;
String al_H2 = "00";
String al_M2 = "00";
String al_R2 = "0";
bool chgA2 = false;

// Turn on debug statements to the serial output
#define DEBUG  1
#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// ESP32 -> Matrix -> ROTATE 90
#define DIN_PIN 32   // ROTATE 90
#define CS_PIN  25   // ROTATE 90   
#define CLK_PIN 27   // ROTATE 90

// ESP32 -> Matrix -> ROTATE 0
//#define DIN_PIN 27   //ROTATE 0
//#define CS_PIN  25   //ROTATE 0 
//#define CLK_PIN 32   //ROTATE 0

#define NUM_MAX 4
#include "max7219.h"
#include "fonts_es.h"

// Global message buffers shared by Wifi and Scrolling functions
#define BUF_SIZE  512
char curMessage[BUF_SIZE];
char newMessage[BUF_SIZE];
bool newMessageAvailable = false;
const char WebResponse[] = "HTTP/1.1 200 OK\nContent-Type: text/html\r\n";

// Define the number of bytes you want to access (first is index 0)
#define EEPROM_SIZE 14

// Size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   60
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer

////////////////////////// MATRIX //////////////////////////////////////////////
#define MAX_DIGITS 20
bool _scroll = false;
bool display_date = true;
bool animated_time = true;
bool show_seconds = false;
byte dig[MAX_DIGITS]={0};
byte digold[MAX_DIGITS]={0};
byte digtrans[MAX_DIGITS]={0};
int dots = 1;
long dotTime = 0;
long clkTime = 0;
int dx=0;
int dy=0;
byte del=0;
int h,m,s;
int dualChar = 0;
int brightness = 5;  //DUTY CYCLE: 11/32
String mDay;
long timeConnect;

WiFiClient client;
WiFiServer server(80);
WebServer server2(80); 

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  timeConnect = millis();

  // Load configuration values from EEPROM memory
  readConfig();

  initMAX7219();
  sendCmdAll(CMD_SHUTDOWN,1);
  sendCmdAll(CMD_INTENSITY,brightness);
  sendCmdAll(CMD_DISPLAYTEST, 0);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15,0);   
  display.println(F("NTP"));
  display.setCursor(6,16);   
  display.println(F("TIME"));

  display.setTextSize(1);   
  display.setCursor(10,32);   
  display.println(HWversion);
  display.setCursor(10,40);   
  display.println(F("Sync..."));
  display.display();
  
  String msg = "  NTP "; 
  if (T_Zone2==false) {
    msg = msg + zone1;    
  }else{
    msg = msg + zone2;    
  }
  msg = msg + "  " + HWversion;
  printStringWithShift(msg.c_str(), matrix_speed);
  delay(1000);

  //------------------------------
  //WiFiManager intialisation. Once completed there is no need to repeat the process on the current board
  WiFiManager wifiManager;
  display_AP_wifi();

  // A new OOB ESP32 will have no credentials, so will connect and not need this to be uncommented and compiled in, a used one will, try it to see how it works
  // Uncomment the next line for a new device or one that has not connected to your Wi-Fi before or you want to reset the Wi-Fi connection
  // wifiManager.resetSettings();
  // Then restart the ESP32 and connect your PC to the wireless access point called 'ESP32_AP' or whatever you call it below
  // Next connect to http://192.168.4.1/ and follow instructions to make the WiFi connection

  // Set a timeout until configuration is turned off, useful to retry or go to sleep in n-seconds
  wifiManager.setTimeout(180);
  
  //fetches ssid and password and tries to connect, if connections succeeds it starts an access point with the name called "ESP32_AP" and waits in a blocking loop for configuration
  if (!wifiManager.autoConnect("ESP32_AP")) {
    PRINTS("\nFailed to connect and timeout occurred");
    display_AP_wifi();
    display_flash(false);
    reset_ESP32();
  }
  // At this stage the WiFi manager will have successfully connected to a network,
  // or if not will try again in 180-seconds
  //---------------------------------------------------------
  // 
  PRINT("\n>>> Connection Delay(ms): ",millis()-timeConnect);
  if(millis()-timeConnect > 30000) {
    PRINTS("\nTimeOut connection, restarting!!!");
    reset_ESP32();
  }
  
  // Print the IP address
  PRINT("\nUse this URL to connect -> http://",WiFi.localIP());
  PRINTS("/");
  GameAudio.PlayWav(&wawTone, false, 1.0);
  display_ip();
  playIP();

  // Syncronize Time and Date
  SetupTime();

  // Select mode: TIME/MESSAGE
  checkServer();
  curMessage[0] = newMessage[0] = '\0';
 
  // Set up first message 
  String stringMsg = "ESP32_Time_Text_Matrix90_JR " + HWversion + " - RTC: ";
  if (T_Zone2 == false) {
    stringMsg = stringMsg + zone1;    
  }else{
    stringMsg = stringMsg + zone2;    
  }
  stringMsg = stringMsg + " - IP: " + WiFi.localIP().toString() + "\n";
  
  stringMsg.toCharArray(curMessage, BUF_SIZE);
  PRINT("\ncurMessage >>> ", curMessage);

  soundTime();
  delay(400);
  soundEnd();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void loop() {
  // Wait for a client to connect and when they do process their requests
  if (display_msg == false) {server2.handleClient();}else{handleWiFi();}

  // Update and refresh of the date and time on the displays
  if (millis() % 60000) UpdateLocalTime();

  Oled_Time();
  on_txt=false;
  matrix_time();

  // Serial display time and date & dots flashing
  if(millis()-dotTime > 500) {
    //PRINT("\n",CurrentTime);
    //PRINT("\n",mDay);
    dotTime = millis();
    dots = !dots;
  }
 
  if (display_msg == false) {
    
    // Show date on matrix display
    if (display_date == true) {
      if(millis()-clkTime > 30000 && !del && dots) { // clock for 30s, then scrolls Date 
        _scroll=true;
        on_txt=true; // Window of checking Alarm  
        Oled_Time();
        printStringWithShift((String("     ")+ mDay + "           ").c_str(), matrix_speed);
        clkTime = millis();
        _scroll=false;
      }
    }
  
  // Display MESSAGE
  }else {

    // Refresh message on matrix display
    if (newMessageAvailable) {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }

    if(millis()-clkTime > 30000 && !del && dots) { // clock for 30s, then scrolls personal message
      _scroll=true;
      on_txt=true; // Window of checking Alarm  
      Oled_Time();
      printStringWithShift((String("     ")+ curMessage + "           ").c_str(), matrix_speed);
      clkTime = millis();
      _scroll=false;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void readConfig(){
  // Initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);

  // 0 - Display Status 
  display_EU = EEPROM.read(0);
  PRINT("\ndisplay_EU: ",display_EU);

  // 1 - Display Date  
  display_date = EEPROM.read(1);
  PRINT("\ndisplay_date: ",display_date);
  
  // 2 - Matrix Brightness  
  brightness = EEPROM.read(2);
  if (brightness < 0 || brightness > 15) {
    brightness = 5;
    EEPROM.write(2, brightness);
  }
  PRINT("\nbrightness: ",brightness);
  sendCmdAll(CMD_INTENSITY,brightness);
  
  // 3 - Animated Time  
  animated_time = EEPROM.read(3);
  PRINT("\nanimated_time: ",animated_time);

  // 4 - Show Seconds  
  show_seconds = EEPROM.read(4);
  PRINT("\nshow_seconds: ",show_seconds);

  // 5 - Speed Matrix (delay)  
  matrix_speed = EEPROM.read(5);
  if (matrix_speed < 10 || matrix_speed > 40) {
    matrix_speed = 25;
    EEPROM.write(5, matrix_speed);
  }
  PRINT("\nmatrix_speed: ",matrix_speed);

  // 6 - Init Display whit Time/Message  
  display_msg = EEPROM.read(6);
  PRINT("\ndisplay_msg: ",display_msg);

  // 7 - Alarm #1 Hour  
  alarm_H = EEPROM.read(7);
  if (alarm_H < 0 || alarm_H > 23) {
    alarm_H = 0;
    EEPROM.write(7, alarm_H);
  }
  PRINT("\nalarm_H: ",alarm_H);

  // 8 - Alarm #1 Minute  
  alarm_M = EEPROM.read(8);
  if (alarm_M < 0 || alarm_M > 59) {
    alarm_M = 0;
    EEPROM.write(8, alarm_M);
  }
  PRINT("\nalarm_M: ",alarm_M);

  // 9 - Alarm #1 Repetitions  
  alarm_R = EEPROM.read(9);
  if (alarm_R < 0 || alarm_R > 8) {
    alarm_R = 0;
    EEPROM.write(9, alarm_R);
  }
  PRINT("\nalarm_R: ",alarm_R);

  // 10 - Alarm #2 Hour  
  alarm_H2 = EEPROM.read(10);
  if (alarm_H2 < 0 || alarm_H2 > 23) {
    alarm_H2 = 0;
    EEPROM.write(10, alarm_H2);
  }
  PRINT("\nalarm_H2: ",alarm_H2);

  // 11 - Alarm #2 Minute  
  alarm_M2 = EEPROM.read(11);
  if (alarm_M2 < 0 || alarm_M2 > 59) {
    alarm_M2 = 0;
    EEPROM.write(11, alarm_M2);
  }
  PRINT("\nalarm_M2: ",alarm_M2);

  // 12 - Alarm #2 Repetitions  
  alarm_R2 = EEPROM.read(12);
  if (alarm_R2 < 0 || alarm_R2 > 8) {
    alarm_R2 = 0;
    EEPROM.write(12, alarm_R2);
  }
  PRINT("\nalarm_R2: ",alarm_R2);

  // 13 - Time Zone
  T_Zone2 = EEPROM.read(13);
  PRINT("\nT_Zone2: ",T_Zone2);

  // Close EEPROM    
  EEPROM.commit();
  EEPROM.end();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Choose your time zone from: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv 
// See below for examples
// Or, choose a time server close to you, but in most cases it's best to use pool.ntp.org to find an NTP server
// then the NTP system decides e.g. 0.pool.ntp.org, 1.pool.ntp.org as the NTP syem tries to find  the closest available servers
// EU "0.europe.pool.ntp.org"
// US "0.north-america.pool.ntp.org"
// See: https://www.ntppool.org/en/                                                           
// UK normal time is GMT, so GMT Offset is 0, for US (-5Hrs) is typically -18000, AU is typically (+8hrs) 28800
// In the UK DST is +1hr or 3600-secs, other countries may use 2hrs 7200 or 30-mins 1800 or 5.5hrs 19800 Ahead of GMT use + offset behind - offset
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean SetupTime() {
  char* Timezone;
  char* ntpServer;
  int gmtOffset_sec;
  int daylightOffset_sec;
  
  // Select Time Zone (Spain/Japan)
  if (T_Zone2 == false) {
    Timezone= "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"; 
    ntpServer= "es.pool.ntp.org";
    gmtOffset_sec= 0;
    daylightOffset_sec= 7200;
  }else{
    Timezone= "UTC-9";  
    ntpServer= "ntp.nict.jp"; 
    gmtOffset_sec= 0;
    daylightOffset_sec= 0;
  }
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.nist.gov");  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  setenv("TZ", Timezone, 1);                                            // setenv()adds the "TZ" variable to the environment with a value TimeZone, only used if set to 1, 0 means no change
  tzset();                                                                    // Set the TZ environment variable
  delay(1000);
  bool TimeStatus = UpdateLocalTime();
  return TimeStatus;
}
//////////////////////////////////////////////////////////////////////////////
boolean UpdateLocalTime() {
  struct tm timeinfo;
  time_t now;
  time(&now);

  //See http://www.cplusplus.com/reference/ctime/strftime/
  // %w >>> Weekday as a decimal number with Sunday as 0 (0-6)
  String esWDay[7] = {"Domingo","Lunes","Martes","Miércoles","Jueves","Viernes","Sábado"};
  String esMonth[13] = {"Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre"};
  String esDate;
  char output[30];
  
  if (T_Zone2 == false && display_EU == true) {
    strftime(output, 30, "%w", localtime(&now));
    mDay = esWDay[atoi(output)];
  }else {
    strftime(output, 30, "%A", localtime(&now));
    mDay = output;
  }
  strftime(output, 30, "%a. ", localtime(&now));
  nDay = output; 
  
  if (display_EU == true) {
    strftime(output, 30,"%d-%m", localtime(&now));
    CurrentDate = nDay + output;
    //%m  Month as a decimal number (01-12)
    if (T_Zone2 == false) {
      strftime(output, 30,", %d", localtime(&now));
      esDate = mDay + output;
      strftime(output, 30,"%m", localtime(&now));
      mDay = esDate + " de " + esMonth[atoi(output)-1];
      strftime(output, 30," de %Y", localtime(&now));
    }else {
      strftime(output, 30,", %d %B %Y", localtime(&now));
    }
    mDay = mDay + output;
    strftime(output, 30, "%H:%M:%S", localtime(&now));
    CurrentTime = output;
  }
  else { 
    strftime(output, 30, "%m-%d", localtime(&now));
    CurrentDate = nDay + output;
    strftime(output, 30, ", %B %d, %Y", localtime(&now));
    mDay = mDay + output;
    strftime(output, 30, "%r", localtime(&now));
    CurrentTime = output;
  }
  return true;
}
/////////////////////////////////////////////////////
//------------------ OLED DISPLAY -----------------//
/////////////////////////////////////////////////////
void Oled_Time() { 
  display.clearDisplay();
  display.setCursor(2,0);   // center date display
  display.setTextSize(1);   
  display.println(CurrentDate);

  display.setTextSize(2);   
  display.setCursor(8,16);  // center Time display
  if (CurrentTime.startsWith("0")){
    display.println(CurrentTime.substring(1,5));
  }else {
    display.setCursor(0,16);
    display.println(CurrentTime.substring(0,5));
  }
  
  if (display_EU == true) {
    display.setCursor(7,33); // center Time display
    if (_scroll) {
      display.print("----");
    }else{
      display.print("(" + CurrentTime.substring(6,8) + ")");
    }
  }else {
    if (_scroll) {
      display.print("----");
    }else{
      display.print("(" + CurrentTime.substring(6,8) + ")");
    }
    display.setTextSize(1);
    display.setCursor(40,33); 
    display.print(CurrentTime.substring(8,11));
  }
  display.display();

  // Load Time and check Alarms
  h = (CurrentTime.substring(0,2)).toInt();
  m = (CurrentTime.substring(3,5)).toInt();
  s = (CurrentTime.substring(6,8)).toInt();
  checkAlarm();
}
/////////////////////////////////////////////////////
//------------------ MATRIX DISPLAY ---------------//
/////////////////////////////////////////////////////
void matrix_time() {
  if (show_seconds == true) {
    if (animated_time == true) {
      showAnimSecClock(); //With ROTATE 90 send 1 times + delay(4)
      delay(4);
      //showAnimSecClock(); //With ROTATE 0 send 4 times, without delay
      //showAnimSecClock();
      //showAnimSecClock();
    }else {
      showSecondsClock();
    }
  } else {
    if (animated_time == true) {
      showAnimClock();  //With ROTATE 90 send 1 times + delay(4)
      delay(4);
      //showAnimClock();  //With ROTATE 0 send 4 times, without delay
      //showAnimClock();
      //showAnimClock();
    }else {
      showSimpleClock();
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
// A short method of adding the same web page header to some text
//////////////////////////////////////////////////////////////////////////////
void append_webpage_header() {
  // webpage is a global variable
  webpage = ""; // A blank string variable to hold the web page
  webpage += "<!DOCTYPE html><html>"; 
  webpage += "<meta charset=\"utf-8\">";
  webpage += "<style>html { font-family:tahoma; display:inline-block; margin:0px auto; text-align:center;}";
  
  webpage += "#mark      {border: 5px solid #316573 ; width: 1120px;} "; 
  webpage += "#header    {background-color:#C3E0E8; width:1100px; padding:10px; color:#13414E; font-size:36px;}";
  webpage += "#section   {background-color:#E6F5F9; width:1100px; padding:10px; color:#0D7693 ; font-size:14px;}";
  webpage += "#footer    {background-color:#C3E0E8; width:1100px; padding:10px; color:#13414E; font-size:24px; clear:both;}";

  webpage += ".content-select select::-ms-expand {display: none;}";
  webpage += ".content-input input,.content-select select{appearance: none;-webkit-appearance: none;-moz-appearance: none;}";
  webpage += ".content-select select{background-color:#C3E0E8; width:15%; padding:10px; color:#0D7693 ; font-size:48px ; border:6px solid rgba(0,0,0,0.2) ; border-radius: 24px;}";
   
  webpage += ".button {box-shadow: 0px 10px 14px -7px #276873; background:linear-gradient(to bottom, #599bb3 5%, #408c99 100%);";
  webpage += "background-color:#599bb3; border-radius:8px; color:white; padding:13px 32px; display:inline-block; cursor:pointer;";
  webpage += "text-decoration:none;text-shadow:0px 1px 0px #3d768a; font-size:50px; font-weight:bold; margin:2px;}";
  webpage += ".button:hover {background:linear-gradient(to bottom, #408c99 5%, #599bb3 100%); background-color:#408c99;}";
  webpage += ".button:active {position:relative; top:1px;}";
 
  webpage += ".button2 {box-shadow: 0px 10px 14px -7px #8a2a21; background:linear-gradient(to bottom, #f24437 5%, #c62d1f 100%);";
  webpage += "background-color:#f24437; text-shadow:0px 1px 0px #810e05; }";
  webpage += ".button2:hover {background:linear-gradient(to bottom, #c62d1f 5%, #f24437 100%); background-color:#f24437;}";
  
  webpage += ".line {border: 3px solid #666; border-radius: 300px/10px; height:0px; width:80%;}";
  
  webpage += "input[type=\"text\"] {font-size:42px; width:90%; text-align:left;}";
  
  webpage += "input[type=range]{height:61px; -webkit-appearance:none;  margin:10px 0; width:70%;}";
  webpage += "input[type=range]:focus {outline:none;}";
  webpage += "input[type=range]::-webkit-slider-runnable-track {width:70%; height:30px; cursor:pointer; animate:0.2s; box-shadow: 2px 2px 5px #000000; background:#C3E0E8;border-radius:10px; border:1px solid #000000;}";
  webpage += "input[type=range]::-webkit-slider-thumb {box-shadow:3px 3px 6px #000000; border:2px solid #FFFFFF; height:50px; width:50px; border-radius:15px; background:#316573; cursor:pointer; -webkit-appearance:none; margin-top:-11.5px;}";
  webpage += "input[type=range]:focus::-webkit-slider-runnable-track {background: #C3E0E8;}";
  webpage += "</style>";
 
  webpage += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/animate.css/4.1.1/animate.min.css\"/>";
  webpage += "<html><head><title>ESP32 NTP Clock</title>";
  webpage += "</head>";

  webpage += "<script>";
  /////////////////////////////////
  webpage += "function GetAlarm()";
  webpage += "{";
  webpage += "  strHour = \"\";";
  webpage += "  strMin = \"\";";
  webpage += "  strRepe = \"\";";
  webpage += "  nocache = \"/&nocache=\" + Math.random() * 1000000;";
  webpage += "  var request = new XMLHttpRequest();";
  webpage += "  var sel_hour = document.getElementById(\"selHour\");";   // get the values from the drop-down select boxes
  webpage += "  if (sel_hour.selectedIndex > 0) {";
  webpage += "  strHour = \"&h=\" + sel_hour.options[sel_hour.selectedIndex].text;";
  webpage += "  } else {";
  webpage += "  strHour = \"\";}";  // the first item in the drop-down box is invalid text, so clear the string

  webpage += "  var sel_min = document.getElementById(\"selMin\");";
  webpage += "  if (sel_min.selectedIndex > 0) {";
  webpage += "  strMin = \"/&m=\" + sel_min.options[sel_min.selectedIndex].text;";
  webpage += "  } else {";
  webpage += "  strMin = \"\";}";  // the first item in the drop-down box is invalid text, so clear the string
  
  webpage += "  var sel_rep = document.getElementById(\"selRep\");";
  webpage += "  if (sel_rep.selectedIndex > 0) {";
  webpage += "  strRepe = \"/&r=\" + sel_rep.options[sel_rep.selectedIndex].text;";
  webpage += "  } else {";
  webpage += "  strRepe = \"\";}";  // the first item in the drop-down box is invalid text, so clear the string
  
  webpage += "  request.open(\"GET\", strHour + strMin + strRepe + nocache, false);";
  webpage += "  request.send(null);";
  webpage += "}";
  /////////////////////////////////
  webpage += "function GetAlarm2()";
  webpage += "{";
  webpage += "  strHour = \"\";";
  webpage += "  strMin = \"\";";
  webpage += "  strRepe = \"\";";
  webpage += "  nocache = \"/&nocache=\" + Math.random() * 1000000;";
  webpage += "  var request = new XMLHttpRequest();";
  webpage += "  var sel_hour = document.getElementById(\"selHour2\");";   // get the values from the drop-down select boxes
  webpage += "  if (sel_hour.selectedIndex > 0) {";
  webpage += "  strHour = \"&h2=\" + sel_hour.options[sel_hour.selectedIndex].text;";
  webpage += "  } else {";
  webpage += "  strHour = \"\";}";  // the first item in the drop-down box is invalid text, so clear the string

  webpage += "  var sel_min = document.getElementById(\"selMin2\");";
  webpage += "  if (sel_min.selectedIndex > 0) {";
  webpage += "  strMin = \"/&m2=\" + sel_min.options[sel_min.selectedIndex].text;";
  webpage += "  } else {";
  webpage += "  strMin = \"\";}";  // the first item in the drop-down box is invalid text, so clear the string
  
  webpage += "  var sel_rep = document.getElementById(\"selRep2\");";
  webpage += "  if (sel_rep.selectedIndex > 0) {";
  webpage += "  strRepe = \"/&r2=\" + sel_rep.options[sel_rep.selectedIndex].text;";
  webpage += "  } else {";
  webpage += "  strRepe = \"\";}";  // the first item in the drop-down box is invalid text, so clear the string
  
  webpage += "  request.open(\"GET\", strHour + strMin + strRepe + nocache, false);";
  webpage += "  request.send(null);";
  webpage += "}";
  /////////////////////////////////
  webpage += "function SendData()";
  webpage += "{";
  webpage += "  strLine = \"\";";
  webpage += "  nocache = \"/&nocache=\" + Math.random() * 1000000;";
  webpage += "  var request = new XMLHttpRequest();";
  webpage += "  strLine = \"&MSG=\" + document.getElementById(\"data_form\").Message.value;";
  webpage += "  strLine = strLine + \"/&SP=\" + document.getElementById(\"data_form\").Speed.value;";
  webpage += "  request.open(\"GET\", strLine + nocache, false);";
  webpage += "  request.send(null);";
  webpage += "}";
  webpage += "function SendBright()";
  webpage += "{";
  webpage += "  strLine = \"\";";
  webpage += "  var request = new XMLHttpRequest();";
  webpage += "  strLine = \"BRIGHT=\" + document.getElementById(\"bright_form\").Bright.value;";
  webpage += "  request.open(\"GET\", strLine, false);";
  webpage += "  request.send(null);";
  webpage += "}";
  /////////////////////////////////
  webpage += "</script>";
  webpage += "<div id=\"mark\">";
  webpage += "<div id=\"header\"><h1 class=\"animate__animated animate__flash\">NTP - Local Time Clock " + HWversion + "</h1>";
}
//////////////////////////////////////////////////////////////////////////////
void button_Home() {
  webpage += "<p><a href=\"\\HOME\"><type=\"button\" class=\"button\">Refresh WEB</button></a></p>";
}
//////////////////////////////////////////////////////////////////////////////
const String format2(int num){
  String f2 = String(num);
  f2 = "0" + f2;
  f2 = f2.substring(f2.length() - 2, f2.length());
  return f2;
}
//////////////////////////////////////////////////////////////////////////////
void WebAlarm(){
  webpage += "<p>EU - 1# Alarm Time -> ";
  al_H = format2(alarm_H);
  webpage += al_H;
  webpage += ":";
  al_M = format2(alarm_M);
  webpage += al_M;
  if (alarm_R == 0) {
    webpage += " (OFF";
  }else {
    webpage += " (R:";
    webpage += alarm_R;
  }
  webpage += ")</p>";
  //--------------------
  webpage += "<p>EU - 2# Alarm Time -> ";
  al_H2 = format2(alarm_H2);
  webpage += al_H2;
  webpage += ":";
  al_M2 = format2(alarm_M2);
  webpage += al_M2;
  if (alarm_R2 == 0) {
    webpage += " (OFF";
  }else {
    webpage += " (R:";
    webpage += alarm_R2;
  }
  webpage += ")</p>";
}
//////////////////////////////////////////////////////////////////////////////
void ESP32_set_message() {
  client = server.available();
  append_webpage_header();
  webpage += "<h5 class=\"animate__animated animate__fadeInLeft\"><p>MSG: ";
  if (newMessageAvailable) {
    webpage += newMessage;
  }else{
    webpage += curMessage;
  }
  webpage += "</p>";
  webpage += "<p>Speed: ";
  webpage += matrix_speed;
  webpage += " ms. delay</p>";

  WebAlarm();
  
  webpage += "</h5></div>";
  webpage += "<div id=\"section\">";
  webpage += "<form id=\"data_form\" name=\"frmText\">";
  webpage += "<a>NEW message<br><input type=\"text\" name=\"Message\" maxlength=\"255\"><br><br>";
  webpage += "Speed (ms. delay)<br>Fast(10)<input type=\"range\" name=\"Speed\" min=\"10\" max=\"40\" value=\"";
  webpage += matrix_speed;
  webpage += "\">(40)Slow</a>";
  webpage += "<br><br>";
  webpage += "<p><a href=\"\\SendData\"><type=\"button\" onClick=\"SendData()\" class=\"button\">Send Data</button></a></p>";

  webpage += "<br><hr class=\"line\"><br>";
  insert_Alarm();
  webpage += "<br><p>";
  button_PacMan();
  webpage += "<a href=\"\\Sound_Time\"><type=\"button\" class=\"button\">Sound TIME</button></a>";
  webpage += "<a href=\"\\CLOCK_TOGGLE\"><type=\"button\" class=\"button\">CLOCK</button></a></p>";
  
  webpage += "<br></form>";
  webpage += "</div>"; 
  webpage += "<div id=\"footer\">Copyright &copy; J_RPM 2021</div></div></html>\r\n";
  client.println(WebResponse);
  client.println(webpage);
  PRINTS("\n>>> ESP32_set_message() OK! ");
}
//////////////////////////////////////////////////////////////////////////////
void button_PacMan() {
  webpage += "<a href=\"\\sPAC\"><type=\"button\" class=\"button\">Pac-Man</button></a>";
}
//////////////////////////////////////////////////////////////////////////////
void insert_Alarm() {
  webpage += "<div class=\"content-select\">";
  // Alarm #1
  webpage += "<p><select id=\"selHour\">";
  selH();
  webpage += "<select id=\"selMin\">";
  selM();
  webpage += "<select id=\"selRep\">";
  selR();
  webpage += "<a href=\"\\Alarm1\"><type=\"button\" onClick=\"GetAlarm()\" class=\"button\">SAVE Alarm #1</button></a></p>";
  // Alarm #2
  webpage += "<p><select id=\"selHour2\">";
  selH();
  webpage += "<select id=\"selMin2\">";
  selM();
  webpage += "<select id=\"selRep2\">";
  selR();
  webpage += "<a href=\"\\Alarm2\"><type=\"button\" onClick=\"GetAlarm2()\" class=\"button\">SAVE Alarm #2</button></a></p>";
  webpage += "</div>";
}
//////////////////////////////////////////////////////////////////////////////
void selH(){
  webpage += "<option selected=\"selected\" value=\"\">HOUR</option>";
  sel23();
  webpage += "</select>";
}
//////////////////////////////////////////////////////////////////////////////
void sel23(){
  webpage += "<option value=\"00\">00</option>";
  webpage += "<option value=\"01\">01</option>";
  webpage += "<option value=\"02\">02</option>";
  webpage += "<option value=\"03\">03</option>";
  webpage += "<option value=\"04\">04</option>";
  webpage += "<option value=\"05\">05</option>";
  webpage += "<option value=\"06\">06</option>";
  webpage += "<option value=\"07\">07</option>";
  webpage += "<option value=\"08\">08</option>";
  webpage += "<option value=\"09\">09</option>";
  webpage += "<option value=\"10\">10</option>";
  webpage += "<option value=\"11\">11</option>";
  webpage += "<option value=\"12\">12</option>";
  webpage += "<option value=\"13\">13</option>";
  webpage += "<option value=\"14\">14</option>";
  webpage += "<option value=\"15\">15</option>";
  webpage += "<option value=\"16\">16</option>";
  webpage += "<option value=\"17\">17</option>";
  webpage += "<option value=\"18\">18</option>";
  webpage += "<option value=\"19\">19</option>";
  webpage += "<option value=\"20\">20</option>";
  webpage += "<option value=\"21\">21</option>";
  webpage += "<option value=\"22\">22</option>";
  webpage += "<option value=\"23\">23</option>";
}
//////////////////////////////////////////////////////////////////////////////
void selM(){
  webpage += "<option selected=\"selected\" value=\"\">MINUTE</option>";
  sel23();
  webpage += "<option value=\"24\">24</option>";
  webpage += "<option value=\"25\">25</option>";
  webpage += "<option value=\"26\">26</option>";
  webpage += "<option value=\"27\">27</option>";
  webpage += "<option value=\"28\">28</option>";
  webpage += "<option value=\"29\">29</option>";
  webpage += "<option value=\"30\">30</option>";
  webpage += "<option value=\"31\">31</option>";
  webpage += "<option value=\"32\">32</option>";
  webpage += "<option value=\"33\">33</option>";
  webpage += "<option value=\"34\">34</option>";
  webpage += "<option value=\"35\">35</option>";
  webpage += "<option value=\"36\">36</option>";
  webpage += "<option value=\"37\">37</option>";
  webpage += "<option value=\"38\">38</option>";
  webpage += "<option value=\"39\">39</option>";
  webpage += "<option value=\"40\">40</option>";
  webpage += "<option value=\"41\">41</option>";
  webpage += "<option value=\"42\">42</option>";
  webpage += "<option value=\"43\">43</option>";
  webpage += "<option value=\"44\">44</option>";
  webpage += "<option value=\"45\">45</option>";
  webpage += "<option value=\"46\">46</option>";
  webpage += "<option value=\"47\">47</option>";
  webpage += "<option value=\"48\">48</option>";
  webpage += "<option value=\"49\">49</option>";
  webpage += "<option value=\"50\">50</option>";
  webpage += "<option value=\"51\">51</option>";
  webpage += "<option value=\"52\">52</option>";
  webpage += "<option value=\"53\">53</option>";
  webpage += "<option value=\"54\">54</option>";
  webpage += "<option value=\"55\">55</option>";
  webpage += "<option value=\"56\">56</option>";
  webpage += "<option value=\"57\">57</option>";
  webpage += "<option value=\"58\">58</option>";
  webpage += "<option value=\"59\">59</option>";
  webpage += "</select>";
}
//////////////////////////////////////////////////////////////////////////////
void selR(){
  webpage += "<option selected=\"selected\" value=\"\">REPETITIONS</option>";
  webpage += "<option value=\"0\">0</option>";
  webpage += "<option value=\"1\">1</option>";
  webpage += "<option value=\"2\">2</option>";
  webpage += "<option value=\"3\">3</option>";
  webpage += "<option value=\"4\">4</option>";
  webpage += "<option value=\"5\">5</option>";
  webpage += "<option value=\"6\">6</option>";
  webpage += "<option value=\"7\">7</option>";
  webpage += "<option value=\"8\">8</option>";
  webpage += "</select>";
}
//////////////////////////////////////////////////////////////////////////////
void NTP_Clock_home_page() {
  append_webpage_header();
  //webpage += "<p><h5 class=\"animate__animated animate__fadeInLeft\">Display Mode is ";
  webpage += "<p><h5 class=\"animate__animated animate__fadeInLeft\">RTC: ";
  if (T_Zone2 == false) webpage += zone1; else webpage += zone2;
  webpage += " - ";
  if (display_EU == true) webpage += "EU"; else webpage += "USA";
  webpage += " mode</p>[";
  if (animated_time == true) webpage += "Animated "; else webpage += "Normal ";
  if (show_seconds == true) webpage += "(hh:mm:ss) "; else webpage += "(HH:MM) ";
  if (display_date == true) webpage += "& Date ";
  webpage += "- B:";     
  webpage += brightness;
  webpage += "]";
  WebAlarm();
  webpage += "</h5></div>";

  webpage += "<div id=\"section\">";
  button_Home();
  webpage += "<p><a href=\"\\DISPLAY_MODE_USA\"><type=\"button\" class=\"button\">USA mode</button></a>";
  webpage += "<a href=\"\\DISPLAY_MODE_EU\"><type=\"button\" class=\"button\">EU mode</button></a></p><p>";

  button_PacMan();
  webpage += "<a href=\"\\TIME_ANIM\"><type=\"button\" class=\"button\">Animated Time</button></a>";
  webpage += "<a href=\"\\TIME_NORMAL\"><type=\"button\" class=\"button\">Normal Time</button></a></p>";

  webpage += "<p><a href=\"\\SOUND\"><type=\"button\" class=\"button\">Sound TIME</button></a>";
  webpage += "<a href=\"\\TIME_MINUTE\"><type=\"button\" class=\"button\">HH:MM</button></a>";
  webpage += "<a href=\"\\TIME_SECOND\"><type=\"button\" class=\"button\">hh:mm:ss</button></a></p>";

  webpage += "<p><a href=\"\\DISPLAY_DATE\"><type=\"button\" class=\"button\">Show Date</button></a>";
  webpage += "<a href=\"\\DISPLAY_NO_DATE\"><type=\"button\" class=\"button\">Only Time</button></a></p>";
  
  webpage += "<form id=\"bright_form\">";
  webpage += "<a>Brightness<br>MIN(0)<input type=\"range\" name=\"Bright\" min=\"0\" max=\"15\" value=\"";
  webpage += brightness;
  webpage += "\">(15)MAX</a></form>";
  webpage += "<p><a href=\"\"><type=\"button\" onClick=\"SendBright()\" class=\"button\">Send Brightness</button></a></p>";
  webpage += "<hr class=\"line\">";
  webpage += "<p><a href=\"\\RESTART_1\"><type=\"button\" class=\"button\">RTC: ";
  webpage += zone1;
  webpage += "</button></a>";
  webpage += "<a href=\"\\RESTART_2\"><type=\"button\" class=\"button\">RTC: ";
  webpage += zone2;
  webpage += "</button></a>";
  webpage += "<a href=\"\\MSG_TOGGLE\"><type=\"button\" class=\"button\">MESSAGE</button></a></p>";
  webpage += "<br><p><a href=\"\\RESET_WIFI\"><type=\"button\" class=\"button button2\">Reset WiFi</button></a></p>";
  webpage += "</div>";
  end_webpage();
}
//////////////////////////////////////////////////////////////////////////////
void reset_wifi() {
  append_webpage_header();
  webpage += "<p><h2>New WiFi Connection</h2></p></div>";
  webpage += "<div id=\"section\">";
  webpage += "<p>&#149; Connect WiFi to SSID: <b>ESP32_AP</b></p>";
  webpage += "<p>&#149; Next connect to: <b><a href=http://192.168.4.1/>http://192.168.4.1/</a></b></p>";
  webpage += "<p>&#149; Make the WiFi connection</p>";
  button_Home();
  webpage += "</div>";
  end_webpage();
  delay(1000);
  WiFiManager wifiManager;
  wifiManager.resetSettings();      // RESET WiFi in ESP32
  reset_ESP32();
}
//////////////////////////////////////////////////////////////
void web_reset_ESP32() {
  append_webpage_header();
  webpage += "<p><h2>Restarting ESP32...</h2></p></div>";
  webpage += "<div id=\"section\">";
  button_Home();
  webpage += "</div>";
  end_webpage();
  delay(1000);
  reset_ESP32();
}
//////////////////////////////////////////////////////////////
void end_webpage(){
  webpage += "<div id=\"footer\">Copyright &copy; J_RPM 2021</div></div></html>\r\n";
  if (display_msg == false) {
    server2.send(200, "text/html", webpage);
  }else {
    client = server.available();
    client.println(WebResponse);
    client.println(webpage);
  }
  PRINTS("\n>>> end_webpage() OK! ");
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void display_mode_toggle() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(0, display_EU);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void display_date_toggle() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(1, display_date);
  end_Eprom();
}
//////////////////////////////////////////////////////////////////////////////
void brightness_matrix() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(2, brightness);
  sendCmdAll(CMD_INTENSITY,brightness);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void display_time_mode() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(3, animated_time);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void display_time_view() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(4, show_seconds);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void display_matrix_speed() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(5, matrix_speed);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void display_init_msg() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(6, display_msg);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void alarm_Hour() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(7, alarm_H);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void alarm_Minute() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(8, alarm_M);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void alarm_Repe() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(9, alarm_R);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void alarm_Hour2() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(10, alarm_H2);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void alarm_Minute2() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(11, alarm_M2);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void alarm_Repe2() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(12, alarm_R2);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void set_Zone2() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(13, T_Zone2);
  end_Eprom();
}
//////////////////////////////////////////////////////////////
void end_Eprom() {
  del=0;
  dots=1;
  EEPROM.commit();
  EEPROM.end();
}
//////////////////////////////////////////////////////////////
void reset_ESP32() {
  sendCmdAll(CMD_SHUTDOWN,0);
  ESP.restart();
  delay(5000);
}
//////////////////////////////////////////////////////////////
void display_AP_wifi () {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(4,0);   
  display.println(F("ENTRY"));
  display.setCursor(10,16);   
  display.println(F("WiFi"));
  display.setTextSize(1);   
  display.setCursor(8,32);   
  display.println("ESP32_AP");
  display.setCursor(0,40);   
  display.println(F("192168.4.1"));
  display.display();
}
//////////////////////////////////////////////////////////////
void display_flash(bool tst) {
  for (int i=0; i<8; i++) {
    if (tst==true)sendCmdAll(CMD_DISPLAYTEST, 1);
    display.invertDisplay(true);
    display.display();
    sendCmdAll(CMD_SHUTDOWN,0);
    delay (50);
    display.invertDisplay(false);
    display.display();
    sendCmdAll(CMD_DISPLAYTEST, 0);
    sendCmdAll(CMD_SHUTDOWN,1);
    delay (50);
  }
}
//////////////////////////////////////////////////////////////
void display_ip() {
  // Print the IP address MATRIX
  printStringWithShift((String("  http:// ")+ WiFi.localIP().toString()).c_str(), matrix_speed);

  // Display OLED & MATRIX
  display.clearDisplay();
  display.setTextSize(2);   
  display.setCursor(4,8);   
  display.print("ENTRY");
  display.setTextSize(1);   
  display.setCursor(0,26);   
  display.print("http://");
  display.print(WiFi.localIP());
  display.println("/");
  display.display();
  GameAudio.PlayWav(&wawIP, false, 1.0);
  display_flash(false);
}
//////////////////////////////////////////////////////////////////////////////
void showSimpleClock(){
  dx=dy=0;
  clr();
   if(h/10==0) {
    showDigit(10,  0, dig6x8);
   }else {
    showDigit(h/10,  0, dig6x8);
   }
  showDigit(h%10,  8, dig6x8);
  showDigit(m/10, 17, dig6x8);
  showDigit(m%10, 25, dig6x8);
  showDigit(s/10, 34, dig6x8);
  showDigit(s%10, 42, dig6x8);
  setCol(15,dots ? B00100100 : 0);
  setCol(32,dots ? B00100100 : 0);
  refreshAll();
}
//////////////////////////////////////////////////////////////////////////////
void showSecondsClock(){
  dx=dy=0;
  clr();
   if(h/10==0) {
    showDigit(10,  0, dig4x8r);
   }else {
    showDigit(h/10,  0, dig4x8r);
   }
  showDigit(h%10,  5, dig4x8r);
  showDigit(m/10, 11, dig4x8r);
  showDigit(m%10, 16, dig4x8r);
  showDigit(s/10, 23, dig4x8r);
  showDigit(s%10, 28, dig4x8r);
  setCol(9,1 ? B00100100 : 0);
  setCol(21,1 ? B00100100 : 0);
  refreshAll();
}
//////////////////////////////////////////////////////////////////////////////
void showAnimClock(){
  byte digPos[6]={0,8,17,25,34,42};
  int digHt = 12;
  int num = 6; 
  int i;
  if(del==0) {
    del = digHt;
    for(i=0; i<num; i++) digold[i] = dig[i];
    dig[0] = h/10 ? h/10 : 10;
    dig[1] = h%10;
    dig[2] = m/10;
    dig[3] = m%10;
    dig[4] = s/10;
    dig[5] = s%10;
    for(i=0; i<num; i++)  digtrans[i] = (dig[i]==digold[i]) ? 0 : digHt;
  } else
    del--;
  
  clr();
  for(i=0; i<num; i++) {
    if(digtrans[i]==0) {
      dy=0;
      showDigit(dig[i], digPos[i], dig6x8);
    } else {
      dy = digHt-digtrans[i];
      showDigit(digold[i], digPos[i], dig6x8);
      dy = -digtrans[i];
      showDigit(dig[i], digPos[i], dig6x8);
      digtrans[i]--;
    }
  }
  dy=0;
  setCol(15,dots ? B00100100 : 0);
  setCol(32,dots ? B00100100 : 0);
  refreshAll();
  if (display_msg == false) {server2.handleClient();}else{handleWiFi();}
}
//////////////////////////////////////////////////////////////////////////////
void showAnimSecClock(){
  byte digPos[6]={0,5,11,16,23,28};
  int digHt = 12;
  int num = 6; 
  int i;
  if(del==0) {
    del = digHt;
    for(i=0; i<num; i++) digold[i] = dig[i];
    dig[0] = h/10 ? h/10 : 10;
    dig[1] = h%10;
    dig[2] = m/10;
    dig[3] = m%10;
    dig[4] = s/10;
    dig[5] = s%10;
    for(i=0; i<num; i++)  digtrans[i] = (dig[i]==digold[i]) ? 0 : digHt;
  } else
    del--;
  
  clr();
  for(i=0; i<num; i++) {
    if(digtrans[i]==0) {
      dy=0;
      showDigit(dig[i], digPos[i], dig4x8r);
    } else {
      dy = digHt-digtrans[i];
      showDigit(digold[i], digPos[i], dig4x8r);
      dy = -digtrans[i];
      showDigit(dig[i], digPos[i], dig4x8r);
      digtrans[i]--;
    }
  }
  dy=0;
  setCol(9,1 ? B00100100 : 0);
  setCol(21,1 ? B00100100 : 0);
  refreshAll();
  if (display_msg == false) {server2.handleClient();}else{handleWiFi();}
}
//////////////////////////////////////////////////////////////////////////////
void showDigit(char ch, int col, const uint8_t *data){
  if(dy<-8 | dy>8) return;
  int len = pgm_read_byte(data);
  int w = pgm_read_byte(data + 1 + ch * len);
  col += dx;
  for (int i = 0; i < w; i++)
    if(col+i>=0 && col+i<8*NUM_MAX) {
      byte v = pgm_read_byte(data + 1 + ch * len + 1 + i);
      if(!dy) scr[col + i] = v; else scr[col + i] |= dy>0 ? v>>dy : v<<-dy;
    }
}
//////////////////////////////////////////////////////////////////////////////
void setCol(int col, byte v){
  if(dy<-8 | dy>8) return;
  col += dx;
  if(col>=0 && col<8*NUM_MAX)
    if(!dy) scr[col] = v; else scr[col] |= dy>0 ? v>>dy : v<<-dy;
}
//////////////////////////////////////////////////////////////////////////////
int showChar(char ch, const uint8_t *data){
  int len = pgm_read_byte(data);
  int i,w = pgm_read_byte(data + 1 + ch * len);
  for (i = 0; i < w; i++)
    scr[NUM_MAX*8 + i] = pgm_read_byte(data + 1 + ch * len + 1 + i);
  scr[NUM_MAX*8 + i] = 0;
  return w;
}
//////////////////////////////////////////////////////////////////////////////
unsigned char convertSpanish(unsigned char _c){
  unsigned char c = _c;
  if(c==195) {
    dualChar = c;
    return 0;
  }
  // UTF8
  if(dualChar) {
    switch(_c) {
      case 161: c = 1+'~'; break;   // 'á'
      case 169: c = 2+'~'; break;   // 'é'
      case 173: c = 3+'~'; break;   // 'í'
      case 179: c = 4+'~'; break;   // 'ó'
      case 186: c = 5+'~'; break;   // 'ú'
      case 188: c = 6+'~'; break;   // 'ü'
      case 156: c = 7+'~'; break;   // 'Ü'
      case 177: c = 8+'~'; break;   // 'ñ'
      case 145: c = 9+'~'; break;   // 'Ñ'
      case 167: c = 10+'~'; break;  // 'ç'
      case 135: c = 11+'~'; break;  // 'Ç'
      case 129: c = 12+'~'; break;  // 'Á'
      case 137: c = 13+'~'; break;  // 'É'
      case 141: c = 14+'~'; break;  // 'Í'
      case 147: c = 15+'~'; break;  // 'Ó'
      case 154: c = 16+'~'; break;  // 'Ú'
      default:  break;
    }
    dualChar = 0;
    return c;
  }    
  // ANSI
  switch(_c) {
    case 225: c = 1+'~'; break;   // 'á'
    case 233: c = 2+'~'; break;   // 'é'
    case 237: c = 3+'~'; break;   // 'í'
    case 243: c = 4+'~'; break;   // 'ó'
    case 250: c = 5+'~'; break;   // 'ú'
    case 252: c = 6+'~'; break;   // 'ü'
    case 220: c = 7+'~'; break;   // 'Ü'
    case 241: c = 8+'~'; break;   // 'ñ'
    case 209: c = 9+'~'; break;   // 'Ñ'
    case 231: c = 10+'~'; break;  // 'ç'
    case 199: c = 11+'~'; break;  // 'Ç'
    case 193: c = 12+'~'; break;  // 'Á'
    case 201: c = 13+'~'; break;  // 'É'
    case 205: c = 14+'~'; break;  // 'Í'
    case 211: c = 15+'~'; break;  // 'Ó'
    case 218: c = 16+'~'; break;  // 'Ú'
    default:  break;
  }
  return c;
}
//////////////////////////////////////////////////////////////////////////////
void printCharWithShift(unsigned char c, int shiftDelay) {
  // To check WiFi inputs faster 
  shiftDelay = shiftDelay / 4;
  
  c = convertSpanish(c);
  if (c < ' ' || c > '~'+23) return;
  c -= 32;
  int w = showChar(c, font);
  for (int i=0; i<w+1; i++) {
    if (display_msg == false) {server2.handleClient();}else{handleWiFi();}
    delay(shiftDelay);
    if (display_msg == false) {server2.handleClient();}else{handleWiFi();}
    delay(shiftDelay);
    if (display_msg == false) {server2.handleClient();}else{handleWiFi();}
    delay(shiftDelay);
    if (display_msg == false) {server2.handleClient();}else{handleWiFi();}
    delay(shiftDelay);
    scrollLeft();
    refreshAll();
  }
}
//////////////////////////////////////////////////////////////////////////////
void printStringWithShift(const char* s, int shiftDelay){
  while (*s) {
    printCharWithShift(*s, shiftDelay);
    s++;
  }
}
//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
const char *err2Str(wl_status_t code){
  switch (code){
  case WL_IDLE_STATUS:    return("IDLE");           break; // WiFi is in process of changing between statuses
  case WL_NO_SSID_AVAIL:  return("NO_SSID_AVAIL");  break; // case configured SSID cannot be reached
  case WL_CONNECTED:      return("CONNECTED");      break; // successful connection is established
  case WL_CONNECT_FAILED: return("PASSWORD_ERROR"); break; // password is incorrect
  case WL_DISCONNECTED:   return("CONNECT_FAILED"); break; // module is not configured in station mode
  default: return("??");
  }
}
/////////////////////////////////////////////////////////////////
uint8_t htoi(char c) {
  c = toupper(c);
  if ((c >= '0') && (c <= '9')) return(c - '0');
  if ((c >= 'A') && (c <= 'F')) return(c - 'A' + 0xa);
  return(0);
}
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
// Check clock config
////////////////////////////////////////////////
void _display_mode_usa() {
  display_EU = false;
  PRINTS("\n-> DISPLAY_MODE_USA");
  display_mode_toggle();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _display_mode_eu() {
  display_EU = true;
  PRINTS("\n-> DISPLAY_MODE_EU");
  display_mode_toggle();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _time_anim() {
  animated_time = true;
  PRINTS("\n-> TIME_ANIM");
  display_time_mode();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _time_normal() {
  animated_time = false;
  PRINTS("\n-> TIME_NORMAL");
  display_time_mode();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _time_minute() {
  show_seconds = false;
  PRINTS("\n-> TIME_MINUTE");
  display_time_view();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _time_second() {
  show_seconds = true;
  PRINTS("\n-> TIME_SECOND");
  display_time_view();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _display_date() {
  display_date = true;
  PRINTS("\n-> DISPLAY_DATE");
  display_date_toggle();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _display_no_date() {
  display_date = false;
  PRINTS("\n-> DISPLAY_NO_DATE");
  display_date_toggle();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _bright_0() {
  PRINTS("\n-> BRIGHT=0");
  brightness = 0;     //DUTY CYCLE: 1/32 (MIN) 
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_1() {
  PRINTS("\n-> BRIGHT=1");
  brightness = 1;    
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_2() {
  PRINTS("\n-> BRIGHT=2");
  brightness = 2;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_3() {
  PRINTS("\n-> BRIGHT=3");
  brightness = 3;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_4() {
  PRINTS("\n-> BRIGHT=4");
  brightness = 4;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_5() {
  PRINTS("\n-> BRIGHT=5");
  brightness = 5;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_6() {
  PRINTS("\n-> BRIGHT=6");
  brightness = 6;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_7() {
  PRINTS("\n-> BRIGHT=7");
  brightness = 7;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_8() {
  PRINTS("\n-> BRIGHT=8");
  brightness = 8;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_9() {
  PRINTS("\n-> BRIGHT=9");
  brightness = 9;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_10() {
  PRINTS("\n-> BRIGHT=10");
  brightness = 10;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_11() {
  PRINTS("\n-> BRIGHT=11");
  brightness = 11;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_12() {
  PRINTS("\n-> BRIGHT=12");
  brightness = 12;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_13() {
  PRINTS("\n-> BRIGHT=13");
  brightness = 13;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_14() {
  PRINTS("\n-> BRIGHT=14");
  brightness = 14;
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _bright_15() {
  PRINTS("\n-> BRIGHT=15");
  brightness = 15;     //DUTY CYCLE: 31/32 (MAX)
  _save_bright();
}
/////////////////////////////////////////////////////////////////
void _save_bright(){
  brightness_matrix();
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _msg_toggle() {
  display_msg = true;
  PRINTS("\n-> MSG_TOGGLE");
  display_init_msg();
  display_msg = false;
  _restart();
}
/////////////////////////////////////////////////////////////////
void _clock_toggle() {
  display_msg = false;
  PRINTS("\n-> CLOCK_TOGGLE");
  display_init_msg();
  display_msg = true;
  _restart();
}
/////////////////////////////////////////////////////////////////
void _restart_1() {
  T_Zone2=false;
  PRINT("\n>>> SYNC Time: ",zone1);
  set_Zone2();
  _restart();
}
/////////////////////////////////////////////////////////////////
void _restart_2() {
  T_Zone2=true;
  PRINT("\n>>> SYNC Time: ",zone2);
  set_Zone2();
  _restart();
}
/////////////////////////////////////////////////////////////////
void _restart() {
  PRINTS("\n-> RESTART");
  web_reset_ESP32();
}
/////////////////////////////////////////////////////////////////
void _reset_wifi() {
  PRINTS("\n-> RESET_WIFI");
  reset_wifi();
}
/////////////////////////////////////////////////////////////////
void _home() {
  PRINTS("\n-> HOME");
  responseWeb();
}
/////////////////////////////////////////////////////////////////
void _pacman() {
  if (pac==false) responseWeb();
  _scroll=true;
  soundEnd();
  delay(400);
}
/////////////////////////////////////////////////////////////////
void _sound() {
  if (sTime==false) responseWeb();
  soundTime();
  delay(400);
  soundEnd();
}
/////////////////////////////////////////////////////////////////
void responseWeb(){
  PRINTS("\nS_RESPONSE");
  if (display_msg == false) {
    NTP_Clock_home_page();
  }else {
    ESP32_set_message();
  }
  PRINT("\n>>> SendPage display_msg: ",display_msg);
}
/////////////////////////////////////////////////////////////////
void checkServer(){
  if (display_msg) {
    server.begin();  // Start the WiFiServer
    PRINTS("\nWiFiServer started");
  }else {
    server2.begin();  // Start the WebServer
    PRINTS("\nWebServer started");
    ////////////////////////////////////////////////////////////////////////
    // Define what happens when a client requests attention
    ////////////////////////////////////////////////////////////////////////
    server2.on("/", _home);
    server2.on("/DISPLAY_MODE_USA", _display_mode_usa); 
    server2.on("/DISPLAY_MODE_EU", _display_mode_eu); 
    server2.on("/TIME_ANIM", _time_anim); 
    server2.on("/TIME_NORMAL", _time_normal); 
    server2.on("/TIME_MINUTE", _time_minute); 
    server2.on("/TIME_SECOND", _time_second); 
    server2.on("/DISPLAY_DATE", _display_date);
    server2.on("/DISPLAY_NO_DATE", _display_no_date);
    server2.on("/MSG_TOGGLE", _msg_toggle);
    server2.on("/CLOCK_TOGGLE", _clock_toggle);
    server2.on("/BRIGHT=0", _bright_0); 
    server2.on("/BRIGHT=1", _bright_1); 
    server2.on("/BRIGHT=2", _bright_2); 
    server2.on("/BRIGHT=3", _bright_3); 
    server2.on("/BRIGHT=4", _bright_4); 
    server2.on("/BRIGHT=5", _bright_5); 
    server2.on("/BRIGHT=5", _bright_6); 
    server2.on("/BRIGHT=7", _bright_6); 
    server2.on("/BRIGHT=8", _bright_8); 
    server2.on("/BRIGHT=9", _bright_9); 
    server2.on("/BRIGHT=10", _bright_10); 
    server2.on("/BRIGHT=11", _bright_11); 
    server2.on("/BRIGHT=12", _bright_12); 
    server2.on("/BRIGHT=13", _bright_13); 
    server2.on("/BRIGHT=14", _bright_14); 
    server2.on("/BRIGHT=15", _bright_15); 
    server2.on("/HOME", _home);
    server2.on("/sPAC", _pacman);
    server2.on("/SOUND", _sound);
    server2.on("/RESTART_1", _restart_1);  
    server2.on("/RESTART_2", _restart_2);  
    server2.on("/RESET_WIFI", _reset_wifi);
  }
}
/////////////////////////////////////////////////////////////////
void getData(char *szMesg, uint16_t len) {
  char *pStart, *pEnd;      // pointer to start and end of text

  ////////////////////////////////////////////////
  // Check clock config
  ////////////////////////////////////////////////

  //-----------------------------------------------
  pStart = strstr(szMesg, "sPAC");
  if (pStart != NULL) {
    pac=true;
    return; 
  }
  //-----------------------------------------------
  pStart = strstr(szMesg, "Sound_Time");
  if (pStart != NULL) {
    sTime=true;
    return; 
  }
  //-----------------------------------------------
  pStart = strstr(szMesg, "CLOCK_TOGGLE");
  if (pStart != NULL) {
    _clock_toggle();
    return; 
  }

  ///////////////////////////////////////////////////////////////////////
  // Check Alarm #1 time and repetitions
  // Recv: GET /&h=12/&nocache=37153.378554600546 HTTP/1.1
  // Recv: GET /&m=20/&nocache=831747.823437562 HTTP/1.1
  // Recv: GET /&r=2/&nocache=311662.6675125849 HTTP/1.1
  // Recv: GET /&h=10/&m=30/&r=5/&nocache=11583.322345389835 HTTP/1.1
  ///////////////////////////////////////////////////////////////////////
  // Alarm HOUR #1
  pStart = strstr(szMesg, "/&h=");
  if (pStart != NULL){
    char *pH = newMessage;
    pStart += 4;  // Skip to start of Hour
    pEnd = strstr(pStart, "/&");
    if (pEnd != NULL) {
      while (pStart != pEnd) {
        *pH++ = *pStart++;
      }
      *pH = '\0'; // terminate the string
      al_H = String(newMessage);
      alarm_H = al_H.toInt();
      alarm_Hour();
      PRINT("\n-> 1# Alarm HOUR: ",al_H);
      chgA1 = true;
    }  
  }
  
  // Alarm MINUTE #1
  pStart = strstr(szMesg, "/&m=");
  if (pStart != NULL){
    char *pM = newMessage;
    pStart += 4;  // Skip to start of Minute
    pEnd = strstr(pStart, "/&");
    if (pEnd != NULL) {
      while (pStart != pEnd) {
        *pM++ = *pStart++;
      }
      *pM = '\0'; // terminate the string
      al_M = String(newMessage);
      alarm_M = al_M.toInt();
      alarm_Minute();
      PRINT("\n-> 1# Alarm MINUTE: ",al_M);
      chgA1 = true;
    }  
  }

  // Alarm REPETITIONS #1
  pStart = strstr(szMesg, "/&r=");
  if (pStart != NULL){
    char *pR = newMessage;
    pStart += 4;  // Skip to start of Minute
    pEnd = strstr(pStart, "/&");
    if (pEnd != NULL) {
      while (pStart != pEnd) {
        *pR++ = *pStart++;
      }
      *pR = '\0'; // terminate the string
      al_R = String(newMessage);
      alarm_R = al_R.toInt();
      alarm_Repe();
      PRINT("\n-> 1# Alarm REPETITIONS: ",al_R);
      chgA1 = true;
    }  
  }

  ///////////////////////////////////////////////////////////////////////
  // Check Alarm #2 time and repetitions
  // Recv: GET /&h2=12/&nocache=37153.378554600546 HTTP/1.1
  // Recv: GET /&m2=20/&nocache=831747.823437562 HTTP/1.1
  // Recv: GET /&r2=2/&nocache=311662.6675125849 HTTP/1.1
  // Recv: GET /&h2=10/&m2=30/&r2=5/&nocache=11583.322345389835 HTTP/1.1
  ///////////////////////////////////////////////////////////////////////
  // Alarm HOUR #2
  pStart = strstr(szMesg, "/&h2=");
  if (pStart != NULL){
    char *pH = newMessage;
    pStart += 5;  // Skip to start of Hour
    pEnd = strstr(pStart, "/&");
    if (pEnd != NULL) {
      while (pStart != pEnd) {
        *pH++ = *pStart++;
      }
      *pH = '\0'; // terminate the string
      al_H2 = String(newMessage);
      alarm_H2 = al_H2.toInt();
      alarm_Hour2();
      PRINT("\n-> 2# Alarm HOUR: ",al_H2);
      chgA2 = true;
    }  
  }
  
  // Alarm MINUTE #2
  pStart = strstr(szMesg, "/&m2=");
  if (pStart != NULL){
    char *pM = newMessage;
    pStart += 5;  // Skip to start of Minute
    pEnd = strstr(pStart, "/&");
    if (pEnd != NULL) {
      while (pStart != pEnd) {
        *pM++ = *pStart++;
      }
      *pM = '\0'; // terminate the string
      al_M2 = String(newMessage);
      alarm_M2 = al_M2.toInt();
      alarm_Minute2();
      PRINT("\n-> 2# Alarm MINUTE: ",al_M2);
      chgA2 = true;
    }  
  }

  // Alarm REPETITIONS #2
  pStart = strstr(szMesg, "/&r2=");
  if (pStart != NULL){
    char *pR = newMessage;
    pStart += 5;  // Skip to start of Minute
    pEnd = strstr(pStart, "/&");
    if (pEnd != NULL) {
      while (pStart != pEnd) {
        *pR++ = *pStart++;
      }
      *pR = '\0'; // terminate the string
      al_R2 = String(newMessage);
      alarm_R2 = al_R2.toInt();
      alarm_Repe2();
      PRINT("\n-> 2# Alarm REPETITIONS: ",al_R2);
      chgA2 = true;
    }  
  }

  /////////////////////////////////////////////////
  // Check text message
  // Message may contain data for:
  // New text (/&MSG=)
  // Speed (/&SP=)
  /////////////////////////////////////////////////
  pStart = strstr(szMesg, "/&MSG=");
  
  if (pStart != NULL){
    char *psz = newMessage;

    pStart += 6;  // skip to start of data
    pEnd = strstr(pStart, "/&");

    if (pEnd != NULL) {
      while (pStart != pEnd) {
        if (*pStart == '%') {
          // replace %xx hex code with the ASCII character
          char c = 0;
          pStart++;
          c += (htoi(*pStart++) << 4);
          c += htoi(*pStart++);
          *psz++ = c;
        } else *psz++ = *pStart++;
      }

      *psz = '\0'; // terminate the string
      newMessageAvailable = (strlen(newMessage) != 0);
      PRINT("\nNew Msg: ", newMessage);
    }
  }

  // check speed
  pStart = strstr(szMesg, "/&SP=");
  if (pStart != NULL) {
    pStart += 5;  // skip to start of data

    int speed = atoi(pStart);
    matrix_speed = speed;
    display_matrix_speed();
    PRINT("\n-> Speed: ",matrix_speed );
    return;
  }
}
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void handleWiFi(void) {
  static enum { S_IDLE, S_WAIT_CONN, S_READ, S_EXTRACT, S_RESPONSE, S_DISCONN } state = S_IDLE;
  static char szBuf[1024];
  static uint16_t idxBuf = 0;
  static uint32_t timeStart;


  switch (state)
  {
  case S_IDLE:   // initialise
    PRINTS("\n-> S_IDLE");
    state = S_WAIT_CONN;
    idxBuf = 0;
    break;

  case S_WAIT_CONN:   // waiting for connection
    client = server.available();
    if (!client) break;
    if (!client.connected()) break;

    #if DEBUG
        char szTxt[20];
        sprintf(szTxt, "%01d.%01d.%01d.%01d", client.remoteIP()[0], client.remoteIP()[1], client.remoteIP()[2], client.remoteIP()[3]);
        PRINT("\nNew client @ ", szTxt);
    #endif

    state = S_READ;
    timeStart = millis();
    break;
    
  case S_READ:   // read message
    PRINTS("\nS_READ ");

    //.................................
    while (client.available()) {
      char c = client.read();
      if ((c == '\r') || (c == '\n')) {
        szBuf[idxBuf] = '\0';
        PRINT("\nOK -> Recv: ", szBuf);
        state = S_EXTRACT;
        client.flush();
      } else {
        szBuf[idxBuf++] = (char)c;
      }
    }
    //.................................
    
    if (millis() - timeStart > 300) {
      PRINTS("\nErr -> Wait timeout");
      state = S_DISCONN;
    }
    break;

  case S_EXTRACT:   // extract message
    PRINTS("\nS_EXTRACT");
    state = S_RESPONSE;
    // Extract the string from the message if there is one
    getData(szBuf, BUF_SIZE);
    break;

  case S_RESPONSE:   // response to the client
    // Return the response to the client (web page)
    state = S_DISCONN;
    responseWeb();
    if (chgA1==true) changedAlarm(1);
    if (chgA2==true) changedAlarm(2);
    if (sTime==true) _sound(); 
    if (pac==true) _pacman(); 
    break;

  case S_DISCONN:   // close client
    PRINTS("\nS_DISCONN");
    state = S_IDLE;
    client.flush();
    client.stop();
    break;


  default:  state = S_IDLE;
  }
}
/////////////////////////////////////////////////////////////////
void checkAlarm(){
  int myH;
  //Convert to EU time for alarm
  String modT = CurrentTime.substring(9,10);
  if (modT == "P" && h!=12) {
    myH=h+12;
  }else if (modT == "A" && h==12) {
    myH=0;
  }else {
    myH=h;
  }
  //PRINT("\nTime: ", CurrentTime + " -> EU:" + myH + " s:" + String(s) + " - " + on_txt);
 
  // Alarm #1
  if (alarm_R > 0 && myH == alarm_H && m == alarm_M) {
    // on_txt: window of checking Alarm
    if ((on_txt==true && s < 30)||(s == 0)){
      soundAlarm(alarm_R);
    }
  // Alarm #2
  }else if (alarm_R2 > 0 && myH == alarm_H2 && m == alarm_M2) {
    // on_txt: window of checking Alarm
    if ((on_txt==true && s < 30)||(s == 0)){
      soundAlarm(alarm_R2);
    }
  }
}
/////////////////////////////////////////////////////////////////
void soundAlarm(int n){
  _scroll=true;
  on_txt=false; 
  for (int i = 0; i < n; i++) {
    PRINT("\nAlarm: ", String(i+1) + " of " + String(n));
    sendCmdAll(CMD_INTENSITY,15);
    GameAudio.PlayWav(&wawTone, false, 1.0); 
    while(GameAudio.IsPlaying()){ } // wait until done

    sendCmdAll(CMD_INTENSITY,0);
    delay(400);
    sendCmdAll(CMD_INTENSITY,15);
    
    soundTime();
    sendCmdAll(CMD_INTENSITY,0);
    delay(600);
  }
  soundEnd();
}
/////////////////////////////////////////////////////////////////
void soundEnd(){
  PRINT("\nRestart, repetitions of alarms #1:" , String(alarm_R) + " - 2#:" + String(alarm_R2));
  sendCmdAll(CMD_INTENSITY,brightness);
  pacmanEffect();
  GameAudio.PlayWav(&pmDeath, false, 1.0);
  display_flash(true); // Flash MATRIX

  UpdateLocalTime();
  clkTime = millis();
  _scroll=false;
}
/////////////////////////////////////////////////////////////////
void pacmanEffect(){
  const uint8_t ghost1[6] PROGMEM = {B11111100, B01110110, B11111110, B01110110, B11111100};
  const uint8_t ghost2[6] PROGMEM = {B01111100, B11110110, B01111110, B11110110, B01111100};
  const uint8_t pac1[6] PROGMEM = {B10000011, B11000111, B11101101, B01111110, B00111100};
  const uint8_t pac2[4] PROGMEM = {B00101100, B01111110, B11111011};
  bool p=false;
  sTime=false;
  pac=false;

  GameAudio.PlayWav(&pmEat, false, 1.0);
  for (int i=0; i<=(NUM_MAX*8)+13; i++) {
    setCol(i-3,1 ? pac1[3] : 0);
    setCol(i-4,1 ? pac1[4] : 0);
    setCol(i-5,1 ? B00000000 : 0);  //Clear last line Pacman
    setCol(i-14,1 ? B00000000 : 0); //Clear last line Ghost
    if (p==true){
      p=false;
      setCol(i-9,1 ? ghost1[0] : 0); 
      setCol(i-10,1 ? ghost1[1] : 0);
      setCol(i-11,1 ? ghost1[2] : 0);
      setCol(i-12,1 ? ghost1[3] : 0);
      setCol(i-13,1 ? ghost1[4] : 0);
      setCol(i,1 ? pac1[0] : 0);
      setCol(i-1,1 ? pac1[1] : 0);
      setCol(i-2,1 ? pac1[2] : 0);
    }else {
      p=true;
      setCol(i-9,1 ? ghost2[0] : 0); 
      setCol(i-10,1 ? ghost2[1] : 0);
      setCol(i-11,1 ? ghost2[2] : 0);
      setCol(i-12,1 ? ghost2[3] : 0);
      setCol(i-13,1 ? ghost2[4] : 0);
      setCol(i,1 ? pac2[0] : 0);
      setCol(i-1,1 ? pac2[1] : 0);
      setCol(i-2,1 ? pac2[2] : 0);
    }
    GameAudio.PlayWav(&pmEat, false, 1.0);
    refreshAll();
    delay(120);
  }
  while(GameAudio.IsPlaying()){ }
}
/////////////////////////////////////////////////////////////////
void soundTime(){
  _scroll=true;
  UpdateLocalTime();
  dots = 1;
  Oled_Time();
  showSimpleClock();
  PRINT("\n>>> SOUND Time: ", CurrentTime);
  PRINTS("\n");
  GameAudio.PlayWav(&wawSon, false, 1.0);
  while(GameAudio.IsPlaying()){ }    // wait until done
  playWawT(CurrentTime.substring(0,2).toInt());
  GameAudio.PlayWav(&wawHor, false, 1.0);
  while(GameAudio.IsPlaying()){ }    // wait until done
  delay(200);
  playWawT(CurrentTime.substring(3,5).toInt());
  GameAudio.PlayWav(&wawMin, false, 1.0);
  while(GameAudio.IsPlaying()){ }    // wait until done
}
/////////////////////////////////////////////////////////////////
void playWawT(int w){
  int wH = w/10;
  if (wH > 0) playWawN(wH);
  playWawN(w%10);
}
/////////////////////////////////////////////////////////////////
void playWawN(int n){
  switch(n) {
    case 0: GameAudio.PlayWav(&waw0, false, 1.0); break;  
    case 1: GameAudio.PlayWav(&waw1, false, 1.0); break;
    case 2: GameAudio.PlayWav(&waw2, false, 1.0); break;  
    case 3: GameAudio.PlayWav(&waw3, false, 1.0); break;  
    case 4: GameAudio.PlayWav(&waw4, false, 1.0); break;  
    case 5: GameAudio.PlayWav(&waw5, false, 1.0); break;  
    case 6: GameAudio.PlayWav(&waw6, false, 1.0); break;  
    case 7: GameAudio.PlayWav(&waw7, false, 1.0); break;  
    case 8: GameAudio.PlayWav(&waw8, false, 1.0); break;  
    case 9: GameAudio.PlayWav(&waw9, false, 1.0); break;
  }
  while(GameAudio.IsPlaying()){ }    // wait until done
}
/////////////////////////////////////////////////////////////////
void playIP(){
  String dg;
  String mIP = WiFi.localIP().toString();
  unsigned int carIP = mIP.length();
  for (int i=0; i<=(carIP-1); i++) {
    dg = mIP.substring(i,i+1);
    if (dg==".") {
      GameAudio.PlayWav(&wawPunto, false, 1.0);
    }else {
      playWawN(dg.toInt());
    }
    while(GameAudio.IsPlaying()){ }    // wait until done
  }
}
/////////////////////////////////////////////////////////////////
void changedAlarm(int n) {
  chgA1=false;
  chgA2=false;
  _scroll=true;
  clrScroll();
  String msg = "Alarma "; 
  GameAudio.PlayWav(&wawTone, false, 1.0);
  if (n==1) {
    msg = msg + "1>  " + format2(alarm_H) + ":" + format2(alarm_M) + ">" + String(alarm_R); 
  }else {
    msg = msg + "2>  " + format2(alarm_H2) + ":" + format2(alarm_M2) + ">" + String(alarm_R2); 
  }
  printStringWithShift(msg.c_str(), 35);
  if (n==1) GameAudio.PlayWav(&waw1, false, 1.0); GameAudio.PlayWav(&waw2, false, 1.0);
  display_flash(false);
  delay (3000);
  clrScroll();
  _scroll=false;
}
/////////////////////////////////////////////////////////////////
void clrScroll() {
   clr();
   refreshAll();
   clkTime = millis();
}
////////////////////////// END //////////////////////////////////
/////////////////////////////////////////////////////////////////
