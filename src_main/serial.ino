#define REFRESH_SETTING   "ref1"
#define REFRESH_ACTUAL    "ref2"
#define SET_SETTING       "set1"
#define SET_ACTUAL        "set2"
#define SET_CONNECTED        "connected"

enum DataReceiveList
{
  Message_Id,
  Auto_Mode,
  Email_1,
  RTC_Time,
  Minimum_ADC,
  Period_Morning_On,
  Period_Morning_Off,
  Period_Night_On,
  Period_Night_Off
};

void Serial_Event2()
{
  while (Serial2.available() && !flagSetSettingGotFromSerial2)
  {
    //    Serial.println(inputString);
    // Serial.println("available!");
    // get the new byte:
    char inChar = (char)Serial2.read();
    // add it to the inputString:
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    // Serial.print(inChar);

    if (inChar == '\n')
    {
      //      Serial.println("end!");
      //      stringComplete = true;
      Serial.println(inputString);
      serialSetting = inputString;
      flagSetSettingGotFromSerial2 = true;
      //      Serial_SplitString();
      inputString = "";
      //      stringComplete = false;
      Serial_Flush();
    }
    else
      inputString += inChar;
    vTaskDelay(20);
  }
  //  delay(10);
}

void Serial_SplitString()
{
  char receivedCharString[130];
  char splitChar = ';';
  char character = ' ';
  uint8_t indexChar = 0;
  uint8_t indexData = 0;
  String stringData = "";
  uint8_t hour, minute, second;

  String commandMode = "";
  receivedCharString[126] = '\n';
  strcpy(receivedCharString, serialSetting.c_str());
  //  Serial.println(receivedCharString);
  while (character != '\n' && indexChar < 125)
  {
    character = receivedCharString[indexChar];
    //    Serial.print(String() + indexChar + " " + character + " ");
    if (character == ';')
    {
      switch (indexData)
      {
        case Message_Id:
          commandMode = stringData;
          if (commandMode == REFRESH_SETTING) {
            Serial_SendRefreshSetting();
            //            delay(1000);
            //            Serial_SendRefreshActual();
            return;
          } else if (commandMode == REFRESH_ACTUAL) {
            Serial_SendRefreshActual();
            return;
          }
          else if (commandMode == SET_CONNECTED) {
            counterBluetooth = Bluetooth_TimeOut;
            bluetoothConnected = true;
            //            Serial.println("Connected!");
            return;
          }
          break;
        case Auto_Mode:
          if (commandMode == SET_SETTING)
          {
            if (stringData == "ON")
              modeOperation = 1;
            else
              modeOperation = 0;
            EEPROM_Set_Mode_Operation(modeOperation);
            //            modeOperation = eep.modeOperation;
            //            Serial.print("Mode Auto :");
            //            Serial.println(eep.modeOperation);
          }
          else if (commandMode == SET_ACTUAL) {
            if (!modeOperation) {
              if (stringData == "ON")
                Setting_StatePump(ON);
              else if (stringData == "OFF")
                Setting_StatePump(OFF);
              Serial_SendRefreshActual();
            }
            return;
          }
          break;
        case Email_1:
          if (commandMode == SET_SETTING)
          {
            int str_len = stringData.length() + 1;
            char char_array[str_len];
            stringData.toCharArray(char_array, str_len);

            Serial.print(char_array);
            EEPROM_Set_Recipient_GMail_Name(char_array);
          }
          break;
        case RTC_Time:
          if (commandMode == SET_SETTING)
          {
            General_GetHourMinuteFromGivenString(stringData, &hour, &minute);
            RTC.setTime(hour, minute, 0);
            Serial.print("Mode RTC :");
            Serial.println(String() + hour + ":" + minute);
          }
          break;
        case Minimum_ADC:
          if (commandMode == SET_SETTING)
          {
            EEPROM_Set_Minimum_Water_Level_ADC(stringData.toInt());
            //            Serial.print("Min ADC :");
            //            Serial.println(eep.minimumWaterLevelADC);
          }
          break;
        case Period_Morning_On:
          if (commandMode == SET_SETTING)
          {
            General_GetHourMinuteFromGivenString(stringData, &minute, &second);
            EEPROM_Set_Pump_Duration(SET_MORNING_ON, ((minute * 60) + second));
            //            eep.pumpDurationMinute[0] = hour;
            //            eep.pumpDurationSecond[0] = minute;
            //            Serial.print("Morning On :");
            //            Serial.println(String() + hour + ":" + minute);
          }
          break;
        case Period_Morning_Off:
          if (commandMode == SET_SETTING)
          {
            General_GetHourMinuteFromGivenString(stringData, &minute, &second);
            EEPROM_Set_Pump_Duration(SET_MORNING_OFF, ((minute * 60) + second));
            //            eep.pumpDurationMinute[1] = hour;
            //            eep.pumpDurationSecond[1] = minute;
            //            Serial.print("Morning Off :");
            //            Serial.println(String() + hour + ":" + minute);
          }
          break;
        case Period_Night_On:
          if (commandMode == SET_SETTING)
          {
            General_GetHourMinuteFromGivenString(stringData, &minute, &second);
            EEPROM_Set_Pump_Duration(SET_NIGHT_ON, ((minute * 60) + second));
            //            eep.pumpDurationMinute[2] = hour;
            //            eep.pumpDurationSecond[2] = minute;
            //            Serial.print("Night On :");
            //            Serial.println(String() + hour + ":" + minute);
          }
          break;
        case Period_Night_Off:
          if (commandMode == SET_SETTING)
          {
            General_GetHourMinuteFromGivenString(stringData, &minute, &second);
            EEPROM_Set_Pump_Duration(SET_NIGHT_OFF, ((minute * 60) + second));
            //            eep.pumpDurationMinute[3] = hour;
            //            eep.pumpDurationSecond[3] = minute;
            //            Serial.print("Night Off :");
            //            Serial.println(String() + hour + ":" + minute);
            return;
          }
          break;
        default:
          break;
      }
      indexData++;
      //Serial.println(stringData);
      stringData = "";
    }
    else
    {
      stringData += character;
    }
    indexChar++;
  }
  //  Serial.println();
}



void Serial_SendRefreshSetting() {
  String strDataToSend = "";
  char split = ';';
  char timeStr[6];

  strDataToSend = "ref1";
  strDataToSend += split;
  strDataToSend += modeOperation ? "ON" : "OFF";
  strDataToSend += split;
  strDataToSend += String() + eep.recipientGMail;
  strDataToSend += split;
  if (stringTime[2] == ' ')stringTime[2] == ':';
  strDataToSend += stringTime;
  strDataToSend += split;
  strDataToSend += String() + eep.minimumWaterLevelADC;
  strDataToSend += split;
  General_GetStringMinuteSecondFromGivenSeconds(timeStr, ((eep.pumpDurationMinute[0] * 60) + eep.pumpDurationSecond[0]));
  strDataToSend += timeStr;
  strDataToSend += split;
  General_GetStringMinuteSecondFromGivenSeconds(timeStr, ((eep.pumpDurationMinute[1] * 60) + eep.pumpDurationSecond[1]));
  strDataToSend += timeStr;
  strDataToSend += split;
  General_GetStringMinuteSecondFromGivenSeconds(timeStr, ((eep.pumpDurationMinute[2] * 60) + eep.pumpDurationSecond[2]));
  strDataToSend += timeStr;
  strDataToSend += split;
  General_GetStringMinuteSecondFromGivenSeconds(timeStr, ((eep.pumpDurationMinute[3] * 60) + eep.pumpDurationSecond[3]));
  strDataToSend += timeStr;
  if (bluetoothConnected)
  { Serial2.println(strDataToSend);
    Serial.println(strDataToSend);
  }
  //  Serial2.println("ref1;ON;05:05;1200;12:00;11:00;10:00;09:10");
}

void Serial_SendRefreshActual() {
  String strDataToSend = "";
  char split = ';';

  strDataToSend = "ref2";
  strDataToSend += split;
  strDataToSend += pumpState ? "ON" : "OFF";
  strDataToSend += split;
  strDataToSend += systemSafe ? "OK" : "LOW";
  strDataToSend += split;
  strDataToSend += General_IntegerToString(phOfWater, 2);
  if (bluetoothConnected) {
    Serial2.println(strDataToSend);
    Serial.println(strDataToSend);
  }
  //  Serial2.println("ref2;ON;OK;10");
}

void Serial_Flush() {
  while (Serial2.available()) {
    Serial2.read();
  }
}
