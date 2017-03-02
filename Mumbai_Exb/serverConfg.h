#ifndef serverConfg_h
#define serverConfg_h




 /***************** Thing Speak Settings ************************************/

    // Milk Chiller Details
    #define milkChiller               0
    #define milkChillerRelay          1
    #define energyMeter               2
    
    
    #define milkChillerKey         "IUBD7SU5KGJJ0E2W"         //"2LL3ZP3Y5DDC1WGR"         //api Key for Milk Chiller Channel
    #define milkChillerRelayKey    "EJPG7IX0N6GBNDVK"         //"Q4K55125X3HKDWHX"        //api Key for Milk Chiller Relay Channel
    #define energyMeterKey         "K9SST9B87LBSLRV4"        //api Key for Energy Meter Channel

    // MilkChiller2_Exb API KEy = IUBD7SU5KGJJ0E2W
    // MilkChillerRelays2_Exb API Key = EJPG7IX0N6GBNDVK
    // This is for Device No 2  
    
    String channel_apiKey[] ={milkChillerKey, milkChillerRelayKey, energyMeterKey };                    //write API key
    
    // Milk Chiller Relays Channel
    
    //const int Channel_ID2 =  184701;                       //Milk Chiller Relays ID 
    //String milkChillerRelays_apiKey ="5CFI74KGNN3Z3CR1";                    //write API key for Channel_ID2(Milk Chiller Relays Section)



 #endif
