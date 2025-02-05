#include "MCP41HVX1.h"
#include "SPI.h"

/////////////////////////////////////////////////////// digitpot setup
#define FORWARD true
#define REVERSE false
#define MAX_WIPER_VALUE 255  //maximum wiper value

#define CS_PIN_1    26 //CC 1   //chip select  for SPI communications
#define WLAT_PIN_1  24          // Latch pin
#define SHDN_PIN_1  22          // resistor network enable

#define CS_PIN_2    32 //CC 1 
#define WLAT_PIN_2  30
#define SHDN_PIN_2  28

#define CS_PIN_3    35 //CV 1
#define WLAT_PIN_3  37
#define SHDN_PIN_3  39

#define CS_PIN_4    45 //CV 1
#define WLAT_PIN_4  43
#define SHDN_PIN_4  41

#define CS_PIN_5    23 //CC 2
#define WLAT_PIN_5  25
#define SHDN_PIN_5  27

#define CS_PIN_6    33 //CC 2
#define WLAT_PIN_6  31
#define SHDN_PIN_6  29
 
#define CS_PIN_7    47 //CV 2
#define WLAT_PIN_7  49
#define SHDN_PIN_7  40

#define CS_PIN_8    34 //CV 2
#define WLAT_PIN_8  36
#define SHDN_PIN_8  38


#define set1 110 //CC 1 
#define set2 120 //CC 1 
#define set3 150 //CV 1
#define set4 100 //CV 1
#define set5 50 //CC 2
#define set6 50 //CC 2
#define set7 255 //CV 2
#define set8 255 //CV 2

     


MCP41HVX1 Digipot1(CS_PIN_1, SHDN_PIN_1, WLAT_PIN_1);     // Create an instance of the MCP41HVX1 class for controlling the digipot
MCP41HVX1 Digipot2(CS_PIN_2, SHDN_PIN_2, WLAT_PIN_2);
MCP41HVX1 Digipot3(CS_PIN_3, SHDN_PIN_3, WLAT_PIN_3);
MCP41HVX1 Digipot4(CS_PIN_4, SHDN_PIN_4, WLAT_PIN_4);
MCP41HVX1 Digipot5(CS_PIN_5, SHDN_PIN_5, WLAT_PIN_5);
MCP41HVX1 Digipot6(CS_PIN_6, SHDN_PIN_6, WLAT_PIN_6);
MCP41HVX1 Digipot7(CS_PIN_7, SHDN_PIN_7, WLAT_PIN_7);
MCP41HVX1 Digipot8(CS_PIN_8, SHDN_PIN_8, WLAT_PIN_8);
 
void setup ()
{
    Serial.begin( 9600);
    
    
}

void loop ()
{   
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();

    Digipot1.WiperSetPosition(set1);
    Serial.print( "Digipot1 = ");
    Serial.println( Digipot1.WiperGetPosition());   
    delay( 100);

    Digipot2.WiperSetPosition(set2);
    Serial.print("Digipot2 = ");
    Serial.println(Digipot2.WiperGetPosition());
    delay(100);

    Digipot3.WiperSetPosition(set3);
    Serial.print("Digipot3 = ");
    Serial.println(Digipot3.WiperGetPosition());
    delay(100);

    Digipot4.WiperSetPosition(set4);
    Serial.print("Digipot4 = ");
    Serial.println(Digipot4.WiperGetPosition());
    delay(100);

    Digipot5.WiperSetPosition(set5);
    Serial.print("Digipot5 = ");
    Serial.println(Digipot5.WiperGetPosition());
    delay(100);

    Digipot6.WiperSetPosition(set6);
    Serial.print("Digipot6 = ");
    Serial.println(Digipot6.WiperGetPosition());
    delay(100);

    Digipot7.WiperSetPosition(set7);
    Serial.print("Digipot7 = ");
    Serial.println(Digipot7.WiperGetPosition());
    delay(100);

    Digipot8.WiperSetPosition(set8);
    Serial.print("Digipot8 = ");
    Serial.println(Digipot8.WiperGetPosition());
    delay(1000);

    
    
    
    
     
    
}
