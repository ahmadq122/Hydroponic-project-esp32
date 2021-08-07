
#define CHAR_KEYBOARD 92
#define CHAR_KEYBOARD1 66

const char *keyboard = "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+={}[]|/?;:'\",.<>";
const char *keyboard1 = "abcdefghijklmnopqrstuvwxyz0123456789_-!@#$%^&*()+={}[]|/?;:'\",.<> ";

//byte findCharKeyboardIndex(char Char) {
//  for (byte i = 0; i < sizeof(keyboard); i++) {
//    if (Char == keyboard[i])return i;
//  }
//  return 0xFF;
//}

void Settings_WiFi()
{
  String available_SSID[10];
  char ssidScanned[17];
  char ssidScannedToDisplay[10][15];
  byte btn;
  int i = 0;
  bool setting = true;

  LCD_ClearAtRow(16, 0, 0);
  LCD_ClearAtRow(16, 0, 1);

  LCD_PrintStringAtCursor("Scanning...", 0, 0);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
    do
    {
      LCD_PrintStringAtCursor("No Netwrks Found", 0, 1);
    } while (!buttonPressed);
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      if (i >= 9)
        continue;
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      available_SSID[i] = WiFi.SSID(i);
      available_SSID[i].toCharArray(ssidScannedToDisplay[i], 14);
      ssidScannedToDisplay[i][14] = '\0';
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
    do
    {
      btn = BUTTON;
      switch (btn)
      {
        case BTN_DOWN:
          if (++i > n - 1)
            i = n - 1;
          while (buttonPressed) {
            delay(50);
          }
          LCD_ClearAtRow(16, 0, 1);
          break;
        case BTN_UP:
          if (--i <= 0)
            i = 0;
          while (buttonPressed) {
            delay(50);
          }
          LCD_ClearAtRow(16, 0, 1);
          break;
        case BTN_SET:
          while (buttonPressed) {
            delay(50);
          }
          available_SSID[i].toCharArray(ssidScanned, 17);
          EEPROM_Set_SSID(ssidScanned);
          LCD_PrintStringAtCursor(String() + "Netework Set : ", 0, 0);
          delay(2000);
          setting = false;
          break;
        case BTN_MENU:
          while (buttonPressed) {
            delay(50);
          }
          break;
        default:
          break;
      }
      LCD_PrintStringAtCursor(String() + "Ntwrks Found " + n + ":", 0, 0);
      LCD_PrintStringAtCursor(String() + (i + 1) + " " + ssidScannedToDisplay[i], 0, 1);
    } while (btn != BTN_MENU && setting);
  }
}

void Settings_SSID_Or_Password(byte type)
{
  bool settingState = true;
  char str[17];
  byte setState = 0;
  byte counter = 0, btn = 0;
  int charIndex = 0;
  char charTemp[2];

  EEPROM_Read_Data();

  if (type == 0)
    strcpy(str, eep.ssid);
  else
    strcpy(str, eep.password);

  lcd.clear();
  while (buttonPressed) {
    delay(50);
  }
  LCD_PrintStringAtCursor("SET to Change   ", 0, 0);
  LCD_PrintStringAtCursor(str, 0, 1);
  if (type)
    delay(250);

  while (settingState)
  {
    btn = BUTTON;
    if (btn == BTN_MENU && btn != BTN_MENU | BTN_SET)
      return;
    if (!setState)
    {
      if (blinkSeconds)
        LCD_PrintStringAtCursor(str, 0, 1);
      else
      {
        if (type)
        {
          for (byte i = 0; i < 16; i++)
          {
            if (str[i] != '\0')
              str[i] = '*';
            else
              i = 16;
          }
        }
        LCD_ClearAtRow(16, 0, 1);
      }
      if (btn == BTN_SET)
      {
        setState++;
        while (buttonPressed) {
          delay(50);
        }
        lcd.clear();
        if (type == 0)
          LCD_PrintStringAtCursor("Set New SSID:   ", 0, 0);
        else
          LCD_PrintStringAtCursor("Set New Password", 0, 0);
        strcpy(str, " ");
      }
    }
    else
    {
      charTemp[0] = keyboard[charIndex];
      charTemp[1] = '\0';
      LCD_PrintStringAtCursor(charTemp, counter, 1);
      if (btn == BTN_UP)
      {
        if (++charIndex >= CHAR_KEYBOARD)
          charIndex = 0;
      }
      if (btn == BTN_DOWN)
      {
        if (--charIndex < 0)
          charIndex = CHAR_KEYBOARD - 1;
      }
      if (btn == BTN_SET)
      {
        counterTimerInterrupt = 5;
        str[counter] = keyboard[charIndex];
        if (type)
        {
          LCD_PrintStringAtCursor("*", counter, 1);
          charIndex = constrain(counter + charIndex, 0, CHAR_KEYBOARD - 1);
        }
        if (++counter >= 15)
          counter = 15;
        do
        {
          if (btn == BTN_SET && !counterTimerInterrupt)
          {
            str[counter] = '\0';
            if (type == 0)
              EEPROM_Set_SSID(str);
            else
              EEPROM_Set_Password(str);

            LCD_ClearAtRow(16, 0, 0);
            delay(500);
            if (type == 0)
              LCD_PrintStringAtCursor("New SSID is Set ", 0, 0);
            else
              LCD_PrintStringAtCursor("New Password Set", 0, 0);
            delay(2000);
            settingState = false;
            break;
          }
        } while (buttonPressed);
        if (counterTimerInterrupt > 0 && counterTimerInterrupt <= 3)
        {
          LCD_PrintStringAtCursor("  ", counter - 1, 1);
          counter -= 2;
          if (counter <= 0)
            counter = 0;
          if (type)
          {
            LCD_PrintStringAtCursor("*", counter, 1);
            charIndex = constrain(counter + charIndex, 0, CHAR_KEYBOARD - 1);
          }
        }
        counterTimerInterrupt = 0;
      }
      delay(150);
      LCD_ClearAtRow(1, counter, 1);
      delay(50);
    }
  }
}

void Settings_Recipient()
{
  bool settingState = true;
  char str[17];
  byte setState = 0;
  byte counter = 0, btn = 0;
  int charIndex = 0;
  char charTemp[2];

  EEPROM_Read_Data();

  strcpy(str, eep.recipientGMail);

  lcd.clear();
  while (buttonPressed) {
    delay(50);
  }

  LCD_PrintStringAtCursor("SET to Change   ", 0, 0);
  LCD_PrintStringAtCursor(str, 0, 1);

  while (settingState)
  {
    btn = BUTTON;
    if (btn == BTN_MENU && btn != BTN_MENU | BTN_SET)
      return;
    if (!setState)
    {
      if (blinkSeconds)
        LCD_PrintStringAtCursor(str, 0, 1);
      else
        LCD_ClearAtRow(16, 0, 1);

      if (btn == BTN_SET)
      {
        setState++;
        while (buttonPressed) {
          delay(50);
        }
        lcd.clear();
        LCD_PrintStringAtCursor("Set Recipient :", 0, 0);
        strcpy(str, " ");
      }
    }
    else
    {
      charTemp[0] = keyboard1[charIndex];
      charTemp[1] = '\0';
      LCD_PrintStringAtCursor(charTemp, counter, 1);
      if (btn == BTN_UP)
      {
        if (++charIndex >= CHAR_KEYBOARD1)
          charIndex = 0;
      }
      if (btn == BTN_DOWN)
      {
        if (--charIndex < 0)
          charIndex = CHAR_KEYBOARD1 - 1;
      }
      if (btn == BTN_SET)
      {
        counterTimerInterrupt = 5;
        str[counter] = keyboard1[charIndex];
        if (++counter >= 15)
          counter = 15;

        do
        {
          if (btn == BTN_SET && !counterTimerInterrupt)
          {
            str[counter] = '\0';

            EEPROM_Set_Recipient_GMail_Name(str);

            LCD_ClearAtRow(16, 0, 0);
            delay(500);
            LCD_PrintStringAtCursor("Recipient is set", 0, 0);
            delay(2000);
            settingState = false;
          }
        } while (buttonPressed);
        if (counterTimerInterrupt > 0 && counterTimerInterrupt <= 3)
        {
          LCD_PrintStringAtCursor("  ", counter - 1, 1);
          counter -= 2;
          if (counter <= 0)
            counter = 0;
        }
        counterTimerInterrupt = 0;
      }
      delay(100);
      LCD_ClearAtRow(1, counter, 1);
      delay(50);
    }
  }
}

void Settings_WaterLevelMinimum()
{
  unsigned int adcValue = 0;
  char strADC[5];
  bool setState = true;
  unsigned char button = 0;
  lcd.setCursor(0, 1);
  lcd.print(" SET to Confirm ");
  do
  {
    button = BUTTON;
    adcValue = ADC_GetWaterLevelValue();
    General_IntegerToString(adcValue, strADC, 4);
    lcd.home();
    lcd.print("Min. ADC :  ");
    lcd.print(strADC);
    if (button == BTN_SET)
    {
      setState = false;
      EEPROM_Set_Minimum_Water_Level_ADC(adcValue);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(300);
      lcd.setCursor(0, 1);
      lcd.print("Min. ADC is set ");
      delay(2000);
    }
    delay(50);
  } while (button != BTN_MENU && setState);
}

void Settings_WaterLevelMaximum()
{
  unsigned int adcValue = 0;
  char strADC[5];
  bool setState = true;
  unsigned char button = 0;
  lcd.setCursor(0, 1);
  lcd.print(" SET to Confirm ");
  do
  {
    button = BUTTON;
    adcValue = ADC_GetWaterLevelValue();
    General_IntegerToString(adcValue, strADC, 4);
    lcd.home();
    lcd.print("Max. ADC :  ");
    lcd.print(strADC);
    if (button == BTN_SET)
    {
      setState = false;
      EEPROM_Set_Maximum_Water_Level_ADC(adcValue);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(300);
      lcd.setCursor(0, 1);
      lcd.print("Max. ADC is set ");
      delay(2000);
    }
    delay(50);
  } while (button != BTN_MENU && setState);
}

void Settings_WaterLevelLow()
{
  int lowValue = eep.lowWaterLevelADC;
  char strLow[5];
  bool setState = true;
  unsigned char button = 0;
  lcd.setCursor(0, 1);
  lcd.print(" SET to Confirm ");
  lcd.home();
  lcd.print("Low Level:");
  do
  {
    button = BUTTON;

    General_IntegerToString(lowValue, strLow, 3);
    lcd.setCursor(10, 0);
    lcd.print(strLow);
    lcd.print("%  ");

    if (miliSecs300) {
      lcd.setCursor(10, 0);
      lcd.print("   ");
      miliSecs300 = false;
    }

    if (button == BTN_UP)
    {
      if (++lowValue > 100)
        lowValue = 0;
    }
    if (button == BTN_DOWN)
    {
      if (--lowValue < 0)
        lowValue = 100;
    }
    if (button == BTN_SET)
    {
      setState = false;
      EEPROM_Set_Low_Water_Level_ADC(lowValue);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      delay(300);
      lcd.setCursor(0, 1);
      lcd.print(" Low level set  ");
      delay(2000);
    }
    if (buttonPressed) {
      lcd.setCursor(10, 0);
      lcd.print(strLow);
      delay(100);
    }
  } while (button != BTN_MENU && setState);
}

void Settings_Time()
{
  bool settingState = true;
  int hh, mm, ss;
  char strTime[17];
  byte setState = 0;
  byte counter = 0, btn = 0;
  tmElements_t d;

  RTC.read(d);
  hh = d.Hour;
  mm = d.Minute;
  ss = d.Second;

  strcpy(strTime, "                ");
  General_IntegerToString(hh, &strTime[4], 2);
  strTime[6] = ':';
  General_IntegerToString(mm, &strTime[7], 2);
  strTime[9] = ':';
  General_IntegerToString(ss, &strTime[10], 2);
  strTime[16] = '\0';
  lcd.clear();
  while (buttonPressed) {
    delay(50);
  }
  lcd.home();
  lcd.print("    HH:MM:SS    ");
  lcd.setCursor(0, 1);
  lcd.print(strTime);
  while (settingState)
  {
    btn = BUTTON;
    if (btn == BTN_MENU)
      return;
    switch (setState)
    {
      case 0:
        if (btn == BTN_UP)
        {
          if (++hh > 23)
            hh = 0;
        }
        if (btn == BTN_DOWN)
        {
          if (--hh < 0)
            hh = 23;
        }
        if (btn == BTN_SET)
        {
          while (buttonPressed) {
            delay(50);
          }
          setState++;
        }
        if (counter > 5)
        {
          strTime[4] = ' ';
          strTime[5] = ' ';
        }
        break;
      case 1:
        if (btn == BTN_UP)
        {
          if (++mm > 59)
            mm = 0;
        }
        if (btn == BTN_DOWN)
        {
          if (--mm < 0)
            mm = 59;
        }
        if (btn == BTN_SET)
        {
          while (buttonPressed) {
            delay(50);
          }
          setState++;
        }
        if (counter > 5)
        {
          strTime[7] = ' ';
          strTime[8] = ' ';
        }
        break;
      case 2:
        if (btn == BTN_UP)
        {
          if (++ss > 59)
            ss = 0;
        }
        if (btn == BTN_DOWN)
        {
          if (--ss < 0)
            ss = 59;
        }
        if (counter > 5)
        {
          strTime[10] = ' ';
          strTime[11] = ' ';
        }
        if (btn == BTN_SET)
        {
          while (buttonPressed) {
            delay(50);
          }
          settingState = false;
          lcd.clear();
          RTC.setTime(hh, mm, ss);
          lcd.home();
          lcd.print("  Time is set   ");
          delay(2000);
        }
        break;
      default:
        break;
    }
    if (counter <= 5)
      General_IntegerToString(hh, &strTime[4], 2);
    strTime[6] = ':';
    if (counter <= 5)
      General_IntegerToString(mm, &strTime[7], 2);
    strTime[9] = ':';
    if (counter <= 5)
      General_IntegerToString(ss, &strTime[10], 2);
    lcd.setCursor(0, 1);
    lcd.print(strTime);
    if (++counter >= 10)
      counter = 0;
    delay(150);
  }
}

void Settings_Pump_Duration(byte type)
{
  bool settingState = true;
  int ss, mm;
  char strTime[17];
  byte setState = 0;
  byte counter = 0, btn = 0;

  EEPROM_Read_Data();

  mm = (int)eep.pumpDurationMinute[type];
  ss = (int)eep.pumpDurationSecond[type];

  strcpy(strTime, "MM:SS      00:00");
  General_IntegerToString(mm, &strTime[11], 2);
  strTime[13] = ':';
  General_IntegerToString(ss, &strTime[14], 2);
  strTime[16] = '\0';
  lcd.clear();
  while (buttonPressed) {
    delay(50);
  }
  lcd.home();

  switch (type)
  {
    case SET_MORNING_ON:
      lcd.print("Pagi On Duration");
      break;
    case SET_MORNING_OFF:
      lcd.print("Pagi Off Durati.");
      break;
    case SET_NIGHT_ON:
      lcd.print("Malam On Durati.");
      break;
    case SET_NIGHT_OFF:
      lcd.print("Malam Off Durat.");
      break;
    default:
      break;
  }
  lcd.setCursor(0, 1);
  lcd.print(strTime);
  while (settingState)
  {
    btn = BUTTON;
    if (btn == BTN_MENU)
      return;
    switch (setState)
    {
      case 0:
        if (btn == BTN_UP)
        {
          if (++mm > 59)
            mm = 0;
        }
        if (btn == BTN_DOWN)
        {
          if (--mm < 0)
            mm = 59;
        }
        if (btn == BTN_SET)
        {
          while (buttonPressed) {
            delay(50);
          }
          setState++;
        }
        if (counter > 5)
        {
          strTime[11] = ' ';
          strTime[12] = ' ';
        }
        break;
      case 1:
        if (btn == BTN_UP)
        {
          if (++ss > 59)
            ss = 0;
        }
        if (btn == BTN_DOWN)
        {
          if (--ss < 0)
            ss = 59;
        }
        if (counter > 5)
        {
          strTime[14] = ' ';
          strTime[15] = ' ';
        }
        if (btn == BTN_SET)
        {
          while (buttonPressed) {
            delay(50);
          }
          settingState = false;
          EEPROM_Set_Pump_Duration(type, (unsigned int)((mm * 60) + ss));
          lcd.setCursor(0, 1);
          lcd.print("  Duration set  ");
          delay(2000);
          countDownPumpDuration = 0;
        }
        break;
      default:
        break;
    }

    if (counter <= 5)
      General_IntegerToString(mm, &strTime[11], 2);
    strTime[13] = ':';
    if (counter <= 5)
      General_IntegerToString(ss, &strTime[14], 2);
    lcd.setCursor(0, 1);
    lcd.print(strTime);
    if (++counter >= 10)
      counter = 0;
    delay(150);
  }
}

void Setting_SetPhAdjustCalib()
{
  float ph[2] = {0.0, 0.0}, phWanted[2] = {0.0, 0.0};
  bool setState = true;
  unsigned char button = 0;
  uint8_t settingState = 0;

  lcd.clear();

  do
  {
    button = BUTTON;
    ph[settingState] = (ADC_GetWaterPhValue());
    lcd.home();
    if (!settingState)
      lcd.print("Curr. pH1 :");
    else
      lcd.print("Curr. pH2 :");
    lcd.print((String() + ph[settingState]).c_str());
    lcd.setCursor(0, 1);
    if (!settingState)
      lcd.print("Wanted pH1:");
    else
      lcd.print("Wanted pH2:");
    lcd.print((String() + phWanted[settingState]).c_str());

    if (button == BTN_UP)
    {
      phWanted[settingState] += 0.5;
      if (phWanted[settingState] > 12)
        phWanted[settingState] = 0;
      lcd.setCursor(11, 1);
      lcd.print("     ");
    }
    if (button == BTN_DOWN)
    {
      phWanted[settingState] -= 0.5;
      if (phWanted[settingState] < 0)
        phWanted[settingState] = 12;
      lcd.setCursor(11, 1);
      lcd.print("     ");
    }

    if (button == BTN_SET)
    {
      if (settingState == 0)
      {
        settingState = 1;
      }
      else {
        setState = false;
        EEPROM_Set_PH_Low_Point(0, ph[0]);
        EEPROM_Set_PH_Low_Point(1, phWanted[0]);
        EEPROM_Set_PH_Nom_Point(0, ph[1]);
        EEPROM_Set_PH_Nom_Point(1, phWanted[1]);

        lcd.setCursor(0, 1);
        lcd.print("                ");
        delay(300);
        lcd.setCursor(0, 1);
        lcd.print(" pH Calib done  ");
        delay(2000);
      }
      lcd.clear();
      delay(500);
    }
    delay(500);
  } while (button != BTN_MENU && setState);
}
