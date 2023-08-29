#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <SPI.h>

#include "messages.h"
#include "commands.h"
#include "modes.h"
#include "blocks.h"

#define RFID_SS_PIN 10
#define RDIF_RST_PIN 9

byte mode = LOCK_MODE;
MFRC522 mfrc522(RFID_SS_PIN, RDIF_RST_PIN);
MFRC522::MIFARE_Key key;


void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Serial.begin(9600);
  while (!Serial)
    ;
  //initialize spi bus for communication with rfid sensor
  SPI.begin();
  mfrc522.PCD_Init();
  delay(10);
  prepare_rfid_key();
}

void loop() {
  receive_command();
  switch (mode) {
    case REGISTER_MODE:
      register_procedure();
      break;
    case LOCK_MODE:
      break;
  }
}

void receive_command() {
  if (!Serial.available()) return;
  byte command = Serial.readString().toInt();
  switch (command) {
    case REGISTER_MODE_COMMAND:
      {
        mode = REGISTER_MODE;
        break;
      }
    default:
      {
        Serial.println("Not valid command");
      }
  }
}

void register_procedure() {
  digitalWrite(2, HIGH);
  Serial.println(ENTERED_REGISTER_MODE);

  while (!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()));

  Serial.println(CARD_PRESENT);

  char buff[32];
  byte len;
  MFRC522::StatusCode status;

  len = Serial.readBytesUntil('~', (char*)buff, 32);

  for (byte i = len; i < 32; i++) buff[i] = ' ';
  for (byte block = NAME_BLOCK; block <= NAME_BLOCK + 1; block++) {
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(CARD_AUTH_FAIL);
      return;
    }

    status = mfrc522.MIFARE_Write(block, buff + (block - NAME_BLOCK) * BLOCK_SIZE_BYTES, BLOCK_SIZE_BYTES);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(CARD_AUTH_FAIL);
      return;
    } else Serial.println(CARD_WRITE_SUCCESS);
  }

  delay(1000);
  mode = LOCK_MODE;
  digitalWrite(2, LOW);
}

void prepare_rfid_key() {
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}
