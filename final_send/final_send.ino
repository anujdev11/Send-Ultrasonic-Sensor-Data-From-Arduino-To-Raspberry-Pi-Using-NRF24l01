#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <NewPing.h>

static char send_payload[256];

#define TRIGGER_PIN 4      //Trig pin
#define ECHO_PIN  3        //Echo pin
#define MAX_DIST 400


const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 1;
int next_payload_size = min_payload_size;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DIST);

RF24 radio(9, 10);
const int role_pin = 5;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

char receive_payload[max_payload_size + 1];

void setup()
{

  Serial.begin(115200);

  //
  // Setup and configure rf radio
  //

  radio.begin();
  // enable dynamic payloads
  radio.enableDynamicPayloads();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(5, 15);

  //
  // Open pipes to other nodes for communication
  //

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);


  //
  // Start listening
  //

  radio.startListening();
}

void loop(void)
{
  int water_level = sonar.ping_cm();
  Serial.print("Sending Data :");
  Serial.print(water_level);
  Serial.println(" cm");
  delay(100);
  String water = String(water_level);
  static char send_payload[50];
  water.toCharArray(send_payload,50);
  Serial.println(send_payload);
  //static char send_payload[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ789012";

  // First, stop listening so we can talk.
  radio.stopListening();

  // Take the time, and send it.  This will block until complete
  Serial.print(F("Now sending length "));
  Serial.println(next_payload_size);
  radio.write( send_payload, next_payload_size );


  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout
}
