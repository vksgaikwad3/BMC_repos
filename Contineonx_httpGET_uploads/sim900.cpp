#include "sim900.h"
#include <avr/wdt.h>


//Note: response[1000] array contains all the response of GSM AT Commands 

// GSM/GPRS Configurations and Think Spwak Cloud Configurations Settings

char aux_str[100];
    
char pin[]="";
//char apn[]="imis/internet";     //idea
char apn[]="airtelgprs.com";     //Airtel
//char apn[]="www";     //Vodaphone

char user_name[]="";
char password[]="";

String getStr="";

//char IP_address[]="184.106.153.149";   //Think Speak Server IP Address
char IP_address[]="52.66.172.58";      //"api.thingspeak.com";   //URL
char port[]= "3041";  //"80";                      // PORT Connected on 

boolean sim900Status = false;

/*************************************************************************************************/

sim900_GPRS::sim900_GPRS()
{
    //power_on();     // GSM ON
    
}

/*************************************************************************************************/

void sim900_GPRS::power_on()
{
   uint8_t answer=0;
    
   sim900Status=true;
   int ctT=0;
 digitalWrite(13,HIGH);
 delay(1000);
 digitalWrite(13,LOW);
 delay(2000);
 digitalWrite(13,HIGH);
 delay(5000);
    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // waits for an answer from the module
        while(answer == 0){     // Send AT every two seconds and wait for the answer
            answer = sendATcommand("AT", "OK", 2000);    
      ctT++;
    if(ctT==10){
     sim900Status=true;
     Serial.println("GSM OFF");
    power_on();
    }  
      }
    }
    Serial.println("GSM ON Sucessfully");
    // Once GSM get ON dothe folowing configurations to Send Systems Setting over SMS
      
    //sendATcommand("ATE0", "OK", 1000);        // ON/OFF AT command info
   // sendATcommand("AT+CMGF=1", "OK",1000);    // Select SMS message Format 0 = PDU mode , 1 = Text Mode
   // sendATcommand("AT+CMGD=1", "OK", 1000);   // Delete an SMS message
   // sendATcommand("AT+CNMI=2,2,0,0,0\r", "OK", 1000);  //New SMS Indications 
   // sendATcommand("AT+CMGR=1", "OK", 1000);            // Read SMS message has just arrived
    
    
   // sendATcommand("AT+CCLK?", "OK", 1000);          // GSM Clock optional
  //   sendATcommand("AT+CCLK=\"16/07/27,14:32:30+05\"", "OK", 1000);
  //  sendSMS("hello");
}

/******************************************************************************************************************************************/

int8_t sim900_GPRS::sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout)
{

    uint8_t x=0,  answer=0;
    char response[1000];         //Response buffer
    unsigned long previous;

    memset(response, '\0', 1000);    // Initialize the string

    delay(100);

    while( Serial.available() > 0) Serial.read();    // Clean the input buffer

    Serial.println(ATcommand);    // Send the AT command 
    Serial1.println(ATcommand); //Response from Serial1 

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(Serial1.available() != 0){    
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = Serial1.read();     //Read Response from Serial1 port
            Serial.print(response[x]);        //Print response on Serial 0 port
            x++;
            // check if the desired answer  is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
            }
        }
    }
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));    //Check till answer = 0 and timout period(ms)

        return answer;
}

/**********************************************************************************************************************************************************/

int8_t sim900_GPRS::sendATcommand2(String ATcommand, char* expected_answer1,char* expected_answer2, unsigned int timeout)
{
  
    uint8_t x=0,  answer=0;
   char response[700];
    unsigned long previous;

    memset(response, '\0', 700);    // Initialize the string

    delay(100);

    while( Serial1.available() > 0) Serial1.read();    // Clean the input buffer

  Serial1.println(ATcommand);    // Send the AT command 
Serial.println(ATcommand);
    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(Serial1.available() != 0){    
            response[x] = Serial1.read();
             Serial.print(response[x]);
            x++;
            // check if the desired answer 1  is in the response of the module
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
            // check if the desired answer 2 is in the response of the module
            else if (strstr(response, expected_answer2) != NULL)    
            {
                answer = 2;
            }
          
         
        }
    }
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}

/**************************************************************************************************************************************************************/

int8_t sim900_GPRS::sendATcommand3(String ATcommand, char* expected_answer1,char* expected_answer2,char*expected_answer3, unsigned int timeout)
{

    uint8_t x=0,  answer=0;
    char response[1000];
    unsigned long previous;

    memset(response, '\0', 500);    // Initialize the string

    delay(100);

    while( Serial1.available() > 0) Serial1.read();    // Clean the input buffer

    Serial1.println(ATcommand);    // Send the AT command 
    Serial.println(ATcommand);
    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(Serial1.available() != 0){    
            response[x] = Serial1.read();
             Serial.print(response[x]);
            x++;
            // check if the desired answer 1  is in the response of the module
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
            // check if the desired answer 2 is in the response of the module
            if (strstr(response, expected_answer2) != NULL)    
            {
                answer = 2;
            }
            else if (strstr(response, expected_answer3) != NULL)    
            {
                answer = 3;
            }
          
         
        }
    }
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}

/*****************************************************************************************************************************************************/

void sim900_GPRS::updateThinkSpeak(String channel_apiKey,float field1=0,float field2=0,float field3=0,float field4=0,float field5=0,float field6=0,float field7=0,float field8=0,
                                    float field9=0,float field10=0,float field11=0,float field12=0,float field13=0,float field14=0,float field15=0,float field16=0)
{
     ///sendATcommand("AT+CGATT=0","OK",3000); delay(3000);
     
   if(sim900Status==true)
   {  // Selects Single-connection mode
      if (sendATcommand2("AT+CIPMUX=0", "OK","ERROR", 1000) == 1)      // CIMPUX=0 is already set in Single-connection mode
       {
        // Waits for status IP INITIAL
          while(sendATcommand("AT+CIPSTATUS","INITIAL", 1000)  == 0 );  // Check Current Connection Status
          delay(5000);                                                   //wait 5 sec
        //aux_str="AT+CSTT=\"airtelgprs.com\",\"\",\"\"";
        //sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000);
          snprintf(aux_str, sizeof(aux_str), "AT+CSTT=\"%s\",\"%s\",\"%s\"", apn, user_name, password);  //Put GPRS setings
        // Sets the APN, user name and password
        //check wether GPRS is attached or detached
        if(sendATcommand("AT+CGATT?","+CGATT: 0\n OK",2000))       // GPRS is detached 
        { // attached the GPRS
           Serial.println("GPRS connections is attached "); 
          while(sendATcommand("AT+CGATT=1","OK",3000) !=1);        //donothing until GPRS gets setup
          Serial.println("GPRS connections is UP Now ");      
        }
        else { Serial.println("GPRS is already attached ");
        }
          
        if (sendATcommand2(aux_str, "OK",  "ERROR", 30000) == 1)        // Stuck here with ERROR
        {            
          // Waits for status IP START
          while(sendATcommand("AT+CIPSTATUS", "START", 500)  == 0 );
          delay(2000);
          // Brings Up Wireless Connection
          sendATcommand("AT+CSQ", "OK", 1000);
          if (sendATcommand2("AT+CIICR", "OK", "ERROR", 30000) == 1)
          {    
              //delay(2000);
              Serial.println("\n Bringup Wireless Connection ...........");
             // Waits for status IP GPRSACT
             while(sendATcommand("AT+CIPSTATUS", "GPRSACT", 1000)  == 0 );
             delay(2000);
            
              // Gets Local IP Address
              if (sendATcommand2("AT+CIFSR", ".", "ERROR", 10000) == 1)
              {
                 // Waits for status IP STATUS
                 while(sendATcommand("AT+CIPSTATUS", "IP STATUS", 500)  == 0 );
                 delay(2000);
                 Serial.println("Opening TCP");
                 //  aux_str="AT+CIPSTART=\"TCP\",\"184.106.153.149\",\"80\"";
                 //  sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000);
                 snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"", IP_address, port);
                 // snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"", thethingsiOserver,port);  // thethings.iO Server   
                // Opens a TCP socket
                 if (sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000) == 1)
                 {
                         Serial.println("Connected");

                         Serial.println("\n Send some data to TCP Socket............");
//                         getStr="GET /update?api_key="+ channel_apiKey +"&field"+ id1 +"=" +field1 + "&field"+ id2 + "=" +field2 +"&field" +id3 + "=" +field3 +"&field" +id4 +"=" +field4 
//                                +"&field5="+ field5  +"&field6="+ field6 +"&field7="+ field7 +"&field8=" + field8 + "\r\n\r\n";      //TCP packet to send GET Request on https (Think Speak)

                          getStr = "DeviceID=1&field1=";
                          getStr += field1;
                          getStr += "&field2=";
                          getStr += field2 ;
                          getStr += "&field3=";
                          getStr += field3 ;
                          getStr += "&field4=";
                          getStr += field4 ;  
                          getStr += "&field5=";
                          getStr += field5 ;
                          getStr += "&field6=";
                          getStr += field6 ;
                          getStr += "&field7=";
                          getStr += field7 ;
                          getStr += "&field8=" ;
                          getStr += field8;
                          
                          getStr += "&field9=" ;
                          getStr += field9 ;
                          getStr += "&field10=";
                          getStr += field10 ;
                          getStr += "&field11=" ;
                          getStr += field11 ;
                          getStr += "&field12=" ;
                          getStr += field12 ;
                          getStr += "&field13=";
                          getStr += field13 ;
                          getStr += "&field14=" ;
                          getStr += field14 ;
                          getStr += "&field15=" ;
                          getStr += field15 ;
                          getStr += "&field16=" ;
                          getStr += field16;
                          getStr += "\r\n\r\n" ;
                          
                          String sendcmd = "AT+CIPSEND="+ String(getStr.length());
                          if (sendATcommand2(sendcmd, ">", "ERROR", 10000) == 1)    
                            {
                              delay(100);
                              //ThingSpeak.writeField(Channel_ID, 1, field1, myWriteAPIKey);
                              sendATcommand2(getStr, "SEND OK", "ERROR", 10000);      //Field1 Data sending from here
                              delay(100);
                            }
                            Serial.println("Closing the Socket............");
                            
                            
                             // Closes the socket
                             sendATcommand2("AT+CIPCLOSE", "CLOSE OK", "ERROR", 20000);
                             Serial.println("Conneection CLOSED OK");
                  }
                  else
                  {
                      Serial.println("Error opening the connection");
                       
                  }  
              }         //AT+CIFSR
          }     //AT+CIICR Bringup connection
          else { Serial.println("GPRS Bringup Failed , Try Again!!!");
                sendATcommand("AT+CSQ=?", "OK", 1000);
                while(sendATcommand2("AT+CIPSHUT", "OK", "ERROR", 10000) != 1);
                delay(5000); 
                //GPRSFailedSMS();        // SMS GPRS connecion Problem
                wdt_enable(WDTO_8S);    // Enable Watchdog Reset 
          }
        }        //AT+CSTT APN,PSW,PORT setting
       }          //AT+CIPMUX =0  Single-connection mode
   }              //if(sim900Status==true)
    Serial.println("Shutting down the connection.........");
    sendATcommand2("AT+CIPSHUT", "OK", "ERROR", 5000);
    delay(5000);
    sendATcommand("AT+CGATT=1","OK",3000);
}


/**********************************************************************************************************************************************************/
void sim900_GPRS::updateMilkChillerRelays(String channel_apiKey,uint8_t field1,uint8_t field2,uint8_t field3,uint8_t field4,uint8_t field5,uint8_t field6,uint8_t field7,uint8_t field8)
{
   if(sim900Status==true)
   {  // Selects Single-connection mode
      if (sendATcommand2("AT+CIPMUX=0", "OK","ERROR", 1000) == 1)      // CIMPUX=0 is already set in Single-connection mode
       {
        // Waits for status IP INITIAL
          while(sendATcommand("AT+CIPSTATUS","INITIAL", 1000)  == 0 );  // Check Current Connection Status
          delay(5000);                                                   //wait 5 sec
        //aux_str="AT+CSTT=\"airtelgprs.com\",\"\",\"\"";
        //sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000);
          snprintf(aux_str, sizeof(aux_str), "AT+CSTT=\"%s\",\"%s\",\"%s\"", apn, user_name, password);  //Put GPRS setings
        // Sets the APN, user name and password
        if (sendATcommand2(aux_str, "OK",  "ERROR", 30000) == 1)
        {            
          // Waits for status IP START
          while(sendATcommand("AT+CIPSTATUS", "START", 500)  == 0 );
          delay(2000);
          // Brings Up Wireless Connection
          if (sendATcommand2("AT+CIICR", "OK", "ERROR", 30000) == 1)
          {    
              //delay(2000);
              Serial.println("\n Bringup Wireless Connection ...........");
             // Waits for status IP GPRSACT
             while(sendATcommand("AT+CIPSTATUS", "GPRSACT", 1000)  == 0 );
             delay(2000);
            
              // Gets Local IP Address
              if (sendATcommand2("AT+CIFSR", ".", "ERROR", 10000) == 1)
              {
                 // Waits for status IP STATUS
                 while(sendATcommand("AT+CIPSTATUS", "IP STATUS", 500)  == 0 );
                 delay(2000);
                 Serial.println("Opening TCP");
                 //  aux_str="AT+CIPSTART=\"TCP\",\"184.106.153.149\",\"80\"";
                 //  sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000);
                 snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"", IP_address, port);
                 // snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"", thethingsiOserver,port);  // thethings.iO Server   
                // Opens a TCP socket
                 if (sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000) == 1)
                 {
                         Serial.println("Connected");

                         Serial.println("\n Send some data to TCP Socket............");
                         getStr="GET /update?api_key="+ channel_apiKey +"&field1="+ field1 + "&field2="+ field2 +"&field3=" +field3 +"&field4=" +field4 
                                                      +"&field5="+ field5  +"&field6="+ field6 +"&field7="+ field7 +"&field8=" + field8 + "\r\n\r\n";      //TCP packet to send GET Request on https (Think Speak)
                          
                          // getStr= "GET /raspberry/api.php?serviceName=test_api&field1=123\r\n\r\n";  //+ "\r\n\r\n";   //Smartnub packet request
                          //String value = "{"values": [ { "key": "fun","value": "5000" } ] }\";
                          //getStr="POST /v2/things/{{doAgkdNly8o29pusozJ-3mNnqET6dKKvcI4DmLVe-t4}}\" + value + "\r\n\r\n";   //thethingsIO request to write
                          
                          //getStr="PUT https://api.thingspeak.com/channels/"+ Channel_ID ;
                         // getStr += apiKey;
                         // getStr +="&field1=";
                         // getStr += field1;
                         // getStr += "\r\n\r\n";
                          
                          String sendcmd = "AT+CIPSEND="+ String(getStr.length());
                          if (sendATcommand2(sendcmd, ">", "ERROR", 10000) == 1)    
                            {
                              delay(100);
                              //ThingSpeak.writeField(Channel_ID, 1, field1, myWriteAPIKey);
                              sendATcommand2(getStr, "SEND OK", "ERROR", 10000);      //Field1 Data sending from here
                              delay(100);
                            }
                            Serial.println("Closing the Socket............");
                            
                            
                             // Closes the socket
                             sendATcommand2("AT+CIPCLOSE", "CLOSE OK", "ERROR", 20000);
                             Serial.println("Conneection CLOSED OK");
                  }
                  else
                  {
                      Serial.println("Error opening the connection");
                       
                  }  
              }
          }
        }
       }
   }
    Serial.println("Shutting down the connection.........");
    sendATcommand2("AT+CIPSHUT", "OK", "ERROR", 10000);
    delay(5000);
}


/**********************************************************************************************************************************************************/

float sim900_GPRS:: hextofloat(uint16_t hByte, uint16_t lByte)
{
  uint32_t hb = 0x0000FFFF & hByte ; 
  uint32_t num = (hb << 16) | lByte ;
  
   //uint32_t lb = Serial.println(44628);
  
  uint32_t myString[1] ={num } ;
  uint32_t val;
  float floatVal;
  
  //Serial.print("Masking No :");Serial.println(hb);
  //Serial.print("After Shift << 16: ");Serial.println(num);
  num = myString[0];
  //Serial.println(num);
  floatVal = *((float*)&num);            // OR floatVal = *((float*)&myString);
  //Serial.println(floatVal);

  return floatVal; 
}
/****************************************************************************************************************************************************************/
void sim900_GPRS::httpInit()
{
  
  sendATcommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"","OK",1000);      //set Connection type as GPRS
  sendATcommand("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"","OK",1000); //Set APN of SIM Card
  sendATcommand("AT+SAPBR= 1,1","OK",1000);     //Enable GPRS if ERROR, its alread Enable
  sendATcommand("AT+SAPBR=2,1","OK",500);     //Get the IP Address 
  
}

/****************************************************************************************************************************************************************/
boolean sim900_GPRS::httpGETupdate(String channelKey,float field1,float field2,float field3,float field4,float field5,float field6,float field7,float field8)
{

  String serverURL = "";
  
  sendATcommand("AT+HTTPINIT","OK",500);
  sendATcommand("AT+HTTPPARA=\"CID\",\"1\"","OK",500);    //Start by setting up the HTTP bearer profile identifier

  serverURL += "AT+HTTPPARA=";
  serverURL += "\"URL\"";
  serverURL += ",";
 //serverURL += "\"http://smartnub.com/raspberry/api.php?serviceName=test_api&field1=";
 //serverURL += "\"http://api.thingspeak.com/update?api_key=";
  serverURL += "\"http://app.contineonx.com/promethean-api/storegatewaydata?accessKey=";
  serverURL += channelKey;
  serverURL += "&DeviceID=1";
  serverURL += "&field1=" ;
  serverURL += field1;
  serverURL += "&field2=";
  serverURL += field2;
  serverURL += "&field3=";
  serverURL += field3;
  serverURL += "&field4=";
  serverURL += field4;
  serverURL += "&field5=";
  serverURL += field5;
  serverURL += "&field6=";
  serverURL += field6;
  serverURL += "&field7=";
  serverURL += field7;
  serverURL += "&field8=";
  serverURL += field8;
  serverURL += "\"";
  
  //Serial.println("\"" + serverURL + "\"");

  Serial.println(serverURL.length());
  sendATcommand(serverURL.c_str(),"OK",7000);
   
 //Serial.println("HTTP READ");
 sendATcommand("AT+HTTPACTION= 0","OK",7000);   //Set GET Mode
 delay(10000);Serial.println("");
 Serial.println(" ***************************** DONE ***************************");

}

/************************** http POST *********************************************/



