#include <SPI.h>
#include <LoRa.h>
#include <avr/sleep.h>//Çalışmadığı zaman düşük güç harcanması için uyuma fonksiyonları
#include <avr/power.h>

const int BUTTON_PIN = 3;

void sleepNow() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUp, LOW);
  sleep_cpu();
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
}

void wakeUp() { }

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  LoRa.setPins(10, 9, 2); 
  LoRa.begin(433E6);      
  LoRa.setSpreadingFactor(12);   //DEĞİŞTİRİLEBİLİR
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.setTxPower(17);   //DEĞİŞTİRİLEBİLİR
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    sleepNow();
  }

  // Paket gönder 
  LoRa.beginPacket();
  LoRa.print("SOS");
  LoRa.endPacket();

  // Jitter ve kısa bekleme süresi
  delay(150 + random(0, 80));
}

