#include <M5Core2.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Wi-Fi credentials
const char* ssid = "WiFi-SSD";
const char* password = "12345678";

// NTP server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, 60000);

// Timezone structure
struct Timezone {
  const char* name;
  int offset;
};

Timezone timezones[] = {
  {"UTC-12", -12}, {"UTC-11", -11}, {"UTC-10", -10}, {"UTC-9", -9},
  {"UTC-8", -8}, {"UTC-7", -7}, {"UTC-6", -6}, {"UTC-5", -5},
  {"UTC-4", -4}, {"UTC-3", -3}, {"UTC-2", -2}, {"UTC-1", -1},
  {"UTC+0", 0}, {"UTC+1", 1}, {"UTC+2", 2}, {"UTC+3", 3},
  {"UTC+4", 4}, {"UTC+5", 5}, {"UTC+6", 6}, {"UTC+7", 7},
  {"UTC+8", 8}, {"UTC+9", 9}, {"UTC+10", 10}, {"UTC+11", 11},
  {"UTC+12", 12}, {"UTC+13", 13}, {"UTC+14", 14},
  {"Sydney", 10}, {"Melbourne", 10}, {"Brisbane", 10},
  {"Perth", 8}, {"Adelaide", 9.5}, {"Hobart", 10},
  {"New York", -5}, {"Los Angeles", -8}, {"Chicago", -6},
  {"Houston", -6}, {"Phoenix", -7}
};

int currentIndex = 0;

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.println("\nConnected to WiFi");

  timeClient.begin();
  timeClient.update();
}

void loop() {
  M5.update();
  timeClient.update();

  // Handle dial rotation
  if (M5.BtnA.wasPressed()) {
    currentIndex = (currentIndex + 1) % (sizeof(timezones) / sizeof(Timezone));
  }
  if (M5.BtnB.wasPressed()) {
    currentIndex = (currentIndex - 1 + (sizeof(timezones) / sizeof(Timezone))) % (sizeof(timezones) / sizeof(Timezone));
  }

  // Calculate time in the selected timezone
  int timezoneOffset = timezones[currentIndex].offset * 3600;
  unsigned long localTime = timeClient.getEpochTime() + timezoneOffset;

  int hours = (localTime % 86400L) / 3600;
  int minutes = (localTime % 3600) / 60;
  int seconds = localTime % 60;

  // Clear screen and update time
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Timezone: %s\n", timezones[currentIndex].name);
  M5.Lcd.printf("Time: %02d:%02d:%02d\n", hours, minutes, seconds);

  // Day/Night icon
  if (hours >= 6 && hours < 18) {
    M5.Lcd.drawBitmap(100, 100, sun_icon, 32, 32, TFT_YELLOW); // Placeholder for sun icon
  } else {
    M5.Lcd.drawBitmap(100, 100, moon_icon, 32, 32, TFT_WHITE); // Placeholder for moon icon
  }

  // Hourly flash
  if (minutes == 0 && seconds == 0) {
    M5.Lcd.fillScreen(TFT_WHITE);
    delay(100);
    M5.Lcd.fillScreen(TFT_BLACK);
  }

  delay(1000); // Update every second
}