#include "things5.h"

String createUUID(void)
{
    // Generating UUID v4
    uint8_t uuid_array[16];
    ESPRandom::uuid4(uuid_array);
    return ESPRandom::uuidToString(uuid_array);
}

String createMetricsPayload(unsigned long long timestamp, t5_data_t * data, size_t size) {
    StaticJsonDocument<JSON_SIZE> doc;
    String payload;

    doc["request_id"] = createUUID();

    // Add metrics --------------------------------------------------
    JsonObject metrics_0 = doc["metrics"].createNestedObject();
    metrics_0["timestamp"] = timestamp;

    JsonArray metrics_0_data = metrics_0.createNestedArray("data");

    for(uint8_t i=0 ; i<size ; i++) {
        StaticJsonDocument<64> obj;
        obj["name"] = (data + i)->label;
        obj["value"] = String((data + i)->value);
        metrics_0_data.add(obj);
        obj.clear();
    }

    serializeJson(doc, payload);

    return payload;
}
