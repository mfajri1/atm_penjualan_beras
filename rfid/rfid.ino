#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_PIN          53         // Configurable, see typical pin layout above
#define BTN1 23
#define BTN2 25
#define BTN3 27
#define BUZZER 7

MFRC522 mfrc522(SS_PIN, RST_PIN);
SoftwareSerial serialMega(11, 10);
String dataRfid = "";
String IDTAG = "";
String serial = "";
String resultWeb = "";
String pecahData;
String arrDataWeb[4];
int indexWeb = 0;
int j = 0;
int dataBtn1, dataBtn2, dataBtn3, jumlah; 
String kirim = "";
void setup() {
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);   // Initialize serial communications with the PC
  serialMega.begin(9600);
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);

  Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
  Serial.print(F("Using key (for A and B):"));
  Serial.println();
  Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));

}

void loop() {
  //  ambil data dari nodemcu
  delay(150);
  dataBtn1 = digitalRead(BTN1);
  dataBtn2 = digitalRead(BTN2);
  dataBtn3 = digitalRead(BTN3);
  if(dataBtn1 == 0){
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
    jumlah = 1;
    Serial.println(jumlah);
  }
  if(dataBtn2 == 0){
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
    jumlah = 2;
    Serial.println(jumlah);
  }
  if(dataBtn3 == 0){
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
    jumlah = 3;
    Serial.println(jumlah);
  }
  
  String dataNode = "";
  while (serialMega.available() > 0) {
    dataNode += char(serialMega.read());
  }
  dataNode.trim();
//  Serial.println(dataNode);

// ============ jika daftar =================
  if (dataNode == "d") {
    jumlah =0;
    //    ambil data rfid
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      IDTAG += mfrc522.uid.uidByte[i];
    }
    Serial.println(IDTAG);
    //    kirim data balasan ke nodemcu
    serialMega.print(IDTAG);
    
// ============ jika beli =================
  } else if (dataNode == "b") {
    //    ambil data rfid
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      IDTAG += mfrc522.uid.uidByte[i];
    }
    Serial.println(IDTAG);
    //    kirim data balasan ke nodemcu

    kirim = IDTAG + '#' + jumlah + '#'; 
    serialMega.print(kirim);
    jumlah=0;

    

  }

  //  reset data lagi
  kirim = "";
  indexWeb = 0;
  resultWeb = "";
  dataNode = "";
  serialMega.flush();
  IDTAG = "";
  delay(500);
}

String getSerial() {
 if(serialMega.available()){
 serial = serialMega.readStringUntil('\n');
 }
 return serial;
}
