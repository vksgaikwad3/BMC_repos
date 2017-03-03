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
    
    
    #define milkChillerKey         "2LL3ZP3Y5DDC1WGR"    //api Key for Milk Chiller Channel
    #define milkChillerRelayKey    "Q4K55125X3HKDWHX"   //api Key for Milk Chiller Relay Channel
    #define geyeserenergyMeterKey  "K9SST9B87LBSLRV4"   //api Key for Energy Meter Channel
    #define threephase_meterKey_1  "IUBD7SU5KGJJ0E2W"   // 3 Ph meter channel 1 
    #define threephase_meterKey_2  "EJPG7IX0N6GBNDVK"   // 3 ph meter channel 2
    #define threephase_meterKey_3  "C8TXG11CR9MH6FWF"   // 3ph meter channel 3
    
    
    String channel_apiKey[] ={milkChillerKey, milkChillerRelayKey, geyeserenergyMeterKey,threephase_meterKey_1,threephase_meterKey_2,threephase_meterKey_3 };                    //write API key
    
    // Milk Chiller Relays Channel
    
    //const int Channel_ID2 =  184701;                       //Milk Chiller Relays ID 
    //String milkChillerRelays_apiKey ="5CFI74KGNN3Z3CR1";                    //write API key for Channel_ID2(Milk Chiller Relays Section)



 #endif
