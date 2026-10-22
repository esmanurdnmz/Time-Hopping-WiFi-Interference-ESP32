#include <esp_now.h>
#include <WiFi.h>

constexpr uint8_t WIFI_CHANNEL = 6; 

typedef struct struct_message {
    uint32_t seq_num;
    uint32_t timestamp_tx;
    char payload[24];
} struct_message;

struct_message myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  uint32_t rx_time = millis();
  memcpy(&myData, incomingData, sizeof(myData));
  
  // Python PyCharm betiğimizin okuyacağı formatta yazdırıyoruz
  Serial.printf("%lu,%lu,%lu,%d\n", rx_time, myData.seq_num, myData.timestamp_tx, len);
}

void setup() {
  Serial.begin(115200);
  
  Serial.println("RX_TIME,SEQ_NUM,TX_TIME,LEN");
  
  WiFi.mode(WIFI_STA);
  ESP_ERROR_CHECK(esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Baslatilamadi!");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  delay(10); 
}