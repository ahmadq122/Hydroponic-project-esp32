
void Task_Setup() {
  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    Task_EvenEverySecond
    ,  "Task_EvenEverySecond"   // A name just for humans
    ,  1024 * 4 // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  6  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    Task_AdcRead1
    ,  "Task_AdcRead1"
    ,  1024 * 4  // Stack size
    ,  NULL
    ,  5  // Priority
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    Task_AdcRead2
    ,  "Task_AdcRead2"
    ,  1024 * 4  // Stack size
    ,  NULL
    ,  4  // Priority
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    Task_EvenSerialReceive
    ,  "Task_EvenSerialReceive"
    ,  1024 * 10 // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    Task_EvenSendEmail
    ,  "Task_EvenSendEmail"
    ,  1024 * 20 // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    Task_EvenEvery5Seconds
    ,  "Task_EvenEvery5Seconds"   // A name just for humans
    ,  1024 * 10  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void Task_EvenEverySecond(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  // initialize digital LED_BUILTIN on pin 13 as an output.
  for (;;) // A Task shall never return or exit.
  {
    if (counterTimerInterrupt)counterTimerInterrupt--;
    if (counterTimerInterrupt1)counterTimerInterrupt1--;
    if (counterTimerInterrupt2)counterTimerInterrupt2--;
    if (countDownPumpDuration)countDownPumpDuration--;
    if (counterBluetooth)counterBluetooth--;
    else {
      Serial.println("Disconnected!");
      bluetoothConnected = false;
    }
    blinkSeconds = !blinkSeconds;
    secondTriggered = true;

    if (++counterTimerMinutes > 59) {
      counterTimerMinutes = 0;
      wifiCheckState = true;
      wifiTimerCheckState = true;
    }

    vTaskDelay(1000);
    //    Serial.println("Task_EvenEverySecond");
  }
}

void Task_AdcRead1(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  // initialize digital LED_BUILTIN on pin 13 as an output.
  for (;;) // A Task shall never return or exit.
  {
    if (loopState) {
      adcValue[WATER_LEVEL] = ADC_GetWaterLevelValue();
      //      Serial.print("wL:");
      //      Serial.println(adcValue[WATER_LEVEL]);
    }
    vTaskDelay(1000);
  }
}

void Task_AdcRead2(void *pvParameters)  // This is a task.
{
  float ph = 0.0;
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
    if (loopState) {
      phOfWater = ADC_GetPhOfWater();
      //      ph = ADC_GetWaterPhValue();
      //      phOfWater = (byte)((ph + 0.5) + phAdjustCalib);
      //      Serial.print("pH:");
      //      Serial.print(phOfWater);
      //      Serial.print(" real:");
      //      Serial.print(ph);
      //      Serial.print(" adjust:");
      //      Serial.println(phAdjustCalib);
    }
    vTaskDelay(1000);
  }
}

void Task_EvenSerialReceive(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  // initialize digital LED_BUILTIN on pin 13 as an output.
  for (;;) // A Task shall never return or exit.
  {
    if (!menuState && loopState) {
      Serial_Event2();
      vTaskDelay(100);
    }
    else
      vTaskDelay(1000);
    //    Serial.println("Task_EvenSerialReceive");
  }
}

void Task_EvenSendEmail(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  // initialize digital LED_BUILTIN on pin 13 as an output.
  for (;;) // A Task shall never return or exit.
  {
    if (wifiConnection == CONNECTED && !menuState && loopState)
    {
      if (!Email_Send_Notification())
        vTaskDelay(1000);
    }
    else {
      vTaskDelay(1000);
    }
  }
}

void Task_EvenEvery5Seconds(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  // initialize digital LED_BUILTIN on pin 13 as an output.
  for (;;) // A Task shall never return or exit.
  {
    if (!menuState && loopState)
      Serial_SendRefreshActual();
    vTaskDelay(5000);
  }
}
