#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>

#include "config.h"
#include "esp_log.h"
#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "led.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "host/util/util.h"
#include "ble.h"

extern void ble_store_config_init(void);

static void nimble_host_task(void *param);
static void start_advertising(void);
static int gap_event_handler(struct ble_gap_event* event, void* arg);
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
void adv_init(void);
static void on_stack_reset(int reason);
static void on_stack_sync(void);
static int led_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg);

#define BLE_GAP_APPEARANCE_GENERIC_TAG 0x0200
#define BLE_GAP_LE_ROLE_PERIPHERAL 0x00
#define BLE_GAP_URI_PREFIX_HTTPS 0x17

static uint8_t esp_uri[] = {BLE_GAP_URI_PREFIX_HTTPS, '/', '/', 'e', 's', 'p', 'r', 'e', 's', 's', 'i', 'f', '.', 'c', 'o', 'm'};

static uint8_t own_addr_type;
static uint8_t addr_val[6] = {0};

static void nimble_host_task(void *param) {
    ESP_LOGI(TAG, "NimBLE host task started!");

    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();

    vTaskDelete(NULL);
}

static void start_advertising(void) {
    /* Local variables */
    int rc = 0;
    const char* name;
    struct ble_hs_adv_fields adv_fields = {0};
    struct ble_hs_adv_fields rsp_fields = {0};
    struct ble_gap_adv_params adv_params = {0};

    /* Set advertising flags */
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Set device name */
    name = ble_svc_gap_device_name();
    adv_fields.name = (uint8_t*)name;
    adv_fields.name_len = strlen(name);
    adv_fields.name_is_complete = 1;

    /* Set device tx power */
    adv_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    adv_fields.tx_pwr_lvl_is_present = 1;

    /* Set device appearance */
    adv_fields.appearance = BLE_GAP_APPEARANCE_GENERIC_TAG;
    adv_fields.appearance_is_present = 1;

    /* Set device LE role */
    adv_fields.le_role = BLE_GAP_LE_ROLE_PERIPHERAL;
    adv_fields.le_role_is_present = 1;

    /* Set advertisement fields */
    rc = ble_gap_adv_set_fields(&adv_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set advertising data, error code: %d", rc);
        return;
    }

    /* Set device address */
    rsp_fields.device_addr = addr_val;
    rsp_fields.device_addr_type = own_addr_type;
    rsp_fields.device_addr_is_present = 1;

    /* Set URI */
    rsp_fields.uri = esp_uri;
    rsp_fields.uri_len = sizeof(esp_uri);

    /* Set advertising interval */
    rsp_fields.adv_itvl = BLE_GAP_ADV_ITVL_MS(500);
    rsp_fields.adv_itvl_is_present = 1;

    /* Set scan response fields */
    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set scan response data, error code: %d", rc);
        return;
    }

    /* Set undirected connectable and general discoverable mode */
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    /* Set advertising interval */
    adv_params.itvl_min = BLE_GAP_ADV_ITVL_MS(500);
    adv_params.itvl_max = BLE_GAP_ADV_ITVL_MS(510);

    /* Start advertising */
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params,
                           gap_event_handler, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to start advertising, error code: %d", rc);
        return;
    }
    ESP_LOGI(TAG, "advertising started!");
}

/*
 * NimBLE applies an event-driven model to keep GAP service going
 * gap_event_handler is a callback function registered when calling
 * ble_gap_adv_start API and called when a GAP event arrives
 */
static int gap_event_handler(struct ble_gap_event* event, void* arg) {
    /* Local variables */
    int rc = 0;
    struct ble_gap_conn_desc desc;

    /* Handle different GAP event */
    switch (event->type) {
        /* Connect event */
        case BLE_GAP_EVENT_CONNECT:
            /* A new connection was established or a connection attempt failed. */
            ESP_LOGI(TAG, "connection %s; status=%d",
                     event->connect.status == 0 ? "established" : "failed",
                     event->connect.status);

            if (event->connect.status != 0) {
                /* Connection failed, restart advertising */
                start_advertising();
            }
            return rc;

        /* Disconnect event */
        case BLE_GAP_EVENT_DISCONNECT:
            /* A connection was terminated, print connection descriptor */
            ESP_LOGI(TAG, "disconnected from peer; reason=%d",
                     event->disconnect.reason);

            /* Restart advertising */
            start_advertising();
            return rc;

        /* Connection parameters update event */
        case BLE_GAP_EVENT_CONN_UPDATE:
            /* The central has updated the connection parameters. */
            ESP_LOGI(TAG, "connection updated; status=%d",
                     event->conn_update.status);
            return rc;

        /* Advertising complete event */
        case BLE_GAP_EVENT_ADV_COMPLETE:
            /* Advertising completed, restart advertising */
            ESP_LOGI(TAG, "advertise complete; reason=%d",
                     event->adv_complete.reason);
            start_advertising();
            return rc;

        /* Notification sent event */
        case BLE_GAP_EVENT_NOTIFY_TX:
            return rc;

        /* Subscribe event */
        case BLE_GAP_EVENT_SUBSCRIBE:
            return rc;

        /* MTU update event */
        case BLE_GAP_EVENT_MTU:
            return rc;
    }

    return rc;
}

/*
 *  Handle GATT attribute register events
 *      - Service register event
 *      - Characteristic register event
 *      - Descriptor register event
 */
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    /* Local variables */
    char buf[BLE_UUID_STR_LEN];

    /* Handle GATT attributes register events */
    switch (ctxt->op) {

    /* Service register event */
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registered service %s with handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        break;

    /* Characteristic register event */
    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG,
                 "registering characteristic %s with "
                 "def_handle=%d val_handle=%d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle, ctxt->chr.val_handle);
        break;

    /* Descriptor register event */
    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD(TAG, "registering descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;

    /* Unknown event */
    default:
        assert(0);
        break;
    }
}

void adv_init(void) {
    /* Local variables */
    int rc = 0;
    char addr_str[18] = {0};

    /* Make sure we have proper BT identity address set (random preferred) */
    rc = ble_hs_util_ensure_addr(0);
    if (rc != 0) {
        ESP_LOGE(TAG, "device does not have any available bt address!");
        return;
    }

    /* Figure out BT address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to infer address type, error code: %d", rc);
        return;
    }

    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);

    /* Start advertising. */
    start_advertising();
}

/*
 *  Stack event callback functions
 *      - on_stack_reset is called when host resets BLE stack due to errors
 *      - on_stack_sync is called when host has synced with controller
 */
static void on_stack_reset(int reason) {
    /* On reset, print reset reason to console */
    ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}

static void on_stack_sync(void) {
    /* On stack sync, do advertising initialization */
    adv_init();
}

static uint16_t led_chr_val_handle;
// handle LED characteristic write events
static int led_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
    int rc = 0;

    switch (ctxt->op) {
        // handle write event
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            /* Verify connection handle */
            if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
                ESP_LOGI(TAG, "characteristic write; conn_handle=%d attr_handle=%d",
                         conn_handle, attr_handle);
            } else {
                ESP_LOGI(TAG,
                         "characteristic write by nimble stack; attr_handle=%d",
                         attr_handle);
            }

            /* Verify attribute handle */
            if (attr_handle == led_chr_val_handle) {
                /* Verify access buffer length */
                if (ctxt->om->om_len == 1) {
                    // turn the LED on/off according to the operation bit
                    if (ctxt->om->om_data[0]) {
                        led_on();
                        ESP_LOGI(TAG, "LED turned on!");
                    } else {
                        led_off();
                        ESP_LOGI(TAG, "LED turned off!");
                    }
                } else {
                    goto error;
                }
                return rc;
            }
            goto error;

        /* Unknown event */
        default:
            goto error;
    }

error:
    ESP_LOGE(TAG, "Unexpected access operation to LED characteristic, opcode: %d", ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
}

static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);
static const ble_uuid128_t led_chr_uuid =
    BLE_UUID128_INIT(0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 0xde, 0xef, 0x12, 0x12, 0x25, 0x15, 0x00, 0x00);

// GATT services table
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    // automation IO service
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &auto_io_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){
            // LED characteristic
            {
                .uuid = &led_chr_uuid.u,
                .access_cb = led_chr_access,
                .flags = BLE_GATT_CHR_F_WRITE,
                .val_handle = &led_chr_val_handle},
            {0},
        },
    },
    {
        0,
    },
};

void app_main(void) {
    // initialize status LED
    ESP_LOGI(TAG, "Initializing LED on pin %d...", STATUS_LED_PIN);
    led_init();

    // initialize flash storage, erasing it on error and trying again
    ESP_LOGI(TAG, "Initializing NVS flash...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing flash...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // initialize NimBLE stack
    ESP_LOGI(TAG, "Initializing NimBLE...");
    ESP_ERROR_CHECK(nimble_port_init());

    // initialize GAP service for advertising
    ESP_LOGI(TAG, "Initializing GAP service...");
    ble_svc_gap_init();
    ble_svc_gap_device_name_set(GAP_NAME);

    // initialze GATT server
    ESP_LOGI(TAG, "Initializing GATT server...");
    ble_svc_gatt_init();
    ble_gatts_count_cfg(gatt_svr_svcs);
    ble_gatts_add_svcs(gatt_svr_svcs);

    // initialize NimBLE host config
    /* Set host callbacks */
    ble_hs_cfg.reset_cb = on_stack_reset;
    ble_hs_cfg.sync_cb = on_stack_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    // store host configuration
    ble_store_config_init();

    /* Start NimBLE host task thread and return */
    xTaskCreate(nimble_host_task, "NimBLE Host", 4 * 1024, NULL, 5, NULL);

    while (1) {
        led_on();
        vTaskDelay(500 / portTICK_PERIOD_MS);
        led_off();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
