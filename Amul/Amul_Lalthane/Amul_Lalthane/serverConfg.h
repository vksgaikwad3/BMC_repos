#ifndef serverConfg_h
#define serverConfg_h




 /***************** Thing Speak Settings ************************************/

    // Milk Chiller Details
    #define milkChiller               0
    #define milkChillerRelay          1
    #define energyMeter               2
    
    
    #define milkChillerKey         "T6HD7OY1WFVSD8I3"        //api Key for Milk Chiller Channel
    #define milkChillerRelayKey    "UCKCNH6358JFJ0FW"        //api Key for Milk Chiller Relay Channel
    #define energyMeterKey         ""                       // Not in Use
    
    
    String channel_apiKey[] ={milkChillerKey, milkChillerRelayKey, energyMeterKey };                    //write API key
    
    // Milk Chiller Relays Channel
    
    //const int Channel_ID2 =  184701;                       //Milk Chiller Relays ID 
    //String milkChillerRelays_apiKey ="5CFI74KGNN3Z3CR1";                    //write API key for Channel_ID2(Milk Chiller Relays Section)



 #endif
