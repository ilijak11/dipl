#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <SPI.h>
//#include <EDB.h>
#include <EEPROM.h>

#include "messages.h"
#include "commands.h"
#include "modes.h"
#include "blocks.h"
#include "db.h"
#include "LED.h"

#define RFID_SS_PIN 10
#define RDIF_RST_PIN 9

// void writer(unsigned long address, byte data)
// {
//   EEPROM.write(address, data);
// }

// byte reader(unsigned long address)
// {
//   return EEPROM.read(address);
// }

struct DBHeader{
  byte count;
  unsigned int start_address;
};

DBHeader db_header;

struct UserRecord{
  char id[16];
  byte finger_id;
};

byte mode = LOCK_MODE;

MFRC522 mfrc522(RFID_SS_PIN, RDIF_RST_PIN);
MFRC522::MIFARE_Key key;

//EDB db(&writer, &reader);
SoftwareSerial softSerial(2,3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&softSerial);


void setup() {
  init_leds();
  Serial.begin(9600);
  while (!Serial);
  //initialize spi bus for communication with rfid sensor
  SPI.begin();
  mfrc522.PCD_Init();
  finger.begin(57600);
  finger.verifyPassword();
  //db.create(0, TABLE_SIZE, (unsigned int)sizeof(UserRecord));
  read_header();
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

void lock_procedure(){

  Serial.println(WAITING_FOR_CARD);
  if(!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())) return;
  Serial.println(CARD_PRESENT);

}

void register_procedure() {
  digitalWrite(REGISTER_MODE, HIGH);
  Serial.println(ENTERED_REGISTER_MODE);

  while (!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()));

  Serial.println(CARD_PRESENT);

  char buff[17];
  byte len;
  MFRC522::StatusCode status;

  len = Serial.readBytesUntil('~', (char*)buff, 17);

  //for (byte i = len; i < 32; i++) buff[i] = ' ';
  // for (byte block = NAME_BLOCK; block <= NAME_BLOCK + 1; block++) {
  //   status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  //   if (status != MFRC522::STATUS_OK) {
  //     Serial.print(CARD_AUTH_FAIL);
  //     return;
  //   }

  //   status = mfrc522.MIFARE_Write(block, buff + (block - NAME_BLOCK) * BLOCK_SIZE_BYTES, BLOCK_SIZE_BYTES);
  //   if (status != MFRC522::STATUS_OK) {
  //     Serial.print(CARD_AUTH_FAIL);
  //     return;
  //   } else Serial.println(CARD_WRITE_SUCCESS);
  // }

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, ID_BLOCK, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(CARD_AUTH_FAIL);
    return;
  } else Serial.println(CARD_AUTH_SUCCESS);

  status = mfrc522.MIFARE_Write(ID_BLOCK, buff, BLOCK_SIZE_BYTES);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(CARD_AUTH_FAIL);
    return;
  } else Serial.println(CARD_WRITE_SUCCESS);

  //  write token to card


  // write record to db
  //uint8_t finger_id = (uint8_t)db.count() + 1;

  // UserRecord user;
  // strncpy(user.id, (const char *) buff, 16);
  // user.finger_id = finger_id;
  // EDB_Status res = db.appendRec(EDB_REC user);
  // if(res != EDB_OK){
  //   Serial.println(DB_WRITE_FAIL);
  //   return;
  // }
  // else Serial.println(DB_WRITE_SUCCESS);
  uint8_t finger_id = (uint8_t)db_header.count + 1;
  add_user_record(buff, finger_id);
  Serial.println(DB_WRITE_SUCCESS);


  // enroll finger

  Serial.println(ENROLLING_FINGER);
  enroll_fingerprint(finger_id);

  delay(1000);
  mode = LOCK_MODE;
  digitalWrite(REGISTER_MODE, LOW);
}

boolean enroll_fingerprint(uint8_t finger_id){
  int p = -1;
  Serial.println(WAITING_FOR_FINGER);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println(IMAGE_TAKEN);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(NO_FINGER);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(COMMUNICATION_ERROR);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println(IMAGE_ERROR);
      break;
    default:
      Serial.println(UNKNOWN_ERROR);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(IMAGE_CONVERTED);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(IMAGE_MESSY);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(COMMUNICATION_ERROR);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println(NO_FEATURES);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println(NO_FEATURES);
      return p;
    default:
      Serial.println(UNKNOWN_ERROR);
      return p;
  }

  Serial.println(REMOVE_FINGER);
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  Serial.println(PLACE_SAME_FINGER);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println(IMAGE_TAKEN);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(NO_FINGER);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(COMMUNICATION_ERROR);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println(IMAGE_ERROR);
      break;
    default:
      Serial.println(UNKNOWN_ERROR);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(IMAGE_CONVERTED);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println(IMAGE_MESSY);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println(COMMUNICATION_ERROR);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println(NO_FEATURES);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println(NO_FEATURES);
      return p;
    default:
      Serial.println(UNKNOWN_ERROR);
      return p;
  }

  // OK converted!
  Serial.println(CREATING_MODEL);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println(MATCH);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(COMMUNICATION_ERROR);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println(NO_MATCH);
    return p;
  } else {
    Serial.println(UNKNOWN_ERROR);
    return p;
  }

  Serial.println(STORING_MODEL);
  p = finger.storeModel(finger_id);
  if (p == FINGERPRINT_OK) {
    Serial.println(STORED);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println(COMMUNICATION_ERROR);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println(BAD_LOCATION);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println(FLASH_ERROR);
    return p;
  } else {
    Serial.println(UNKNOWN_ERROR);
    return p;
  }

  return true;
}

void read_header(){
  EEPROM.get(DB_HEADER_ADDRESS, db_header);
}

void write_header(){
  EEPROM.put(DB_HEADER_ADDRESS, db_header);
}

void add_user_record(const char* id, byte finger_id){
  UserRecord user;
  strncpy(user.id, id, 16);
  user.finger_id = finger_id;
  read_header();
  EEPROM.put(calculate_address(db_header.count), user);
  db_header.count += 1;
  write_header();
}

unsigned int calculate_address(byte index){
  return db_header.start_address + index * (unsigned int)sizeof(UserRecord);
}

void prepare_rfid_key() {
  for (uint8_t i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

void init_leds(){
  pinMode(REGISTER_MODE, OUTPUT);
  digitalWrite(REGISTER_MODE, LOW);
}

