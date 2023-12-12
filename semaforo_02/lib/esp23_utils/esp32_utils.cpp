#include "esp32_utils.h"

void enableWDT(int _time)
{
    int ret = esp_task_wdt_init(_time, true);
    if (ret == ESP_OK)
        Serial.println("[SUCCESS]: TWDT successfully deinitialized");
    if (ret == ESP_ERR_INVALID_STATE)
        Serial.println("[ERRROR]: Error, tasks are still subscribed to the TWDT");
    if (ret == ESP_ERR_NOT_FOUND)
        Serial.println("[ERROR]: TWDT has already been deinitialized");
    esp_task_wdt_add(NULL);
}

bool enableWifi(String ssid, String password, int timeout)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    unsigned long tick = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print("[INFO]: WiFi Status: ");
        Serial.println(WiFi.status());
        if (millis() - tick >= timeout)
        {
            ESP.restart();
            return false;
        }
    }
    Serial.println();
    Serial.print("[INFO]: IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    return true;
}
/****************************************************************/
void disableRadio()
{
    adc_power_off();
    WiFi.disconnect(true); // Disconnect from the network
    WiFi.mode(WIFI_OFF);   // Switch WiFi off
}

bool listDir()
{
    File root = SPIFFS.open("/");
    if (!root)
    {
        Serial.println("Falha ao abrir o diretório");
        return false;
    }
    File file = root.openNextFile();
    int qtdFiles = 0;
    while (file)
    {
        Serial.print("  FILE : ");
        Serial.print(file.name());
        Serial.print("\tSIZE : ");
        Serial.println(file.size());
        qtdFiles++;
        file = root.openNextFile();
    }
    if (qtdFiles == 0)
    {
        Serial.print(" Partição vazia");
        return false;
    }
    return true; // retorna true se não houver nenhum erro
}

String readFile(String path)
{
    File file = SPIFFS.open(path);
    String msg;
    if (!file)
    {
        Serial.println(" - falha ao abrir arquivo para leitura");
        return "";
    }
    while (file.available())
    {
        msg += (char)file.read();
    }
    file.close();
    return msg;
}

bool writeFile(String path, String message)
{
    Serial.print("Gravando o arquivo ");
    Serial.print(path);
    Serial.println(" : ");
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println(" - falha ao abrir arquivo para gravação");
        return false;
    }
    if (file.print(message))
    {
        Serial.println(" - arquivo escrito");
    }
    else
    {
        Serial.println(" - falha na gravação do arquivo");
        return false;
    }
    file.close();
    return true;
}

std::vector<String> getNetworksCredentials(String path)
{
    SPIFFS.begin(false);
    listDir();
    String data = readFile(path);
    std::vector<String> nets;

    // Remove espaços e quebras de linhas
    data.replace(" ", "");
    data.replace(String("\r\n"), "");

    while (data.length() > 0)
    {
        int index = data.indexOf(';');
        // Caso não seja encontrado o caractere de separação
        if (index == -1)
        {
            Serial.printf("[WIFI] %s\n", data.c_str());
            break;
        }
        else
        {
            nets.push_back(data.substring(0, index));
            data = data.substring(index + 1);
        }
    }
    return nets;
}

int autoConnectWiFi()
{
    WiFiMulti wifiMulti;
    // int i = 0;
    std::vector<String> networks = getNetworksCredentials(WIFI_SETTINGS);
    // Serial.println("Buscando cadastrando redes WiFi ...");
    // Loop para tentar conexão com as redes salvas previamente
    for (int i = 0; i < networks.size(); i++)
    {
        int index = networks[i].indexOf(',');
        String ssid = networks[i].substring(0, index);
        String password = networks[i].substring(index + 1);
        Serial.printf("SSID: %s,\tPASS: %s\n",ssid.c_str(), password.c_str());
        wifiMulti.addAP(ssid.c_str(), password.c_str());
    }
    unsigned long tick = millis();
    Serial.println("Buscando redes");
    int status = wifiMulti.run();
    while (status != WL_CONNECTED)
    {
        status = WiFi.status();
        Serial.print("[INFO]: WiFi Status: ");
        Serial.println(status);
        if (millis() - tick >= 20000)
        {
            return 0;
        }
        delay(500);
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.print("[INFO]: IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.println();
        return 1;
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Rede WiFi não encontrada");
        return 0;
    }
    return 0;
}