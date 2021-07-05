byte indexPh = 0;
byte indexLevel = 0;

void ADC_ValueArrayInit() {
  while (indexPh < sample_movAverage - 1) {
    adcValueArray[WATER_LEVEL][indexPh] = analogRead(ADC_PIN_WATER_LEVEL);
    adcValueArray[WATER_PH][indexLevel] = analogRead(ADC_PIN_WATER_PH);
    delay(100);
    indexPh++;
    indexLevel = indexPh;
  }
}

unsigned int ADC_GetWaterLevelValue() {
  unsigned long adcValue = 0;

  adcValueArray[WATER_LEVEL][indexLevel] = analogRead(ADC_PIN_WATER_LEVEL);
  for (byte j = 0; j < sample_movAverage; j++)
    adcValue += adcValueArray[WATER_LEVEL][j];
  adcValue /= sample_movAverage;
  for (byte j = 0; j < sample_movAverage - 1; j++)
    adcValueArray[WATER_LEVEL][j] = adcValueArray[WATER_LEVEL][j + 1];
  indexLevel = sample_movAverage - 1;
  return (unsigned int)adcValue;
}

float calibration_value = 12.0;

float ADC_GetWaterPhValue() {
  unsigned long adcValue = 0;
  float pH = 0;

  adcValueArray[WATER_PH][indexPh] = analogRead(ADC_PIN_WATER_PH);
  for (byte j = 0; j < sample_movAverage; j++)
    adcValue += adcValueArray[WATER_PH][j];
  adcValue /= sample_movAverage;
  for (byte j = 0; j < sample_movAverage - 1; j++)
    adcValueArray[WATER_PH][j] = adcValueArray[WATER_PH][j + 1];
  indexPh = sample_movAverage - 1;

  float volt = ((float)adcValue / 4095.0) * 3.3;
  //  Serial.println(String() + "Volt:" + volt);
  pH = (-5.0 * volt) + calibration_value;

  return pH;
}

float ADC_CalculateVoltPh(float x1, float y1, float x2, float y2, float ph)
{
  float m, b, phWater;

  if (x1 >= x2 || y1 >= y2) return 0.0;
  /* Straight Line Equation Forming

                  +------------------+
              | y = mx + b     |
                  +------------------+
         where
                 y = y coordinate
                 x = x coordinate
                 m = (y0 - y1)/(x0 - x1)

         so,     b = y - mx
  */

  m = ((float)x2 - (float)x1) / (y2 - y1);
  b = x1 - m * y1;
  phWater = ((ph - b) / m) + 0.004;
  if (phWater <= 0) phWater = 0;
  return phWater;
}

float ADC_GetPhOfWater(void)
{
  float ph;
  ph = ADC_GetWaterPhValue();
  return (ADC_CalculateVoltPh(eep.phLow[0], eep.phLow[1], eep.phNom[0], eep.phNom[1], ph));
}
