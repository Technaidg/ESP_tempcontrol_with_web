void saveSettings() {
  // Write setpoints to EEPROM
  EEPROM.put(addrlowtemppoint, SET_POINT_LO);
  EEPROM.put(addrhightemppoint, SET_POINT_HI);
  EEPROM.put(addrtemprelaymode, isForward);

  EEPROM.put(addrwaterperc, percentageignored);
  EEPROM.put(addrmaxwater, maxwaterpercentvalue);
  EEPROM.put(addrminwater, minwaterpercentvalue);
  EEPROM.put(addrwaterrelay, iswaterforwardmode);

  EEPROM.commit();

  //update the serial values

  for(int i=0; i<NUM_ID_REGISTERS; i++)
  {
    if(i==relayStateTemp_ID)
      continue;
    if(i==relayStateWater_ID)
      continue;
    if(i==temp_ID)
      continue;
    if(i==currentwaterlevel_ID)
      continue;
    MB.Hreg(holdingRegisters[i], *(uint16_t*)holdingRegisterVars[i]);
  }

  for(int i=0; i<NUM_COIL_REGISTERS; i++)
  {
    if(i==relayStateTemp_COIL)
      continue;
    if(i==relayStateWater_COIL)
      continue;
    MB.Coil(coils[i], *(uint16_t*)holdingCoilVars[i]);
  }
}

//===================================================================
void loadSettings() {
  // Read setpoints from EEPROM
  EEPROM.get(addrlowtemppoint, SET_POINT_LO);
  Serial.print("Old var low temp point: ");
  Serial.println(SET_POINT_LO);

  EEPROM.get(addrhightemppoint, SET_POINT_HI);
  Serial.print("Old var high temp point: ");
  Serial.println(SET_POINT_HI);

  EEPROM.get(addrtemprelaymode, isForward);
  Serial.print("Old temp relay mode: ");
  Serial.println(isForward);

  EEPROM.get(addrwaterperc, percentageignored);
  Serial.print("Water tank percent: ");
  Serial.println(percentageignored);

  EEPROM.get(addrmaxwater, maxwaterpercentvalue);
  Serial.print("Max Water value: ");
  Serial.println(maxwaterpercentvalue);

  EEPROM.get(addrminwater, minwaterpercentvalue);
  Serial.print("Min Water value: ");
  Serial.println(minwaterpercentvalue);

  EEPROM.get(addrwaterrelay, iswaterforwardmode);
  Serial.print("Is water in forward mode?: ");
  Serial.println(iswaterforwardmode);

  //update the serial values

  for(int i=0; i<NUM_ID_REGISTERS; i++)
  {
    if(i==relayStateTemp_ID)
      continue;
    if(i==relayStateWater_ID)
      continue;
    if(i==temp_ID)
      continue;
    if(i==currentwaterlevel_ID)
      continue;
    Serial.println("Loading to serial values ");
    MB.Hreg(holdingRegisters[i], *(uint16_t*)holdingRegisterVars[i]);
  }

  for(int i=0; i<NUM_COIL_REGISTERS; i++)
  {
    if(i==relayStateTemp_COIL)
      continue;
    if(i==relayStateWater_COIL)
      continue;
    Serial.println("Loading to coil values ");
    MB.Coil(coils[i], *(uint16_t*)holdingCoilVars[i]);
  }
}
//===================================================================
void saveSlaveID()
{
  EEPROM.put(addrSlave, SLAVE_ID);
  EEPROM.commit();
}
//===================================================================
void loadSlaveID()
{
  EEPROM.get(addrSlave, SLAVE_ID);
  Serial.print("Slave ID is: ");
  Serial.println(SLAVE_ID);
}
//===================================================================
void saveNetwork()
{
  EEPROM.put(addrNetwork, isnetworkon);
  MB.Hreg(holdingRegisters[isnetworkon_ID], isnetworkon);
  MB.Coil(coils[isnetworkon_COIL], isnetworkon);
  EEPROM.commit();
}
//===================================================================
void loadNetwork()
{
  EEPROM.get(addrNetwork, isnetworkon);
  MB.Hreg(holdingRegisters[isnetworkon_ID], isnetworkon);
  MB.Coil(coils[isnetworkon_COIL], isnetworkon);
  Serial.print("Network State is: ");
  Serial.println(isnetworkon);
}