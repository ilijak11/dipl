#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <EEPROM.h>

#include "messages.h"
#include "commands.h"
#include "modes.h"
#include "blocks.h"
#include "db.h"
#include "LED.h"
#include "sound.h"
#include "config.h"
#include "operations.h"

DBHeader db_header;
byte mode = LOCK_MODE;

MFRC522 mfrc522(RFID_SS_PIN, RDIF_RST_PIN);
MFRC522::MIFARE_Key key;

SoftwareSerial softSerial(2,3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&softSerial);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  //initialize spi bus for communication with rfid sensor
  SPI.begin();
  mfrc522.PCD_Init();
  finger.begin(57600);
  finger.verifyPassword();
  read_header();
  delay(10);
  prepare_rfid_key();
  randomSeed(analogRead(A0));
}

void loop() {
  receive_command();
  switch (mode) {
    case REGISTER_MODE:
      register_procedure();
      break;
    case LOCK_MODE:
      lock_procedure();
      break;
    case READER_MODE:
      print_card_data_procedure();
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
    case READER_MODE_COMMAND:
        mode = READER_MODE;
        break;
    default:
      {
        Serial.println("Not valid command");
      }
  }
}

void error_procedure(){
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  led_control(SUCC_1_2_OFF);
  led_control(ERR_ON);
  delay(20);
  sound_control(BEEP_ERROR);
  delay(1000);
  led_control(ERR_OFF);
}

void lock_procedure(){

  if(!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())) return;
  //Serial.println(CARD_PRESENT);
  Serial.println("Card present");

  char card_user_id[16];
  char card_user_tag[16];
  char card_user_token[16];
  char card_user_mask[16];
  char res[16];
  byte user_id;
  byte operation;

  byte buff[18];
  byte len = 18;

  // read user_id from card
  if(read_block(ID_BLOCK, buff, &len) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }
  strncpy(card_user_id, buff, USER_TOKEN_LEN);
  user_id = card_user_id[0];
  operation = card_user_id[1];

  // read user_tag from card
  if(read_block(TAG_BLOCK, buff, &len) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }
  strncpy(card_user_tag, buff, USER_TOKEN_LEN);

  // read user_token from card
  if(read_block(TOKEN_BLOCK, buff, &len) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }
  strncpy(card_user_token, buff, USER_TOKEN_LEN);

  // read user_mask from card
  if(read_block(MASK_BLOCK, buff, &len) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }
  strncpy(card_user_mask, buff, USER_TOKEN_LEN);

  execute_operation(card_user_token, card_user_mask, res, operation);

  // test
  // Serial.print("id: ");
  // Serial.println(card_user_id);
  // Serial.print("op: ");
  // Serial.println(operation);

  // Serial.print("token: ");
  // for (uint8_t i = 0; i < 16; i++)
  // {
  //   Serial.print(card_user_token[i]);
  // }
  // Serial.println();

  // Serial.print("mask: ");
  // for (uint8_t i = 0; i < 16; i++)
  // {
  //   Serial.print(card_user_mask[i]);
  // }
  // Serial.println();

  // Serial.print("res: ");
  // for (uint8_t i = 0; i < 16; i++)
  // {
  //   Serial.print(res[i]);
  // }
  // Serial.println();

  // -------------------------------------------------

  UserRecord user;
  if(read_user_record(user_id - 1, &user) != DB_SUCCESS){
    Serial.println(DB_READ_FAIL);
    error_procedure();
    return;
  } else Serial.println(DB_READ_SUCCESS);

  // test
  // print_user_record(user);
  // -------------------------------------------------

  // compare tokens
  if(!(strncmp(user.tag, card_user_tag, USER_TOKEN_LEN) == 0) && strncmp(user.token, res, USER_TOKEN_LEN) == 0){
    Serial.println("Tokens dont match!");
    error_procedure();
    return;
  }

  led_control(SUCC_1_ON);
  sound_control(BEEP_1);

  // get fingerprint id
  getFingerprint();
  // compare fingerptins

  if(!(finger.fingerID == user.finger_id && finger.confidence >= FINGERPRINT_CONF_TRESH)){
    Serial.println("Fingerprints dont match");
    error_procedure();
    return;
  }

  led_control(SUCC_2_ON);
  sound_control(BEEP_2);

  // modify token
  write_random(card_user_token);
  write_random(card_user_mask);
  operation = get_operation();

  execute_operation(card_user_token, card_user_mask, res, operation);

  // write new token to card and db
  //  wait for card
  Serial.println("Waiting for card:");
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  while(!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())) delay(10);
  
  card_user_id[1] = operation;
  if(write_block(ID_BLOCK, card_user_id, BLOCK_SIZE_BYTES) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }
  Serial.println("Wrote id");

  if(write_block(TOKEN_BLOCK, card_user_token, BLOCK_SIZE_BYTES) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }
  Serial.println("Wrote new token");

  if(write_block(MASK_BLOCK, card_user_mask, BLOCK_SIZE_BYTES) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }
  Serial.println("Wrote new mask");

  // update user record
  if(update_user_record(user.id, user.tag, res, user.finger_id) != DB_SUCCESS){
    Serial.println(DB_WRITE_FAIL);
    error_procedure();
    return;
  } else Serial.println(DB_WRITE_SUCCESS);
  Serial.println("Updated user record");

  // unlock procedure
  delay(2000);
  
  delay(500);
  led_control(SUCC_1_2_OFF);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void register_procedure() {

  led_control(REGISTER_MODE_ON);

  Serial.println(ENTERED_REGISTER_MODE);
  Serial.println(WAITING_FOR_CARD);

  while (!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()));

  Serial.println(CARD_PRESENT);

  char tag[17]; 
  byte len;

  len = Serial.readBytesUntil('~', (char*)tag, 17);

  char token[16];
  char mask[16];
  char res[16];

  write_random(token);
  write_random(mask);
  byte operation = get_operation();
  execute_operation(token, mask, res, operation);

  // write record to db
  read_header();
  uint8_t finger_id = (uint8_t)db_header.count + 1;
  if(add_user_record(finger_id, tag, res, finger_id) != DB_SUCCESS){
    Serial.println(DB_WRITE_FAIL);
    error_procedure();
    return;
  } 
  Serial.println(DB_WRITE_SUCCESS);

  // write id block
  byte finger_id_buff[] = {finger_id, operation, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  if(write_block(ID_BLOCK, finger_id_buff, BLOCK_SIZE_BYTES) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }
  // write tag block
  if(write_block(TAG_BLOCK, tag, BLOCK_SIZE_BYTES) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  }

  // write token block
  if(write_block(TOKEN_BLOCK, token, BLOCK_SIZE_BYTES) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  } 

  // write mask block
  if(write_block(MASK_BLOCK, mask, BLOCK_SIZE_BYTES) != MFRC522::STATUS_OK){
    error_procedure();
    return;
  } 

  // enroll finger
  Serial.println(ENROLLING_FINGER);
  enroll_fingerprint(finger_id);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1000);
  mode = LOCK_MODE;
  led_control(REGISTER_MODE_OFF);
}

void print_card_data_procedure(){
  Serial.println("Reading card data...");
  Serial.println("Waiting for card...");
  while (!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()));

  byte buff[18];
  byte len = 18;

  if(read_block(ID_BLOCK, buff, &len) != MFRC522::STATUS_OK){
    error_procedure();
    mode = LOCK_MODE;
    return;
  }
  Serial.print("ID: ");
  for(byte i = 0; i < BLOCK_SIZE_BYTES; i++){
    Serial.write(buff[i]);
  }
  Serial.println();

  if(read_block(TAG_BLOCK, buff, &len) != MFRC522::STATUS_OK){
    error_procedure();
    mode = LOCK_MODE;
    return;
  }
  Serial.print("TAG: ");
  for(byte i = 0; i < BLOCK_SIZE_BYTES; i++){
    Serial.write(buff[i]);
  }
  Serial.println();

  if(read_block(TOKEN_BLOCK, buff, &len) != MFRC522::STATUS_OK){
    error_procedure();
    mode = LOCK_MODE;
    return;
  }
  Serial.print("TOKEN: ");
  for(byte i = 0; i < BLOCK_SIZE_BYTES; i++){
    Serial.write(buff[i]);
  }
  Serial.println();

  if(read_block(MASK_BLOCK, buff, &len) != MFRC522::STATUS_OK){
    error_procedure();
    mode = LOCK_MODE;
    return;
  }
  Serial.print("MASK: ");
  for(byte i = 0; i < BLOCK_SIZE_BYTES; i++){
    Serial.write(buff[i]);
  }
  Serial.println();


  mode = LOCK_MODE;
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
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

uint8_t add_user_record(byte user_id, const char* tag, const char* token,  byte finger_id){
  read_header();
  if(db_header.count == DB_MAX_USER_RECORDS) return DB_FAIL;
  UserRecord user;
  strncpy(user.tag, tag, USER_TOKEN_LEN);
  strncpy(user.token, token, USER_TOKEN_LEN);
  user.finger_id = finger_id;
  user.id = user_id;
  user.used = DB_FIELD_USED;
  EEPROM.put(calculate_address(db_header.count), user);
  db_header.count += 1;
  write_header();
  return DB_SUCCESS;
}

uint8_t update_user_record(byte user_id, const char* tag, const char* token, byte finger_id){
  UserRecord user;
  strncpy(user.tag, tag, USER_TOKEN_LEN);
  strncpy(user.token, token, USER_TOKEN_LEN);
  user.finger_id = finger_id;
  user.id = user_id;
  user.used = DB_FIELD_USED;
  EEPROM.put(calculate_address(user_id - 1), user);
  return DB_SUCCESS;
}

uint8_t read_user_record(byte user_id, UserRecord* user){
  read_header();
  if(db_header.count <= user_id) return DB_FAIL;
  EEPROM.get(calculate_address(user_id), *user);
  return DB_SUCCESS;
}

void print_user_record(UserRecord user){
  Serial.println("User record: ");
  Serial.print(" id: ");
  Serial.print(user.id);
  Serial.print(" tag: ");
  for (uint8_t i = 0; i < 16; i++)
  {
    Serial.write(user.tag[i]);
  }
  Serial.print(" token: ");
  for (uint8_t i = 0; i < 16; i++)
  {
    Serial.write(user.token[i]);
  }
  Serial.print(" finger_id: ");
  Serial.print(user.finger_id);
  Serial.print(" used: ");
  Serial.println(user.used);
}

unsigned int calculate_address(byte index){
  return db_header.start_address + index * (unsigned int)sizeof(UserRecord);
}

void prepare_rfid_key() {
  for (uint8_t i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

MFRC522::StatusCode write_block(byte block, byte* data, byte size){

  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println(CARD_AUTH_FAIL);
    return status;
  } else Serial.println(CARD_AUTH_SUCCESS);

  status = mfrc522.MIFARE_Write(block, data, size);
  if (status != MFRC522::STATUS_OK) {
    Serial.println(CARD_WRITE_FAIL);
    return status;
  } else Serial.println(CARD_WRITE_SUCCESS);

  return status;
}

MFRC522::StatusCode read_block(byte block, byte* buff, byte* size){

  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.println(CARD_AUTH_FAIL);
    return status;
  } else Serial.println(CARD_AUTH_SUCCESS);

  status = mfrc522.MIFARE_Read(block, buff, size);
  if (status != MFRC522::STATUS_OK) {
    Serial.println(CARD_READ_FAIL);
    return status;
  } else Serial.println(CARD_READ_SUCCESS);

  return status;
}

void led_control(byte command){
  switch (command) {
    case SUCC_1_ON: analogWrite(BLUE_LED_1, LED_ON); break;
    case SUCC_2_ON: analogWrite(BLUE_LED_2, LED_ON); break;
    case SUCC_1_OFF: analogWrite(BLUE_LED_1, LED_OFF); break;
    case SUCC_2_OFF: analogWrite(BLUE_LED_2, LED_OFF); break;
    case SUCC_1_2_OFF: analogWrite(BLUE_LED_1, LED_OFF); analogWrite(BLUE_LED_2, LED_OFF); break;
    case REGISTER_MODE_ON: analogWrite(ORANGE_LED, LED_ON); break;
    case REGISTER_MODE_OFF: analogWrite(ORANGE_LED, LED_OFF); break;
    case ERR_ON: analogWrite(RED_LED, LED_ON); break;
    case ERR_OFF: analogWrite(RED_LED, LED_OFF); break;
  }
}

void sound_control(byte command){
  switch(command){
    case BEEP_2: 
      for(byte i = 0; i < 2; i++){
        analogWrite(SOUND, SOUND_ON);
        delay(SOUND_TIME);
        analogWrite(SOUND, SOUND_OFF);
        if(i != 1) delay(SOUND_INTERVAL);
      }
      break;
    case BEEP_1:
      analogWrite(SOUND, SOUND_ON);
      delay(SOUND_TIME);
      analogWrite(SOUND, SOUND_OFF);
      break;
    case BEEP_ERROR:
      analogWrite(SOUND, SOUND_ON);
      delay(SOUND_TIME_ERROR);
      analogWrite(SOUND, SOUND_OFF);
  }
}

uint8_t getFingerprint() {
  int p = -1;
  Serial.println("Waiting for finger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

void write_random(byte* buff){
  for(byte i = 0; i < USER_TOKEN_LEN; i++){
    buff[i] = byte(random(0,256));
  }
}

byte get_operation(){
  return byte(random(0,3));
}

void execute_operation(byte* buff1, byte* buff2, byte* res, byte op){
  for(byte i = 0; i < USER_TOKEN_LEN; i++){
    switch (op) {
      case XOR: res[i] = buff1[i] ^ buff2[i]; break;
      case AND: res[i] = buff1[i] & buff2[i]; break;
      case OR:  res[i] = buff1[i] | buff2[i]; break;
    }
  }
}
