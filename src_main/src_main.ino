#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

//////PREPROCESSORE INCLUDE
#include <Arduino.h>
//Library wajib ketika menggunakan komunikasi I2C
#include <DS1307RTC.h>
#include <Wire.h>
//library modul LCD + I2C
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

/////////////////Email
#include "ESP32_MailClient.h"
#include <WiFi.h>
/////////////////
#define EEPROM_SIZE 512
//////////////
#define Bluetooth_TimeOut 10
//////////////
#define BTN_UP_PIN      35
#define BTN_DOWN_PIN    34
#define BTN_MENU_PIN    19
#define BTN_SET_PIN     18
#define BTN_UP          0x01
#define BTN_DOWN        0x02
#define BTN_MENU        0x04
#define BTN_SET         0x08
#define BUTTON          readPushButton()
#define buttonPressed   readPushButton()!=0?true:false
/////////////////////////////
#define ADC_PIN_WATER_LEVEL     36
#define ADC_PIN_WATER_PH        39
#define ADC_OFFSET      50

#define WATER_LEVEL 0
#define WATER_PH  1
///////////////////////////
#define RELAY_PUMP_PIN  2
#define ON              HIGH
#define OFF             LOW
#define statePump(x)    digitalWrite(RELAY_PUMP_PIN, x)
//////////////
#define MANUAL 0
#define AUTO   1

#define UNSAFE 0
#define SAFE   1

#define MORNING   0
#define NIGHT     1
/////
#define SET_MORNING_ON    0
#define SET_MORNING_OFF   1
#define SET_NIGHT_ON      2
#define SET_NIGHT_OFF     3
////////////////////////////
//EMAIL
#define GMAIL_SMTP_SEVER    "smtp.gmail.com"
#define GMAIL_SMTP_USERNAME "espnodemcu32@gmail.com"
#define GMAIL_SMTP_PASSWORD "32espnodemcu"
#define GMAIL_SMTP_PORT     465
//////////////
#define CONNECTED 0xAA
#define NOT_CONNECTED 0xBB
#define UNKNOWN 0xFF
//////////
///////GLOBAL VARIABLE
#define sample_movAverage 20
unsigned int adcValueArray[2][sample_movAverage];
///////////Variable yang akan disimpan di flash memory
struct eeprom {
  unsigned char pumpDurationMinute[4];//MoorningOn, MorningOff, NightOn, NightOff - Minute
  unsigned char pumpDurationSecond[4];//MoorningOn, MorningOff, NightOn, NightOff - Second
  unsigned int minimumWaterLevelADC;
  char ssid[17];
  char password[17];
  char recipientGMail[17];
  byte modeOperation;
  float phLow[2];
  float phNom[2];
};

//Global variable untuk menyimpan data RTC dalam bentuk string
char stringTime[6];
char stringTimeBlink[6];
//char stringDay[5];
//char stringDate[9];
///////////////////
bool systemSafe     = SAFE;
byte modeOperation  = MANUAL;
uint16_t adcValue[2] = {0, 0};
float phAdjustCalib = 0.0;
////////////////////
bool secondTriggered = false;
bool     blinkSeconds = false;
bool     blink2Seconds = false;
bool     wifiCheckState = false;
uint8_t  counterTimerInterrupt = 0;
uint16_t counterTimerInterrupt1 = 0;
uint16_t counterTimerInterrupt2 = 0;
uint16_t countDownPumpDuration = 0;
uint16_t counterTimerMinutes = 0;
uint8_t counterBluetooth = Bluetooth_TimeOut;
/////////Wifi
char ssid[17]           = "my12IPhone";
char password[17]       = "password";
char recipientGMail[17] = "ahmadqodri122";
byte wifiConnection = 0xFF;
bool wifiTimerCheckState = false;
///////////////////////
bool pumpState = false;
bool timeState = MORNING;
int phOfWater = 7;
bool menuState = false;
bool rtcTimeSettingState = false;
bool flagSetSettingGotFromSerial2 = false;
/////////////////////////////
bool loopState = false;
String inputString = "";      // a String to hold incoming data
String serialSetting = "";
bool stringComplete = false;  // whether the string is complete
/////////////////////////////
//EEPROM DATA
eeprom eep;
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
// EMail Data
SMTPData smtp;

tmElements_t rtc;

void Task_EvenEverySecond( void *pvParameters );
void Task_AdcRead1(void *pvParameters);
void Task_AdcRead2(void *pvParameters);
void Task_EvenSerialReceive( void *pvParameters );
void Task_EvenSendEmail( void *pvParameters );
void Task_EvenEvery5Seconds( void *pvParameters );
void Task_ReadRtcEverySecond( void *pvParameters );
///////////////
bool bluetoothConnected = false;
/////////////////
//////SETUP
void setup() {
  //initial setup to enable I2C communication
  Wire.begin();
  // initial setup LCD
  //  lcd.init();//for atmega
  lcd.begin();//for esp
  // Turn on the blacklight and print a message.
  lcd.backlight();

  LCD_PrintStringAtCursor("System Setup....", 0, 0);
  LCD_PrintStringAtCursor("Please wait!    ", 0, 1);

  //init setup flash memory to use 512 byte storage
  EEPROM.begin(EEPROM_SIZE);

  // initial setup Pin
  initialPin();
  //initial Setup Serial Communication At 19200 bit/second
  Serial.begin(115200);
  while (!Serial);
  Serial2.begin(9600);
  while (!Serial2);
  delay(100);

  Task_Setup();

  //Read EEprom Date
  EEPROM_Read_Data();
  modeOperation = eep.modeOperation;

  //setup wifi
  counterTimerInterrupt = 15;
  while (counterTimerInterrupt && wifiConnection != CONNECTED) {
    wifiConnection = Wifi_ConnectToWiFi();
  }
  if (wifiConnection != CONNECTED) {
    lcd.clear();
    LCD_PrintStringAtCursor("Fail Connect to:", 0, 0);
    LCD_PrintStringAtCursor(eep.ssid, 0, 1);
    delay(1000);
  }

  ADC_ValueArrayInit();
  strcpy(stringTime, "00:00");
  delay(2000);
  lcd.clear();
}

void loop() {
  loopState = true;

  while (1) {
    //    Serial.println("Phase-01");
    systemSafe = adcValue[WATER_LEVEL] < eep.minimumWaterLevelADC + ADC_OFFSET ? UNSAFE : SAFE;
    //    Serial.println("Phase-03");
    homeScreenDisplay();
    //    Serial.println("Phase-04");
    if (modeOperation == AUTO)
      checkPumpSchedule();
    //    Serial.println("Phase-05");
    switch (BUTTON) {
      case BTN_UP:
        if (modeOperation == MANUAL)
        {
          Setting_StatePump(ON);
          Serial_SendRefreshActual();
        }
        while (buttonPressed);
        break;
      case BTN_DOWN:
        if (modeOperation == MANUAL) {
          Setting_StatePump(OFF);
          Serial_SendRefreshActual();
        }
        while (buttonPressed);
        break;
      case BTN_SET:
        modeOperation = modeOperation == MANUAL ? AUTO : MANUAL;
        {
          EEPROM_Set_Mode_Operation(modeOperation);
          Serial_SendRefreshSetting();
        }
        while (buttonPressed);
        break;
      case BTN_MENU:
        menuState = true;
        Menu_MenuDisplay();
        Serial_Flush();
        while (buttonPressed);
        menuState = false;
        break;
      default: break;
    }
    //    Serial.println("Phase-06");
    if (wifiTimerCheckState) {
      counterTimerInterrupt = 10;
      wifiConnection = Wifi_ConnectToWiFi();
      wifiTimerCheckState = false;
    }
    //    Serial.println("Phase-07");
    if (wifiCheckState) {
      if (wifiConnection != CONNECTED)
        wifiConnection = Wifi_ConnectToWiFi();
    }
    //    Serial.println("Phase-08");
    if (flagSetSettingGotFromSerial2)
    {
      Serial_SplitString();
      flagSetSettingGotFromSerial2 = false;
    }
  }
}

void Setting_StatePump(bool state) {
  String stateP = state ? "ON" : "OFF";
  pumpState = state;
  statePump(pumpState);
  Serial.print("Pump:");
  Serial.println(stateP);
}

void homeScreenDisplay() {
  char stringRow1_1[17];
  char stringRow2_1[17];

  if (secondTriggered) {
    RTC.read(rtc);
    sprintf(stringTime, "%02d:%02d", rtc.Hour, rtc.Minute);

    strcpy(stringTimeBlink, stringTime);
    if (blinkSeconds)
      stringTimeBlink[2] = ' ';
    stringTimeBlink[5] = '\0';
    //    Serial.println(str);
    secondTriggered = false;
  }

  sprintf(stringRow1_1, "WLevel %s %s", (systemSafe ? "OK " : "Low"), stringTimeBlink);
  sprintf(stringRow2_1, "%s pH:%02d W:%s", (modeOperation ? "Auto " : "Mnual"), phOfWater, (wifiConnection == CONNECTED ? "C " : (wifiConnection == NOT_CONNECTED ? "NC" : (blinkSeconds ? ". " : ".."))));

  lcd.home();
  lcd.print(stringRow1_1);
  lcd.setCursor(0, 1);
  lcd.print(stringRow2_1);
}

/////////////////////PUSHBUTTON
void initialPin() {
  pinMode(BTN_UP_PIN,   INPUT);
  pinMode(BTN_DOWN_PIN, INPUT);
  pinMode(BTN_MENU_PIN, INPUT);
  pinMode(BTN_SET_PIN,  INPUT);
  pinMode(RELAY_PUMP_PIN,  OUTPUT);
  //  adcAttachPin(ADC_PIN_WATER_LEVEL);
  //  analogSetWidth(10);
  //  analogSetSamples(12);
  //  analogReadResolution(10);
  Setting_StatePump(OFF);
}

unsigned char readPushButton() {
  unsigned char btn = 0;
  if (!digitalRead(BTN_UP_PIN))   btn |= BTN_UP;
  if (!digitalRead(BTN_DOWN_PIN)) btn |= BTN_DOWN;
  if (!digitalRead(BTN_MENU_PIN)) btn |= BTN_MENU;
  if (!digitalRead(BTN_SET_PIN))  btn |= BTN_SET;
  return btn;
}

void checkPumpSchedule() {
  static bool setDurationState = false;

  //  Serial.println(String() + rtc.Hour + ":" + rtc.Minute + ":" + rtc.Second);

  if (secondTriggered) {
    //    Serial.println(String() + "Pump State: " + pumpState + " Duration left: " + countDownPumpDuration);
    secondTriggered = false;
  }

  if (!countDownPumpDuration)
  {
    Setting_StatePump(!pumpState);
    setDurationState = true;
  }

  if (!setDurationState) return;

  if (rtc.Hour >= 6 && rtc.Hour < 18) {
    //    timeState = MORNING;
    if (pumpState)
      countDownPumpDuration = eep.pumpDurationMinute[SET_MORNING_ON] * 60 + eep.pumpDurationSecond[SET_MORNING_ON];
    else
      countDownPumpDuration = eep.pumpDurationMinute[SET_MORNING_OFF] * 60 + eep.pumpDurationSecond[SET_MORNING_OFF];
  } else {
    //    timeState = NIGHT;
    if (pumpState)
      countDownPumpDuration = eep.pumpDurationMinute[SET_NIGHT_ON] * 60 + eep.pumpDurationSecond[SET_NIGHT_ON];
    else
      countDownPumpDuration = eep.pumpDurationMinute[SET_NIGHT_OFF] * 60 + eep.pumpDurationSecond[SET_NIGHT_OFF];
  }
  setDurationState = false;
}
