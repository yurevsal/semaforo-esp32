#include <plan.h>

bool Plan_model::listDir()
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

String Plan_model::readFile(String path)
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

bool Plan_model::writeFile(String path, String message)
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

void Plan_model::init(int _id)
{
    id = _id;
    if (load())
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                delays[j][i] = 1;
            }
        }
    }
    else
    {
        Serial.printf("Falha ao ler o arquivo: plano_%d.json\n", id);
    }
}

void Plan_model::save()
{
    DynamicJsonDocument doc(1024);
    doc["tipo"] = type;
    doc["n_fases"] = n_phases;
    doc["h_inicio"] = start_hour;
    doc["h_fim"] = end_hour;
    doc["m_inicio"] = start_min;
    doc["m_fim"] = end_min;
    for (int i = 0; i < 3; i++)
    {
        doc["fase_1"][i] = delays[0][i];
        doc["fase_2"][i] = delays[1][i];
        doc["fase_3"][i] = delays[2][i];
        doc["fase_4"][i] = delays[3][i];
        doc["fase_5"][i] = delays[4][i];
        doc["fase_6"][i] = delays[5][i];
        doc["fase_7"][i] = delays[6][i];
        doc["fase_8"][i] = delays[7][i];
    }
    String msg;
    String path = "/plano_{0}.json";
    serializeJson(doc, msg);
    path.replace("{0}", String(id));
    writeFile(path, msg);
}

bool Plan_model::load()
{
    Serial.printf("[INSTANCE] carregando plano: %d\n", id);
    DynamicJsonDocument doc(1024);
    String path = "/plano_{0}.json";
    path.replace("{0}", String(id));
    String json = readFile(path);
    if (json == "")
    {
        for (int i = 0; i < 3; i++)
        {
            delays[0][i] = 1;
            delays[1][i] = 1;
            delays[2][i] = 1;
            delays[3][i] = 1;
            delays[4][i] = 1;
            delays[5][i] = 1;
            delays[6][i] = 1;
            delays[7][i] = 1;
        }
        return false;
    }
    Serial.printf("[INSTANCE] %s\n", json.c_str());
    DeserializationError error = deserializeJson(doc, json);
    type = doc["tipo"];
    n_phases = doc["n_fases"];
    start_hour = doc["h_inicio"];
    end_hour = doc["h_fim"];
    start_min = doc["m_inicio"];
    end_min = doc["m_fim"];
    for (int i = 0; i < 3; i++)
    {
        delays[0][i] = doc["fase_1"][i];
        delays[1][i] = doc["fase_2"][i];
        delays[2][i] = doc["fase_3"][i];
        delays[3][i] = doc["fase_4"][i];
        delays[4][i] = doc["fase_5"][i];
        delays[5][i] = doc["fase_6"][i];
        delays[6][i] = doc["fase_7"][i];
        delays[7][i] = doc["fase_8"][i];
    }
    // Serial.printf("Verdes: %d, %d, %d, %d, %d, %d, %d, %d\n",delays[0][0],delays[1][0],delays[2][0],delays[3][0],delays[4][0],delays[5][0],delays[6][0],delays[7][0]);
    // Serial.printf("Amarelos: %d, %d, %d, %d, %d, %d, %d, %d\n",delays[0][1],delays[1][1],delays[2][1],delays[3][1],delays[4][1],delays[5][1],delays[6][1],delays[7][1]);
    // Serial.printf("Vermelhos: %d, %d, %d, %d, %d, %d, %d, %d\n",delays[0][2],delays[1][2],delays[2][2],delays[3][2],delays[4][2],delays[5][2],delays[6][2],delays[7][2]);
    return true;
}
