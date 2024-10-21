#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NewPing.h>

// Pin untuk sensor DS18B20, LED, dan sensor jarak
#define ONE_WIRE_BUS 6
#define LED_RED 2
#define LED_GREEN 3
#define TRIG_PIN 4
#define ECHO_PIN 5
#define RELAY_PIN 7

// Setup objek OneWire dan DallasTemperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Setup objek LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Setup sensor jarak
NewPing sonar(TRIG_PIN, ECHO_PIN);

bool systemState = false;      // Awalnya sistem mati
bool messageDisplayed = false;  // Status tampilan

// Setup relay
unsigned long lastMillis1 = 0, lastMillis2 = 0;
float distance = 0;

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  return pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;
}

void setup() {
  // Setup LCD
  lcd.init();
  lcd.backlight();

  // Set pin LED sebagai OUTPUT
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Set pin sensor sebagai input
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Set relay sebagai OUTPUT
  pinMode(RELAY_PIN, OUTPUT);

  // Setup sensor DS18B20
  Serial.begin(9600);
  sensors.begin();
}

void loop() {
  // Pembacaan jarak
  float jarak = 3;

  if (millis() - lastMillis1 >= 100) {
    lastMillis1 = millis();
    distance = getDistance(); // Fungsi untuk mendapatkan jarak dari sensor
  }

  if (millis() - lastMillis2 >= 500) {
    lastMillis2 = millis();
    digitalWrite(RELAY_PIN, distance < jarak ? LOW : HIGH);
  }

  // Jika jarak kurang dari 0.5 cm, toggle sistem
  if (distance < jarak) {
    if (systemState) {
      // Mematikan sistem
      systemState = false;  
      messageDisplayed = false;  // Reset tampilan
      delay(1000);  // Debounce delay
    } else {
      // Menghidupkan sistem
      systemState = true;  
      text();              // Tampilkan pesan "SYSTEM ON"
      LoadingScreen();     // Tampilkan layar loading sekali
      delay(1000);        // Delay untuk loading
    }
  }

  // Jika sistem menyala, jalankan fungsi pembacaan suhu
  if (systemState) {
    system();  // Jalankan fungsi system() untuk membaca suhu
  } else {
    // Jika sistem mati, matikan LED dan tampilkan pesan "SYSTEM OFF"
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SYSTEM OFF");
    delay(1000); // Debounce delay
  }
}

// Fungsi untuk membaca dan menampilkan suhu
void system() {
  // Minta pembacaan suhu dari sensor
  sensors.requestTemperatures();
  sensors.setResolution(11);
  
  float temperatureC = sensors.getTempCByIndex(0);  // Suhu dalam Celsius
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;  // Konversi ke Fahrenheit

  // Tampilkan suhu pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SUHU: ");
  lcd.print(temperatureC);
  lcd.print((char)223);  // Karakter derajat
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("SUHU: ");
  lcd.print(temperatureF);
  lcd.print((char)223);
  lcd.print("F");

  // Kontrol LED berdasarkan suhu
  if (temperatureC > 40.0 || temperatureC < 0.0) {
    digitalWrite(LED_RED, HIGH);    // Nyalakan LED merah
    digitalWrite(LED_GREEN, LOW);   // Matikan LED hijau
  } else {
    digitalWrite(LED_RED, LOW);     // Matikan LED merah
    digitalWrite(LED_GREEN, HIGH);  // Nyalakan LED hijau
  }

  delay(300);  // Tunggu sebelum pembacaan berikutnya
}

// Fungsi untuk menampilkan pesan "SYSTEM ON"
void text() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SYSTEM ON");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature");
  lcd.setCursor(0, 1);
  lcd.print("Digital");
  delay(1000);
}

// Fungsi untuk menampilkan layar loading
void LoadingScreen() {
  unsigned long startTime = millis();
  String loadingText = "Loading";
  String dots = "";

  while (millis() - startTime < 1000) {
    for (int i = 0; i < 4; i++) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(loadingText);
      lcd.print(dots);

      // Update titik-titik
      dots += ".";
      if (dots.length() > 3) {
        dots = "";  // Reset titik-titik setelah 3 titik
      }

      delay(200);  // Delay untuk animasi
    }
  }
}
