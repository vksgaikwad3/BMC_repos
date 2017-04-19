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
#define timeout 5000
#define polling 200
#define retry_count 20

#define TxEnablePin 2 

#define LED 13
const uint8_t ChipSelect = 53 ;   //SD Card ChipSelect pin. Dont Change


#define TOTAL_NO_OF_REGISTERS 17        // prev.cnt = 33 Control Panel Resisters = 17 , Energy Meter Resisters = 16

enum
{
  PACKET1,
 
  TOTAL_NO_OF_PACKETS // leave this last entry
};

Packet packets[TOTAL_NO_OF_PACKETS];

int regs[TOTAL_NO_OF_REGISTERS];     // All the Data from Modbus Resisters gets stored here in this buffer.
int cnt = 0;
//char* apn;

void setup()
{
  

  pinMode(ChipSelect,OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600);     // ModBus Port
  Serial3.begin(9600);

  //char apn[] = "";
 
  //apn = myGateway.Check_Operator();
//  Serial.print("APN is :");Serial.println(apn);
//  delay(1000);

  
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
  
  
  
  modbus_configure(&Serial2, baud, SERIAL_8N1, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);
  delay(100);
  //pinMode(LED, OUTPUT);

  // Setup GSM Module 
  Serial1.begin(9600);             // To connect SIM900A and send AT Commands
  myGateway.power_on();           // POWER ON GSM Module for communication 
  
/************** SD Card Init/Startup Code ***********************/
 //isSDCardCheck("chiller.csv");     //provide a File Name to Store log of ModBus Devices
 pinMode(A14,OUTPUT);
 digitalWrite(A14,LOW);
 myGateway.SendMessage();   //Send SMS on Device ON
 
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

  
 Serial.print("Connection ID:");Serial.println(packets[PACKET1].id);
 Serial.print("Connection Status Dev 1:");Serial.println(packets[PACKET1].connection);
 Serial.print("Sucessful Request Dev 1 :");Serial.println(packets[PACKET1].successful_requests);
 Serial.print("Failed Request :");Serial.println(packets[PACKET1].failed_requests);
 Serial.print("Connection Retries :");Serial.println(packets[PACKET1].retries);
 Serial.print("Exception Errors :");Serial.println(packets[PACKET1].exception_errors);
 Serial.print("Connection Request :");Serial.println(packets[PACKET1].requests);

 
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
 
  
 if( cnt == 0)
 {
  myGateway.verifyData(packets[PACKET1].requests,packets[PACKET1].failed_requests,packets[PACKET1].successful_requests,battery_Temp,milk_Temp);
  cnt++;
 }  
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

/*   Sending Data to Cloud *************/
//Milk Chiller Channel

myGateway.sendATcommand("AT+CSQ", "OK", 1000);
myGateway.updateThinkSpeak(channel_apiKey[0],1,2,3,4,battery_Temp,milk_Temp,auxillary_Temp,battery_Volt,ac_Volt,compressor_Current,pump_Current,compressor_run_Hour);    //update Milk Chiller Channel 1 - 4 fields
delay(3000);

// Milk Chiller Relay Channel

myGateway.sendATcommand("AT+CSQ", "OK", 1000);
myGateway.updateThinkSpeak(channel_apiKey[1],1,2,3,4,charg_pump_Relay,condensor_Relay,compressor_Relay,inverter_Relay,agitator_Relay,tank_Relay,shiva_Relay,discharge_pump_Relay);    //update Milk Chiller Relays Channel field 1 -4
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


