//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Black Box 3.6.0 version (final) * bug free / stable *
//
// Computer system for large isolation grow units
// Working proof of concept             - Demo release  
//
// Date:        3.6.0 - release 4/20/2019
// Written by:  Steven Andrews II       - 503 lines. 
//
// Development discontinued as of (2020) 
// Published to public:                   03/15/2024
// Gen 2 board compliant                ( build 3.6+ )
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Warning / documentation : 
//
// This code is free software and open source material and not intended sale/ resale.
// All code and hardware was created by Steven Andrews II
// Not liable for any damages or injuries.
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Developer final notes :
//
// This project was created in 2019 as a demo project for a proof of concept board, the code has been tested and is in working order at time of publication.
// See read_me for additional information on settings. 
// >> No know bugs or ghosts in the program, contact me for help on issues
// See PDF for guide.
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// WARNING !!!!! : DANGER AHEAD!
//
// -- can edit, but should not under normal operation conditions... this is used by the computer to run its functions and creates initial conditions for the code to run--
// -- editing this code can cause the Arduino to crash and not run properly or at all, may damage hardware if changed!! ... ( library/software state variables )
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int HUMIDITY = 0;
int TEMPERATURE = 0;                                   // initial variables

unsigned long lastTime = millis();                     // arduino time keeping 

int seconds                 = 0;                       // cycle clock (used by all systems)
int minutes                 = 0;
int hours                   = 0;

int short light_state       = 0;                       // state machine/ starts as on

int humidifier_cycle_timer  = 0;                       // counter
int short humidifier_state  = 1;                       // 0 it will start as OFF until the clock has reached the time required for the code to trip, 1 it will start out as ON         
int humidifier_seconds      = 0;                       // counter
int humidifier_min          = 0;                       // counter

int short ozenator_state    = 0;                       // state machine
int ozenator_cycle_tick     = 2;                       // counter
int ozenator_min            = 0;                       // counter

bool door_bypass            = false;                   // time out indicator. 
bool door_state             = 0;                       // door ajar input state 
int door_timer              = 0;                       // count minutes until time out. 

int valve_chanel_selection  = 0;                       // 3 valve system - current open valve = 0 = nil 
bool valve_activation       = false;                   // toggle


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//     Settings: (changeable) 
//
//    YOU MAY CHANGE SETTINGS HERE !!
//    Important Note :
//    you can still manage to damage your hardware, its important to read the comments and make educated decisions 
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const short int   max_tmp                   = 87;       // Emergency shut off
const short int   rst_tmp                   = 80;       // Temperature at which the state will reset to on. 

volatile bool temperature_shutdown_enable   = false;    // Temperature shut down override. ( enable over heat protection )

const int         cycle                     = 12;       // Best modes for lighting - (8x16 10x24 12x24) ( if unsure use one of these )
const int         clock_reset               = 24;       // Day end time. [example:[ 8x 16 is every 8 light 8 dark (16hr day) ], [ 12x24 is 12 on 12 off ( 24hr day ) ] ]. 
volatile bool     lighting_enable           = true;     // Enable light rigging
const int         humidifier_cycle          = 6;        // Humidity cycle    ( fixed in ver 3 )  - how often in hours should the system pump humidity. 
const int         humidifier_ontime         = 6;        // Run time   ( how long should it run ) - replaces 1.8 cubic feet in 3 min. 6 min to push out old air and fully replace air. 
volatile bool     humidifier_enable         = true;     // Enable humidity system

const int         ozenator_cycle            = 2;        // Ozone cycle skip ( based off humidity run time )
const int         ozenator_ontime           = 3;        // Minutes ( You should do a calculation on how much air is pumped into the ozone system, should determine humidity run times )
const bool        ozenator_enable           = true;     // Enable ozone system 

const int         door_timeout              = 5;        // Minutes - time out for door sensor
volatile bool     door_sensor_enable        = true;     // Enable/disable the door sensor 

volatile bool     valve_enable              = false;    // Enable valve system - multi isolation FC on a single air system ( rev 3.60 )





//-------------------------------------------------------------------
// PIN OUT
//-------------------------------------------------------------------

const short int   light_pin               = 8;        // Arduino out_pin (lights)  
const short int   humidifier_pin          = 10;       // Arduino out_pin (Accessory/humidifier)
const short int   ozenator_pin            = 11;       // Arduino out_pin (ozone genartor/air pump- water purification)
const short int   door_pin                = 6;        // Arduino inout pin for door ajar sensor

const short int   valve_1pin              = 3;        // Air system valves
const short int   valve_2pin              = 4;        
const short int   valve_3pin              = 5;        




//---<<<<<<<<<<<<<<<<<<<<<<<<<<<<<DO NOT EDIT BELOW FILE, may cause code to damage hardware...>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



// DHT Temperature & Humidity Sensor set up ( used adafruit lib -- DHT22 is selected as default, and pin 2 is the digital read pin for the sensor)

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;


void setup() {

  // initialize comunication 
  Serial.begin(9600);
  
  // Initialize device pin out...
  pinMode(light_pin, OUTPUT);
  pinMode(humidifier_pin, OUTPUT);
  pinMode(ozenator_pin, OUTPUT);
  pinMode(door_pin, INPUT);

  pinMode(valve_1pin,OUTPUT);
  pinMode(valve_2pin, OUTPUT);
  pinMode(valve_3pin, OUTPUT);
  
  // run display image: 
  display.display();
  delay(2000); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000); // boot up pause



  // initialize sensor comunication: print out sensor data
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("�C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("�C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("�C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
   
}



// display drawing/output 
void dsp(){

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text

    display.setCursor(0, 0);
    display.println("T:");
    display.setCursor(30, 0);
    display.println(TEMPERATURE);
    
    display.setCursor(60, 0);
    display.println("H%:");
    display.setCursor(90, 0);
    display.println(HUMIDITY);

    display.setCursor(0,25); 
    display.println(hours);
    display.setCursor(20,25); 
    display.println(":");
    display.setCursor(30,25); 
    display.println(minutes);
    

    display.setCursor(50,25); 
    display.println(":");
    display.setCursor(60,25); 
    display.println(seconds);

    if (door_state == HIGH and humidifier_state != 2) {
      display.setTextSize(1);
     display.setCursor(0,50); 
     display.println("Door Is Ajar !");
    }
    if (humidifier_state == 2  ) {
     display.setTextSize(1);
     display.setCursor(0,50); 
     display.println("Humidifier Active!");
    }  
    display.display(); 
}


void atmocalc(){
sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("�C"));
    TEMPERATURE = (event.temperature * 1.8) + 32; // convert C to F
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    HUMIDITY = event.relative_humidity;
    Serial.println(F("%"));
  }
}


void loop() {
  box_timer();    // keep track of time 
  dsp();          // display 
  atmocalc();     // temp/humidity input
  light_cycle();  // controls the light cycle and bypass.
  humidifier();   // humidifier state machine.
  door_sensors(); // door sensor input
}



void door_sensors() {
  // read the state of the pushbutton value:
  door_state = digitalRead(door_pin);
}



void humidifier() {                                                           

  if (humidifier_enable == true){
     if (humidifier_cycle_timer >= humidifier_cycle && humidifier_state == 0 ) {
       // trigger
       humidifier_state = 1; 
       humidifier_cycle_timer = 0;  
     }
    
      if (valve_activation == true ) {
           
           if ( valve_chanel_selection == 0 ){
            digitalWrite(valve_1pin, HIGH);
            digitalWrite(valve_2pin, LOW);
            digitalWrite(valve_3pin, LOW);
           }

           if ( valve_chanel_selection == 1 ){
            digitalWrite(valve_1pin, LOW);
            digitalWrite(valve_2pin, HIGH);
            digitalWrite(valve_3pin, LOW);
           }

           if ( valve_chanel_selection == 2 ){
            digitalWrite(valve_1pin, LOW);
            digitalWrite(valve_2pin, LOW);
            digitalWrite(valve_3pin, HIGH);
           }
      }
    
    
     if (humidifier_state == 1) {                                            
         if (valve_enable == true){
          valve_activation = true;
         }
         digitalWrite(humidifier_pin, HIGH);                                 
         humidifier_state = 2;                                                
     }
    
     if (humidifier_state == 2 && humidifier_min >= humidifier_ontime){
          humidifier_min      =  0;
          humidifier_seconds  =  0;
          ozenator_state      =  0;
          ozenator_min        =  0;
          if ( valve_enable == false) {
            digitalWrite(humidifier_pin, LOW);                                 
            humidifier_state    =  0;
            ozenator_cycle_tick ++;                                            
          }
          if ( valve_enable == true) {
            valve_chanel_selection            ++;   
   
              if (valve_chanel_selection >= 3){
                   digitalWrite(valve_1pin, LOW);
                   digitalWrite(valve_2pin, LOW);
                   digitalWrite(valve_3pin, LOW);
                   digitalWrite(humidifier_pin, LOW);                           
                   valve_activation         = false;
                   ozenator_cycle_tick        ++; 
                   valve_chanel_selection   =  0;   
                   humidifier_state         =  0;   // rst humidifier & valves
                   // reset ozone clock
                   if (ozenator_min >= ozenator_ontime && ozenator_state == 1 ){
                      digitalWrite(ozenator_pin, LOW);
                      ozenator_cycle_tick = 0;                                            
                   }
                   return;
              }  
              if (valve_chanel_selection < 3){      // ( update/bug fix 3.60 )
                humidifier_state         =   1;
              }
          }
     }

     if (ozenator_enable == true) {
       if (ozenator_min < ozenator_ontime && humidifier_state >= 1 && ozenator_state == 0 && ozenator_cycle_tick >= ozenator_cycle ){ // added ozenator skip
        digitalWrite(ozenator_pin, HIGH);
        ozenator_state = 1;
       }
       if (ozenator_min >= ozenator_ontime && ozenator_state == 1 ){
        digitalWrite(ozenator_pin, LOW);
         if ( valve_enable == false) {
             ozenator_cycle_tick = 0;                                               
         }
       }
     }

  }else{
    Serial.println("Humidifier is not enabled...");
  }
} // EOF




void light_cycle(){ 

  if (lighting_enable == true){
    //------------ overheating  ---------- 
    if (temperature_shutdown_enable == true){             // temp shut off....
        if (TEMPERATURE >= max_tmp && light_state == 1 ){
            light_state = 2;                              // turn on the bypass switch 
            digitalWrite(light_pin, LOW);
        }
        if (TEMPERATURE <= rst_tmp && light_state == 2) { // turn off the bypass state 
            light_state = 0;
        }
    }   
     // door sensor interjection
      if ( door_sensor_enable == true ){
      
      if (light_state == 0 and door_state == HIGH and door_bypass == false) {
         digitalWrite(light_pin, HIGH);
         light_state = 3;
      }
      
      if (light_state == 3 and door_state == LOW){
        door_timer = 0;
        light_state = 1;
      }
      
      if (light_state == 3 and door_timer >= door_timeout){
        light_state = 1;
        door_bypass = true;
      }
      
      if (door_bypass == true and door_state == LOW){
        door_timer = 0;
        door_bypass = false;
      }
      }

              
       // -- timer input 

      if (hours < cycle && light_state == 0 ){
         digitalWrite(light_pin, HIGH);
         light_state = 1;
      }

      if (hours >= cycle && light_state == 1 ){
          digitalWrite(light_pin, LOW);
         light_state = 0;
      }

  }else{
    Serial.println("Lighting is not enabled..");
  }
    
}

// -- the system is not time accurate due to imperfections in the arduinos crystal oscillator...  
// -- the best I can do is approximate... (( change interval to increase some accuracy )) 1000 = 1 second... (( if its slow + // if its fast - ))
// update schematics for a REAL TIME CLOCK // Use RTC to get times to fire the sequences. 
 

void box_timer() {
  const short int interval = 970;  
  if (millis() - lastTime >= interval) {
    seconds ++;
    if (humidifier_state == 2){
      humidifier_seconds ++;
      
    }
    if (humidifier_state == 2 && humidifier_seconds >= 60){  
      humidifier_min ++;
      ozenator_min ++;
      humidifier_seconds = 0;
    }
    if (seconds >= 60) {
      minutes ++;
      if (light_state == 3) {
        door_timer++;
      }
      seconds = 0;
    }

    if (minutes >= 60) {
      hours++;
      humidifier_cycle_timer ++; // tracking for starting humidifier..
      minutes = 0;
    }
    
    if (hours >= clock_reset) {
      hours = 0;
      minutes = 0;
      seconds = 0;
    }
    lastTime = millis(); // reset last time fired
  }
}
