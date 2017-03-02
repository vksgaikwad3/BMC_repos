#ifndef serverConfg_h
#define serverConfg_h




 /***************** Thing Speak Settings ************************************/

    // Milk Chiller Details
    #define milkChiller        0
    #define milkChillerRelay   1
    #define energyMeter        2
    
    #define milkChillerKey        "XTUFVMDZYZLA0TB6"        //api Key for Milk Chiller Channel
    #define milkChillerRelayKey   "5CFI74KGNN3Z3CR1"        //api Key for Milk Chiller Relay Channel
    #define energyMeterKey        "DIU0GB1C3XJQTOFY"        //api Key for Energy Meter Channel
    
    //const int Channel_ID1 =  176340;                       //Milk Chiller ID 
    //const char * myWriteAPIKey = "XTUFVMDZYZLA0TB6";
    
    String channel_apiKey[] ={milkChillerKey, milkChillerRelayKey, energyMeterKey };                    //write API key
    
    // Milk Chiller Relays Channel
    
    //const int Channel_ID2 =  184701;                       //Milk Chiller Relays ID 
    //String milkChillerRelays_apiKey ="5CFI74KGNN3Z3CR1";                    //write API key for Channel_ID2(Milk Chiller Relays Section)



 #endif
