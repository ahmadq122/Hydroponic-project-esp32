/////////////////////////////
#define MENU_EXIT       0xFF
////////////////////////////

void Menu_MenuDisplay() {
  String mainMenu [6] = {
    "SET SENS. LEVEL",
    "SET PH CALIB.  ",
    "SET MODE       ",
    "SET TIME       ",
    "SET WIFI       ",
    "GMAIL RECIPIENT",
  };
  byte option = 0;
  do {
    option = Menu_GetSelectedMenu (mainMenu, 6, option);
    if (option == 0)Menu_SensorLevel();
    else if (option == 1)Setting_SetPhAdjustCalib();
    else if (option == 2)Menu_Set_Mode();
    else if (option == 3)Settings_Time();
    else if (option == 4)Menu_Set_WIFI();
    else if (option == 5)Menu_Set_GMAIL();
  } while (option != MENU_EXIT);
}

void Menu_SensorLevel() {
  String setLevelPointMenu[3] {
    "SET MINIMUM PT.",
    "SET MAXIMUM PT.",
    "SET LOW LEVEL  "
  };
  byte option = 0;
  do {
    option = Menu_GetSelectedMenu (setLevelPointMenu, 3, option);
    if (option == 0)Settings_WaterLevelMinimum();//Settings_SSID_Or_Password(0);
    else if (option == 1)Settings_WaterLevelMaximum();
    else if (option == 2)Settings_WaterLevelLow();
  } while (option != MENU_EXIT);
}

void Menu_Set_WIFI() {
  String setWIFIMenu[2] {
    "SET SSID       ",
    "SET PASSWORD   "
  };
  byte option = 0;
  do {
    option = Menu_GetSelectedMenu (setWIFIMenu, 2, option);
    if (option == 0)Settings_WiFi();//Settings_SSID_Or_Password(0);
    else if (option == 1)Settings_SSID_Or_Password(1);
  } while (option != MENU_EXIT);
}

void Menu_Set_GMAIL() {
  Settings_Recipient();
}


byte Menu_Set_Mode() {
  String setModeMenu[2] {
    "MODE AUTO      ",
    "MODE MANUAL    "
  };
  byte option = 0;
  do {
    option = Menu_GetSelectedMenu (setModeMenu, 2, option);
    if (option == 0)Menu_Set_ModeAuto();
    else if (option == 1)Menu_Set_ModeManual();
  } while (option != MENU_EXIT);
}

void Menu_Set_ModeAuto() {
  String setModeAutoMenu[2] {
    "SET PAGI (6-18)",
    "SET NIGHT(18-6)"
  };
  byte option = 0;
  do {
    option = Menu_GetSelectedMenu (setModeAutoMenu, 2, option);
    if (option == 0)Menu_Setting_Pump_Duration(MORNING);
    else if (option == 1)Menu_Setting_Pump_Duration(NIGHT);
  } while (option != MENU_EXIT);
}

void Menu_Set_ModeManual() {
  String setModeManualMenu[2] {
    "PUMP ON        ",
    "PUMP OFF       "
  };
  byte option = 0;
  do {
    option = Menu_GetSelectedMenu (setModeManualMenu, 2, option);
    if (option == 0)Setting_StatePump(ON);
    else if (option == 1)Setting_StatePump(OFF);
  } while (option != MENU_EXIT);
}

void Menu_Setting_Pump_Duration(byte type) {
  String setPeriodMenu[2] {
    "SET ON DURATION",
    "SET OFF DURATI."
  };
  byte option = 0;
  do {
    option = Menu_GetSelectedMenu (setPeriodMenu, 2, option);
    if (option == 0) {
      if (type == MORNING)
        Settings_Pump_Duration(SET_MORNING_ON);
      else
        Settings_Pump_Duration(SET_NIGHT_ON);
    }
    else if (option == 1) {
      if (type == MORNING)
        Settings_Pump_Duration(SET_MORNING_OFF);
      else
        Settings_Pump_Duration(SET_NIGHT_OFF);
    }
  } while (option != MENU_EXIT);
}

byte Menu_GetSelectedMenu (String * menu, byte numberMenu, byte initOptionState) {
  bool dirSelection = true;
  bool menuState = true;
  int optionState = initOptionState;
  lcd.clear();
  while (buttonPressed);
  while (menuState) {
    do {
      Menu_DisplayMenuSelection(menu, optionState, dirSelection);
      switch (BUTTON) {
        case BTN_UP:
          dirSelection = false;
          if (--optionState <= 0)optionState = 0;
          break;
        case BTN_DOWN:
          dirSelection = true;
          if (++optionState >= numberMenu - 1)optionState = numberMenu - 1;
          break;
        case BTN_MENU:
          menuState = false;
          optionState = MENU_EXIT;
          break;
        case BTN_SET:
          menuState = false;
          break;
        default: break;
      }
      while (buttonPressed);
    } while (optionState != MENU_EXIT && menuState);
  }
  return optionState;
}

void Menu_DisplayMenuSelection(String * menu, byte optionState, bool dirSelection) {
  if (optionState == 0) {
    Menu_DisplayStringMenu(menu[optionState], 0, 1);
    Menu_DisplayStringMenu(menu[optionState + 1], 1, 0);
  }  else {
    if (dirSelection) {
      Menu_DisplayStringMenu(menu[optionState - 1], 0, 0);
      Menu_DisplayStringMenu(menu[optionState], 1, 1);
    } else {
      Menu_DisplayStringMenu(menu[optionState], 0, 1);
      Menu_DisplayStringMenu(menu[optionState + 1], 1, 0);
    }
  }
}

void Menu_DisplayStringMenu(String menu, bool row, bool selected) {
  lcd.setCursor(0, row);
  if (selected)lcd.print(">");
  else lcd.print(" ");
  lcd.setCursor(1, row);
  lcd.print(menu);
}
