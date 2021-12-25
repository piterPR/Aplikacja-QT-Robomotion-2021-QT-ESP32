#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
//SPI
#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_SS   (5)
#define PN532_MISO (19)
// #define LF          0x0A 

//IR Sensors
#define GATE_1
//#define GATES_2 

#ifdef GATE_1
#define SENSORPIN_START (36)
#elif defined (GATES_2)
#define SENSORPIN_START (36)
#define SENSORPIN_STOP  (27)
#endif

bool marker = false;
unsigned long actualTime = 0;
unsigned long lastTime = 0;
unsigned long interval = 3000;

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
const uint8_t ledPin = LED_BUILTIN;
int flag = 0;
char newuid[16] = {};

/* ---- Gate init prototype ---- */
float startTime, stopTime;
void Gate_init();


void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10); // producent delay
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  nfc.begin();
  
  
  uint32_t versiondata;
  do{
     versiondata = nfc.getFirmwareVersion();
  
  }while(versiondata);
  

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
  lastTime = millis();
  
  /* ---- Gate init ---- */
  Gate_init();

}

void loop(void) {


}

void Gate_init(){
  pinMode(SENSORPIN_START, INPUT);
  digitalWrite(SENSORPIN_START, HIGH);

  #ifdef GATES_2
  pinMode(SENSORPIN_STOP, INPUT);
  digitalWrite(SENSORPIN_STOP, HIGH);
  #endif
}


int nfc_read(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  Serial.println("Proszę przyłożyć kartę :) ");
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 5000); 

  
  
  Serial.println("GEH");
  Serial.println(success);
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");   
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA6 };
      //uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    
       // Proba uzyskania dostepu do Bloku #4 pamieci
      
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
      
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);
    
        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          Serial.print("UID:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
      
          // Wait a bit before reading the card again
          //delay(1000);
        }//
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    
    if (uidLength == 7)
    {
      // Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
    
      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");
    
        // Wait a bit before reading the card again
        //delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
    return 0;
  }
  else{ //If succes == 0
   return 1;
  } 
}

void nfcWrite(char newuid[]){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // UID
  uint8_t uidLength;                        //Dlugosc UID (4 albo 7 w zaleznosci od typu karty) 
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success)
  {
    Serial.println();
    Serial.println("Znaleziono znacznik ISO14443A");
    
    // wyswietlenie dlugosci unikatowego identyfikatora
    Serial.print("  Dlugosc: ");
    Serial.print(uidLength, DEC);
    Serial.println("B");
    
    // wyswietlenie unikatowego identyfikatora
    Serial.print("  UID: ");
    nfc.PrintHex(uid, uidLength);
 
    // wyswietlenie typu znacznika
    Serial.print("  TYP: ");
    if (uidLength == 4)
    {
      Serial.println("Mifare Classic");
      Serial.println("");
 
      Serial.println("Proba autoryzacji za pomoca klucza A");
      
      // Klucz
      // Defaultowy klucz A 
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
     // uint8_t keya[6] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA6 };
 
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
 
      if (success)
      {
        Serial.println("Uzyskano dostep do bloku #4 (oraz sektora 1)");
        
        // Zapisujemy dane do bloku 4
        success = nfc.mifareclassic_WriteDataBlock (4, (uint8_t*)newuid);
        
        if (success)
        {
          Serial.println("Zapisano dane do bloku #4");          
        } else
        {
          Serial.println("Blad podczas zapisu danych bloku #4");          
        }
       
        // Nowy Sector Trailer
        uint8_t sectortrailer[16] = {
          0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA6,  // Klucz A
          0xFF, 0x07, 0x80, 0x69,              // Bity dostepu
          0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB6   // Klucz B
        };        
 
        // Zapisujemy Sector Trailer do bloku 7
        success = nfc.mifareclassic_WriteDataBlock (7, sectortrailer);
 
        if (success)
        {
          Serial.println("Zapisano nowe klucze i bity dostepu sektora 1 (#4 - #7)");          
        } else
        {
          Serial.println("Blad podczas zapisu nowych kluczy i bitow dostepu do sektora 1 (#4 - #7)");          
        }
        
      } else
      {
        Serial.println("Nie mozna uzyskac dostepu do bloku #4 (oraz sektora 1)");  
      }
    }
  }
 
  delay(2000);
}

void readGate(){

  uint8_t timerState = 0;
  bool toContinue = true;
  uint16_t sensorState = 0;
  //Serial.println("Jestem przed while");
  while(toContinue){

    sensorState = analogRead(SENSORPIN_START);  
    //Serial.println("Jestem w while");

    switch (timerState){
      
      case 0: // Waiting for starting pass the gate
        Serial.println("Oczekiwanie na start zawodnika (Przejazd przez bramkę startową)");
        if (sensorState <= 512){
          Serial.println("Przejazd rozpoczęty/Odliczam czas.");
          timerState = 1;
          digitalWrite(ledPin, HIGH);
          startTime = millis();
        }else{
          timerState = 0;
        }
        break; 
      case 1: // Start timer 
        if (sensorState <= 512 && (millis() - startTime) >= 3000){  
          stopTime = (millis() - startTime );
          timerState = 2 ;          
        }else{
          timerState = 1;
        }
        break;
      case 2: // Send info 
        //Serial.println("Jestem w case 2 while");    
        Serial.print("Czas przejazdu: ");
        Serial.println(stopTime);
        timerState = 0;
        toContinue = false;
        break;
    }

  }

}

#ifdef GATES_2
void read2Gate(){

  uint8_t timerState = 0;
  bool toContinue = true;
  uint16_t sensorState = 0;
  Serial.println("Jestem przed while");
  while(toContinue){

    sensorState = analogRead(SENSORPIN_START);  
    //Serial.println("Jestem w while");

    switch (timerState){
      
      case 0: // Waiting for starting pass the gate
        Serial.println("Oczekiwanie na start zawodnika (Przejazd przez bramkę startową)");
        if (sensorState <= 512){
          Serial.println("Przejazd rozpoczęty/Odliczam czas.");
          timerState = 1;
          digitalWrite(ledPin, HIGH);
          // Serial.println(sensorState);
          startTime = millis();
        }else{
          timerState = 0;
        }
        break; 
      case 1: // Start timer 
        if (digitalRead(SENSORPIN_STOP) == LOW && (millis() - startTime) >= 2000){  
          stopTime = (millis() - startTime );
          timerState = 2 ;          
        }else{
          timerState = 1;
        }
        break;
      case 2: // Send info 
        //Serial.println("Jestem w case 2 while");    
        Serial.print("Czas przejazdu: ");
        Serial.println(stopTime);
        timerState = 0;
        toContinue = false;
        break;
    }
 
  }

}
#endif


void fillUid(char newuid[]){
  bool to_continue = true;
   
  int rlen;
  while(to_continue){
    if (Serial.available() > 0) {
      rlen = Serial.readBytesUntil('x',newuid,16);
      if(rlen < 16){
        Serial.println("Wpisane UID posiada mniej niz 13 znaki");
        break;
      }
      else if(rlen == 16){
        to_continue = false;
        Serial.println("OK");
        break;
      }
      else{
        Serial.println("Wpisane UID posiada wi�cej niz 32 znaki");
        break;
      }
  

      }
  } 
  Serial.println("otrzymalem");
  for(int i=0;i<=16;i++){
    Serial.println(newuid[i]);
  }    

}

void checknewUID(char newuid[]){
  Serial.println("Sprawdzam nowy id");
  for(int i=0;i<16;i++){
    Serial.println(newuid[i]);
  }
}

void serialEvent() {
  while (Serial.available()) {
    char state = (char)Serial.read();
    switch(state){
      case '0':
        digitalWrite(ledPin, 1);
        break;
      case '1':
        digitalWrite(ledPin, 0);
        break;
      case '2':
        if (nfc_read() == 1){
          Serial.println("nfcTimeout");
          break;
        }else    
          break;
      case '3':
        break;
      case '4':
        Serial.println("jestem w case 4");
        fillUid(newuid);
        //checknewUID(newuid);
        nfcWrite(newuid);
        break;
        Serial.println("wyszedlem z case 4");       
      case '5':
        Serial.println("Odczyt z jednej bramki");
        readGate();
        break;
      #ifdef GATES_2  
      case '6':
        Serial.println("Odczyt z dwóch bramek");
        read2Gate();
        break;
      #endif
    }       
    Serial.println("Potwierdzam odbior. Status = " + String(state));
  }
}
