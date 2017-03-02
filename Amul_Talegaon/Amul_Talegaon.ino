/* Author : Vikas Gaikwad
 * Date : 10 Nov. 2016
 * 
 *  Description : Read all the Data from MODBUS Slaves.
   Hardware Connections : Arduino MEGA SERIAL2(Hardware Serial) 
        
                          RX2 -----> R0 (MAX485 Chip Adapter)
                          TX2 -----> DI (MAX485 Chip Adapter)
                          D2  -----> DE/RE (MAX485 Chip Adapter)
   MODBUS  Device Settings : 

   SLAVE 1 : Milk Chiller Panel 
   Slave ID : 1, Baud Rate : 9600, Parity : NONE, Stop bit : 1    

   SLAVE 2 : Energy Meter
   Slave ID : 2, Baud Rate: 9600, Parity :None, Stop bit : 1
 * 
 * MODBUS Electrical Connections: 
 * 1> MILK Chiller Serial(RS 232) --> 485 Converter Pin 1 --> A of MAX485 Chip Adapter.
 *                                    485 Converter Pin 2 --> B of MAX485 Chip Adapter.        
 * 2> Energy Meter (RS485 Signal) --> A (Green Wire) -->  B of MAX485 Chip Adapter. 
 *                                    B (Yellow Wire) --> A of MAX485 Chip Adapter.
 *   
 *   
 */


#include <SimpleModbusMaster.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "sim900.h"     
#include "serverConfg.h"  
#include "sdcardConfig.h"        
#include <avr/wdt.h>


String getlogTime();


sim900_GPRS myGateway;    // Gateway object
RTC_DS3231 rtc;           // RTC instance



#define baud 9600
#define timeout 1000
#define polling 200
#define retry_count 20

#define TxEnablePin 2 

#define LED 13
const uint8_t ChipSelect = 53 ;   //SD Card ChipSelect pin. Dont Change


#define TOTAL_NO_OF_REGISTERS 33        // prev.cnt = 33 Control Panel Resisters = 17 , Energy Meter Resisters = 16

enum
{
  PACKET1,
  
  PACKET2,    //3 Phase Energy Meter
  PACKET3,
  PACKET4,
  PACKET5,
  PACKET6,
  PACKET7,
  PACKET8,
  PACKET9,
  TOTAL_NO_OF_PACKETS // leave this last entry
};

Packet packets[TOTAL_NO_OF_PACKETS];

int regs[TOTAL_NO_OF_REGISTERS];     // All the Data from Modbus Resisters gets stored here in this buffer.

void setup()
{
  

  pinMode(ChipSelect,OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600);     // ModBus Port
  Serial3.begin(9600);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
   modbus_construct(&packets[PACKET1], 1, READ_HOLDING_REGISTERS, 4, 17, 0);  // Control Panel Resisters [40005 - 40021]
  // modbus_construct(&packets[PACKET10], 1, READ_HOLDING_REGISTERS, 21,11,17 );  // Control Panel Resisters [400022 - 40032] Faults 
  // ENERGY METER PACKET
  
  // Slave 2 :  3 Phase Energy Meter
   
  
    modbus_construct(&packets[PACKET2], 2,READ_INPUT_REGISTERS , 0, 2, 17);    //30001-2 Line Voltage reg[17-18]       HB :17278 LB :44628
    modbus_construct(&packets[PACKET3], 2,READ_INPUT_REGISTERS , 6, 2, 19);    // 30007-8 Line Current reg[19-20]      
    modbus_construct(&packets[PACKET4], 2,READ_INPUT_REGISTERS , 62,2, 21);    // 30063-64 reg[21-22]                 Power Factor 
    
    modbus_construct(&packets[PACKET5], 2,READ_INPUT_REGISTERS , 72,2, 23);    // 30073-74 Energy Consumption  reg[23-24](kWh)   Reading 16384(73)[IMP] History Value
 
    modbus_construct(&packets[PACKET6], 2,READ_INPUT_REGISTERS , 50,2, 25);    // 30089-90 Sys Power(kWh) reg[25-26]  //86 --> 50 Live Value
    modbus_construct(&packets[PACKET7], 2,READ_INPUT_REGISTERS , 226,2,27);    // 30227-28 Device Run Hr reg[27-28]     
    modbus_construct(&packets[PACKET8], 2,READ_INPUT_REGISTERS , 228,2,29);    // 30227-28 Power Available Time reg[29-30]    
    modbus_construct(&packets[PACKET9], 2,READ_INPUT_REGISTERS , 54, 2,31);    // 30055-56 VAvg reg[31-32]   VA Avg

  
  modbus_configure(&Serial2, baud, SERIAL_8N1, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);
  delay(100);
  //pinMode(LED, OUTPUT);

  // Setup GSM Module 
  Serial1.begin(9600);             // To connect SIM900A and send AT Commands
  myGateway.power_on();           // POWER ON GSM Module for communication 
  
/************** SD Card Init/Startup Code ***********************/
 isSDCardCheck("chiller.csv");     //provide a File Name to Store log of ModBus Devices
 pinMode(A14,OUTPUT);
 digitalWrite(A14,LOW);
 myGateway.SendMessage();   //Send SMS on Device ON
 // myGateway.GPRSFailedSMS("+919952233029","Device Runing in Damodar Hilli");
 //wdt_enable(WDTO_8S);

}

void loop()
{     
  DateTime now = rtc.now();
   //DateTime now = rtc.now();
  for(int i=0;i<600;i++)
  {
    modbus_update();
    delay(10); //Serial.println(i);
  }

  digitalWrite(A14,HIGH);
 
  float battery_Temp = regs[0]/10.0;
  float milk_Temp = regs[1]/10.0;
  float auxillary_Temp = regs[2]/10.0;
  uint8_t battery_Volt = regs[3];
  uint8_t ac_Volt = regs[4];
  float compressor_Current = regs[5]/10.0;
  float pump_Current = regs[6]/10.0;
  boolean charg_pump_Relay = regs[7];
  boolean condensor_Relay = regs[8];
  boolean compressor_Relay = regs[9];
  boolean inverter_Relay = regs[10];
  boolean agitator_Relay = regs[11];
  boolean tank_Relay = regs[12];
  boolean shiva_Relay = regs[13];
  boolean discharge_pump_Relay = regs[14];
  uint32_t compressor_run_Hour = regs[16];
    
  Serial.print("Battery Temp.: ");Serial.println(battery_Temp,DEC);
  Serial.print("Milk Temp.: ");Serial.println(milk_Temp,DEC);
  Serial.print("Auxillary Temp.: ");Serial.println(auxillary_Temp,DEC);
  Serial.print("Battery Volt.: ");Serial.println(battery_Volt,DEC);
  Serial.print("AC Voltage.: ");Serial.println(ac_Volt,DEC);
  Serial.print("Copmressor Curr.: ");Serial.println(compressor_Current,DEC);
  Serial.print("Pump Current.: ");Serial.println(pump_Current,DEC);
  Serial.print("Ch Pump Relay.: ");Serial.println(charg_pump_Relay,DEC);
  Serial.print("Condensor Relay.: ");Serial.println(condensor_Relay,DEC);
  Serial.print("Compressor Relay.: ");Serial.println(compressor_Relay,DEC);
  Serial.print("Inverter Relay.: ");Serial.println(inverter_Relay,DEC);
  Serial.print("Agitator Relay.: ");Serial.println(agitator_Relay,DEC);
  Serial.print("Tank Relay.: ");Serial.println(tank_Relay,DEC);
  Serial.print("Shiva Relay.: ");Serial.println(shiva_Relay,DEC);
  Serial.print("Discharge Pump Relay.: ");Serial.println(discharge_pump_Relay,DEC);
  Serial.print("Compressor Run Hr.: ");Serial.println(compressor_run_Hour,DEC);
 // delay(20);

    // Energy Meter Readings 
 // Serial.println("************ Energy Meter Readings ************* ");
 
  delay(50);
  

  // Energy Meter 3 Geyser 
 // Serial.println("***********************************************************************");
  
  float em2_lineVolts = myGateway.hextofloat(regs[17],regs[18]);
  float em2_lineCurrent = myGateway.hextofloat(regs[19],regs[20]);
  float em2_powerFactor = myGateway.hextofloat(regs[21],regs[22]);
  
  float em2_power = myGateway.hextofloat(regs[23],regs[24]);          // kWh Reading
  float em2_energyConsump = myGateway.hextofloat(regs[25],regs[26]); //dont want
  float em2_deviceRunHr = myGateway.hextofloat(regs[27],regs[28]);
  float em2_powerAvailableTime = myGateway.hextofloat(regs[29],regs[30]);

  float em2_VA_Avg = myGateway.hextofloat(regs[31],regs[32]);

  Serial.print("EM2_line Volts:");Serial.println(em2_lineVolts);
  Serial.print("EM2_line Current:");Serial.println(em2_lineCurrent);
  Serial.print("EM2_powerFactor:");Serial.println(em2_powerFactor);

  //Serial.print("EM3_Kwh Reading:");Serial.println(em3_energyConsump);
  Serial.print("EM2_ kWh Reading:");Serial.println(em2_power);
  
  Serial.print("EM2_Device Run Hr:");Serial.println(em2_deviceRunHr);
  Serial.print("EM2_powerAvailableTime:");Serial.println(em2_powerAvailableTime);
  Serial.print("EM2_KVAh Reading:");Serial.println(em2_VA_Avg);


 // Serial.println("***********************************************************************");
 // ************ Sending Data to SD card ***********************
//
 writetoSDCard ("chiller.csv",FILE_WRITE ,getlogTime(), packets[PACKET1].id, battery_Temp, milk_Temp, auxillary_Temp, battery_Volt, ac_Volt, compressor_Current, pump_Current,
                 charg_pump_Relay, condensor_Relay, compressor_Relay, inverter_Relay, agitator_Relay,tank_Relay, shiva_Relay, discharge_pump_Relay,compressor_run_Hour,
                 packets[PACKET2].id,em2_lineVolts, em2_lineCurrent,em2_powerFactor,em2_power, em2_deviceRunHr,em2_powerAvailableTime,em2_VA_Avg);


/*   Sending Data to Cloud *************/
//Milk Chiller Channel

myGateway.sendATcommand("AT+CSQ", "OK", 1000);
myGateway.updateThinkSpeak(channel_apiKey[0],1,2,3,4,battery_Temp,milk_Temp,auxillary_Temp,battery_Volt,ac_Volt,compressor_Current,pump_Current,compressor_run_Hour);    //update Milk Chiller Channel 1 - 4 fields
delay(3000);

// Milk Chiller Relay Channel

//myGateway.sendATcommand("AT+CSQ", "OK", 1000);
myGateway.updateThinkSpeak(channel_apiKey[1],1,2,3,4,charg_pump_Relay,condensor_Relay,compressor_Relay,inverter_Relay,agitator_Relay,tank_Relay,shiva_Relay,discharge_pump_Relay);    //update Milk Chiller Relays Channel field 1 -4
delay(2000);

// Geyser Energy Meter Upload

myGateway.updateThinkSpeak(channel_apiKey[2],1,2,3,4,em2_lineVolts,em2_lineCurrent,em2_powerFactor,em2_power,0,em2_deviceRunHr,em2_powerAvailableTime,em2_VA_Avg);                  //update Energy meter 1 - 4 fields
delay(2000);




//Serial.println("**** done ****");
digitalWrite(A14,LOW);
  delay(200);
}
String getlogTime()
{   
   DateTime now = rtc.now();
   String logTime ; 
      
   logTime += now.day();
   logTime += "/";
   logTime += now.month();
   logTime += "/";
   logTime += now.year();
   logTime += ",";
   logTime += now.hour() ;//- 5;
   logTime += ":";
   logTime += now.minute();// + 13 ;
   logTime += ":";
   logTime += now.second();
   
   //Serial.print("Time:");Serial.println(logTime);
   //delay(2000);
   return logTime;
}


