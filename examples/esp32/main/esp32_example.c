/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_tls.h"

#include "esp_http_client.h"
#include "vertices.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "HTTP_CLIENT";

/* Root cert for howsmyssl.com, taken from howsmyssl_com_root_cert.pem

   The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
extern const char
    howsmyssl_com_root_cert_pem_start[] asm("_binary_howsmyssl_com_root_cert_pem_start");
extern const char
    howsmyssl_com_root_cert_pem_end[]   asm("_binary_howsmyssl_com_root_cert_pem_end");

extern const char
    algoexplorer_root_cert_pem_start[] asm("_binary_algoexplorer_root_cert_pem_start");
extern const char algoexplorer_root_cert_pem_end[] asm("_binary_algoexplorer_root_cert_pem_end");

static provider_info_t providers =
    {.url = (char *) "https://api.testnet.algoexplorer.io", .port = 0, .header = (char *) "", .cert_pem = algoexplorer_root_cert_pem_start};

// Alice's account is used to send data, private key is taken from config/private_key.bin
static account_info_t alice_account = {.public_b32 = { "E3PGTXKDOODVQ3E2ZB5PMJF2W3YOKIPUPLFDTESSP6562QE4GTLAKO4VXY"}, .private_key = {0}, .amount = 0};
// Bob is receiving the money 😎
static account_info_t bob_account =
    {.public_b32 = "27J56E73WOFSEQUECLRCLRNBV3D74H7BYB7USEXCJOYPLBTACULABWMLVU", .private_key = {
        0}, .amount = 0};

static ret_code_t
vertices_evt_handler(vtc_evt_t *evt);

static vertex_t m_vertex = {
    .provider = &providers,
    .vertices_evt_handler = vertices_evt_handler};

static ret_code_t
vertices_evt_handler(vtc_evt_t *evt)
{
    ret_code_t err_code = VTC_SUCCESS;

    switch (evt->type)
    {
        default:ESP_LOGI(TAG, "Unhandled event: %u", evt->type);
            break;
    }

    return err_code;
}

void
vtc_wallet_task(void *param)
{
    // create new vertex
    ret_code_t err_code = vertices_new(&m_vertex);
    VTC_ASSERT(err_code);

    // making sure the provider is accessible
    err_code = vertices_ping();
    VTC_ASSERT(err_code);

    // ask for provider version
    provider_version_t version = {0};
    err_code = vertices_version(&version);
    if (err_code == VTC_ERROR_OFFLINE)
    {
        ESP_LOGW(TAG, "Version might not be accurate: old value is being used");
    }
    else
    {
        VTC_ASSERT(err_code);
    }

    ESP_LOGI(TAG, "🏎 Running on %s v.%u.%u.%u",
             version.network,
             version.major,
             version.minor,
             version.patch);

    // create accounts
    size_t alice_account_handle = 0;
    err_code = vertices_add_account(&alice_account, &alice_account_handle);
    VTC_ASSERT(err_code);

    ESP_LOGI(TAG, "🤑 %f Algos on Alice's account (%s)", alice_account.amount / 1.e6, alice_account.public_b32);

    // creating a receiver account is not mandatory but we can use it to load the public key from the
    // base32-encoded string
    size_t bob_account_handle = 0;
    err_code = vertices_add_account(&bob_account, &bob_account_handle);
    VTC_ASSERT(err_code);

    if (alice_account.amount < 1001000)
    {
        // todo get amount on Alice account and retry later

        ESP_LOGE(TAG,
            "🙄 Amount available on account is too low to pass a transaction, consider adding Algos");
        ESP_LOGI(TAG, "👉 Go to https://bank.testnet.algorand.network/, dispense Algos to: %s",
                 alice_account.public_b32);
        ESP_LOGI(TAG, "😎 Then wait for a few seconds for transaction to pass...");

        while(1)
        {
            vTaskDelay(30000 / portTICK_RATE_MS);
        }
    }

    while (1)
    {
        if (err_code == VTC_SUCCESS)
        {
            ESP_LOGI(TAG, "Wallet is alive");
        }
        else
        {
            ESP_LOGW(TAG, "Wallet is offline: 0x%x", err_code);
        }
        vTaskDelay(30000 / portTICK_RATE_MS);
    }
}

void
app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    ESP_LOGI(TAG, "Connected to AP, begin http example");

    xTaskCreatePinnedToCore(&vtc_wallet_task, "vtc_wallet_task", 16000, NULL, 5, NULL, 1);
}
