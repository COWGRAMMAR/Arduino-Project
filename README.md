Hello, welcome. Here I give a little preview of this source code.

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
