#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
Adafruit_SSD1306 OLED(-1);
byte addresses[][6] = {"TR_001", "RC_001"};

int track_left_enable = 9;
int track_left_in1 = 3;
int track_left_in2 = 4;

int track_right_enable = 10;
int track_right_in1 = 5;
int track_right_in2 = 6;


//L298N track_left(track_left_enable,track_left_in1,track_left_in2,-1,-1,-1);
//L298N track_right(track_right_enableA,track_right_in1,track_right_in2,-1,-1,-1);

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
  int track_left_pwm;
  int track_right_pwm;
} inst_t;
inst_t inst;

void setup()   {
  Serial.begin(115200);
  memset(data_to_send, 0, sizeof(data_to_send));
  memset(data_to_read, 0, sizeof(data_to_read));
  pinMode(track_left_enable, OUTPUT);
  pinMode(track_left_in1, OUTPUT);
  pinMode(track_left_in2, OUTPUT);
  pinMode(track_right_enable, OUTPUT);
  pinMode(track_right_in1, OUTPUT);
  pinMode(track_right_in2, OUTPUT);
  // radio.setAutoAck(1);                    // Ensure autoACK is enabled
  //radio.enableAckPayload();               // Allow optional ack payloads
  // radio.setRetries(0,15);                 // Smallest time between retries, max no. of retries
  //radio.setPayloadSize(1);                // Here we are sending 1-byte payloads to test the call-response speed
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  radio.setPALevel(RF24_PA_MAX);
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

 inst.track_left = data_to_read[track_left];
  inst.track_right = data_to_read[track_right];
  calculate_right_track((int)inst.track_right);
  calculate_left_track((int)inst.track_left);

  set_display();
  memset(data_to_send, 0, sizeof(data_to_send));
  memset(data_to_read, 0, sizeof(data_to_read));
}

void set_display()
{
  OLED.clearDisplay();
  OLED.setCursor(0, 0); OLED.println(inst.track_left, DEC);
  OLED.setCursor(0, 10); OLED.println(inst.track_right, DEC);
  OLED.setCursor(64, 0); OLED.println(inst.track_left_pwm , DEC);
  OLED.setCursor(64, 10); OLED.println(inst.track_right_pwm, DEC);
  OLED.display();
}

void calculate_left_track(int track_value)
{
  int value;
  if (track_value > 1)
  {
    digitalWrite(track_left_in1, LOW);
    digitalWrite(track_left_in2, HIGH);
    value = track_value;
  }
  else if (track_value  < -1)
  {
    digitalWrite(track_left_in1, HIGH);
    digitalWrite(track_left_in2, LOW);
    value = track_value * -1;
  }

  inst.track_left_pwm = (int)(map(value, 0, 100, 0, 255));

  analogWrite(track_left_enable, inst.track_left_pwm);
}

void calculate_right_track(int track_value)
{
  int value = 0;
  if (track_value  > 1)
  {
    digitalWrite(track_right_in1, LOW);
    digitalWrite(track_right_in2, HIGH);
    value = track_value;
  }
  else if (inst.track_right  < -1)
  {
    digitalWrite(track_right_in1, HIGH);
    digitalWrite(track_right_in2, LOW);
    value = track_value * -1;
  }

  inst.track_right_pwm = (int)(map(value, 0, 100, 0, 255));
  analogWrite(track_right_enable, inst.track_right_pwm);
}

