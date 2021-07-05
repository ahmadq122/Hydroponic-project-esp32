bool Email_Send_Notification() {
  if (systemSafe && counterTimerInterrupt2 > 0)
    counterTimerInterrupt2 = 0;
  if (!systemSafe && !counterTimerInterrupt2) {
    counterTimerInterrupt2 = 3600;
    if (wifiConnection) {
      switch (eep.numberOfRecipient) {
        case 1:
          Email_SendStringNotifTo(eep.recipientGMail, "", "", 1);
          Serial.println("Send Email to 1 Person");
          break;
        case 2:
          Email_SendStringNotifTo(eep.recipientGMail, eep.recipientGMail_1, "", 2);
          //          Email_SendStringNotifTo(eep.recipientGMail);
          //          //          delay(1000);
          //          Email_SendStringNotifTo(eep.recipientGMail_1);
          Serial.println("Send Email to 2 Persons");
          break;
        case 3:
          Email_SendStringNotifTo(eep.recipientGMail, eep.recipientGMail_1, eep.recipientGMail_2, 3);
          //          Email_SendStringNotifTo(eep.recipientGMail);
          //          //          delay(1000);
          //          Email_SendStringNotifTo(eep.recipientGMail_1);
          //          //          delay(1000);
          //          Email_SendStringNotifTo(eep.recipientGMail_2);
          Serial.println("Send Email to 3 Persons");
          break;
        default: break;
      }
      return true;
    }
  }
  return false;
}

void Email_SendStringNotifTo(char * recipientGMailName1, char * recipientGMailName2, char * recipientGMailName3, byte noRecipient) {
  char Str5[5];
  char recipient_GMail1[27];
  char recipient_GMail2[27];
  char recipient_GMail3[27];
  // Send HTML email
  char body [150] = "<b>WARNING!!!</b><br>Water level is LOW,<br>ADC Minimum: ";
  char body1 [25]  = "<br>ADC Actual : ";
  char body2 [60]  = "<br><i>Thanks...</i>";

  strcpy(recipient_GMail1, recipientGMailName1);
  strcat(recipient_GMail1, "@gmail.com");
  strcpy(recipient_GMail2, recipientGMailName2);
  strcat(recipient_GMail2, "@gmail.com");
  strcpy(recipient_GMail3, recipientGMailName3);
  strcat(recipient_GMail3, "@gmail.com");

  General_IntegerToString(eep.minimumWaterLevelADC, Str5, 4);
  strcat(body, Str5);
  General_IntegerToString(ADC_GetWaterLevelValue(), Str5, 4);
  strcat(body1, Str5);
  strcat(body, body1);
  strcat(body, body2);
  //  String result = sendEmail("Test", "ESP32", body, "survivingwithandroid@gmail.com", true);
  // Send plain email
  if (noRecipient == 1)
    Email_Send("ESP32-Notification", "ESP32-BOT", body, recipient_GMail1, true);
  else if (noRecipient == 2)
    Email_Send2("ESP32-Notification", "ESP32-BOT", body, recipient_GMail1, recipient_GMail2, true);
  else if (noRecipient == 3)
    Email_Send3("ESP32-Notification", "ESP32-BOT", body, recipient_GMail1, recipient_GMail2, recipient_GMail3, true);
}

String Email_Send(char *subject, char *sender, char *body, char *recipient, boolean htmlFormat) {
  smtp.setLogin(GMAIL_SMTP_SEVER, GMAIL_SMTP_PORT, GMAIL_SMTP_USERNAME, GMAIL_SMTP_PASSWORD);
  smtp.setSender(sender, GMAIL_SMTP_USERNAME);
  smtp.setSubject(subject);
  smtp.setMessage(body, htmlFormat);
  smtp.addRecipient(recipient);
  //  smtp.addRecipient(recipient2);
  if (!MailClient.sendMail(smtp))
    return MailClient.smtpErrorReason();

  return "";
}

String Email_Send2(char *subject, char *sender, char *body, char *recipient1, char *recipient2, boolean htmlFormat) {
  smtp.setLogin(GMAIL_SMTP_SEVER, GMAIL_SMTP_PORT, GMAIL_SMTP_USERNAME, GMAIL_SMTP_PASSWORD);
  smtp.setSender(sender, GMAIL_SMTP_USERNAME);
  smtp.setSubject(subject);
  smtp.setMessage(body, htmlFormat);
  smtp.addRecipient(recipient1);
  smtp.addRecipient(recipient2);
  if (!MailClient.sendMail(smtp))
    return MailClient.smtpErrorReason();

  return "";
}
String Email_Send3(char *subject, char *sender, char *body, char *recipient1, char *recipient2, char *recipient3, boolean htmlFormat) {
  smtp.setLogin(GMAIL_SMTP_SEVER, GMAIL_SMTP_PORT, GMAIL_SMTP_USERNAME, GMAIL_SMTP_PASSWORD);
  smtp.setSender(sender, GMAIL_SMTP_USERNAME);
  smtp.setSubject(subject);
  smtp.setMessage(body, htmlFormat);
  smtp.addRecipient(recipient1);
  smtp.addRecipient(recipient2);
  smtp.addRecipient(recipient3);
  if (!MailClient.sendMail(smtp))
    return MailClient.smtpErrorReason();

  return "";
}

bool wifiBeginState = false;
byte Wifi_ConnectToWiFi() {
  //  counterTimerInterrupt = 10;
  if (!wifiBeginState) {
    //    Serial.print("\nConnecting to ");
    //    Serial.println(eep.ssid);
    WiFi.begin(eep.ssid, eep.password);
    wifiBeginState = true;
  }

  //  Serial.print(eep.ssid);
  //  while (WiFi.status() != WL_CONNECTED && counterTimerInterrupt > 0) {
  //    Serial.print(".");
  //    delay(500);
  //  }
  if (counterTimerInterrupt > 0 && WiFi.status() == WL_CONNECTED) {
    //    Serial.println("\nConnected!");
    counterTimerInterrupt = 0;
    wifiCheckState = false;
    return CONNECTED;
  } else if (counterTimerInterrupt == 0 && WiFi.status() != WL_CONNECTED) {
    //    Serial.println("\nNot Connected!");
    wifiCheckState = false;
    wifiBeginState = false;
    return NOT_CONNECTED;
  } else {
    //    Serial.println("\nScanning..!");
    if (!counterTimerInterrupt) {
      wifiBeginState = false;
      wifiCheckState = false;
    }
  }
  //  Serial.println(WiFi.localIP);
  return UNKNOWN;
}
