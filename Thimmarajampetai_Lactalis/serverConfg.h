#ifndef serverConfg_h
#define serverConfg_h




 /***************** Thing Speak Settings ************************************/

    // Milk Chiller Details
    #define milkChiller               0
    #define milkChillerRelay          1
    #define energyMeter               2
    #define threephase_meterCh_1      3
    #define threephase_meterCh_2      4
    #define threephase_meterCh_3      5
    
    
    #define milkChillerKey         "3JE1MX7R5H4JECA4"    //api Key for Milk Chiller Channel
    #define milkChillerRelayKey    "479JM3P3602Z455P"   //api Key for Milk Chiller Relay Channel
    #define geyeserenergyMeterKey  "OSODHIM24J4ASYN7"   //api Key for Energy Meter Channel
    #define threephase_meterKey_1  "FN0UWWWY4YQV1LYM"   // 3 Ph meter channel 1 
    #define threephase_meterKey_2  "X7G905XJPGZGCM8Q"   // 3 ph meter channel 2
    #define threephase_meterKey_3  "JQS3503AL2RYX2M0"   // 3ph meter channel 3
    
    
    String channel_apiKey[] ={milkChillerKey, milkChillerRelayKey, geyeserenergyMeterKey,threephase_meterKey_1,threephase_meterKey_2,threephase_meterKey_3 };                    //write API key
    
    // Milk Chiller Relays Channel
    
    //const int Channel_ID2 =  184701;                       //Milk Chiller Relays ID 
    //String milkChillerRelays_apiKey ="5CFI74KGNN3Z3CR1";                    //write API key for Channel_ID2(Milk Chiller Relays Section)



 #endif
