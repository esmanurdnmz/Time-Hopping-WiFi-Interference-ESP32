#include "WiFi.h"

void setup(){
  Serial.begin(115200);
  // ESP32'yi istasyon (bağlantı kuran) modunda başlatıyoruz
  WiFi.mode(WIFI_MODE_STA); 
  Serial.println();
  Serial.print("Alici ESP32'nin MAC Adresi: ");
  Serial.println(WiFi.macAddress());
}

void loop(){
  // Sürekli yazmaması için boş bırakıyoruz
}