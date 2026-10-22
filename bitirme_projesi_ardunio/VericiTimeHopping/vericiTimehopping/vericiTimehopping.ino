#include <esp_now.h>
#include <WiFi.h>

// Alıcı ESP32'nin MAC Adresini buraya giriniz
uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
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

// TEMEL DEĞİŞİKLİK BURADA: Sabit süre yerine, her döngüde değişecek bir değişken tanımlıyoruz.
long currentDelay = 1000; 

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ESP_ERROR_CHECK(esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));

  // Rastgele sayı üreticisini (Random Number Generator) analog pindeki gürültü ile besliyoruz 
  // ki gerçekten her seferinde öngörülemez (true random) sayılar üretsin.
  randomSeed(analogRead(0));

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
  if ((millis() - lastTime) > currentDelay) {
    myData.seq_num = sequence_counter++;
    myData.timestamp_tx = millis();
    
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
    
    if (result == ESP_OK) {
      Serial.printf("Paket %lu gonderildi. (Bekleme Suresi: %ld ms)\n", myData.seq_num, currentDelay);
    }
    
    lastTime = millis();
    
    // TIME HOPPING ALGORİTMASI: 
    // Bir sonraki gönderim için 1000 ms üzerine -50 ile +50 arası rastgele bir sapma (jitter) ekle.
    long jitter = random(-50, 51); 
    currentDelay = 1000 + jitter;
  }
}