#include <TinyGPS.h> /* GPS Lib */
#include "../lib/RN2483/src/rn2xx3.h" /* RN2483 Lib */
/* for standby ability */
#include <RTClock.h>
#include <libmaple/pwr.h>
#include <libmaple/scb.h>

/*
STM32 port mappings
UART1 -> Serial  -> DEBUG
UART2 -> Serial1 -> GPS
UART3 -> Serial2 -> LoRaWAN
*/

String APPEUI = "70B3D57EF0003AA2";
String APPKEY = "23D8583ACF5A8B628540A53F0A18876D";

#define DEBUG 1
#define ENABLE_GPS 1
#define ENABLE_LORAWAN 1

const int debug_baud = 9600;
const int gps_baud = 9600;

bool LoRaWAN_connection = false;

float flat, flon;
unsigned long age;
bool new_gps_data = false;

RTClock rt(RTCSEL_LSE); /* RTC register for STM32 */

TinyGPS gps;
rn2xx3 LoRaWAN(Serial2);

static void noop() {}; /* For RTC? */

void STM32_sleep(uint8_t i){
  if(DEBUG) Serial.println("[SLEEP] Setting Alarm");
  rt.createAlarm(&noop, rt.getTime() + i); /* set alarm for interval i */

  // Clear PDDS and LPDS bits
  PWR_BASE->CR &= ~PWR_CR_PDDS;
  PWR_BASE->CR &= ~PWR_CR_LPDS;

  // Clear previous wakeup register
  PWR_BASE->CR |= PWR_CR_CWUF;

  // Set standby
  PWR_BASE->CR |= PWR_CR_PDDS;

  PWR_BASE->CR |= PWR_CR_LPDS;
  SCB_BASE->SCR |= SCB_SCR_SLEEPDEEP;
  if(DEBUG) Serial.println("[SLEEP] Entering standby");
  asm("    wfi");
}

void setup(){
  /* give time to attach debug monitor */
  delay(5000);

  if(DEBUG) Serial.begin(debug_baud);
  /* Initialise uart communication to GPS module */
  if(ENABLE_GPS){
    if(DEBUG) Serial.print("[GPS] Initialising UART...");
    Serial1.begin(gps_baud);
    if(DEBUG) Serial.print("Done\n");
  }
  /* Initialise uart communication to LoRaWAN module */
  if(ENABLE_LORAWAN){
    if(DEBUG) Serial.print("[LoRaWAN] Initialising UART...");
    Serial2.begin(9600); //serial port to radio
    Serial2.flush();
    LoRaWAN.autobaud();
    if(DEBUG) Serial.print("Done\n");
    /* Join LoRaWAN */
    if(DEBUG) Serial.print("[LoRaWAN] Connecting...");
    LoRaWAN_connection = LoRaWAN.initOTAA(APPEUI, APPKEY);
    if(DEBUG){
      if(LoRaWAN_connection){
        Serial.print("Success!\n");
      }else{
        Serial.print("Failed\n");
      }
    }
  }
}

void loop(){
  if(ENABLE_GPS){
    if(DEBUG) Serial.print("[GPS] Reading UART...\n");
    if(DEBUG) delay(1000); /* Wait for Serial to clear for debug */
    while (!Serial1.available()){}
    String out = "";
    while (Serial1.available()){
      uint8_t c = Serial1.read();
      out += char(c);
      if (gps.encode(c))
      {
        new_gps_data = true;
      }
    }
    if(DEBUG) Serial.println(out);
    if(DEBUG) delay(1000); /* Wait for Serial to clear for debug */
    if(new_gps_data){
      if(DEBUG) Serial.print("[GPS] New data\n");
      gps.f_get_position(&flat, &flon, &age);
      if(DEBUG){
        Serial.print("LAT: ");
        Serial.print(flat);
        Serial.print("| LON: ");
        Serial.print(flon);
        Serial.print("| AGE: ");
        Serial.print(age);
      }
    }
  }
  if(ENABLE_LORAWAN){
    if(!LoRaWAN_connection){
      if(DEBUG) Serial.print("[LoRaWAN] Connecting...");
      LoRaWAN_connection = LoRaWAN.init();
      if(DEBUG){
        if(LoRaWAN_connection){
          Serial.print("Success!\n");
        }else{
          Serial.print("Failed\n");
        }
      }
    }else{
      LoRaWAN.tx("!");
    }
    if(DEBUG) Serial.println(LoRaWAN.deveui());
  }
  delay(5000);
  /* STM32 sleep */
  //STM32_sleep(1); /* engange STM32 stanby for 5 seconds */
}
