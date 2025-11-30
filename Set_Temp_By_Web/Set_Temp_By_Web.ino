#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <esp_task_wdt.h>
#include <ModbusRTU.h>
#include <HardwareSerial.h>

//===================================================================
//defined pins
#define WDT_TIMEOUT 2000  // 2000mS = 2 second...,LBTR,.
#define temppin 33
#define ledPin 5          // GPIO pin connected to WS2812
#define BRIGHTNESS 64     // Adjust brightness (0-255)
#define CONFIG_FREERTOS_NUMBER_OF_CORES 1
#define temprelayPin 12
#define waterrelayPin 32
#define watervoltagedividerPin 35
#define EEPROM_SIZE 256
#define buttonPin 34 //pullup button to switch between network and non network

// RS485 control pins
#define MAX485_DE 17
#define MAX485_RE_NEG 19
#define TX_PIN 22     // GPIO17 for RS485 TX ,for LILYGO CAN485 its pin-22
#define RX_PIN 21     // GPIO16 for RS485 RX ,for LILYGO CAN485 its pin-21
#define PIN_5V_EN 16  // ENABLE CHIP POWER SUPPLY 5VDC

//===================================================================
// Global variables
WebServer server(80);
//default values
uint16_t SET_POINT_LO = 50;   
uint16_t SET_POINT_HI = 100;  
bool isForward = true;       
bool relayStateTemp = false;      

static unsigned long lastUpdate;
float temp;
float prevtemp;
float isnetworkon = true;

int percentageignored = 0;
int maxwaterpercentvalue = 80;
int minwaterpercentvalue = 20;
bool iswaterforwardmode;
float currentwaterlevel;
bool relayStateWater = false; 

float vwatermin;
float vwatermax;

float prevwater;

//===================================================================
// EEPROM addresses
const int addrlowtemppoint = 0;   //Low point
const int addrhightemppoint = 16;  //High point
const int addrtemprelaymode = 32;  //Forward or inverse mode
const int addrSlave = 48; //slave address
const int addrNetwork = 64; //network state (active or not)
const int addrwaterperc = 80; //the current water percentage
const int addrmaxwater = 96; //max water tank value
const int addrminwater = 112; //min water tank value
const int addrwaterrelay = 128; // is water relay in forward mode

//===================================================================
// Fixed IP Configuration
IPAddress local_IP(192, 168, 4, 1);  // Fixed IP address
IPAddress gateway(192, 168, 4, 1);   // Gateway
IPAddress subnet(255, 255, 255, 0);  // Subnet mask
//===================================================================

// Fixes complier error “invalid conversion from ‘int’ to ‘const esp_task_wdt_config_t*'”:
esp_task_wdt_config_t twdt_config = {
  .timeout_ms = WDT_TIMEOUT,
  .idle_core_mask = 0,  // Bitmask of cores
  .trigger_panic = true,
};

//===================================================================
// Data storage
int coils[16];                  // 00001-00016
int discreteInputs[16];         // 10001-10016
uint16_t holdingRegisters[16];  // 40001-40016
uint16_t SLAVE_ID = 1; //default value 1

//===================================================================
// Configure hardware serial for RS485
HardwareSerial SerialRS485(2);  // UART2 (RX:21, TX:22,FOR LILYGO CAN485)
ModbusRTU MB;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
//===================================================================
//if adding anything to the enums make sure to add it to the void pointer accordingly
enum RegistersID{
  isnetworkon_ID,
  SET_POINT_LO_ID, 
  SET_POINT_HI_ID, 
  isForward_ID, 
  relayStateTemp_ID, 
  temp_ID, 
  percentageignored_ID, 
  maxwaterpercentvalue_ID, 
  minwaterpercentvalue_ID, 
  iswaterforwardmode_ID, 
  relayStateWater_ID,
  currentwaterlevel_ID,
  NUM_ID_REGISTERS //get the size of the enum
};

enum CoilsID{
  isForward_COIL,
  iswaterforwardmode_COIL,
  isnetworkon_COIL,
  relayStateTemp_COIL,
  relayStateWater_COIL,

  NUM_COIL_REGISTERS
};
//Create a pointer to be able to do loops, if adding anything here or to the enum make sure to fix it accordingly

void* holdingRegisterVars[NUM_ID_REGISTERS]= {
  &isnetworkon,
  &SET_POINT_LO,
  &SET_POINT_HI,
  &isForward,
  &relayStateTemp,
  &temp,
  &percentageignored,
  &maxwaterpercentvalue,
  &minwaterpercentvalue,
  &iswaterforwardmode,
  &relayStateWater,
  &currentwaterlevel
};

void* holdingCoilVars[NUM_COIL_REGISTERS] = {
  &isForward,
  &iswaterforwardmode,
  &isnetworkon
};
//===================================================================
void setup() {
  analogReadResolution(10);
  //this allows us to accept 3.3v analog in
  analogSetPinAttenuation(temppin, ADC_11db);
  analogSetPinAttenuation(watervoltagedividerPin, ADC_11db);
  //LBTR add watchdog timer,.
  esp_task_wdt_deinit();            //wdt is enabled by default, so we need to 'deinit' it first
  esp_task_wdt_init(&twdt_config);  //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);           //add current thread to WDT watch

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(temppin, INPUT);
  pinMode(temprelayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  pinMode(watervoltagedividerPin, INPUT);
  pinMode(waterrelayPin, OUTPUT);
  digitalWrite(temprelayPin, LOW);
  Serial.begin(115200);

  

  EEPROM.begin(EEPROM_SIZE);

  delay(1);

  loadSettings();
  delay(1);
  loadSlaveID();
  delay(1);
  loadNetwork();
  delay(1);

  setupaddresses();
  delay(1);
  
  if(SLAVE_ID <=0 || SLAVE_ID>=248)
  {
    SLAVE_ID=1;
    Serial.println("Invalid SLAVE ID");
    Serial.println("Setting the SLAVE ID to 1");
  }

  if(isnetworkon)
  {
    networkstart();
  }

  // Initialize timer: timer 0, prescaler 80 (1 µs per tick), count up
  timer = timerBegin(1000000);

  // Attach the ISR
  timerAttachInterrupt(timer, &timer1interupt);

  // Set timer to trigger every 3,000,000 µs = 3 seconds
  timerAlarm(timer, 700000, true, 0);


}

unsigned long update;
unsigned long updatenetworktimer;
volatile bool ledstate = false;
volatile bool blinkingled = false;
volatile bool running = false;
volatile bool switcheable = true;

void loop() {
  esp_task_wdt_reset();  //reset the watchdog
  MB.task();
  yield();


  blinkingled = isnetworkon;

  if(blinkingled && !running)
    {
      // Enable the timer
      delay(100);
      timerStart(timer);
      delay(100);
    }
  else if(!blinkingled && running)
  {
    delay(100);
    timerStop(timer);
    delay(100);
    running = false;
    digitalWrite(ledPin, LOW);
  }

  if(isnetworkon)
    server.handleClient();          //handle the connected client

  if (update > 4294967296 - 1100)
    update = 0;

  if (update < millis() - 1000) {
    update = millis();
    prevtemp = temp;
    temp = readTemp();

    currentwaterlevel = calculateWaterLevel();
    prevwater = currentwaterlevel;

    if ((int)prevtemp != (int)temp)
    {
      noInterrupts();
      MB.Hreg(holdingRegisters[temp_ID], (int)temp);
      interrupts();
    }

    if((int)prevwater != (int)currentwaterlevel)
    {
      noInterrupts();
      MB.Hreg(holdingRegisters[currentwaterlevel_ID]);
    }

    if (isForward) {
      forwardmodetemprelay();
    } else {
      inversemodetemprelay();
    }

    if(iswaterforwardmode)
      forwardmodewaterrelay();
    else
      inversemodewaterrelay();
  }

  if(digitalRead(buttonPin) == LOW)
  {
    if(millis() >= 3000 + updatenetworktimer && switcheable)
    {
      Serial.println("Button clicked");
      digitalWrite(ledPin, HIGH);
      
      blinkingled = isnetworkon;
      switcheable = false;

      if(!isnetworkon)
        networkstart();
      else
        networkstop();

      isnetworkon = !isnetworkon;
      
      updatenetworktimer = millis();
      delay(10);
    }
    saveNetwork();
  }
  else
  {
    updatenetworktimer = millis();
    switcheable = true;
  }  

  for(int i=0; i<NUM_ID_REGISTERS; i++)
  {
    //if the holding register got edited from usb, change the value on the board
    //except the varying ones we don't want to be saved like current temp
    if(i==relayStateTemp_ID)
      continue;
    if(i==relayStateWater_ID)
      continue;
    if(i==temp_ID)
      continue;
    if(i==currentwaterlevel_ID)
      continue;

    if(MB.Hreg(holdingRegisters[i]) != *(uint16_t*)holdingRegisterVars[i])
    {
      *(uint16_t*)holdingRegisterVars[i] = MB.Hreg(holdingRegisters[i]);
      saveSettings();
    }
  }

  for(int i =0; i<NUM_COIL_REGISTERS; i++)
  {
    if(i==relayStateTemp_COIL)
      continue;
    if(i==relayStateWater_COIL)
      continue;

    if(MB.Coil(coils[i]) != *(uint16_t*)holdingCoilVars[i])
    {
      *(uint16_t*)holdingCoilVars[i] = MB.Coil(coils[i]);
      saveSettings();
    }
  }
}