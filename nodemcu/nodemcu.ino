#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h> //memasukan library HX711
//libarary rfid
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define DOUT  14 //mendefinisikan pin arduino yang terhubung dengan pin DT module HX711
#define CLK  12 //mendefinisikan pin arduino yang terhubung dengan pin SCK module HX711

HX711 scale(DOUT, CLK);
LiquidCrystal_I2C lcd(0x27, 16, 2);


//Network SSID
const char* ssid = "empty";
const char* password = "makansamba1122";

//pengenal host (server) = IP Address komputer server
const char* host = "192.168.43.118";

SoftwareSerial mySerial(0, 2); // RX, TX

unsigned long previousMillis = 0;
const long interval = 2000;

WiFiClient client;
int status1 = WL_IDLE_STATUS;
String statusKartu = "";
String mintaData = "";
String modeWeb = "";
String idRfid = "";
String hasilWeb = "";
String serial = "";
String arrDataWeb[4];
String arrDataMega[2];
int indexWeb = 0;
int indexMega = 0;
char kondisi;
String hasilParsing = "";
String jmlBeras = "";

bool stts = true;

int dataBtn1, dataBtn2, dataBtn3, angka;
float unit;
float calibration_factor = 483;
Servo servo;
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  //  mySerial.setTimeout(5000);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selamat Datang");
  delay(1000);
  //setting koneksi wifi
  WiFi.hostname("NodeMCU");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    //progress sedang mencari WiFi
    delay(500);
    Serial.print(".");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(".");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP=");
  lcd.print(WiFi.localIP());
  Serial.println("Wifi Connected");
  Serial.println("IP Address : ");
  Serial.println(WiFi.localIP());

  servo.attach(15);
  servo.write(0);

  scale.set_scale();
  scale.tare(); // auto zero / mengenolkan pembacaan berat
  long zero_factor = scale.read_average(); //membaca nilai output sensor saat tidak ada beban

  delay(1000);
}

void loop() {
  modeWeb = prosesBacaModeWeb();
  //  membaca mode dari web
  //======================mode standby============
  if (modeWeb == "1") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode = Standby");
    kondisi = 's';
    mySerial.write(kondisi);

  }
  //  ===================mode daftar==============
  else if (modeWeb == "2") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode = Daftar");
    delay(250);
    kondisi = 'd';
    mySerial.write(kondisi);
    //    pengiriman data ke web untuk daftar
    idRfid = getSerial();
    if (idRfid != "") {
      hasilWeb = getWeb(idRfid, "", "D");
      Serial.println(hasilWeb);
    }
  }
  //  ==========mode scan transaksi==============
  else if (modeWeb == "3") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode = Transaksi");
    delay(250);
    kondisi = 'b';
    mySerial.write(kondisi);
    idRfid = getSerial();
    // hasil parsing
    if (idRfid != "") {
      Serial.println(idRfid);
      for (int i = 0; i <= idRfid.length(); i++) {
        char delimiter = '#';
        if (idRfid[i] != delimiter) {
          arrDataMega[indexMega] += idRfid[i];
        } else {
          indexMega++;
        }
      }
      Serial.println(arrDataMega[0]);
      Serial.println(arrDataMega[1]);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Berat Beras = " + arrDataMega[1]);
      //      kirim Data Ke web
      hasilWeb = getWeb(arrDataMega[0], arrDataMega[1], "S");
      Serial.println(hasilWeb);

      //      parsing data dari balasan web
      for (int i = 0; i <= hasilWeb.length(); i++) {
        char delimiter2 = '#';
        if (hasilWeb[i] != delimiter2) {
          arrDataWeb[indexWeb] += hasilWeb[i];
        } else {
          indexWeb++;
        }
      }
      arrDataWeb[1].trim();
      arrDataWeb[2].trim();
      Serial.print("Data Web ");
      Serial.println(arrDataWeb[1]);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Proses Pembelian");
      lcd.setCursor(0, 1);
      lcd.print(arrDataWeb[1]);
      if (arrDataWeb[1] == "berhasil") {
        servo.write(90);
        if (arrDataMega[1] == "1") {
          while (stts != false) {
            scale.set_scale(calibration_factor);
            unit = scale.get_units();
            Serial.println(unit, 1);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Proses Pengisian");
            lcd.setCursor(0,1);
            lcd.print("pengisian =");
            lcd.print(unit);
            if (unit >= 500) {
              servo.write(0);
              delay(1500);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Beras Terisi");
              lcd.setCursor(0, 1);
              lcd.print("Silahkan Di Ambil");
              unit = 0;
              stts = false;
              delay(3000);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Tempelkan Kartu");
              delay(1000);
            } else {
              scale.set_scale(calibration_factor);
              unit = scale.get_units();
            }
            delay(250);
          }
        } 
        if (arrDataMega[1] == "2") {
          while (stts != false) {
            scale.set_scale(calibration_factor);
            unit = scale.get_units();
            Serial.println(unit, 1);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Proses Pengisian");
            lcd.setCursor(0,1);
            lcd.print("pengisian =");
            lcd.print(unit);
            if (unit >= 1000) {
              servo.write(0);
              delay(1500);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Beras Terisi");
              lcd.setCursor(0, 1);
              lcd.print("Silahkan Di Ambil");
              unit = 0;
              stts = false;
              delay(3000);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Tempelkan Kartu");
              delay(1000);
            }else {
              scale.set_scale(calibration_factor);
              unit = scale.get_units();
            }
            delay(250);
            
          }
        } 
        if (arrDataMega[1] == "3") {
          while (stts != false) {
            scale.set_scale(calibration_factor);
            unit = scale.get_units();
            Serial.println(unit, 1);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Proses Pengisian");
            lcd.setCursor(0,1);
            lcd.print("pengisian =");
            lcd.print(unit);
            if (unit >= 1500) {
              servo.write(0);
              delay(1500);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Beras Terisi");
              lcd.setCursor(0, 1);
              lcd.print("Silahkan Di Ambil");
              unit = 0;
              stts = false;
              delay(3000);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Tempelkan Kartu");
              delay(1000);
            }
            else {
              scale.set_scale(calibration_factor);
              unit = scale.get_units();
            }
            delay(250);
          }
        }

      } else if (arrDataWeb[1] == "saldoHabis") {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Saldo Habis");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Tempelkan Kartu");
      }
    }
    arrDataMega[0] = "";
    arrDataMega[1] = "";
    arrDataMega[2] = "";
    arrDataWeb[0] = "";
    arrDataWeb[1] = "";
    arrDataWeb[2] = "";
    arrDataWeb[3] = "";
    indexMega = 0;
    indexWeb = 0;
    idRfid = "";
    stts = true;
    hasilWeb = "";
    mySerial.flush();

  }

  arrDataMega[0] = "";
  arrDataMega[1] = "";
  arrDataMega[2] = "";
  serial = "";
  hasilWeb = "";
  idRfid = "";
  modeWeb = "";
  mintaData = "";
  stts = true;
  mySerial.flush();
  delay(500);
  //  menangkap balasan dari arduino mega

}

String prosesBeras(int outBeras) {
  if (unit >= outBeras) {
    servo.write(0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Beras Terisi");
    lcd.setCursor(0, 1);
    lcd.print("Silahkan Di Ambil");
    unit = 0;
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tempelkan Kartu");
    return "berhasil";
  } else {
    return "gagal";
  }


}

String getSerial() {
  if (mySerial.available() > 0) {
    serial = mySerial.readStringUntil('\n');
  }

  return serial;
}

String getWeb(String rfidId, String jumlahBeli, String statusKartu) {
  String Link;
  HTTPClient http;
  if (jumlahBeli == 0 && statusKartu == "D") {
    Link = "http://192.168.43.118/penjualan_beras/kirimkartuDaftar.php?nokartu=" + rfidId;
  } else if (jumlahBeli != 0 && statusKartu == "S") {
    Link  = "http://192.168.43.118/penjualan_beras/kirimkartu.php?nokartu=" + rfidId + "&jumlah=" + jumlahBeli ;
  }
  http.begin(Link);
  int httpCode = http.GET();
  String payload = http.getString();
  client.stop();
  http.end();

  return payload;

}

String prosesBacaModeWeb() {
  String Link;
  HTTPClient http;
  Link = "http://192.168.43.118/penjualan_beras/bacaMode.php";
  http.begin(Link);

  int httpCode = http.GET();
  String payload = http.getString();
  client.stop();
  http.end();
  return payload;
}
