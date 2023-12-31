/**
 *******************************************************************************
 * @copyright   Copyright (C) by SenseAir AB. All rights reserved.
 * @file        sunrise_i2c_single.ino
 * @brief       Example for reading sensor data in single measurement mode.
 *              Only for revision.
 *              
 *              Based on the "I2C on Senseair Sunrise" documentation 
 *              (available on the www.senseair.com website). This example mainly
 *              covers operations in single measurement mode.
 * @details     Tested on Arduino Mega 2560, Adafruit ESP32 Feather     
 *              
 * @author      William Sandkvist
 * @version     0.08
 * @date        2019-12-20
 *
 *******************************************************************************
 */

#include <SRI2C.h>

/* WARINING!!!
  Some wire driver implementations do not corectly implement Wire.endTransmission(false) function, 
  so please check this before disable WIRE_WORKAROUND!
  For example, known "bad" implementations are: Nucleo STM32
 */
#define WIRE_WORKAROUND   (0)

/* It could be necessary to disable ABC if sensor will be tested with CO2 concentrations below 400ppm. */
#define DISABLE_ABC       (1)

/* Define serial EN pin */
const int       SUNRISE_EN              = 1;

/* Sunrise communication address, both for Modbus and I2C */
const uint8_t   SUNRISE_ADDR            = 0x68;

/* Amount of wakeup attempts before time-out */
const int       ATTEMPTS                 = 5;
/* It takes 25ms to write one EE register */
const int       EEPROM_UPDATE_DELAY_MS   = 25;


/* Register Addresses */
const uint8_t ERROR_STATUS             = 0x01;
const uint8_t MEASUREMENT_MODE         = 0x95;
const uint8_t METER_CONTROL            = 0xA5;
const uint8_t START_MEASUREMENT        = 0xC3;
const uint8_t ABC_TIME                 = 0xC4;

/* Measurement modes */
const uint16_t CONTINUOUS               = 0x0000;
const uint16_t SINGLE                   = 0x0001;

/* Delays in milliseconds*/
const int STABILIZATION_MS              = 35;
const int WAIT_FOR_PIN_MS               = 2000;

/* Reading period, in milliseconds. Default is 4 seconds */
int readPeriodMs = 4000;


/* Array for storing sensor state data */
uint8_t state[24];

uint16_t SRco2Val;

/* Initialize I2C bus and pins */
void  reInitI2C() {
  /* Initialize I2C and use default pins defined for the board */
  Wire.begin();
  /* Setup I2C clock to 100kHz */
  Wire.setClock(100000);  
}


/* Workaround regarding BAD implementations of Wire.endTransmission(false) function */
int WireRequestFrom(uint8_t dev_addr, uint8_t bytes_numbers, uint8_t offset_to_read, bool stop) {
 int error;
#if (WIRE_WORKAROUND == 1)
 error = Wire.requestFrom((uint8_t)dev_addr, (uint8_t)bytes_numbers /* how many bytes */, (uint32_t)offset_to_read /* from address*/, (uint8_t)1/* Address size - 1 byte*/, stop /* STOP*/); 
#else 
  Wire.beginTransmission(dev_addr);
  Wire.write(offset_to_read); //starting register address, from which read data
  Wire.endTransmission(false);
  error = Wire.requestFrom((uint8_t)dev_addr, (uint8_t)bytes_numbers /* how many bytes */, (uint8_t)stop /* STOP*/);
#endif
  return error;
}

/** 
 * @brief  Wakes up the sensor by initializing a write operation
 *         with no data.
 * 
 * @param  target:      The sensor's communication address
 * @note   This example shows a simple way to wake up the sensor.
 * @retval true if successful, false if failed
 */
bool _wakeup(uint8_t target)
{
  int attemps = ATTEMPTS;
  int error;
 
  do {
    uint8_t byte_0;    
    /* */
    Wire.beginTransmission(target);
    error = Wire.endTransmission(true);
  } while(((error != 0 /*success */) && (error != 2 /*Received NACK on transmit of address*/) && (error != 1 /* BUG in STM32 library*/)) && (--attemps > 0)); 
  /* STM32 driver can stack under some conditions */
  if(error == 4) {
    /* Reinitialize I2C*/
    reInitI2C();
    return false;
  } 
  return (attemps > 0);
}


/**
 * @brief  Enable or disable ABC.
 * 
 * @param  target: The sensor's communication address
 * @param  enable: Set true to enable or false to disable ABC
 * @note   This example shows a simple way to enable/disable ABC. 
 * It could be necessary to disable ABC if sensor will be tested with CO2 concentrations below 400ppm.
 * @retval None
 */
void setABC(uint8_t target, bool enable) {
  /* Wakeup */
  if(!(_wakeup(target))) {
    Serial.print("Failed to wake up sensor.");
    return;
  }

  /* Request values */
  int error = WireRequestFrom(target, 1, METER_CONTROL /* from address*/, true /* STOP*/);    
  if(error != 1 ) {
    Serial.print("Failed to write to target. Error code : ");
    Serial.println(error);
    return;
  }

  uint8_t  meterControl = Wire.read();

  if(enable) {
    Serial.println("Enabling ABC...");
    meterControl &= (uint8_t)~0x02U;
  } else {
    Serial.println("Disabling ABC...");
    meterControl |= 0x02U;
  }

  /* Wakeup */
  if(!(_wakeup(target))) {
    Serial.print("Failed to wake up sensor.");
    return;
  }

  Wire.beginTransmission(target);
  Wire.write(METER_CONTROL);
  Wire.write(meterControl);
  error = Wire.endTransmission(true);
  delay(EEPROM_UPDATE_DELAY_MS);
  
  if(error != 0) {
    Serial.print("Failed to send request. Error code: ");
    Serial.println(error); 
    /* FATAL ERROR */
  //  while(true);
  }
}

/**
 * @brief  Reads and prints the sensor's current measurement mode,
 *         measurement period and number of samples.
 * 
 * @param  target: The sensor's communication address
 * @note   This example shows a simple way to read the sensor's
 *         measurement configurations.
 * @retval None
 */
void read_sensor_config(uint8_t target) {
  /* Function variables */
  int error;
  int numBytes = 7;

  /* Wakeup */
  if(!(_wakeup(target))) {
    Serial.print("Failed to wake up sensor.");
    return;
  }

  /* Request values */
  error = WireRequestFrom(target, numBytes /* how many bytes */, MEASUREMENT_MODE /* from address*/, true /* STOP*/);    
  if(error != numBytes ) {
    Serial.print("Failed to write to target. Error code : ");
    Serial.println(error);
    return;
  }

  /* Read values */
  /* Measurement mode */
  uint8_t measMode = Wire.read();

  /* Measurement period */
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();
  uint16_t measPeriod = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo;

  /* Number of samples */
  byteHi = Wire.read();
  byteLo = Wire.read();
  uint16_t numSamples = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo;

  /* ABCPeriod */
  byteHi = Wire.read();
  byteLo = Wire.read();
  uint16_t abcPeriod = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo;

  /* Most propable that the sensor will not go into sleep mode, but to be insure...*/
  /* Wakeup */
  if(!(_wakeup(target))) {
    Serial.print("Failed to wake up sensor.");
    return;
  }

  /* Request values */
  error = WireRequestFrom(target, 1, METER_CONTROL /* from address*/, true /* STOP*/);    
  if(error != 1 ) {
    Serial.print("Failed to write to target. Error code : ");
    Serial.println(error);
    return;
  }
 
  uint8_t  meterControl = Wire.read();

  Serial.print("Measurement Mode: ");
  Serial.println(measMode);
  readPeriodMs = measPeriod * 1000;

  Serial.print("Measurement Period, sec: ");
  Serial.println(measPeriod);

  Serial.print("Number of Samples: ");
  Serial.println(numSamples);  
  
  if((0U == abcPeriod) ||  (0xFFFFU == abcPeriod) || (meterControl & 0x02U)) {
    Serial.println("ABCPeriod: disabled");
  } else {
    Serial.print("ABCPeriod, hours: ");
    Serial.println(abcPeriod);  
  }  
  
  Serial.print("MeterControl: ");
  Serial.println(meterControl, HEX); 

#if (DISABLE_ABC ==0)
  /* If we do not implicity disable ABC try to enable it in case if someone forget to do that...*/
  if((meterControl & 0x02U) != 0) {    
    setABC(target, true);
  }
#endif
  
}

/**
 * @brief  Changes the sensor's current measurement mode, if it's
 *         currently in single mode. 
 * 
 * @param  target:      The sensor's communication address
 * @note   This example shows a simple way to change the sensor's
 *         measurement mode. The sensor has to be manually restarted after the
 *         changes.
 * @retval None
 */
void change_measurement_mode(uint8_t target) {
  /* Function variables */
  int error;
  int numBytes = 1;
  
  /* Wakeup */
  if(!(_wakeup(target))) {
    Serial.print("Failed to wake up sensor.");
    /* FATAL ERROR */
  //  while(true);
  }

  /* Read Value */
  error = WireRequestFrom(target, numBytes /* how many bytes */, MEASUREMENT_MODE /* from address*/, true /* STOP*/);    
  if(error != numBytes ) {  
    Serial.print("Failed to read measurement mode. Error code: ");
    Serial.println(error);
    /* FATAL ERROR */
  //  while(true);
  }

  /* Change mode if continuous */
  if(Wire.read() != SINGLE) {
    Serial.println("Changing Measurement Mode to Single...");
    /* Wakeup */
    if(!_wakeup(target)) {
      Serial.print("Failed to wake up sensor.");
      /* FATAL ERROR */
    //  while(true);
   }
    Wire.beginTransmission(target);
    Wire.write(MEASUREMENT_MODE);
    Wire.write(SINGLE);
    error = Wire.endTransmission(true);
    delay(EEPROM_UPDATE_DELAY_MS);
    
    if(error != 0) {
      Serial.print("Failed to send request. Error code: ");
      Serial.println(error); 
      /* FATAL ERROR */
    //  while(true);
    }
    Serial.println("Sensor restart is required to apply changes");
     /* Turn-off sensor */
    digitalWrite(SUNRISE_EN, LOW);
    /* Wait for sensor restart */
    delay(STABILIZATION_MS);
    /* Turn-on sensor */
    digitalWrite(SUNRISE_EN, HIGH);
  }
}

/**
 * @brief  Reads the sensors current state data.
 * 
 * @param  target:      The sensor's communication address
 * @note   If host device has no state data, it is very important 
 *         that host do not write '0' to address 0xC2 - 0xDB the 
 *         first time it starts a measurement.
 * @retval None
 */
void save_state(uint8_t target) {
  /* Function variables */
  int error;

  int numBytes = 24;

  /* Drive EN pin HIGH */
  digitalWrite(SUNRISE_EN, HIGH);

  /* Wait for sensor start-up and stabilization */
  delay(STABILIZATION_MS);


  /* Wakeup */
  if(!(_wakeup(target))) {
    Serial.print("Failed to wake up sensor.");
    /* FATAL ERROR */
    digitalWrite(SUNRISE_EN, LOW);
    //while(true);
  }

  /* Request state data */
  error = WireRequestFrom(target, numBytes /* how many bytes */, ABC_TIME /* from address*/, true /* STOP*/);    
  if(error != numBytes ) { 
    Serial.print("Failed to read measurements command. Error code: ");
    Serial.println(error);
    digitalWrite(SUNRISE_EN, LOW);
    /* FATAL ERROR */
//    while(true);
  }

  /* Read and save state data */
  for(int n = 0 ; n < numBytes ; n++) {
    state[n] = Wire.read();
  }

  /* Drive EN pin LOW */
  digitalWrite(SUNRISE_EN, LOW);

  Serial.println("Saved Sensor State Successfully\n");
}

/**
 * @brief  Reads and prints the sensor's current CO2 value and
 *         error status.
 * 
 * @param  target:      The sensor's communication address
 * @note   This example shows a simple way to read the sensor's
 *         CO2 measurement and error status.
 * @retval true if successful, false if failed
 */
bool read_sensor_measurements(uint8_t target) {
  /* Function variables */
  int error;
  bool  readSRstatus = 1;
  int numRegCmd = 25;
  int numRegRead = 7;
  int numRegState = 24;

  uint8_t cmdArray[25];

  cmdArray[0] = 0x01;

  for(int n = 1 ; n < numRegCmd ; n++) {
    cmdArray[n] = state[n-1];
  }

  /* Drive EN pin HIGH */
  digitalWrite(SUNRISE_EN, HIGH);

  /* Wait for sensor start-up and stabilization */
  delay(STABILIZATION_MS);

  /* Wakeup */
  if(!_wakeup(target)) {
    Serial.print("Failed to wake up sensor.");
    readSRstatus = 0;
    return readSRstatus;
  }

  /* Write measurement command and sensor state to 0xC3 */
  Wire.beginTransmission(target);
  Wire.write(START_MEASUREMENT);
  for(int reg_n =0; reg_n < numRegCmd; reg_n++) {
    Wire.write(cmdArray[reg_n]);
  }
  error = Wire.endTransmission(true);
  if(error != 0) {
    Serial.print("Failed to send measurement command. Error code: ");
    Serial.println(error);
    digitalWrite(SUNRISE_EN, LOW);
    readSRstatus = 0;
    return readSRstatus;
  }

  /* Wait until ready pin goes low */
  delay(WAIT_FOR_PIN_MS);

  /* Wakeup */
  if(!(_wakeup(target))) {
    Serial.print("Failed to wake up sensor.");
    digitalWrite(SUNRISE_EN, LOW);
    readSRstatus = 0;
    return readSRstatus;
  }

  /* Request values */
  error = WireRequestFrom(target, numRegRead /* how many bytes */, ERROR_STATUS /* from address*/, true /* STOP*/);    
  if(error != numRegRead ) {  
    Serial.print("Failed to read values. Error code: ");
    Serial.println(error);
    digitalWrite(SUNRISE_EN, LOW);
    readSRstatus = 0;
    return readSRstatus;
  }

  /* Read values */
  /* Error status */
  uint8_t eStatus = Wire.read();

  /* Reserved */
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();

  byteHi = Wire.read();
  byteLo = Wire.read();

  /* CO2 value */
  byteHi = Wire.read();
  byteLo = Wire.read();
  SRco2Val = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo;

  /* Wakeup */
  if(!_wakeup(target)) {
    Serial.print("Failed to wake up sensor.");
    digitalWrite(SUNRISE_EN, LOW);
    readSRstatus = 0;
    return readSRstatus;
  }

  /* Read sensor state data from 0xC4-0xDB and save it for next measurement */
  error = WireRequestFrom(target, numRegState /* how many bytes */, ABC_TIME /* from address*/, true /* STOP*/); 
  if(error != numRegState) {
    Serial.print("Failed to read measurements command. Error code: ");
    Serial.println(error);
    digitalWrite(SUNRISE_EN, LOW);
    readSRstatus = 0;
    return readSRstatus;
  }
  
  for(int n = 0 ; n < numRegState ; n++) {
    state[n] = Wire.read();
  }

  /* Drive EN pin LOW */
  digitalWrite(SUNRISE_EN, LOW);
  #ifdef main_SERIAL_DEBUG
  /* Print values */
  Serial.print("CO2: ");
  Serial.print(SRco2Val);
  Serial.println(" ppm");

  Serial.print("Error Status: 0x");
  Serial.println(eStatus, HEX);
  Serial.println();
  #endif
return readSRstatus;  
}

/**
 * @brief  This function runs once at the start.
 *
 * @retval None
 */
void init_SRI2C_single(void)
{
  

  pinMode(SUNRISE_EN, OUTPUT);
  digitalWrite(SUNRISE_EN, HIGH);
  /* Wait for sensor start-up and stabilization */
  delay(STABILIZATION_MS);
  /* I2C */
  /* Initialize I2C and use default pins defined for the board */
  reInitI2C();
 
  Serial.println("Initialization Sunrise I2C single complete\n");

  /* Read the sensor's configs */
  Serial.println("Sensor Measurement Configurations:");
  read_sensor_config(SUNRISE_ADDR);
  Serial.println();

#if (DISABLE_ABC ==1)  
  setABC(SUNRISE_ADDR, false);
#endif


  /* Change to single measurement mode if continuous */
  change_measurement_mode(SUNRISE_ADDR);
 
  digitalWrite(SUNRISE_EN, LOW); 
   
  /* Initial measurement */
  Serial.println("Saving Sensor State");
  save_state(SUNRISE_ADDR);

  delay(readPeriodMs);
}

/**
 * @brief  The main function loop. Reads the sensor's current
 *         CO2 value and error status and prints them to the 
 *         Serial Monitor.
 * 
 * @retval true if successful, false if failed
 */
bool readout_SRI2Csingle(void) {
  static int pin_value = HIGH;
  static unsigned long last_abc_stamp = 0;
  bool readSR_status;  
//   /* When an hour has passed, increase ABC Time */  
//   if(3600000 < (unsigned long)((long)millis() - (long)last_abc_stamp)) {
//     /* Use ABC time stored in the sensor state */
//     uint16_t abc_time = ((int16_t)(int8_t) state[0] << 8) | (uint16_t)state[1];
//     abc_time = abc_time + 1;
//     state[0] = abc_time >> 8;
//     state[1] = abc_time & 0x00FF;
    
//     last_abc_stamp = millis();
//     Serial.println("ABC time incremented.");
//   }
  /* Read measurements */
  readSR_status = read_sensor_measurements(SUNRISE_ADDR);

//   /* Delay between readings */
//   Serial.println("\nWaiting...\n");
//   delay(readPeriodMs);


 /* Indicate working state */
 //230823 close to use the LED for write data to SD card indication only.
  // digitalWrite(LED_BUILTIN, pin_value);
  // pin_value  = ((pin_value == HIGH) ? LOW : HIGH);
return readSR_status;
}