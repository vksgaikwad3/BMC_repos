#include <ThingSpeak.h>

/* Author : Vikas Gaikwad
 * Project : Wireless Data Logger with GSM/GPRS and Cloud visualization.
 * Company: Promethean Power Pune.
 * Date : 05/11/2016 
 * 
 */


char aux_str[100];
    
char pin[]="";
//char apn[]="imis/internet";
char apn[]="airtelgprs.com";    
char user_name[]="";
char password[]="";
//char IP_address[]="184.106.153.149";   //Think Speak Server IP Address

///char IP_address[]="api.thingspeak.com";   //URL
char ubidots_url[] = "things.ubidots.com";

char IP_address[]="app.contineonx.com";   //URL
char port[]="80";                         // PORT Connected on 

String getStr="";

const int Channel_ID =  176340;
const char * myWriteAPIKey = "XTUFVMDZYZLA0TB6";

String apiKey ="XTUFVMDZYZLA0TB6";    //write API key
//String talkback_api="A0240MZ11ZL0L52I";

boolean sim900Status = false;

// thethings.iO Details

byte thethingsiOserver[] = "77.73.82.243" ;    //server IP Address
//char thethingsiOClient::server[] = "api.thethings.io";
// activationcode = 1yBhA60ckC43zyhDpEfpLxgl-AS2wxEXww

// SmartNub Web Server Details

// http://smartnub.com/raspberry/api.php?serviceName=test_api

char smartnubServer[] = "68.65.120.175";  //smartnub.com";

void setup() {
  // put your setup code here, to run once:

Serial.begin(9600);   //Serial to print serial megssages and for MODBUS communication
Serial1.begin(9600);   // To connect SIM900A and send AT Commands
//ThingSpeak.begin(client);
power_on();            // POWER ON GSM Module for communication 
pinMode(13,OUTPUT);

pinMode(A0,INPUT);
}

void loop() {

  int tempSensor = map(analogRead(A0),0,1023,-20,80);  //Read Sensor data (Milk Temperature Range (-20 oC to 80 oC )
  
  int batteryTemp = map(analogRead(A1),0,1023,-20,80);
  
  int AuxTemp = map(analogRead(A2),0,1023,-20,80);
 
  Serial.println(tempSensor);
  
  updateThinkSpeak(tempSensor,batteryTemp,AuxTemp) ;      //Send to Think Speak 
  //updateThinkSpeak("586d410d76254216d82f3cee","07p6khoa3RZ7MhcwOSPyzXOlS2a1HZ",tempSensor);
  delay(1000);  
  
// put your main code here, to run repeatedly:
//Serial.println("Checking GSM MODEM.....");
//Serial1.println("AT");
//Serial.println(Serial1.read());
//delay(2000);
//Serial1.println("AT+CPIN?");
//Serial.println(char(Serial1.read()));
//delay(2000);
//int ans = sendATcommand("AT","OK",2000);
//Serial.println("");Serial.println(ans);
//power_on();

}


void power_on(){

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
    {   Serial.println("SIM ON");
        // power on pulse
//       digitalWrite(relay,HIGH);
//        delay(3000);
//        digitalWrite(relay,LOW);
//    
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


int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[3000];         //Response buffer
    unsigned long previous;

    memset(response, '\0', 3000);    // Initialize the string

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


int8_t sendATcommand2(String ATcommand, char* expected_answer1,char* expected_answer2, unsigned int timeout){

    uint8_t x=0,  answer=0;
   char response[3000];
    unsigned long previous;

    memset(response, '\0', 3000);    // Initialize the string

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
int8_t sendATcommand3(String ATcommand, char* expected_answer1,char* expected_answer2,char*expected_answer3, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[3000];
    unsigned long previous;

    memset(response, '\0', 3000);    // Initialize the string

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

void updateThinkSpeak(int field1,int field2,int field3)
//void updateThinkSpeak(String VID,String token,float no)
{
  
   if(sim900Status==true)
   {  // Selects Single-connection mode
      if (sendATcommand2("AT+CIPMUX=0", "OK","ERROR", 1000) == 1)      // CIMPUX=0 is already set in Single-connection mode
       {
        // Waits for status IP INITIAL
          while(sendATcommand("AT+CIPSTATUS","INITIAL", 1000)  == 0 );  // Check Current Connection Status
          delay(2000);                                                   //wait 5 sec
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
              delay(2000);
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
                 snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",IP_address, port); //IP_address  
                 // snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"", thethingsiOserver,port);  // thethings.iO Server   
                // Opens a TCP socket
                 if (sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000) == 1)
                 {
                         Serial.println("Connected");

                         Serial.println("\n Send some data to TCP Socket............");
                         //getStr="GET /update?api_key="+ apiKey +"&field1=" +field1 + "&field2=" +field2 +"&field3=" +field3 + "\r\n\r\n";   //TCP packet to send GET Request on https (Think Speak)
                         
                         getStr="GET /promethean-api/storegatewaydata?accessKey=yTTMDHQZGLJOnuhreJXbMEQG1wb5drfgmETLH0ko6Jk%3D&DeviceID=1&field1=10&field2=12&field3=45\r\n\r\n"; 
                         
                         //getStr = "GET /promethean-api/storegatewaydata?accessKey=yTTMDHQZGLJOnuhreJXbMEQG1wb5drfgmETLH0ko6Jk%3D&DeviceID=1&field1=" + field1 + "&field2=" + field2 +"&field3="+ field3 + "\r\n\r\n";

                          
                          // getStr= "GET /raspberry/api.php?serviceName=test_api&field1=123\r\n\r\n";  //+ "\r\n\r\n";   //Smartnub packet request
                          //String value = "{"values": [ { "key": "fun","value": "5000" } ] }\";
                          //getStr="POST /v2/things/{{doAgkdNly8o29pusozJ-3mNnqET6dKKvcI4DmLVe-t4}}\" + value + "\r\n\r\n";   //thethingsIO request to write
                          
                          //getStr="PUT https://api.thingspeak.com/channels/"+ Channel_ID ;
                         // getStr += apiKey;
                         // getStr +="&field1=";
                         // getStr += field1;
                         // getStr += "\r\n\r\n";
                          
                       /*  int num=0;
                         String var = "{\"value\":"+ String(no) + "}";
                         num = var.length();
                          
//                          String values = "[{\"variable\" : \"{ 586e9ad376254252a498ca6b }\",\"value\":  230 },{\"variable\": \"{ 586cbeef76254220e800e765 }\", \"value\": 3.25 }]";
//                          num = values.length();
                          
                         getStr = "POST /api/v1.6/variables/"+VID+"/values HTTP/1.1\nContent-Type: application/json\nContent-Length: "+String(num)+"\nX-Auth-Token: "+token+"\nHost: things.ubidots.com\n\n";
                         getStr += var;
                         //Send Multiple Values 
                         
//                         getStr = "POST /api/v1.6/collections/?token={"+ token +"}HTTP/1.1\nHost: things.ubidots.com\nContent-Type: application/json\nContent-Length: "+String(num)+"\n" +
//                         getStr += values;
//                         getStr += "\r\n\r\n";                                  
            */
                         String sendcmd = "AT+CIPSEND="+ String(getStr.length());
                          
                          if (sendATcommand2(sendcmd, ">", "ERROR", 10000) == 1)    
                            {
                              delay(100);
                              //ThingSpeak.writeField(Channel_ID, 1, field1, myWriteAPIKey);
                              sendATcommand2(getStr, "SEND OK", "ERROR", 10000);      //Field1 Data sending from here
                            }
                              Serial.println("Closing the Socket............");
                            
                             // sendATcommand2("AT+CIPCLOSE", "CLOSE OK", "ERROR", 10000);
                                  
                             // Closes the socket
                             sendATcommand2("AT+CIPCLOSE", "CLOSE OK", "ERROR", 10000);
                        
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
   
