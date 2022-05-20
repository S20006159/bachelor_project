#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#define WIFI_CHANNEL_SWITCH_INTERVAL  500000 // time in microseconds
#define WIFI_CHANNEL_MAX               14

uint8_t level = 0, channel = 1;

static wifi_country_t wifi_country = {.cc="GB", .schan = 1, .nchan = 13};

typedef struct {
  unsigned frame_ctrl:32;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  unsigned sequence_ctrl:16;
  uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0];
} wifi_ieee80211_packet_t;

static esp_err_t event_handler(void *ctx, system_event_t *event);
static void wifi_sniffer_init(void);
static void wifi_sniffer_set_channel(uint8_t channel);
static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);

esp_err_t event_handler(void *ctx, system_event_t *event)
{
  return ESP_OK;
}

void wifi_sniffer_init(void)
{
  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_country(&wifi_country) ); /* set country for channel range [1, 13] */
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
  ESP_ERROR_CHECK( esp_wifi_start() );
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void wifi_sniffer_set_channel(uint8_t channel)
{
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{
  if (type == WIFI_PKT_MISC) {
    return;
  }
  else {
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
    
    if (hdr->addr2[0] == 0xAC) {
      Serial.printf("%02d,%02x:%02x:%02x:%02x:%02x:%02x\n",
        //ppkt->rx_ctrl.channel,
        ppkt->rx_ctrl.rssi,
        /* ADDR1 */
        //hdr->addr1[0],hdr->addr1[1],hdr->addr1[2],
        //hdr->addr1[3],hdr->addr1[4],hdr->addr1[5],
        /* ADDR2 */
        hdr->addr2[0],hdr->addr2[1],hdr->addr2[2],
        hdr->addr2[3],hdr->addr2[4],hdr->addr2[5]
        /* ADDR3 */
        //hdr->addr3[0],hdr->addr3[1],hdr->addr3[2],
        //hdr->addr3[3],hdr->addr3[4],hdr->addr3[5]
      );   
    }
    Serial2.printf("%02d,%02x:%02x:%02x:%02x:%02x:%02x\n",
      //ppkt->rx_ctrl.channel,
      ppkt->rx_ctrl.rssi,
      /* ADDR1 */
      //hdr->addr1[0],hdr->addr1[1],hdr->addr1[2],
      //hdr->addr1[3],hdr->addr1[4],hdr->addr1[5],
      /* ADDR2 */
      hdr->addr2[0],hdr->addr2[1],hdr->addr2[2],
      hdr->addr2[3],hdr->addr2[4],hdr->addr2[5]
      /* ADDR3 */
      //hdr->addr3[0],hdr->addr3[1],hdr->addr3[2],
      //hdr->addr3[3],hdr->addr3[4],hdr->addr3[5]
    );
  }

}

void setup() {
  setCpuFrequencyMhz(240);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 17, 16);
  delay(10);
  wifi_sniffer_init();
  Serial.println("Setup complete");
}

String dataString;
unsigned long long lastChannelSwitch = 0;
byte channelArray[WIFI_CHANNEL_MAX] = {1,2,3,4,5,6,7,8,9,10,0,0,0,0};

void loop() {
  if (esp_timer_get_time() - lastChannelSwitch > WIFI_CHANNEL_SWITCH_INTERVAL) {
    // Serial.print("switching to channel ");
    // Serial.println(channelArray[channel]);
    lastChannelSwitch = esp_timer_get_time();
    wifi_sniffer_set_channel(channel);
    channel = (channel % WIFI_CHANNEL_MAX) + 1;
    if (channelArray[channel] == 0) {
      channel = 0;
    }
  }

  if (Serial2.available())  {
    Serial2.readBytes(channelArray, 14);
    for(int i = 0; i < 14; i++) {
      char hexChar[2];
      sprintf(hexChar, "%02X", channelArray[i]);
      Serial.print(hexChar);
      Serial.print(" ");
    }
    Serial.println("");
  }

}