#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <HTTPClient.h>
#include <WiFi.h>

#define WIFI_SCAN_INTERVAL          120000000
#define BLE_SCAN_INTERVAL           10000000 
#define BLE_SCAN_TIME               1

const char* ssid     = "ESP32_TRCKD";
const char* password = "IHatePasswords";      
  
String restServer = "10.33.3.27";
int restPort = 8000;

const char *ID        = "Beacon_2";  

int status = WL_IDLE_STATUS;  
WiFiClient  wifiClient;
HTTPClient http;

BLEScan* pBLEScan;

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    String bleDataString;
    String bleMac;
    String bleRssi;
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      bleMac = advertisedDevice.getAddress().toString().c_str();
      bleRssi = advertisedDevice.getRSSI();
      bleDataString = ("{\"id\":\"" + bleMac + "\",\"rssi\":" + bleRssi + ",\"beacon\":\"" + ID + "\"}");

      String ble_post_uri = "http://" + restServer + ":" + restPort + "/collector/ble";
      http.begin(wifiClient, ble_post_uri);
      http.addHeader("Content-Type", "application/json");
      http.POST(bleDataString);
      http.end();
    }
};

void setup()
{
  setCpuFrequencyMhz(240);
  Serial.begin(115200);
  Serial2.setRxBufferSize(4096);
  Serial2.begin(115200, SERIAL_8N1, 17, 16);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (status != WL_CONNECTED) {
      status = WiFi.status();
      delay(500);
      Serial.print(status);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup the BLE device
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(50);
  pBLEScan->setWindow(49);  // less or equal setInterval value
}

unsigned long long lastWiFiScan = 0;
String wifiDataString;
String wifiMac;
String wifiRssi;

unsigned long long lastBLEScan = 0;

byte channelArray[14] = {0};
byte channelCounter;

void loop() 
{  
  if ((esp_timer_get_time() - lastWiFiScan > WIFI_SCAN_INTERVAL) || (lastWiFiScan == 0)) {
    lastWiFiScan = esp_timer_get_time();
    
    //request SSID from REST Server

    Serial.println("Requesting SSID...");

    String ssid_uri = "http://" + restServer + ":" + restPort + "/ssid";
    http.begin(wifiClient, ssid_uri);
    int resp_code = http.GET();
    Serial.print("Response code: ");
    Serial.println(resp_code);

    String SSID = http.getString();
    Serial.print("SSID: ");
    Serial.println(SSID);
    
    http.end();

    if (resp_code != 200) {
      delay(2000);
      lastWiFiScan = 0;
    }
    else {    
      Serial.println("scan start");
      int n = WiFi.scanNetworks();
      Serial.println("scan done");
      channelCounter = 0;
      memset(channelArray,0,14);
      if (n == 0) {
          Serial.println("no networks found");
      } else {
            Serial.print(n);
            Serial.println(" networks found");
            for (int i = 0; i < n; ++i) {
              if (WiFi.SSID(i) == SSID) {
                  // Print SSID and RSSI for each network found that matches the SSID
                  Serial.print(i + 1);
                  Serial.print(": ");
                  Serial.print(WiFi.SSID(i));
                  Serial.print(" (");
                  Serial.print(WiFi.RSSI(i));
                  Serial.print(") ");
                  Serial.print("Channel: ");
                  Serial.println(WiFi.channel(i));
                  bool alreadyFound = false;
                  for (int j = 0; j < 14; j++){
                    if (WiFi.channel(i) == channelArray[j]) {
                      alreadyFound = true;  
                    }
                  }
                  if (!alreadyFound) {
                    channelArray[channelCounter] = WiFi.channel(i);
                    channelCounter++;
                  }
                  delay(10);
              }
            }
      }
      Serial.println("");
      // send channelArray to sniffer
      Serial2.write(channelArray,14);
    }
  }

  if ((esp_timer_get_time() - lastBLEScan > BLE_SCAN_INTERVAL) || (lastBLEScan == 0)) {
    lastBLEScan = esp_timer_get_time();  
    BLEScanResults foundDevices = pBLEScan->start(BLE_SCAN_TIME, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    Serial.println("Scan done!");
    pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  }

  if (Serial2.available()) {
    char c = Serial2.read();  //gets one byte from serial buffer
    if (c == '\n') {  //looks for end of data packet marker 
      // Serial2.read(); //gets rid of following \r 
      int delimiter = wifiDataString.indexOf(",");
      wifiRssi = wifiDataString.substring(0,delimiter);
      wifiMac = wifiDataString.substring(delimiter+1);
      wifiDataString = ("{\"id\":\"" + wifiMac + "\",\"rssi\":" + wifiRssi + ",\"beacon\":\"" + ID + "\"}");
      // Serial.println(wifiDataString);
      String wifi_post_uri = "http://" + restServer + ":" + restPort + "/collector/wifi";
      http.begin(wifiClient, wifi_post_uri);
      http.addHeader("Content-Type", "application/json");
      http.POST(wifiDataString);
      http.end();
              
      wifiDataString=""; //clears variable for new input      
     }  
    else {     
      wifiDataString += c; //makes the string dataString
    }
  }
  
}
