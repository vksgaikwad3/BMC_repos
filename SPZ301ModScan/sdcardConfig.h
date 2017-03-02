#ifndef sdcardconfig_h
#define sdcardconfig_h

#include <SPI.h>
#include <SD.h>

File myFile;

 
volatile const int chipSelect  = 10;
void isSDCardCheck(String filename);
void readSDCard(String filename);
boolean writetoSDCard (String filename,byte mode,String logTime,int deviceID,float battery_Temp,float milk_Temp,float auxillary_Temp,float battery_Volt,float ac_Volt,float compressor_Current,float pump_Current,
                       boolean charg_pump_Relay,boolean condensor_Relay,boolean compressor_Relay,boolean inverter_Relay,boolean agitator_Relay,boolean tank_Relay,boolean shiva_Relay,boolean discharge_pump_Relay,float compressor_run_Hour,
                       float lineVolts,float lineCurrent,float deviceVolts,float deviceCurrent,float power,float powerConsump,float deviceRunHr,float powerAvailable);

 
boolean isSDCardexist(String filename);



void isSDCardCheck(String filename)
{
  
  Serial.print("Initializing SD card...");
  Serial.print("Chip Select Status :");Serial.println(digitalRead(chipSelect));
  delay(1000);
  while (SD.begin(chipSelect) == 0 ) {
    Serial.println("initialization failed!  NO SD CARD FOUND :( InsertCorrectly !!! "); delay(100);
    Serial.print("Chip Select Status IN :");Serial.println(digitalRead(chipSelect));
    
    continue;
    //return;
  }
  Serial.println("initialization done.");

/*** Checking if File Exist or not *******************/

  if (SD.exists(filename)) {
    Serial.println("chiller.csv exists.");
    myFile = SD.open("chiller.csv", FILE_WRITE);
    if(myFile.size() <= 0)    //check Size of file is zero or not
    {
     Serial.println("Start Writting Tabs !!! .");
     myFile.println("Time.,DeviceID,BatteryTemp(oC),MilkTemp(oC),AuxillaryTemp(oC),BatteryVoltage(VDC),ACVoltage(ACV),CompresorCurrent(A),PumpCurrent(A),ChargingPumpRelay,CondensorRelay,CompressorRelay,InverterRelay,AgitatorRelay,TankRelay,ShivaRelay,Disch.PumpRelay,CompressorRunHours ");
    
     myFile.close();
    }
//    else if(myFile.size() > 0 )
//    {
//      Serial.println("In a LOOOP()....Already Data present in SD Card. Appending the Data to .CSV file!!!");
//      if(data.length() > 0)
//      {
//        myFile.println(data);
//      }
//      Serial.println("No DATA in Buffer ....");
//      Serial.print("Chip Select Status in DATA BUF :");Serial.println(digitalRead(chipSelect));
//
//    }
    
    //SD.remove("chiller.csv");
  } 
  else if(myFile != 1) 
  {
    Serial.println("chiller.csv doesn't exist. Creating a New .csv file");
  

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  
  myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  while(myFile != 1)
   {
    myFile = SD.open(filename, FILE_WRITE);
    Serial.println("Trying to OPEN  and Create file ");
    continue;
   }
    Serial.print("IN File Status:");Serial.println(myFile);
    
    delay(1000);
    Serial.print("STATE:");Serial.println(myFile);
    if(myFile.size() <= 0)        //Check if Data is available or not
    {
      Serial.print("File Size:");Serial.println(myFile.size());
      Serial.print("Create Tabs in .csv...");
      myFile.println("Time,DeviceID,BatteryTemp(oC),MilkTemp(oC),AuxillaryTemp(oC),BatteryVoltage(VDC),ACVoltage(ACV),CompresorCurrent(A),PumpCurrent(A),ChargingPumpRelay,CondensorRelay,CompressorRelay,InverterRelay,AgitatorRelay,TankRelay,ShivaRelay,Disch.PumpRelay,CompressorRunHours,\tDeviceID,LineVolts[VAC],LineCurrent[A],DeviceVolts[VDC],DeviceCurrent[A],Power[W],PowerConsump[kWh],DeviceRunHr[Hrs],PowerAvailable[W]");
      myFile.close();
      Serial.println("Write Complete .");
   }
//   else if(myFile.size() > 0 )
//    {
//      Serial.println("In a LOOOP()....,,,Already Data present in SD Card. Appending the Data to .CSV file!!!");
//       if(data.length() > 0)
//       {
//          myFile.println(data);
//       }
//       Serial.println(" Data Buffer is Empty ...");
//    }
     
 }
/******* Reading .CSV File ******************/ 
//  readSDCard(filename);

}

void readSDCard(String filename)
{
    
/******* Reading .CSV File ******************/ 
  // re-open the file for reading:
  myFile = SD.open(filename);       //default in READ mode
  if (myFile) {
    Serial.print("File OPEN :");Serial.println(filename);

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
      
    }
    Serial.println("Read Complete");
    // close the file:
    myFile.close();
    Serial.println("*******************File Closed ******************************");
  } else {
    // if the file didn't open, print an error,and try to open:
    myFile = SD.open(filename, FILE_WRITE);
    while(myFile !=1)
    {
      Serial.println("error opening and Reading chiller.csv, ... Trying to Open file");
      myFile = SD.open(filename);    // Try to Open File
      Serial.println(myFile);
      continue;
    }
    while (myFile.available()) {      //Read All the Contents from File
      Serial.write(myFile.read());
      
    }
    Serial.println("Read Complete in 2nd try...");
    // close the file:
    myFile.close();
    
   }
}


/********************** Write Data to SD Card  ********************************/

boolean writetoSDCard (String filename,byte mode,String logTime,int deviceID1,float battery_Temp,float milk_Temp,float auxillary_Temp,float battery_Volt,float ac_Volt,float compressor_Current,float pump_Current,
                       boolean charg_pump_Relay,boolean condensor_Relay,boolean compressor_Relay,boolean inverter_Relay,boolean agitator_Relay,boolean tank_Relay,boolean shiva_Relay,boolean discharge_pump_Relay,float compressor_run_Hour,
                       int deviceID2,float lineVolts,float lineCurrent,float deviceVolts,float deviceCurrent,float power,float powerConsump,float deviceRunHr,float powerAvailable)
{
  //volatile int srNo = 1;
  String data ="" ;


  
  myFile = SD.open(filename, mode); // FILE_WRITE

  // if the file opened okay, write to it:
    data += logTime;
    data += ",";
    data += deviceID1;
    data += "," ;
    data += battery_Temp;
    data += ",";
    data += milk_Temp;
    data += ",";
    data += auxillary_Temp;
    data += ",";
    data += battery_Volt;
    data +=  "," ;
    data += ac_Volt ;
    data += "," ;
    data += compressor_Current;
    data += "," ;
    data += pump_Current ;
    data += "," ;
    data += charg_pump_Relay ;
    data += "," ;
    data += condensor_Relay ;
    data += "," ;
    data += compressor_Relay ;
    data += "," ;
    data += inverter_Relay ;
    data += "," ; 
    data += agitator_Relay ;
    data += "," ; 
    data += tank_Relay;
    data += "," ;
    data += shiva_Relay ;
    data += "," ;   
    data += discharge_pump_Relay ;
    data += "," ;
    data += compressor_run_Hour;
    data += "\t\t";
    data += deviceID2;
    data += ",";
    data += lineVolts;
    data += ",";
    data += deviceVolts;
    data += ",";
    data += lineCurrent;
    data += ",";
    data += deviceCurrent;
    data += ",";
    data += power;
    data += ",";
    data += powerConsump;
    data += ",";
    data += deviceRunHr;
    data += ",";
    data += powerAvailable;
     
    //srNo++;
  Serial.print("File Status:");Serial.println(myFile);
  if(myFile)
  {  
     Serial.println(data);
     delay(1000);
     myFile.println(data);      //file is open and write to file
     myFile.close();
     Serial.println("Write Complete .");
  }
  
  else {
    // if the file didn't open, print an error:
    Serial.println("error in opening .CSV... SD Card NOT  FOUND !!!!!");

   myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  while(myFile != 1)
   {
    myFile = SD.open(filename, FILE_WRITE);
    Serial.println("Trying to OPEN  and Create file ");
    continue;
   }
    Serial.print("IN File Status:");Serial.println(myFile);
    delay(1000);
    //Serial.print("STATE:");Serial.println(myFile);
    myFile.println(data);
    Serial.println(data);
    
    myFile.close(); delay(1000);
    
    Serial.println("Write Complete in Next Attempt ....");
  }

  //isSDCardCheck(filename);
  return myFile;
}


//boolean isSDCardexist(String filename)
//{
//  while (SD.begin(chipSelect) ==0 ) {
//    Serial.println("initialization failed!  NO SD CARD FOUND :( InsertCorrectly !!! ");
//    continue;
//    //return;
//  }
//  Serial.println("initialization done.");
//
///*** Checking if File Exist or not *******************/
//
//  if (SD.exists(filename)) {
//    Serial.println("chiller.csv exists.");
//    myFile = SD.open(filename, FILE_WRITE);
//    
//    Serial.print("Write data in .csv... !!!");
//    
//    Serial.println(data);
//    //delay(2000);
//    myFile.println(data);
//    
//    myFile.close();
//    Serial.println("Write Complete .");
//    
//    return true;
//  }
//
//  Serial.println("CARD NOT FOUND ....");
//  return false;  
//}

#endif
