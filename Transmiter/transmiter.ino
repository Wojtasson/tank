#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
Adafruit_SSD1306 OLED(-1);

byte addresses[][6] = {"TR_001", "RC_001"};

int track_left_pin = 2;
int track_right_pin = 3;
int gun_horizon_pin = 0;
int gun_vertical_pin = 1;
int fire_button_pin = 7;
int laser_button_pin = 8;

RF24 radio(7, 8); // CE, CSN

typedef enum {
  pad_status = 0,
  track_left = 1,
  track_right = 2,
  gun_horizon = 3,
  gun_vertical = 4,
  laser_tag = 5,
  pad_voltage = 6,
  lights = 7,
  fire = 8,
  pad_data_enum
} pad_data_e;

typedef enum {
  tank_status = 0,
  tank_voltage = 1,
  tank_data_enum
} tank_data_e;

char data_to_send[pad_data_enum] = {0,};
char data_to_read[tank_data_enum] = {0,};

typedef struct {
  char track_left;
  char track_right;
  char gun_vertical;
  char gun_horizon;
} inst_t;
inst_t inst;

void setup()   {
 // Serial.begin(115200);
  memset(data_to_send, 0, sizeof(data_to_send));
  memset(data_to_read, 0, sizeof(data_to_read));
 //radio.setAutoAck(1);                    // Ensure autoACK is enabled
  //radio.enableAckPayload();               // Allow optional ack payloads
  //radio.setRetries(10,15);                 // Smallest time between retries, max no. of retries
  //radio.setPayloadSize(1);   
  // Here we are sending 1-byte payloads to test the call-response speed
    radio.begin();
  radio.openWritingPipe(addresses[1]); // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002
  //radio.setPALevel(RF24_PA_MAX);

  //radio.stopListening();
  //Serial.print("\n\RECEIVER");
  OLED.begin();
  OLED.clearDisplay();

  //Add stuff into the 'display buffer'
  OLED.setTextWrap(false);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE);
  OLED.display();
  //radio.printDetails();



}

void loop() {

delay(10);
  radio.stopListening();
  read_track_values();
  read_gun_values();
  radio.write(&data_to_send, sizeof(data_to_send));
  delay(10);
  radio.startListening();

  if (radio.available())
  {
    while (radio.available()) {
    radio.read(&data_to_read, sizeof(data_to_read));
 }
 }
  
  set_display();
  memset(data_to_send, 0, sizeof(data_to_send));
  memset(data_to_read, 0, sizeof(data_to_read));
}



void read_track_values()
{

   float x = map(analogRead(track_left_pin),0,1020,-100,100); 
   x = x * -1; 
   float y = map(analogRead(track_right_pin),0,1020,-100,100);  

  float v = (100-abs(x)) * (y/100) + y;
  float w = (100-abs(y)) * (x/100) + x;

   char l = ((v+w)/2)+1;
   char r = ((v-w)/2)+1;

    if (l > 100){l = 100;}
    if (r > 100){r = 100;}
    
    if (l <= -99){l = -100;}
    if (r <= -99){r = -100;}
       
    inst.track_left = l;
    inst.track_right = r;  
   data_to_send[track_left] = inst.track_left;
   data_to_send[track_right] = inst.track_right;
}

void read_gun_values()
{
   inst.gun_horizon = map(analogRead(gun_horizon_pin), 0, 1020, 0, 180);
   inst.gun_vertical = map(analogRead(gun_vertical_pin), 0, 1020, 0, 180);
   
   data_to_send[gun_horizon] = inst.gun_horizon;
   data_to_send[gun_vertical] = inst.gun_vertical;
}

void set_display()
{
    OLED.clearDisplay();
    OLED.setCursor(0, 0); OLED.print(inst.track_left, DEC);
    OLED.setCursor(100, 0); OLED.print(inst.track_right, DEC); 
    OLED.setCursor(0, 16); OLED.println(inst.gun_horizon, DEC);
    OLED.setCursor(100, 16); OLED.println(inst.gun_vertical, DEC);   
    OLED.display(); 
  }
