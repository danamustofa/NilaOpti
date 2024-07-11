#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_sleep.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xA5, 0x10, 0x98};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
    float pH;
    unsigned long timestamp; // added timestamp field
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// Constants for pH measurement
const float calibration_value = 22.31 - 0.7;
const int analogPin = 34; // ESP32 analog pin

// Variables for pH measurement
int buffer_arr[10];
float ph_act;

// NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS? "Delivery Success" : "Delivery Fail");
}

void measure_pHValue() {
  // Read analog values and sort them
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(analogPin);
    delay(30);
  }
  sort(buffer_arr, 10); // Corrected the sort function call
  
  // Calculate average of middle 6 values
  unsigned long int avgval = 0;
  for (int i = 2; i < 8; i++) {
    avgval += buffer_arr[i];
  }
  float volt = (float)avgval * 3.3 / 4096 / 6; // Adjust for ESP32's 12-bit ADC and 3.3V reference
  ph_act = -5.70 * volt + calibration_value;
    
  // Set values to send
  myData.id = 1;
  myData.pH = ph_act; // Send pH value
  myData.timestamp = millis(); // add timestamp
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}

void sort(int *a, int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = i + 1; j < size; j++) {
      if (a[i] > a[j]) {
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
      }
    }
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init()!= ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo)!= ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Set up NTP Client
  timeClient.begin();
  timeClient.setTimeOffset(25200); // Offset for Indonesia time zone (WIB)
}

void loop() {
  // Get current time from NTP Server
  timeClient.update();

  // Membaca waktu saat ini dalam format jam:menit:detik
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds();

  // Check if it's time to send pH data (every 5 minutes)
  if (timeClient.getMinutes() % 5 == 0 && timeClient.getSeconds() == 0) {
    measure_pHValue();

    // Mendefinisikan isi value Sender (ID dan Suhu)
    myData.id = 1;
    myData.pH = ph_act;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

      if (result == ESP_OK) {
      Serial.print("Sent with success at ");
      Serial.print(hour);
      Serial.print(":");
      Serial.print(minute);
      Serial.print(":");
      Serial.print(second);
      Serial.print(" - pH Value: ");
      Serial.print(ph_act);
      Serial.println("");
    } else {
      Serial.println("Error sending the data");
    }
    // Enter deep sleep for 5 minutes
    Serial.println("Entering deep sleep for 5 minutes...");
    esp_sleep_enable_timer_wakeup(300000000); // 5 minutes in microseconds
    esp_deep_sleep_start();
  }
}