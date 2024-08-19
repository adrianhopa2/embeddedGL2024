#pragma once

#define CONFIG_EXAMPLE_CONNECT_WIFI 1

#if CONFIG_EXAMPLE_CONNECT_WIFI
    #define CONFIG_EXAMPLE_WIFI_SSID_PWD_FROM_STDIN        0               // Give the WiFi SSID and password from stdin.
    #define CONFIG_EXAMPLE_PROVIDE_WIFI_CONSOLE_CMD        1               // Provide wifi connect commands for esp_console. Please use `CONFIG_EXAMPLE_register_wifi_connect_commands` to register them.
    #define CONFIG_EXAMPLE_WIFI_SSID                       "SA52_S"         // SSID (network name) for the example to connect to.
    #define CONFIG_EXAMPLE_WIFI_PASSWORD                   "nokia123"  // WiFi password (WPA or WPA2) for the example to use. Can be left blank if the network has no security set.
    #define CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY             6               // Set the Maximum retry to avoid station reconnecting to the AP unlimited, in case the AP is really inexistent.
    #define CONFIG_EXAMPLE_WIFI_SCAN_METHOD_FAST           0               // If "Fast" is selected, scan will end after find SSID match AP.
    // Select one SCAN Method
    #define CONFIG_EXAMPLE_WIFI_SCAN_METHOD_ALL_CHANNEL    1               // If "All Channel" is selected, scan will end after scan all the channel.
    #define CONFIG_EXAMPLE_WIFI_SCAN_RSSI_THRESHOLD        -127            // The minimum rssi to accept in the scan mode.
    // Select "WiFi Scan auth mode threshold"
    #define CONFIG_EXAMPLE_WIFI_AUTH_OPEN                  0 // OPEN (The weakest authmode to accept in the scan mode)
    #define CONFIG_EXAMPLE_WIFI_AUTH_WEP                   0 // WEP
    #define CONFIG_EXAMPLE_WIFI_AUTH_WPA_PSK               0 // WPA PSK
    #define CONFIG_EXAMPLE_WIFI_AUTH_WPA2_PSK              1 // WPA2 PSK
    #define CONFIG_EXAMPLE_WIFI_AUTH_WPA_WPA2_PSK          0 // WPA WPA2 PSK
    #define CONFIG_EXAMPLE_WIFI_AUTH_WPA2_ENTERPRISE       0 // WPA2 ENTERPRISE
    #define CONFIG_EXAMPLE_WIFI_AUTH_WPA3_PSK              0 // WPA3 PSK
    #define CONFIG_EXAMPLE_WIFI_AUTH_WPA2_WPA3_PSK         0 // WPA2 WPA3 PSK
    #define CONFIG_EXAMPLE_WIFI_AUTH_WAPI_PSK              0 // WAPI PSK
    // Select "WiFi Connect AP Sort Method"
    #define CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SIGNAL       1 // If "Signal" is selected, Sort matched APs in scan list by RSSI.
    #define CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SECURITY     0 // If "Security" is selected, Sort matched APs in scan list by security mode.
#endif

#define CONFIG_EXAMPLE_CONNECT_IPV4 1