#include <ESP32_LoRaWAN.h>
#include "Arduino.h"

#define INT_PIN 12
#define LEDPIN 25

// heltectest01 - OTAA
//uint8_t DevEui[] = { 0x05, 0x3a, 0x3a, 0xc3, 0x06, 0x0f, 0xe3, 0xeb };
//uint8_t AppKey[] = { 0xAE, 0x14, 0x3F, 0x10, 0xCB, 0x68, 0xF2, 0x68, 0x46, 0x45, 0xE5, 0x6E, 0xDB, 0x80, 0xC6, 0x06 };
//uint32_t license[4] = { 0x744E9957, 0x6EB80B56, 0x270EE620, 0x976396DC };

// heltectest02 - OTAA
//uint8_t DevEui[] = { 0x14, 0x2c, 0xcc, 0x59, 0x09, 0x7f, 0x3f, 0x14 };
//uint8_t AppKey[] = { 0x63, 0xEF, 0x48, 0x72, 0x5D, 0xC9, 0x64, 0x6C, 0xA2, 0x60, 0xA1, 0x39, 0xD6, 0x70, 0xE9, 0xAB };
//uint32_t license[4] = { 0xC8141DAC, 0xA678F35C, 0x87230A2A, 0x9758C2D7 };

// heltectest03 - OTAA
//uint8_t DevEui[] = { 0xf8, 0xe5, 0x9c, 0x67, 0x78, 0x03, 0x4b, 0xab };
//uint8_t AppKey[] = { 0xBF, 0xA9, 0x9F, 0x96, 0x56, 0x39, 0x01, 0xA3, 0x58, 0xC5, 0xA1, 0x1D, 0x68, 0x68, 0x07, 0x62 };
//uint32_t license[4] = { 0x3C7009A5, 0x766E1BD0, 0xF5F77CED, 0xC751ADC7 };

// heltectest04 - OTAA
uint8_t DevEui[] = { 0x49, 0x08, 0xa0, 0x25, 0xea, 0x66, 0x8b, 0xb5 };
uint8_t AppKey[] = { 0x1E, 0xCA, 0x98, 0xE3, 0x3E, 0x1B, 0x52, 0x15, 0xE6, 0x3B, 0xB8, 0x54, 0x53, 0xBD, 0xCE, 0x17 };
uint32_t license[4] = { 0xD862CDB1, 0x0AC16874, 0xB7279A0A, 0x007A7A7B };

/*license for Heltec ESP32 LoRaWan, quary your ChipID relevant license: http://resource.heltec.cn/search */
/* OTAA AppEui NOT IN USE*/
uint8_t AppEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
/* ABP para -- NOT IN USE*/
uint32_t DevAddr =  ( uint32_t )0x00000000;
uint8_t NwkSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00 };
uint8_t AppSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00 };

/*LoraWan channelsmask, default channels 8-15*/ 
uint16_t userChannelsMask[6]={ 0xFF00,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = CLASS_C;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 60*1000;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = true;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;

/* Max number of retransmissions */ 
uint8_t confirmedNbTrials = 8;

/*LoraWan debug level, select in arduino IDE tools.
* None : print basic info.
* Freq : print Tx and Rx freq, DR info.
* Freq && DIO : print Tx and Rx freq, DR, DIO0 interrupt and DIO1 interrupt info.
* Freq && DIO && PW: print Tx and Rx freq, DR, DIO0 interrupt, DIO1 interrupt, MCU sleep and MCU wake info.
*/
uint8_t debugLevel = LoRaWAN_DEBUG_LEVEL;

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

void app(uint8_t port, uint8_t data)
 {
  //lora_printf("data:%d\r\n",data);
  if ( port == 9) {
    switch(data)
    {
      // in dec, 49 is char 1 in Ascii table
      // https://www.barcodefaq.com/ascii-chart-char-set/
      case 49:
      {
        pinMode(LEDPIN,OUTPUT);
        digitalWrite(LEDPIN, HIGH);
        break;
      }
      // in dec, 48 is char 0 in Ascii table
      case 48:
      {
        pinMode(LEDPIN,OUTPUT);
        digitalWrite(LEDPIN, LOW);
        break;
      }
      default:
      {
        break;
      }
    }
  }
 }

void  downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
	lora_printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
	lora_printf("+REV DATA:");
  // Send first downlink byte to app() function to validate/run a remote command
  app(mcpsIndication->Port, mcpsIndication->Buffer[0]);

  // Print the whole payload
  for(uint8_t i=0;i<mcpsIndication->BufferSize;i++)
  {
    // print payload in hex
    lora_printf("%02X",mcpsIndication->Buffer[i]);
  }
  lora_printf("\r\n");
}

static void prepareTxFrame( uint8_t port )
{
    appDataSize = 1;
    appData[0] = digitalRead(LEDPIN);
}

void keyDown()
{
  delay(10);
  if(digitalRead(INT_PIN)==1 && IsLoRaMacNetworkJoined)
  {
    deviceState = DEVICE_STATE_SEND;
  }
}

// Add your initialization code here
void setup()
{
  if(mcuStarted==0)
  {
    LoRaWAN.displayMcuInit();
  }
  Serial.begin(115200);
  while (!Serial);
  SPI.begin(SCK,MISO,MOSI,SS);
  Mcu.init(SS,RST_LoRa,DIO0,DIO1,license);
  delay(100);
  pinMode(INT_PIN,INPUT_PULLUP);
  attachInterrupt(INT_PIN,keyDown,RISING);
  deviceState = DEVICE_STATE_INIT;

  // Get ChipID
  uint64_t chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32ChipID=%04X",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes.
  Serial.println();

  // Use the LED pin to signal transmission.
  pinMode(LEDPIN,OUTPUT);

  // Set the default state of LED pin
  digitalWrite(LEDPIN, LOW);
}

// The loop function is called in an endless loop
void loop()
{
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
      LoRaWAN.init(loraWanClass,loraWanRegion);
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      LoRaWAN.displayJoining();
      LoRaWAN.join();
      break;
    }
    case DEVICE_STATE_SEND:
    {
      LoRaWAN.displaySending();
      prepareTxFrame( appPort );
      LoRaWAN.send(loraWanClass);
      deviceState = DEVICE_STATE_CYCLE;
      break;
    }
    case DEVICE_STATE_CYCLE:
    {
      // Schedule next packet transmission
      txDutyCycleTime = appTxDutyCycle + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
      LoRaWAN.cycle(txDutyCycleTime);
      deviceState = DEVICE_STATE_SLEEP;
      break;
    }
    case DEVICE_STATE_SLEEP:
    {
      LoRaWAN.displayAck();
      LoRaWAN.sleep(loraWanClass,debugLevel);
      break;
    }
    default:
    {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
}