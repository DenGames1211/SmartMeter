#include <NetworkConnection.h>

// Vector of Credential Pairs (SSID, Password)
std::vector<WiFiCredentials> wiFiCredentials{
    {"Wifi1211", ""},
    {"FASTWEB-Q4EGAH", "T9UEAUHC4U"},
    {"WebCube4-89F4", "XMZSGT7X"},
};


/**
 * Initialize connection to a specific network
 * 
 * @param ssid     The SSID of the network to connect to.
 * @param password The password of the network to connect to.
 * @return         True if successfully connected, false otherwise.
 */
bool wifiConnect(const char *ssid, const char *password)
{
    // If already connected, no need to execute connection operations
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }

    Serial.println();
    Serial.print("\nConnecting to ");
    Serial.println(ssid);

    // If not connected, set WiFi Module Power to max, and start connection to Network
    if (WiFi.status() != WL_CONNECTED)
    {
        esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
        WiFi.begin(ssid, password);
    }

    delay(3000);

    // If successfully connected, return true; if not, return false
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Error: the SSID or the password may be wrong.");
        return false;
    }
    else
    {
        return true;
    }
}


// Initialize WiFi module and invoke wifiConnect() to initialize connection
void setupWifi()
{
    // If already connected, return
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    // Wait 100 milliseconds
    delay(100);

    // Enable and set Station Mode on Module (Client Mode, connection to other networks)
    WiFi.enableSTA(true);
    WiFi.mode(WIFI_STA);

    bool connected = false;
    int index = 0;
    while (!connected)
    {
        // Try to connect to all networks in defined vector
        connected = wifiConnect(wiFiCredentials[index].ssid, wiFiCredentials[index].password);
        // If reached networks' number limit, break
        if (++index > wiFiCredentials.size())
        {
            break;
        }
    }

    // Restart the MCU if not connected to a network
    if (!connected)
    {
        Serial.println("Restarting ...");
        ESP.restart();
    }

    // Set WiFi Module Transmission Power to max
    esp_wifi_set_max_tx_power(78);
    delay(10);

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}