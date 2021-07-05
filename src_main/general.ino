void General_IntegerToString(uint32_t number, char *buffer, uint8_t len)
{
  //  String tempStr = String() + number;
  char numbStr[11];
  //  char charFirstZero[10];
  //  uint8_t a = 0, b = 0;

  if (len > 10)
    return;

  sprintf(buffer, (String() + "%0" + len + "d").c_str(), number);
}

String General_IntegerToString(uint32_t number, uint8_t len)
{
  //  String tempStr = String() + number;
  char numbStr[11];
  //  char charFirstZero[10];
  //  uint8_t a = 0, b = 0;

  if (len > 10)
    return "";

  sprintf(numbStr, (String() + "%0" + len + "d").c_str(), number);

  return (String() + numbStr);
}
void General_GetHourMinuteFromGivenString(String timeStr, uint8_t *hour, uint8_t *minute)
{
  char strCharTime[6];
  String dataStr[2];
  uint8_t indexData = 0;
  strcpy(strCharTime, timeStr.c_str());
  for (uint8_t i = 0; i < 6; i++)
  {
    if (strCharTime[i] != ':')
      dataStr[indexData] += strCharTime[i];
    else
    {
      indexData++;
    }
  }
  *hour = dataStr[0].toInt();
  *minute = dataStr[1].toInt();
}

void General_GetStringHourMinuteFromGivenSeconds(char *buffer, uint16_t seconds) {
  uint8_t hour = seconds / 3600;
  uint8_t minute = (seconds - (hour * 3600)) / 60;

  sprintf(buffer, "%02d:%02d", hour, minute);
}

void General_GetStringMinuteSecondFromGivenSeconds(char *buffer, uint16_t seconds) {
  uint8_t hour = seconds / 60;
  uint8_t minute = seconds % 60;

  sprintf(buffer, "%02d:%02d", hour, minute);
}
