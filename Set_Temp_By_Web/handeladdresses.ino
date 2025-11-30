void setupaddresses()
{
  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE_NEG, OUTPUT);
  digitalWrite(MAX485_DE, 0);
  digitalWrite(MAX485_RE_NEG, HIGH);

  SerialRS485.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  MB.begin(&SerialRS485, MAX485_DE); // DE pin controls transceiver
  MB.setBaudrate(9600);
  MB.slave(SLAVE_ID);
  pinMode(PIN_5V_EN, OUTPUT);
  digitalWrite(PIN_5V_EN, HIGH);


  //for testing only rn
  for(int i = 0; i<16; i++)
    coils[i] = i;
  for(int i =0; i<16; i++)
    MB.addCoil(coils[i], 1); // Coils 00001-00016
  for(int i =0; i<16;i++)
    MB.Coil(coils[i], false);
  
  MB.Coil(coils[1], true);

  //for testing only rn
  for(int i = 0; i<16; i++)
    discreteInputs[i] = i;
  for(int i =0; i<16; i++)
    MB.addIsts(discreteInputs[i], 1); // Discrete Inputs 10001-10016
  for(int i =0; i<16; i++)
    MB.Ists(discreteInputs[i],false);
      

  for(int i = 0; i<16; i++)
    holdingRegisters[i] = i;
  for(int i =0; i<16; i++)
    MB.addHreg(holdingRegisters[i], 1); // Holding Registers 40001-40016
  for(int i =0; i<16; i++)
    MB.Hreg(holdingRegisters[i], 0);


}


