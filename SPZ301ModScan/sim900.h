#ifndef sim900_h
#define sim900_h

#include<Arduino.h>

//600 --> NOT HTTP PDU
//601 --> Network Error 
//602 --> No Memory
//603 --> DNS Error
//604 --> Stack Busy
    

// SIM900 GSM/GPRS class

class sim900_GPRS
{
  public: 

    sim900_GPRS();    //Constructer
    void power_on();  //Power on GSM method
    int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);
    int8_t sendATcommand2(String ATcommand, char* expected_answer1,char* expected_answer2, unsigned int timeout);
    int8_t sendATcommand3(String ATcommand, char* expected_answer1,char* expected_answer2,char*expected_answer3, unsigned int timeout);
    void updateThinkSpeak(String channel_apiKey,uint8_t id1,uint8_t id2,uint8_t id3,uint8_t id4,float field1,float field2,float field3,float field4,float field5,float field6,float field7,float field8); //,float field6,float field7,float field8);
    void updateMilkChillerRelays(String channel_apiKey,uint8_t field1,uint8_t field2,uint8_t field3,uint8_t field4,uint8_t field5,uint8_t field6,uint8_t field7,uint8_t field8);
    float hextofloat(uint16_t hByte, uint16_t lByte); 
    boolean httpGETupdate(String channelKey,float field1,float field2,float field3,float field4,float field5,float field6,float field7,float field8);  
    void httpInit();
    unsigned int hexToDec(String hexString);
  private:

  
   
  
};

#endif

