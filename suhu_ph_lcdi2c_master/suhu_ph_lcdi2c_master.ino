#include <esp_now.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  float pH;
  float temperature;
}struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;

// Create an array with all the structures
struct_message boardsStruct[2] = {board1, board2};

// Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address, columns, rows

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t* mac_addr, const uint8_t* incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].pH = myData.pH;
  boardsStruct[myData.id-1].temperature = myData.temperature;

  Serial.printf("pH value: %f \n", boardsStruct[myData.id-1].pH);
  Serial.printf("Temp value: %f \n", boardsStruct[myData.id-1].temperature);
  Serial.println();

  // Display the values on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Board ");
  lcd.print(myData.id);
  lcd.print(":");
  lcd.setCursor(1, 0);
  lcd.print("pH: ");
  lcd.print(boardsStruct[myData.id-1].pH, 2);
  lcd.setCursor(1, 1);
  lcd.print("Temp: ");
  lcd.print(boardsStruct[myData.id-1].temperature, 2);
}

void setup() {
  // Initialize the LCD display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
}

void loop() {
  delay(10000);  
}