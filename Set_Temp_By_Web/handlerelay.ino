void forwardmodetemprelay()
{
  if(temp >= SET_POINT_HI)
  {
    turnontemp();
    //Serial.println("ALERT, forward");
  }
  else if(temp <= SET_POINT_LO)
  {
    turnofftemp();
    //Serial.println("Calm, forward");
  }
}

void inversemodetemprelay()
{
  if(temp <= SET_POINT_LO)
  {
    turnontemp();
    //Serial.println("ALERT, inverse");
  }
  else if(temp >= SET_POINT_HI)
  {
    turnofftemp();
    //Serial.println("Calm, inverse");
  }
}

void forwardmodewaterrelay()
{
  if(currentwaterlevel <= minwaterpercentvalue)
    turnonwater();
  else if(currentwaterlevel >= maxwaterpercentvalue)
    turnoffwater();
}

void inversemodewaterrelay()
{
  if(currentwaterlevel <= minwaterpercentvalue)
    turnoffwater();
  else if(currentwaterlevel >= maxwaterpercentvalue)
    turnonwater();
}


void turnontemp()
{
  digitalWrite(temprelayPin, HIGH);
  relayStateTemp = true;
  MB.Hreg(holdingRegisters[relayStateTemp_ID], relayStateTemp);
  MB.Coil(coils[relayStateTemp_COIL], relayStateTemp);
}

void turnofftemp()
{
  digitalWrite(temprelayPin, LOW);
  relayStateTemp = false;
  MB.Hreg(holdingRegisters[relayStateTemp_ID], relayStateTemp);
  MB.Coil(coils[relayStateTemp_COIL], relayStateTemp);
}

void turnonwater()
{
  digitalWrite(waterrelayPin, HIGH);
  relayStateWater = true;
  MB.Hreg(holdingRegisters[relayStateWater_ID], relayStateWater);
  MB.Coil(coils[relayStateWater_COIL], relayStateWater);
}

void turnoffwater()
{
  digitalWrite(waterrelayPin, LOW);
  relayStateWater = false;
  MB.Hreg(holdingRegisters[relayStateWater_ID], relayStateWater);
  MB.Coil(coils[relayStateWater_COIL], relayStateWater);
}