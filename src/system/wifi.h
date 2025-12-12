// wifi.h - 简单版WiFi功能头文件，和wifi.c配套使用
#ifndef WIFI_H
#define WIFI_H

#include "mongoose.h"  // 项目里已有的mongoose库头文件
#include <stdbool.h>

// WiFi配置（前端需要的参数）
typedef struct {
    char ssid[64];        // WiFi名称
    char key[64];         // WiFi密码
    char mode[4];         // 2.4G/5G
    int channel;          // 信道
    char encryption[16];  // 加密方式（固定WPA2-PSK）
    bool enabled;         // WiFi是否开启
} WiFiConfig;

// 初始化WiFi（程序启动时调用）
void wifi_init(void);

// 处理前端的WiFi请求（比如/cgi-bin/traffic.cgi）
void handle_wifi_request(struct mg_connection *c, struct mg_http_message *hm);

#endif
