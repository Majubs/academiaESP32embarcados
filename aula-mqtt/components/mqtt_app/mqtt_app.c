#include "mqtt_client.h"

#include "mqtt_app.h"

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t client; // handler for mqtt client
static EventGroupHandle_t status_mqtt_event_group;

static void mqtt_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", event_base, event_id);
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("topic: %.*s\n", event->topic_len, event->topic);
        printf("message: %.*s\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_BEFORE_CONNECT:
        ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
        break;
    case MQTT_USER_EVENT:
        ESP_LOGI(TAG, "MQTT_USER_EVENT");
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "ERROR %s", strerror(event->error_handle->esp_transport_sock_errno));
        break;
    default:
        ESP_LOGI(TAG, "Outro Evento id:%d", event->event_id);
        break;
    }
}

int mqtt_app_is_connected(void)
{
    EventBits_t bits = xEventGroupGetBits(status_mqtt_event_group);
    if (bits & MQTT_CONNECTED)
    {
        return 1;
    }
    return 0;
}

void mqtt_app_start()
{
    status_mqtt_event_group = xEventGroupCreate();

    esp_mqtt_client_config_t esp_mqtt_client_config = {.network.disable_auto_reconnect = false,
                                                       .session.keepalive = 30,
                                                       .broker.address.uri = CONFIG_BROKER_URL,
                                                       .broker.address.port = 1883,
                                                       .session.last_will = {
                                                           .topic = "esp32_mjbs/status",
                                                           .msg = "Offline",
                                                           .msg_len = strlen("Offline"),
                                                           .qos = 1,
                                                       }};

    ESP_LOGI(TAG, "Starting MQTT client");
    client = esp_mqtt_client_init(&esp_mqtt_client_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    mqtt_app_publish("esp32_mjbs/status", "Online", 1, 0);
}

void mqtt_app_stop()
{
    esp_mqtt_client_stop(client);
    ESP_LOGI(TAG, "MQTT client stopped");
}

void mqtt_app_subscribe(char *topic, int qos)
{
    int msg_id = esp_mqtt_client_subscribe(client, topic, qos);
    ESP_LOGI(TAG, "Sent subscribe successful, msg_id=%d", msg_id);
}

void mqtt_app_unsubscribe(char *topic)
{
    int msg_id = esp_mqtt_client_unsubscribe(client, topic);
    ESP_LOGI(TAG, "Unsubscribe successful, msg_id=%d", msg_id);
}

void mqtt_app_publish(char *topic, char *payload, int qos, int retain)
{
    int msg_id = esp_mqtt_client_publish(client, topic, payload, strlen(payload), qos, retain);
    ESP_LOGI(TAG, "Sent publish sucessful, msg_id=%d", msg_id);
}
