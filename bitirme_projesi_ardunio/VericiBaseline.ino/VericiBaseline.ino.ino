#include <esp_now.h>
#include <WiFi.h>

// DİKKAT: Buraya MacBulucu sketch'inden bulduğun adresi yazmalısın.
// Örnek: 30:AE:A4:07:0D:64 için -> {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64}
uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

// Raspberry Pi iperf3 AP'si ile aynı kanalda (Örn: 6) olmalı
constexpr uint8_t WIFI_CHANNEL = 6; 

typedef struct struct_message {
    uint32_t seq_num;
    uint32_t timestamp_tx;
    char payload[24]; 
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

uint32_t sequence_counter = 0;
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000; // 1. Hafta: Sabit 1000 ms periyot (Baseline)

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Gönderim callback'i (Seri portu yormamak için içi boş bırakılabilir)
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ESP_ERROR_CHECK(esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Baslatilamadi!");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = WIFI_CHANNEL;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Alici (Peer) eklenemedi");
    return;
  }
  strcpy(myData.payload, "DUMMY_SENSOR_DATA_12345");
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    myData.seq_num = sequence_counter++;
    myData.timestamp_tx = millis();
    
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
    
    if (result == ESP_OK) {
      Serial.printf("Paket %lu gonderildi. TX: %lu\n", myData.seq_num, myData.timestamp_tx);
    } else {
      Serial.printf("Paket %lu HATA!\n", myData.seq_num);
    }
    lastTime = millis();
  }
}