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
 * 1> MILK Chiller Ser  ial(RS 232) --> 485 Converter Pin 1 --> A of MAX485 Chip Adapter.
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
#define retry_count 40

#define TxEnablePin 2 

#define LED 13
const uint8_t ChipSelect = 53 ;   //SD Card ChipSelect pin. Dont Change


#define TOTAL_NO_OF_REGISTERS 75        // prev.cnt = 33 Control Panel Resisters = 17 , Energy Meter Resisters = 16

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
  PACKET10,    
  PACKET11,
  PACKET12,
  PACKET13,
  PACKET14,
  PACKET15,
  PACKET16,
  PACKET17,
  PACKET18,  
  PACKET19,
  PACKET20,
  PACKET21,
  PACKET22,
  PACKET23, 
  
  PACKET24,   // Geyser Energy Meter
  PACKET25,
  PACKET26,      
  PACKET27,
  PACKET28,
  PACKET29,
  PACKET30,

  PACKET31,
  PACKET32,
  PACKET33,
  
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
    
    modbus_construct(&packets[PACKET2], 2,READ_INPUT_REGISTERS , 0, 2, 17);    //30001-2 Line1 Voltage reg[17-18]       Volt L1 
    modbus_construct(&packets[PACKET3], 2,READ_INPUT_REGISTERS , 2, 2, 19);    //30003-4 Line2 Voltage reg[19-20]       Volt L2 
    modbus_construct(&packets[PACKET4], 2,READ_INPUT_REGISTERS , 4, 2, 21);    //30005-6 Line3 Voltage reg[21-22]       Volt L3
   
    modbus_construct(&packets[PACKET5], 2,READ_INPUT_REGISTERS , 6, 2, 23);    // 30007-8 Line1 Current reg[23-24]      L1_Current
    modbus_construct(&packets[PACKET6], 2,READ_INPUT_REGISTERS , 8, 2, 25);    // 30009-10 Line2 Current reg[25-26]     L2_Current
    modbus_construct(&packets[PACKET7], 2,READ_INPUT_REGISTERS , 10, 2, 27);    // 30011-12 Line3 Current reg[27-28]    L3_Current

    // W1 ,W2,W3 & Wavg
    modbus_construct(&packets[PACKET8], 2,READ_INPUT_REGISTERS , 12, 2, 29);    // 30013-14 W1_Watt reg[29-30]    W1
    modbus_construct(&packets[PACKET9], 2,READ_INPUT_REGISTERS , 14, 2, 31);    // 30015-16 W2_Watt reg[31-32]    W2
    modbus_construct(&packets[PACKET10], 2,READ_INPUT_REGISTERS , 16, 2, 33);    // 30017-18 W3_Watt reg[33-34]   W3
    modbus_construct(&packets[PACKET11], 2,READ_INPUT_REGISTERS , 50, 2, 35);    // 30051-52 Watt_Avg reg[35-36]  Watt Avg

    // VA1,VA2,VA3 & VAvg
    modbus_construct(&packets[PACKET12], 2,READ_INPUT_REGISTERS , 18, 2, 37);    // 30019-20 VA1 reg[37-38]    VA_1
    modbus_construct(&packets[PACKET13], 2,READ_INPUT_REGISTERS , 20, 2, 39);    // 30021-22 VA2 reg[39-40]    VA_2
    modbus_construct(&packets[PACKET14], 2,READ_INPUT_REGISTERS , 22, 2, 41);    // 30023-24 VA3 reg[41-42]    VA_3
    modbus_construct(&packets[PACKET15], 2,READ_INPUT_REGISTERS , 80, 2, 43);    // 30055-56 VAvg reg[43-44]   VA Avg

   // PF1,PF2,PF3 & PF Avg
   
    modbus_construct(&packets[PACKET16], 2,READ_INPUT_REGISTERS , 30, 2, 45);    // 30031-32 PF1 reg[45-46]    PF1
    modbus_construct(&packets[PACKET17], 2,READ_INPUT_REGISTERS , 32, 2, 47);    // 30033-34 PF2 reg[47-48]    PF2
    modbus_construct(&packets[PACKET18], 2,READ_INPUT_REGISTERS , 34, 2, 49);    // 30035-36 PF1 reg[49-50]    PF3
    modbus_construct(&packets[PACKET19], 2,READ_INPUT_REGISTERS , 62, 2, 51);    // 30063-64 PF1 reg[51-52]    PF Avg
    
    modbus_construct(&packets[PACKET20], 2,READ_INPUT_REGISTERS , 72,2, 53);    // 30073-74 Power Consumption  reg[53-54]   Reading 16384(73)[IMP] History Value 
                                                                                // (make 74 for Ram Nagar)        
    modbus_construct(&packets[PACKET21], 2,READ_INPUT_REGISTERS , 84,2, 55);    // 30089-90 Sys Power(W) reg[55-56]  // 84 Live Value of Watts
    modbus_construct(&packets[PACKET22], 2,READ_INPUT_REGISTERS , 226,2,57);    // 30227-28 Device Run Hr reg[57-58]     
    modbus_construct(&packets[PACKET23], 2,READ_INPUT_REGISTERS , 228,2,59);    // 30227-28 Power Available Time reg[59-60]    

// Slave 3 Energy Meter [ Gyser]
  
    modbus_construct(&packets[PACKET24], 3,READ_INPUT_REGISTERS , 0, 2, 61);    // 30001-2 Line Voltage reg[61-62]       HB :17278 LB :44628
    modbus_construct(&packets[PACKET25], 3,READ_INPUT_REGISTERS , 6, 2, 63);    // 30007-8 Line Current reg[63-64]      
    modbus_construct(&packets[PACKET26], 3,READ_INPUT_REGISTERS , 62,2, 65);    // 30063-64 reg[65-66]                 Power Factor 
    
    modbus_construct(&packets[PACKET27], 3,READ_INPUT_REGISTERS , 72,2, 67);    // 30073-74 Energy Consumption  reg[67-68](kWh)   Reading 16384(73)[IMP] History Value
 
    modbus_construct(&packets[PACKET28], 3,READ_INPUT_REGISTERS , 50,2, 69);    // 30089-90 Sys Power(kWh) reg[69-70]  //86 --> 50 Live Value
    modbus_construct(&packets[PACKET29], 3,READ_INPUT_REGISTERS , 226,2,71);    // 30227-28 Device Run Hr reg[71-72]     
    modbus_construct(&packets[PACKET30], 3,READ_INPUT_REGISTERS , 228,2,73);    // 30227-28 Power Available Time reg[73-74]    
    modbus_construct(&packets[PACKET31], 3,READ_INPUT_REGISTERS , 54, 2,75);    // 30055-56 VAvg reg[75-76]   VA Avg

  
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
  for(int i=0;i<700;i++)
  {
    modbus_update();
    delay(40); //Serial.println(i);
  }
//  if(now.second() == 30)
//  { 
//    Serial.println("RESET");
//    asm volatile ("  jmp 0"); 
//    //Reset the code
//  }

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
  //Serial.print("Discharge Pump Relay.: ");Serial.println(regs[14],DEC);
  Serial.print("Discharge Pump Relay.: ");Serial.println(discharge_pump_Relay,DEC);
  Serial.print("Compressor Run Hr.: ");Serial.println(compressor_run_Hour,DEC);
  delay(20);
 
    // Energy Meter Readings 
 // Serial.println("************ Energy Meter Readings ************* ");
 
  delay(50);
  float em2_line1_Volts = myGateway.hextofloat(regs[17],regs[18]);
  float em2_line2_Volts = myGateway.hextofloat(regs[19],regs[20]);
  float em2_line3_Volts = myGateway.hextofloat(regs[21],regs[22]);


  float em2_line1_Current = myGateway.hextofloat(regs[23],regs[24]);
  float em2_line2_Current = myGateway.hextofloat(regs[25],regs[26]);
  float em2_line3_Current = myGateway.hextofloat(regs[27],regs[28]);

  float em2_W1_Watt = myGateway.hextofloat(regs[29],regs[30]);
  float em2_W2_Watt = myGateway.hextofloat(regs[31],regs[32]);
  float em2_W3_Watt = myGateway.hextofloat(regs[33],regs[34]);
  float em2_Watt_Avg = myGateway.hextofloat(regs[35],regs[36]);

  float em2_VA1 = myGateway.hextofloat(regs[37],regs[38]);
  float em2_VA2 = myGateway.hextofloat(regs[39],regs[40]);
  float em2_VA3 = myGateway.hextofloat(regs[41],regs[42]);
  
  float KVAh_Reading = myGateway.hextofloat(regs[43],regs[44]);

  float em2_PF1 = myGateway.hextofloat(regs[45],regs[46]);
  float em2_PF2 = myGateway.hextofloat(regs[47],regs[48]);
  float em2_PF3 = myGateway.hextofloat(regs[49],regs[50]);
  float em2_PF_Avg = myGateway.hextofloat(regs[51],regs[52]);
  
  float kWh_Reading = myGateway.hextofloat(regs[53],regs[54]);
  float em2_energyConsump = myGateway.hextofloat(regs[55],regs[56]);
  float em2_deviceRunHr = myGateway.hextofloat(regs[57],regs[58]);
  float em2_powerAvailableTime = myGateway.hextofloat(regs[59],regs[60]);
 
  //float energy_meter[] = { lineVolts,lineCurrent,deviceVolts,deviceCurrent,power,powerConsump,deviceRunHr,powerAvailable };

  Serial.print("EM2_line1 Volts:");Serial.println(em2_line1_Volts);
  Serial.print("EM2_line2 Volts:");Serial.println(em2_line2_Volts);
  Serial.print("EM2_line3 Volts:");Serial.println(em2_line3_Volts);


  Serial.print("EM2_line1 Current:");Serial.println(em2_line1_Current);
  Serial.print("EM2_line2 Current:");Serial.println(em2_line2_Current);
  Serial.print("EM2_line3 Current:");Serial.println(em2_line3_Current);

  Serial.print("EM2_Watt_1:");Serial.println(em2_W1_Watt);
  Serial.print("EM2_Watt_2:");Serial.println(em2_W2_Watt);
  Serial.print("EM2_Watt_3:");Serial.println(em2_W3_Watt);

 
  Serial.print("EM2_VA1:");Serial.println(em2_VA1);
  Serial.print("EM2_VA2:");Serial.println(em2_VA2);
  Serial.print("EM2_VA3:");Serial.println(em2_VA3);
 
  Serial.print("EM2_KVAh_Reading:");Serial.println(KVAh_Reading);

  Serial.print("EM2_PF1:");Serial.println(em2_PF1);
  Serial.print("EM2_PF2:");Serial.println(em2_PF2);
  Serial.print("EM2_PF3:");Serial.println(em2_PF3);
  
  Serial.print("EM2_PF_Avg:");Serial.println(em2_PF_Avg);

  Serial.print("EM2_ kWh Reading:");Serial.println(kWh_Reading);           //History
  Serial.print("EM2_powerAvailableTime:");Serial.println(em2_powerAvailableTime);

// Energy Meter 3 Geyser 
 // Serial.println("***********************************************************************");
  
  float em3_lineVolts = myGateway.hextofloat(regs[61],regs[62]);
  float em3_lineCurrent = myGateway.hextofloat(regs[63],regs[64]);
  float em3_powerFactor = myGateway.hextofloat(regs[65],regs[66]);
  
  float em3_power = myGateway.hextofloat(regs[67],regs[68]);
  float em3_energyConsump = myGateway.hextofloat(regs[69],regs[70]); //dont want
  float em3_deviceRunHr = myGateway.hextofloat(regs[71],regs[72]);
  float em3_powerAvailableTime = myGateway.hextofloat(regs[73],regs[74]);

  float em3_VA_Avg = myGateway.hextofloat(regs[75],regs[76]);

  Serial.print("EM3_line Volts:");Serial.println(em3_lineVolts);
  Serial.print("EM3_line Current:");Serial.println(em3_lineCurrent);
  Serial.print("EM3_powerFactor:");Serial.println(em3_powerFactor);

  //Serial.print("EM3_Kwh Reading:");Serial.println(em3_energyConsump);
  Serial.print("EM3_ kWh Reading:");Serial.println(em3_power);
  
  Serial.print("EM3_Device Run Hr:");Serial.println(em3_deviceRunHr);
  Serial.print("EM3_powerAvailableTime:");Serial.println(em3_powerAvailableTime);
  Serial.print("EM3_KVAh Reading:");Serial.println(em3_VA_Avg);

// Energy Meter 4 
 // Serial.println("***********************************************************************");
 // ************ Sending Data to SD card ***********************
//
 writetoSDCard ("chiller.csv",FILE_WRITE ,getlogTime(), packets[PACKET1].id, battery_Temp, milk_Temp, auxillary_Temp, battery_Volt, ac_Volt, compressor_Current, pump_Current,
                 charg_pump_Relay, condensor_Relay, compressor_Relay, inverter_Relay, agitator_Relay,tank_Relay, shiva_Relay, discharge_pump_Relay,compressor_run_Hour,
                 packets[PACKET2].id,em2_line1_Volts,em2_line2_Volts,em2_line3_Volts, em2_line1_Current,em2_line2_Current,em2_line3_Current,
                 em2_W1_Watt,em2_W2_Watt,em2_W3_Watt,em2_VA1,em2_VA2,em2_VA3,KVAh_Reading,em2_PF1,em2_PF2,em2_PF3,em2_PF_Avg,kWh_Reading,em2_powerAvailableTime,
                 packets[PACKET24].id,em3_lineVolts, em3_lineCurrent,em3_powerFactor,em3_power, em3_deviceRunHr,em3_powerAvailableTime,em3_VA_Avg);


/*   Sending Data to Cloud *************/
//Milk Chiller Channel
//
myGateway.updateThinkSpeak(channel_apiKey[0],1,2,3,4,battery_Temp,milk_Temp,auxillary_Temp,battery_Volt,ac_Volt,compressor_Current,pump_Current,compressor_run_Hour);    //update Milk Chiller Channel 1 - 4 fields
delay(2000);

// Milk Chiller Relay Channel

myGateway.sendATcommand("AT+CSQ", "OK", 1000);
myGateway.updateThinkSpeak(channel_apiKey[1],1,2,3,4,charg_pump_Relay,condensor_Relay,compressor_Relay,inverter_Relay,agitator_Relay,tank_Relay,shiva_Relay,discharge_pump_Relay);    //update Milk Chiller Relays Channel field 1 -4
delay(2000);

// Geyser Energy Meter Upload

myGateway.updateThinkSpeak(channel_apiKey[2],1,2,3,4,em3_lineVolts,em3_lineCurrent,em3_powerFactor,em3_power,0,em3_deviceRunHr,em3_powerAvailableTime,em3_VA_Avg);                  //update Energy meter 1 - 4 fields
delay(2000);

/***************************** 3 Ph Meter Readings Upload ****************************************/

// 3 Phase meter Parameters Uploading

myGateway.updateThinkSpeak(channel_apiKey[3],1,2,3,4,em2_line1_Volts,em2_line2_Volts,em2_line3_Volts, em2_line1_Current,em2_line2_Current,em2_line3_Current,kWh_Reading,em2_powerAvailableTime);
delay(2000);
//
// 3 Ph meter Parameter ID 2
myGateway.updateThinkSpeak(channel_apiKey[4],1,2,3,4,em2_W1_Watt,em2_W2_Watt,em2_W3_Watt,em2_VA1,em2_VA2,em2_VA3,KVAh_Reading,0); // Field 8 = NC
delay(2000);

// 3 Ph meter Parameter ID 3 
myGateway.updateThinkSpeak(channel_apiKey[5],1,2,3,4,em2_PF1,em2_PF2,em2_PF3,em2_PF_Avg,0,0,0,0); // Field 5-8 = 0 NC
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
  // return (myGateway.sendATcommand("AT+CCLK=?", "OK", 1000));
}


