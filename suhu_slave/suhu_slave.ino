#include <OneWire.h>
#include <DallasTemperature.h>
#include <esp_now.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <esp_sleep.h>

// Pin yang digunakan untuk berkomunikasi dengan sensor DS18B20
#define ONE_WIRE_BUS 4

// Set up oneWire instance untuk berkomunikasi dengan perangkat OneWire
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference ke Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// MAC Address Receiver ESP32
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xA5, 0x10, 0x98};

// Mendefinisikan value id dan suhu bertipe float
typedef struct struct_message {
    int id; // ID harus beda untuk mengidentifikasi Sender
    float temperature; // Suhu dalam derajat Celsius
} struct_message;

// Membuat struct dengan nama myData
struct_message myData;

// Mendefinisikan peer/hubungan pada interface
esp_now_peer_info_t peerInfo;

// Membuat fungsi onDataSent dimana akan memberikan status data yang telah dikirim
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS? "Delivery Success" : "Delivery Fail");
}

// NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  // Mendefinisikan Baudrate
  Serial.begin(115200);
  
  // Mulai library Dallas Temperature
  sensors.begin();

  // Mengatur ESP32 sebagai WI-FI Statiun
  WiFi.mode(WIFI_STA);

  // Mendefiniskan ESP-NOW
  if (esp_now_init()!= ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Mendefinisikan Send CB 
  // Mendapatkan status dari paket yg dikirim
  esp_now_register_send_cb(OnDataSent);
  
  // Mendefinisikan peer/hubungan antara ESP32
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Menambahkan peer/hubungan antara ESP32     
  if (esp_now_add_peer(&peerInfo)!= ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Mengatur NTP Client
  timeClient.begin();
  timeClient.setTimeOffset(25200); // Offset waktu untuk zona waktu Indonesia (WIB)
}

void loop() {
  // Mengambil waktu saat ini dari NTP Server
  timeClient.update();

  // Membaca waktu saat ini dalam format jam:menit:detik
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds();

  // Cek apakah waktu saat ini adalah kelipatan 5 menit
  if (minute % 5 == 0 && second == 0) {
    // Meminta sensor untuk mengambil pembacaan suhu
    sensors.requestTemperatures();

    // Membaca suhu dalam derajat Celsius
    float temperatureC = sensors.getTempCByIndex(0);

    // Mendefinisikan isi value Sender (ID dan Suhu)
    myData.id = 2;
    myData.temperature = temperatureC;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
    if (result == ESP_OK) {
      Serial.print("Sent with success at ");
      Serial.print(hour);
      Serial.print(":");
      Serial.print(minute);
      Serial.print(":");
      Serial.print(second);
      Serial.print(" - Suhu: ");
      Serial.print(temperatureC);
      Serial.println(" °C");
    } else {
      Serial.println("Error sending the data");
    }

    // Masuk ke dalam deep sleep untuk 5 menit
    Serial.println("Entering deep sleep for 5 minutes...");
    esp_sleep_enable_timer_wakeup(300000000); // 5 menit dalam microseconds
    esp_deep_sleep_start();
  }
}