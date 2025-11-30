float readTemp() {
  float temperature = 0;
  int samplesize = 32;
  float voltage = 0;
  for (int i = 0; i < samplesize; i++) {
    voltage = 0;
    for (int i = 0; i < samplesize; i++)
      //voltage += analogRead(temppin) * (3.3/1023.0) + 0.175;
      voltage += analogReadMilliVolts(temppin);
    float avgvoltage = voltage / samplesize;
    //temperature += (avgvoltage-0.5)/0.01;
    temperature += (avgvoltage - 500) / 10;
  }

  float avgtemperature = temperature / samplesize;

  return avgtemperature;
}

float readvoltageDivider(){
  float voltageread = 0;
  int samplesize = 32;
  for(int i = 0; i< samplesize; i++)
    voltageread += analogReadMilliVolts(watervoltagedividerPin);
  float avgvoltage = voltageread/samplesize;
  return avgvoltage;
}

float calculateWaterLevel()
{
  float levelvalue;

  levelvalue = map(readvoltageDivider(), 0, 1.65, 0-percentageignored, 100);

  return levelvalue;
}