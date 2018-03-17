#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
Adafruit_SSD1306 OLED(-1);
byte addresses[][6] = {"TR_001", "RC_001"};

int track_left_pin = 2;
int track_right_pin = 3;
int gun_horizon_pin = 1;
int gun_vertical_pin = 2;
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

char data_to_read[pad_data_enum] = {0,};
char data_to_send[tank_data_enum] = {0,};

typedef struct {
  char track_left;
  char track_right;
  char gun_vertical;
  char gun_horizon;
} inst_t;
inst_t inst;

void setup()   {
  Serial.begin(115200);
  memset(data_to_send, 0, sizeof(data_to_send));
  memset(data_to_read, 0, sizeof(data_to_read));
 // radio.setAutoAck(1);                    // Ensure autoACK is enabled
  //radio.enableAckPayload();               // Allow optional ack payloads
 // radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
  //radio.setPayloadSize(1);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  OLED.begin();
  OLED.clearDisplay();

  //Add stuff into the 'display buffer'
  OLED.setTextWrap(false);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE);
  OLED.display();
  radio.printDetails();


}

void loop() {

  delay(10);
  radio.stopListening();
  radio.write(&data_to_send, sizeof(data_to_send));
  delay(10);
  radio.startListening();

  if (radio.available())
  {
    while (radio.available()) {
      //Serial.print(data_to_read);
      radio.read(&data_to_read, sizeof(data_to_read));
    }
  }
  
  set_display();
  memset(data_to_send, 0, sizeof(data_to_send));
  memset(data_to_read, 0, sizeof(data_to_read));
}

void set_display()
{
    OLED.clearDisplay();
    OLED.setCursor(0, 0);
    OLED.println(data_to_read[track_left], DEC);
    OLED.setCursor(0, 10);
    OLED.println(data_to_read[track_right], DEC); 
    OLED.display(); 
  }

