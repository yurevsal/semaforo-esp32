#include "functions.h"

// ESP32Time rtc;
ESP32Time esp32_rtc; // offset in seconds GMT+3

// Objeto referente ao RTC
RTC_DS1307 rtc;
String hours_str;

// Cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

WiFiManager wm;
WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
WiFiManagerParameter custom_mqtt_port("server_port", "mqtt port", "", 10);
WiFiManagerParameter custom_mqtt_user("server_user", "mqtt user", "", 20);
WiFiManagerParameter custom_mqtt_pass("server_pass", "mqtt pass", "", 20);

PCF8574 canal1(0x27, SDA, SCL);
PCF8574 canal2(0x23, SDA, SCL);
PCF8574 canal3(0x21, SDA, SCL);
PCF8574 canal4(0x20, SDA, SCL);

LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 2);

void initOutputs()
{
  for (int i = 0; i < 6; i++)
  {
    canal1.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal2.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal3.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal4.pinMode(i, OUTPUT, 0);
  }
  canal1.begin();
  canal2.begin();
  canal3.begin();
  canal4.begin();
}

void initDisplay()
{
  lcd.init();
  lcd.clear();
  lcd.backlight();
  // lcd.noBacklight();
}

void initInputs()
{
  pinMode(BT_UP, INPUT_PULLUP);
  pinMode(BT_DOWN, INPUT_PULLUP);
  pinMode(BT_LEFT, INPUT_PULLUP);
  pinMode(BT_RIGHT, INPUT_PULLUP);
  pinMode(BT_CENTER, INPUT_PULLUP);
  pinMode(BT_PEDE, INPUT_PULLUP);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void mqttSetup()
{
  client.setServer(custom_mqtt_server.getValue(),atoi(custom_mqtt_port.getValue()));
  client.setCallback(callback);
}

void saveParamsCallback () {
  Serial.println("Get Params:");
  Serial.print(custom_mqtt_server.getID());
  Serial.print(" : ");
  Serial.println(custom_mqtt_server.getValue());
}

void initWiFiManager()
{
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP        
    //reset settings - wipe credentials for testing
    //wm.resetSettings();

    wm.setConfigPortalBlocking(false);
    wm.setConfigPortalTimeout(60);
    wm.addParameter(&custom_mqtt_server);
    wm.addParameter(&custom_mqtt_port);
    wm.addParameter(&custom_mqtt_user);
    wm.addParameter(&custom_mqtt_pass);
    wm.setSaveParamsCallback(saveParamsCallback);
    //automatically connect using saved credentials if they exist
    //If connection fails it starts an access point with the specified name
    if(wm.autoConnect("AutoConnectAP")){
        Serial.println("connected...yeey :)");
        mqttSetup();
    }
    else {
        Serial.println("Configportal running");
    }
}
void reconnect(void *p) {
  for(;;)
  {
  if(!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), custom_mqtt_user.getValue(), custom_mqtt_pass.getValue())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(".b.r.a.s.i.l", "hello world");
      // ... and resubscribe
      client.subscribe("b,r,a,s,i,l");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
    }
  }
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
   vTaskDelete(NULL);
}
void process()
{
  wm.process();
  client.loop();
}
// Menus
Menu main_menu("Main menu", BT_UP, BT_DOWN, BT_RIGHT, BT_LEFT, BT_CENTER, BT_PEDE);
// Menu menu1_item1("Item 1", BT_UP, BT_DOWN, BT_RIGHT, BT_LEFT, BT_CENTER, BT_PEDE);
// Menu menu1_item2("Item 2", BT_UP, BT_DOWN, BT_RIGHT, BT_LEFT, BT_CENTER, BT_PEDE);
// Menu menu1_item3("Item 3", BT_UP, BT_DOWN, BT_RIGHT, BT_LEFT, BT_CENTER, BT_PEDE);
// Menu menu1_item4("Item 4", BT_UP, BT_DOWN, BT_RIGHT, BT_LEFT, BT_CENTER, BT_PEDE);
// Menu menu1_item5("Item 5", BT_UP, BT_DOWN, BT_RIGHT, BT_LEFT, BT_CENTER, BT_PEDE);

void mainScreen(void *p)
{
  for (;;)
  {
    lcd.setCursor(0, 0);
    lcd.printf("millis: %lu", millis() / 1000);
    delay(1000);
  }
}
