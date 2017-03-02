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

// 565 mb (51), (...852-dhaili)-1381 mb 11may DATA USE 121 
// 3PH Slave ID 2
// Gyser Slave ID  3

#include <SimpleModbusMaster.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "sim900.h"     
#include "serverConfg.h"  
#include "sdcardConfig.h"        

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


#define TOTAL_NO_OF_REGISTERS 72        // Control Panel Resisters = 17 , Energy Meter Resisters = 16

enum
{
  PACKET1,
  PACKET2,
  PACKET3,
  PACKET4,
  PACKET5,
  PACKET6,
  PACKET7,
  PACKET8,
  PACKET9,
  PACKET10,             //Alerts
  PACKET11,             //Alerts
  PACKET12,             //Alerts
  PACKET13,             //Alerts
  PACKET14,             //Alerts
  PACKET15,             //Alerts
  PACKET16,             //Alerts
  PACKET17,             //Alerts
  PACKET18,             //Alerts
  PACKET19,             //Alerts
  PACKET20,             //Alerts
  PACKET21,             //Alerts
  PACKET22,
  PACKET23,
  PACKET24,
  PACKET25,
  PACKET26,
  PACKET27,
  PACKET28,
  PACKET29,
  //PACKET30,
  TOTAL_NO_OF_PACKETS // leave this last entry
};

Packet packets[TOTAL_NO_OF_PACKETS];

int regs[TOTAL_NO_OF_REGISTERS];     // All the Data from Modbus Resisters gets stored here in this buffer.

void setup()
{
  pinMode(ChipSelect,OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
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
  
  // modbus_construct(&packets[PACKET1], 1, READ_HOLDING_REGISTERS, 4, 17, 0);  // Control Panel Resisters [40005 - 40021]
   //modbus_construct(&packets[PACKET1], 1, READ_HOLDING_REGISTERS, 21,11,0 );  // Control Panel Resisters [400022 - 40032] Faults 
   // ENERGY METER PACKET
      modbus_construct(&packets[PACKET1], 2,READ_HOLDING_REGISTERS,3000,4,0); //Reading from 3000 - 3004 reg[0-3]
     
      modbus_construct(&packets[PACKET2], 2,READ_HOLDING_REGISTERS,3004,6, 4); //Reading from 3004 - 3009   reg[4-9]
      modbus_construct(&packets[PACKET3], 2,READ_HOLDING_REGISTERS,3010,2,10);  //  3010 - 3011    reg[10-11]  Avg Power Factor
      modbus_construct(&packets[PACKET4], 2,READ_HOLDING_REGISTERS,3012,2,12);  //  3012 - 3013    reg[12-13]
      modbus_construct(&packets[PACKET5], 2,READ_HOLDING_REGISTERS,3014,6,14);  //  3014 - 3018    reg[14-19]
      modbus_construct(&packets[PACKET6], 2,READ_HOLDING_REGISTERS,3020,1,20);  //  3020          reg[20]      // CT PRimary 
      
      modbus_construct(&packets[PACKET7], 2,READ_HOLDING_REGISTERS,3022,2,21);  //  3022          reg[21-22]  Maximum Demand EB
      modbus_construct(&packets[PACKET8], 2,READ_HOLDING_REGISTERS,3027,2,23);  //  3027          reg[23-24]  Power On Hr EB 
      modbus_construct(&packets[PACKET9], 2,READ_HOLDING_REGISTERS,3029,2,25);  //  3029          reg[25-26]  Load On Hr EB

      modbus_construct(&packets[PACKET10], 2,READ_HOLDING_REGISTERS,3032,2,27);  //  3032          reg[27-28] Maximum Demand DB

      modbus_construct(&packets[PACKET11], 2,READ_HOLDING_REGISTERS,3037,2,29);  //  3037          reg[29-30]Power On Hr DG

      modbus_construct(&packets[PACKET12], 2,READ_HOLDING_REGISTERS,3039,2,31);  //  3039          reg[31-32]Load On Hr

      modbus_construct(&packets[PACKET13], 2,READ_HOLDING_REGISTERS,3041,6,33);  //  3041 - 3046   reg[33-38] RYB Total Harmonic Distortions

      modbus_construct(&packets[PACKET14], 2,READ_HOLDING_REGISTERS,3048,2,39);  //  3048-49          reg[39-40] R Phase KVA

      modbus_construct(&packets[PACKET15], 2,READ_HOLDING_REGISTERS,3050,2,41);  //  3050-51          reg[41-42] Y Phase KVA

      modbus_construct(&packets[PACKET16], 2,READ_HOLDING_REGISTERS,3052,2,43);  //  3052-53          reg[43-44] B Phase KVA

      modbus_construct(&packets[PACKET17], 2,READ_HOLDING_REGISTERS,3054,2,45);  //  3054-55          reg[45-46] RYB KW
      
      modbus_construct(&packets[PACKET18], 2,READ_HOLDING_REGISTERS,3056,2,47);  //  3056-57          reg[47-48] RY Line - Line Voltage
      
      modbus_construct(&packets[PACKET19], 2,READ_HOLDING_REGISTERS,3058,2,49);  //  3058-59          reg[49-50] YB Line - Line Voltage
      
      modbus_construct(&packets[PACKET20], 2,READ_HOLDING_REGISTERS,3060,2,51);  //  3060-61          reg[51-52] BR Line - Line Voltage
      
      modbus_construct(&packets[PACKET21], 2,READ_HOLDING_REGISTERS,200,2,53);   //  3200-201          reg[53-54] Cum Energy for KVAh
      modbus_construct(&packets[PACKET22], 2,READ_HOLDING_REGISTERS,202,2,55);   //  3202-203          reg[55-56] Cum Energy for KWh
      modbus_construct(&packets[PACKET23], 2,READ_HOLDING_REGISTERS,204,2,57);   //  3204-205          reg[57-58] Cum Energy for- KVArh lag
      modbus_construct(&packets[PACKET24], 2,READ_HOLDING_REGISTERS,206,2,59);   //  3206-207          reg[59-60] Cum Energy for  KVArh lead
      modbus_construct(&packets[PACKET25], 2,READ_HOLDING_REGISTERS,208,2,61);   //  3208-209          reg[61-62] Cum Energy Rev KVAh
      modbus_construct(&packets[PACKET26], 2,READ_HOLDING_REGISTERS,210,2,63);   //  3210-211          reg[63-64] Cum Energy Rev KWh
      modbus_construct(&packets[PACKET27], 2,READ_HOLDING_REGISTERS,212,2,65);   //  3212-213          reg[65-66] Cum Energy Rev for - KVArh for lag
      modbus_construct(&packets[PACKET28], 2,READ_HOLDING_REGISTERS,214,2,67);   //  3214-215          reg[67-68] Cum Energy Rev for - KVArh for lead

      modbus_construct(&packets[PACKET29], 2,READ_HOLDING_REGISTERS,248,2,69);   //  3248-249          reg[69-70] Average PF

      
      
      
      
      
      





      
 
     
     
      
      
      
      
//    modbus_construct(&packets[PACKET3], 3,READ_INPUT_REGISTERS , 0, 2, 17);    //30001-2 Line Voltage reg[17-18]       HB :17278 LB :44628
//    modbus_construct(&packets[PACKET3], 3,READ_INPUT_REGISTERS , 6, 2, 19);    // 30007-8 Line Current reg[19-20]      
//    modbus_construct(&packets[PACKET4], 3,READ_INPUT_REGISTERS , 62,2, 21);    // 30063-64 reg[21-22]                 Power Factor 
//    
//    modbus_construct(&packets[PACKET5], 3,READ_INPUT_REGISTERS , 72,2, 23);    // 30073-74 Energy Consumption  reg[23-24](kWh)   Reading 16384(73)[IMP] History Value
//
//    modbus_construct(&packets[PACKET6], 3,READ_INPUT_REGISTERS , 50,2, 25);    // 30089-90 Sys Power(kWh) reg[25-26]  //86 --> 50 Live Value
//    modbus_construct(&packets[PACKET7], 3,READ_INPUT_REGISTERS , 226,2,27);    // 30227-28 Device Run Hr reg[27-28]     
//    modbus_construct(&packets[PACKET8], 3,READ_INPUT_REGISTERS , 228,2,29);    // 30227-28 Power Available Time reg[29-30]    
//    modbus_construct(&packets[PACKET9], 3,READ_INPUT_REGISTERS , 54, 2,31);    // 30055-56 VAvg reg[31-32]   VA Avg

  //modbus_construct(&packets[PACKET2], 1, PRESET_MULTIPLE_REGISTERS, 0, 10, 0);
  
  modbus_configure(&Serial2, baud, SERIAL_8N1, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);
  delay(100);
  //pinMode(LED, OUTPUT);

  // Setup GSM Module 
  Serial1.begin(9600);             // To connect SIM900A and send AT Commands
  //myGateway.power_on();           // POWER ON GSM Module for communication 
  //myGateway.httpInit();          //Initialize http connections (please change APN as per your Network Operator)
  
/************** SD Card Init/Startup Code ***********************/
 //isSDCardCheck("chiller.csv");     //provide a File Name to Store log of ModBus Devices
 pinMode(A12,OUTPUT);
 digitalWrite(A12,LOW);
  
}

void loop()
{     
  //DateTime now = rtc.now();

  //Serial.println(getlogTime());
   //DateTime now = rtc.now();
  for(int i=0;i<200;i++)
  {
    modbus_update();
    delay(10); //Serial.println(i);
  }
//  if(now.second() == 30)
//  { 
//    Serial.println("RESET");
//    asm volatile ("  jmp 0"); 
//    //Reset the code
//  }

  digitalWrite(A12,HIGH);

  float cumultative_Eng_kWh_DG = myGateway.hextofloat(regs[0],regs[1]);
  
  float cumultative_Eng_kWh_EB = myGateway.hextofloat(regs[2],regs[3]);  //(0x0000,0x01AE)

  //int kwh_EB = (regs[2] << 16) | regs[3] ;
  
  Serial.print("HEX to DEC :");
  Serial.println( myGateway.hexToDec("333"));

  float Rphase_Volts = regs[4]/10;
  float Yphase_Volts = regs[5]; 
  float Bphase_Volts = regs[6]; 
  float Rphase_Current = regs[7]/10.0;
  float Yphase_Current = regs[8];
  float Bphase_Current = regs[9]; ///10.0;
  
  float Avgpower_Factor = myGateway.hextofloat(regs[10],regs[11]);
  
  float LineFrequencey = regs[12];  // line Freq.
  int EB_DG_Status = regs[13];   // EB DG Status

  
  float Rphase_active_power = myGateway.hextofloat(regs[14],regs[15]);
  
  float Yphase_active_power = myGateway.hextofloat(regs[16],regs[17]);

  float Bphase_active_power = myGateway.hextofloat(regs[18],regs[19]);
  
 
  float CT_primary = regs[20];

  float  MaximumDemand_EB = myGateway.hextofloat(regs[21],regs[22]);
 
  float PowerOnHr_EB = myGateway.hextofloat(regs[23],regs[24]);  
  float LoadOnHr_EB = myGateway.hextofloat(regs[25],regs[26]);
  
  float MaximumDemand_DB = myGateway.hextofloat(regs[27],regs[28]);  
 
  float PowerOnHr_DG = myGateway.hextofloat(regs[29],regs[30]);
  float LoadOnHr_DG = myGateway.hextofloat(regs[31],regs[32]);

  float THD_VR = regs[33]/10.0;
  float THD_VY = regs[34];
  float THD_VB = regs[35];
  
  float THD_IR = regs[36]/10.0;
  float THD_IY = regs[37];
  float THD_IB = regs[38];

  float RPhase_KVA = myGateway.hextofloat(regs[39],regs[40]);
  float YPhase_KVA = myGateway.hextofloat(regs[41],regs[42]);
  float BPhase_KVA = myGateway.hextofloat(regs[43],regs[44]);

  float RYB_KW = myGateway.hextofloat(regs[45],regs[46]);
  
  float RY_L2LVolts = myGateway.hextofloat(regs[47],regs[48]);
  float YB_L2LVolts = myGateway.hextofloat(regs[49],regs[50]);  
  float BR_L2LVolts = myGateway.hextofloat(regs[51],regs[52]);


  float CumEnergy_KVAh = myGateway.hextofloat(regs[53],regs[54]);
  float CumEnergy_KWh = myGateway.hextofloat(regs[55],regs[56]);
  float CumEnergy_KVArh_lag = myGateway.hextofloat(regs[57],regs[58]);
  float CumEnergy_KVArh_lead = myGateway.hextofloat(regs[59],regs[60]);

  float CumEnergy_RevKVAh = myGateway.hextofloat(regs[61],regs[62]);
  float CumEnergy_RevKWh = myGateway.hextofloat(regs[63],regs[64]);
  float CumEnergy_RevKVArh_lag = myGateway.hextofloat(regs[65],regs[66]);
  float CumEnergy_RevKVArh_lead = myGateway.hextofloat(regs[67],regs[68]);
  
  float Average_PF = myGateway.hextofloat(regs[69],regs[70]);
  
  
  

  Serial.print("cumultative_Eng_kWh_DG[HB].: ");Serial.println(regs[0],HEX);
  Serial.print("cumultative_Eng_kWh_DG[LB].: ");Serial.println(regs[1],HEX);

  Serial.print("[3000-1]: ");Serial.println(cumultative_Eng_kWh_DG,DEC);

  Serial.print("cumultative_Eng_kWh_EB[HB].: ");Serial.println(regs[2],HEX);
  Serial.print("cumultative_Eng_kWh_EB[LB].: ");Serial.println(regs[3],HEX);

  Serial.print("[3002-3]: ");Serial.println(cumultative_Eng_kWh_EB,DEC);

  Serial.print("[3004]: ");Serial.println(Rphase_Volts,DEC);
  Serial.print("[3005]: ");Serial.println(Yphase_Volts,DEC);
  Serial.print("[3006]: ");Serial.println(Bphase_Volts,DEC);
  Serial.print("[3007]: ");Serial.println(Rphase_Current,DEC);
  Serial.print("[3008]: ");Serial.println(Yphase_Current,DEC);
  Serial.print("[3009]: ");Serial.println(Bphase_Current,DEC);
  
  Serial.print("Avgpower_Factor[HB].: ");Serial.println(regs[10],HEX);
  Serial.print("Avgpower_Factor[LB].: ");Serial.println(regs[11],HEX);

  Serial.print("[3010-11]: ");Serial.println(Avgpower_Factor,DEC);

  Serial.print("[3012]: ");Serial.println(LineFrequencey,DEC);
  Serial.print("[3013]: ");Serial.println(EB_DG_Status,DEC); 

  Serial.print("Rphase_active_power[HB].: ");Serial.println(regs[14],HEX);
  Serial.print("Rphase_active_power[LB].: ");Serial.println(regs[15],HEX);

  Serial.print("[3014-15]: ");Serial.println(Rphase_active_power,DEC);
 
  Serial.print("Yphase_active_power[HB].: ");Serial.println(regs[16],HEX);
  Serial.print("Yphase_active_power[LB].: ");Serial.println(regs[17],HEX);

  Serial.print("[3016-17]: ");Serial.println(Yphase_active_power,DEC);
  
  Serial.print("Bphase_active_power[HB].: ");Serial.println(regs[18],HEX);
  Serial.print("Bphase_active_power[LB].: ");Serial.println(regs[19],HEX);

  Serial.print("[3018-19]: ");Serial.println(Bphase_active_power,DEC);


  Serial.print("[3020]: ");Serial.println(CT_primary,DEC);
  
  Serial.print("Maximum Demand EB [HB].: ");Serial.println(regs[21],HEX);
  Serial.print("Maximum Demand EB [LB].: ");Serial.println(regs[22],HEX);

  Serial.print("[3021-22]: ");Serial.println(MaximumDemand_EB,DEC);

   Serial.print("Power On Hr EB [HB].: ");Serial.println(regs[23],HEX);
   Serial.print("Power On Hr EB [LB].: ");Serial.println(regs[24],HEX);

  Serial.print("[3027-28]: ");Serial.println(PowerOnHr_EB,DEC);

   Serial.print("Load On Hr EB [HB].: ");Serial.println(regs[25],HEX);
   Serial.print("Load On Hr EB [LB].: ");Serial.println(regs[26],HEX);

  Serial.print("[3029-30]: ");Serial.println(LoadOnHr_EB,DEC);

   Serial.print("MaximumDemand_DB [HB].: ");Serial.println(regs[27],HEX);
   Serial.print("MaximumDemand_DB [LB].: ");Serial.println(regs[28],HEX);

  Serial.print("[3032-33]: ");Serial.println(MaximumDemand_DB,DEC);
  
   Serial.print("Power On Hr DG [HB].: ");Serial.println(regs[29],HEX);
   Serial.print("Power On Hr DG [LB].: ");Serial.println(regs[30],HEX);

  Serial.print("[3037-38]: ");Serial.println(MaximumDemand_DB,DEC);

   Serial.print("Load On Hr DG [HB].: ");Serial.println(regs[29],HEX);
   Serial.print("Load On Hr DG [LB].: ");Serial.println(regs[30],HEX);

  Serial.print("[3039-40]: ");Serial.println(LoadOnHr_DG,DEC);

  Serial.print("[3041]: ");Serial.println(THD_VR,DEC); 
  Serial.print("[3042]: ");Serial.println(THD_VY,DEC); 
  Serial.print("[3043]: ");Serial.println(THD_VB,DEC);

  Serial.print("[3044]: ");Serial.println(THD_IR,DEC);
  Serial.print("[3045]: ");Serial.println(THD_IY,DEC);
  Serial.print("[3046]: ");Serial.println(THD_IB,DEC); 
 
   Serial.print("RPhase_KVA [HB].: ");Serial.println(regs[39],HEX);
   Serial.print("RPhase_KVA [LB].: ");Serial.println(regs[40],HEX);

  Serial.print("[3048-49]: ");Serial.println(RPhase_KVA,DEC);
 
    Serial.print("YPhase_KVA [HB].: ");Serial.println(regs[41],HEX);
   Serial.print("YPhase_KVA [LB].: ");Serial.println(regs[42],HEX);

  Serial.print("[3050-51]: ");Serial.println(YPhase_KVA,DEC);

   Serial.print("BPhase_KVA [HB].: ");Serial.println(regs[43],HEX);
   Serial.print("BPhase_KVA [LB].: ");Serial.println(regs[44],HEX);

  Serial.print("[3052-53]: ");Serial.println(BPhase_KVA,DEC);

   Serial.print("RYB KW [HB].: ");Serial.println(regs[45],HEX);
   Serial.print("RYB KW [LB].: ");Serial.println(regs[46],HEX);

  Serial.print("[3054-55]: ");Serial.println(RYB_KW,DEC);
  
   Serial.print("RY_L2LVolts [HB].: ");Serial.println(regs[47],HEX);
   Serial.print("RY_L2LVolts [LB].: ");Serial.println(regs[48],HEX);

  Serial.print("[3056-57]: ");Serial.println(RY_L2LVolts,DEC);

   Serial.print("YB_L2LVolts [HB].: ");Serial.println(regs[49],HEX);
   Serial.print("YB_L2LVolts [LB].: ");Serial.println(regs[50],HEX);

  Serial.print("[3058-59]: ");Serial.println(YB_L2LVolts,DEC);

   Serial.print("BR_L2LVolts [HB].: ");Serial.println(regs[51],HEX);
   Serial.print("BR_L2LVolts [LB].: ");Serial.println(regs[52],HEX);

  Serial.print("[3060-61]: ");Serial.println(BR_L2LVolts,DEC);
  

   Serial.print("CumEnergy_KVAh [HB].: ");Serial.println(regs[53],HEX);
   Serial.print("CumEnergy_KVAh [LB].: ");Serial.println(regs[54],HEX);

  Serial.print("[3200-3201]: ");Serial.println(CumEnergy_KVAh,DEC);

  
   Serial.print("CumEnergy_KWh [HB].: ");Serial.println(regs[55],HEX);
   Serial.print("CumEnergy_KWh [LB].: ");Serial.println(regs[56],HEX);

  Serial.print("[3202-203]: ");Serial.println(CumEnergy_KWh,DEC);

   Serial.print("CumEnergy_KVArh_lag [HB].: ");Serial.println(regs[57],HEX);
   Serial.print("CumEnergy_KVArh_lag [LB].: ");Serial.println(regs[58],HEX);

  Serial.print("[3204-205]: ");Serial.println(CumEnergy_KVArh_lag,DEC);

   Serial.print("CumEnergy_KVArh_lead [HB].: ");Serial.println(regs[59],HEX);
   Serial.print("CumEnergy_KVArh_lead [LB].: ");Serial.println(regs[60],HEX);

  Serial.print("[3206-207]: ");Serial.println(CumEnergy_KVArh_lead,DEC);

   Serial.print("CumEnergy_RevKVAh [HB].: ");Serial.println(regs[61],HEX);
   Serial.print("CumEnergy_RevKVAh [LB].: ");Serial.println(regs[62],HEX);

  Serial.print("[3208-209]: ");Serial.println(CumEnergy_KVArh_lead,DEC);


   Serial.print("CumEnergy_RevKWh [HB].: ");Serial.println(regs[63],HEX);
   Serial.print("CumEnergy_RevKWh [LB].: ");Serial.println(regs[64],HEX);

  Serial.print("[3210-211]: ");Serial.println(CumEnergy_RevKWh,DEC);

  
   Serial.print("CumEnergy_RevKVArh_lag [HB].: ");Serial.println(regs[65],HEX);
   Serial.print("CumEnergy_RevKVArh_lag [LB].: ");Serial.println(regs[66],HEX);

  Serial.print("[3212-213]: ");Serial.println(CumEnergy_RevKVArh_lag,DEC);

   Serial.print("CumEnergy_RevKVArh_lead [HB].: ");Serial.println(regs[67],HEX);
   Serial.print("CumEnergy_RevKVArh_lead [LB].: ");Serial.println(regs[68],HEX);

  Serial.print("[3214-215]: ");Serial.println(CumEnergy_RevKVArh_lead,DEC);
  
   Serial.print("Average_PF [HB].: ");Serial.println(regs[69],HEX);
   Serial.print("Average_PF [LB].: ");Serial.println(regs[70],HEX);

  Serial.print("[3248-249]: ");Serial.println(Average_PF,DEC);

 
    // Energy Meter Readings 
 /*
  Serial.println("************ Energy Meter Readings ************* ");
  
  Serial.print("Line Voltages[HB].: ");Serial.println(regs[17],HEX);
  Serial.print("Line Voltages[LB].: ");Serial.println(regs[18],HEX);
  Serial.print("Line Current [HB].: ");Serial.println(regs[19],HEX);
  Serial.print("Line Current [LB].: ");Serial.println(regs[20],HEX);
  Serial.print("Device Voltage [HB].: ");Serial.println(regs[21],HEX);
  Serial.print("DEvice Voltage [LB].: ");Serial.println(regs[22],HEX);
  Serial.print("Device Current [HB].: ");Serial.println(regs[23],HEX);
  Serial.print("DEvice Current [LB].: ");Serial.println(regs[24],HEX);
  Serial.print("POWER[Sys W] [HB].: ");Serial.println(regs[25],HEX);
  Serial.print("POWER[Sys W] [LB].: ");Serial.println(regs[26],HEX);
  Serial.print("Power Consumption [HB].: ");Serial.println(regs[27],HEX);
  Serial.print("Power Consumption [LB].: ");Serial.println(regs[28],HEX);
  Serial.print("Device Run Hr [HB].: ");Serial.println(regs[29],HEX);
  Serial.print("DEvice Run Hr [LB].: ");Serial.println(regs[30],HEX);
  Serial.print("Power Available Time [HB].: ");Serial.println(regs[31],HEX);
  Serial.print("Power Available Time [LB].: ");Serial.println(regs[32],HEX);
  
  
  delay(50);
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


//  for(int i=0;i<sizeof(energy_meter);i++)
//  {
//    Serial.println(energy_meter[i]);
//    delay(5);
//  }
// 
*/ 
  Serial.println("**** done ****");
  digitalWrite(A12,LOW);
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
   logTime += "@";
   logTime += now.hour();
   logTime += ":";
   logTime += now.minute();
   logTime += ":";
   logTime += now.second();
   
   //Serial.print("Time:");Serial.println(logTime);
   //delay(2000);
   return logTime;
}


