
/////////////
void EEPROM_Read_Data()
{
  EEPROM.get(0, eep);
  //  Serial.println("Executed");
  for (uint8_t i = 0; i < 2; i++) {
    eep.phLow[i] = checkFloatValue(eep.phLow[i]);
    eep.phNom[i] = checkFloatValue(eep.phNom[i]);
  }
  Serial.println("DataFlashRead");
  delay(10);
}

float checkFloatValue(float value) {
  if (isnan(value)) return 0.0;
  if (isinf(value)) return 0.0;
  if (value > 4294967040.0) return 0.0;  // constant determined empirically
  if (value < -4294967040.0) return 0.0;
  return value;
}

void EEPROM_Set_SSID(char *ssid)
{
  strcpy(eep.ssid, ssid);
  Serial.print("SSIDSet:");
  Serial.println(ssid);
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}

void EEPROM_Set_Password(char *password)
{
  strcpy(eep.password, password);
  Serial.print("PWDSet:");
  Serial.println(password);
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}

void EEPROM_Set_numberOfRecipient(byte numberOfRecipient)
{
  eep.numberOfRecipient = numberOfRecipient;
  Serial.print("NoOfRecSet:");
  Serial.println(numberOfRecipient);
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}

void EEPROM_Set_Recipient_GMail_Name(char *gmailName, byte forNumberOfRecipient)
{
  if (forNumberOfRecipient == 0)
  {
    strcpy(eep.recipientGMail, gmailName);
    Serial.print("Rec1Set:");
  }
  else if (forNumberOfRecipient == 1)
  {
    strcpy(eep.recipientGMail_1, gmailName);
    Serial.print("Rec2Set:");
  }
  else if (forNumberOfRecipient == 2)
  {
    strcpy(eep.recipientGMail_2, gmailName);
    Serial.print("Rec3Set:");
  }
  Serial.println(gmailName);
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}

void EEPROM_Set_Pump_Duration(byte type, unsigned int duration)
{
  unsigned int pumpDuration = duration;

  switch (type)
  {
    case SET_MORNING_ON:
      Serial.print("PumpDurMorningOn set (MM:SS):");
      break;
    case SET_MORNING_OFF:
      Serial.print("PumpDurMorningOff set (MM:SS):");
      break;
    case SET_NIGHT_ON:
      Serial.print("PumpDurNightOn set (MM:SS):");
      break;
    case SET_NIGHT_OFF:
      Serial.print("PumpDurNightOff set (MM:SS):");
      break;
    default:
      break;
  }
  eep.pumpDurationMinute[type] = pumpDuration / 60;
  eep.pumpDurationSecond[type] = pumpDuration % 60;

  Serial.println(String() + eep.pumpDurationMinute[type] + ":" + eep.pumpDurationSecond[type]);
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}

void EEPROM_Set_Mode_Operation(byte modeOperation)
{
  eep.modeOperation = modeOperation;
  Serial.print("ModOperSet:");
  Serial.println(modeOperation == MANUAL ? "Manual" : "Auto");
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}

void EEPROM_Set_PH_Low_Point(uint8_t data, float value)
{
  eep.phLow[data] = value;
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}

void EEPROM_Set_PH_Nom_Point(uint8_t data, float value)
{
  eep.phNom[data] = value;
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}

void EEPROM_Set_Minimum_Water_Level_ADC(unsigned int minimumWaterLevelADC)
{
  eep.minimumWaterLevelADC = minimumWaterLevelADC;
  Serial.print("MinADCSet:");
  Serial.println(minimumWaterLevelADC);
  EEPROM.put(0, eep);
  EEPROM.commit();
  delay(10);
}
