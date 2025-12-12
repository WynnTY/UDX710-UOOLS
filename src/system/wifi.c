#include "wifi.h"
#include "mongoose.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// WiFi 配置结构体（与前端数据对应）
typedef struct {
  char ssid[64];
  char key[64];
  char mode[4];       // "2.4G" 或 "5G"
  int channel;
  char encryption[16];// 加密方式（如 WPA2-PSK）
  bool enabled;
} wifi_config_t;

static wifi_config_t s_wifi = {
  .ssid = "UDX710-5G",
  .key = "12345678",
  .mode = "5G",
  .channel = 149,
  .encryption = "WPA2-PSK",
  .enabled = true
};

// 模拟获取已连接客户端（实际应从系统接口读取）
static void get_wifi_clients(struct mg_connection *c) {
  // 示例数据：实际应通过 iwlist 或系统 API 获取
  const char *clients = "[{\"mac\":\"aa:bb:cc:dd:ee:ff\",\"ip\":\"192.168.0.2\",\"name\":\"手机设备\"}]";
  mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"clients\":%s}", clients);
}

// 处理 WiFi 配置获取请求
static void handle_get_wifi_config(struct mg_connection *c) {
  mg_http_reply(c, 200, "Content-Type: application/json\r\n",
    "{"
    "\"wifi_enabled\":\"%s\","
    "\"wifi_mode\":\"%s\","
    "\"wifi_ssid\":\"%s\","
    "\"wifi_key\":\"%s\","
    "\"wifi_channel\":%d,"
    "\"wifi_encryption\":\"%s\""
    "}",
    s_wifi.enabled ? "on" : "off",
    s_wifi.mode,
    s_wifi.ssid,
    s_wifi.key,
    s_wifi.channel,
    s_wifi.encryption
  );
}

// 处理 WiFi 配置更新请求（开关、频段、参数等）
static void handle_post_wifi_config(struct mg_connection *c, struct mg_http_message *hm) {
  char action[32] = {0};
  mg_json_get_str(hm->body, "$.action", action, sizeof(action));

  if (strcmp(action, "toggle_wifi") == 0) {
    s_wifi.enabled = !s_wifi.enabled;
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
      "{\"message\":\"WiFi %s成功\"}", s_wifi.enabled ? "开启" : "关闭");
  } else if (strcmp(action, "switch_wifi_mode") == 0) {
    char mode[4] = {0};
    mg_json_get_str(hm->body, "$.target_mode", mode, sizeof(mode));
    if (strcmp(mode, "2.4G") == 0 || strcmp(mode, "5G") == 0) {
      strncpy(s_wifi.mode, mode, sizeof(s_wifi.mode)-1);
      // 切换频段时自动调整默认信道
      s_wifi.channel = strcmp(mode, "5G") == 0 ? 36 : 6;
      mg_http_reply(c, 200, "Content-Type: application/json\r\n",
        "{\"message\":\"切换至%s成功\"}", mode);
    } else {
      mg_http_reply(c, 400, "Content-Type: application/json\r\n",
        "{\"error\":\"无效的频段模式\"}");
    }
  } else if (strcmp(action, "update_wifi_settings") == 0) {
    char ssid[64] = {0}, key[64] = {0};
    mg_json_get_str(hm->body, "$.ssid", ssid, sizeof(ssid));
    mg_json_get_str(hm->body, "$.key", key, sizeof(key));
    if (strlen(ssid) > 0) strncpy(s_wifi.ssid, ssid, sizeof(s_wifi.ssid)-1);
    if (strlen(key) >= 8) strncpy(s_wifi.key, key, sizeof(s_wifi.key)-1);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
      "{\"message\":\"WiFi设置更新成功\"}");
  } else if (strcmp(action, "switch_wifi_channel") == 0) {
    int channel = mg_json_get_num(hm->body, "$.target_channel", -1);
    // 简单校验信道合法性（实际应根据频段校验）
    if (channel > 0 && channel <= 165) {
      s_wifi.channel = channel;
      mg_http_reply(c, 200, "Content-Type: application/json\r\n",
        "{\"message\":\"信道切换至%d成功\"}", channel);
    } else {
      mg_http_reply(c, 400, "Content-Type: application/json\r\n",
        "{\"error\":\"无效的信道\"}");
    }
  } else {
    mg_http_reply(c, 400, "Content-Type: application/json\r\n",
      "{\"error\":\"无效的操作\"}");
  }
}

// 注册 WiFi 相关 API 路由
void wifi_init(struct mg_mgr *mgr) {
  // 实际项目中需将路由注册到 mongoose 服务器
  // 示例路由：/api/wifi/config (GET/POST)、/api/wifi/clients (GET)
}

// 处理 WiFi 相关请求（被 HTTP 服务器回调）
bool wifi_handle_request(struct mg_connection *c, struct mg_http_message *hm) {
  if (mg_http_match_uri(hm, "/api/wifi/config")) {
    if (strcmp(hm->method, "GET") == 0) {
      handle_get_wifi_config(c);
    } else if (strcmp(hm->method, "POST") == 0) {
      handle_post_wifi_config(c, hm);
    }
    return true;
  } else if (mg_http_match_uri(hm, "/api/wifi/clients") && strcmp(hm->method, "GET") == 0) {
    get_wifi_clients(c);
    return true;
  }
  return false;
}
