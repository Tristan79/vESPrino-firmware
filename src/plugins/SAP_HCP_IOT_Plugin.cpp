#include "common.hpp"
#include "SAP_HCP_IOT_Plugin.hpp"
#include "plugins/AT_FW_Plugin.hpp"
#define COLOR_COUNT 10

void SAP_HCP_IOT_Plugin::registerCommands(MenuHandler *handler) {
   handler->registerCommand(new MenuEntry(F("cfgiot1"), CMD_BEGIN, &SAP_HCP_IOT_Plugin::cfgHCPIOT1, F("HCP Cfg 1")));
   handler->registerCommand(new MenuEntry(F("cfgiot2"), CMD_EXACT, &SAP_HCP_IOT_Plugin::cfgHCPIOT1, F("HCP Cfg 2")));
}

void SAP_HCP_IOT_Plugin::doSend() {
  if (shouldSend == false) return;
  shouldSend = false;
  char tmp[200];
  SERIAL_PORT << F("Button Clicked!") << endl;
  if(WiFi.status() != WL_CONNECTED) {
    SERIAL_PORT << F("Will not send: No WiFi") << endl;
    return;
  }
  if (!getJSONConfig(SAP_IOT_HOST, tmp)[0]) {
    SERIAL_PORT << F("Will not send: No configuration") << endl;
    return;
  }
  HTTPClient http;
  //https://iotmmsi024148trial.hanatrial.ondemand.com/com.sap.iotservices.mms/v1/api/http/push/e46304a8-a410-4979-82f6-ca3da7e43df9
  //{"method":"http", "sender":"My IoT application", "messageType":"42c3546a088b3ef8b8d3", "messages":[{"command":"yellow"}]}
  String rq = String("https://") + getJSONConfig(SAP_IOT_HOST, tmp) + F("/com.sap.iotservices.mms/v1/api/http/data/") + getJSONConfig(SAP_IOT_DEVID, tmp) + "/" + getJSONConfig(SAP_IOT_BTN_MSGID, tmp) + "/sync?button=IA==";
  SERIAL_PORT << "Sending: " << rq << endl;
  http.begin(rq);
  http.addHeader("Content-Type",  "application/json;charset=UTF-8");
  //http.setAuthorization("P1940433103", "Abcd1234");
  http.addHeader("Authorization", String("Bearer ") + getJSONConfig(SAP_IOT_TOKEN, tmp));
//  String post = "";
//  post += "{\"method\":\"http\", \"sender\":\"My IoT application\", \"messageType\":\"" + getJSONConfig(SAP_IOT_BTN_MSGID) + "\", \"messages\":[{\"button\":\"" + colors[(clicks++) % COLOR_COUNT] + "\"}]}";
  int httpCode = http.POST("");
  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      SERIAL_PORT.printf(String(F("[HTTP] GET... code: %d\n")).c_str(), httpCode);
      String payload = http.getString();
      SERIAL_PORT.println(payload);
  } else {
      SERIAL_PORT.printf(String(F("[HTTP] GET... failed, error: %s\n")).c_str(), http.errorToString(httpCode).c_str());
  }
}



//String colors[] = {"red","pink","lila","violet","blue","mblue","cyan","green","yellow","orange"};


//h iotmmsi024148trial.hanatrial.ondemand.com
//d c5c73d69-6a19-4c7d-9da3-b32198ba71f9
//m 2023a0e66f76d20f47d7
//v co2
//t 46de4fc404221b32054a8405f602fd


//uint32_t lastSAPCheck = -1000000L;
void SAP_HCP_IOT_Plugin::handleSAP_IOT_PushService() {
    char tmp[200];
    if(WiFi.status() != WL_CONNECTED) return;
    heap("");
    //Serial << " before get json config" << endl;
    if (!getJSONConfig(SAP_IOT_HOST, tmp)[0]) return;
    //Serial << " after " << endl;
    String url = String("https://") + getJSONConfig(SAP_IOT_HOST, tmp) + F("/com.sap.iotservices.mms/v1/api/http/data/") + getJSONConfig(SAP_IOT_DEVID, tmp) ;
//        Serial << " before get json config" << end;

    //SERIAL_PORT << url << endl;
//    SERIAL_PORT << getJSONConfig(SAP_IOT_HOST) << endl;
//    if (getJSONConfig(SAP_IOT_HOST)) SERIAL_PORT << "ok" << endl;
//    else SERIAL_PORT << "false" << endl;
    //SERIAL_PORT << (getJSONConfig(SAP_IOT_HOST) == true) << endl;
    HTTPClient http;
    http.begin(url);
    //Serial << " after begin " << endl;
    http.addHeader("Content-Type",  "application/json;charset=UTF-8");
//    SERIAL_PORT <<  getJSONConfig(SAP_IOT_TOKEN) << endl;
    http.addHeader("Authorization", String("Bearer ") + getJSONConfig(SAP_IOT_TOKEN, tmp));
    //SERIAL_PORT << "make req" << endl;
 //   Serial << " after begin " << endl;
    int httpCode = http.GET();
    //SERIAL_PORT << "make req " << httpCode << endl;
    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        //SERIAL_PORT.printf("[HTTP] GET... code: %d\n", httpCode);
        String payload = http.getString();
        //SERIAL_PORT.println(payload);
        processMessage(payload);
    } else {
      SERIAL_PORT << F("Failed to push message to: ") << url << ", due to: " <<http.errorToString(httpCode) << endl;
        //SERIAL_PORT.printf("[HTTP] GET... failed, error: %s, url\n", http.errorToString(httpCode).c_str());
    }

    //lastSAPCheck = millis();
}



void SAP_HCP_IOT_Plugin::cfgHCPIOT1(const char *p) {
  //POST https://iotmmsi024148trial.hanatrial.ondemand.com/com.sap.iotservices.mms/v1/api/http/data/c5c73d69-6a19-4c7d-9da3-b32198ba71f9/2023a0e66f76d20f47d7/sync?co2=34
  // host: iotmmsi024148trial.hanatrial.ondemand.com
  // deviceId: c5c73d69-6a19-4c7d-9da3-b32198ba71f9
  // messageId: 2023a0e66f76d20f47d7
  // variable name: co2

  // Authorization: Bearer 46de4fc404221b32054a8405f602fd

  char buf[140], devId[40], msgId[25], varName[20];
  p = extractStringFromQuotes(p, buf, sizeof(buf)); // host
  storeToEE(EE_IOT_HOST_60B, buf, 60);     //host
  putJSONConfig(SAP_IOT_HOST, buf);
  //SERIAL_PORT << "IOT Host: " << buf << endl;

  p = extractStringFromQuotes(p, devId, sizeof(devId));
  putJSONConfig(SAP_IOT_DEVID, devId);
  p = extractStringFromQuotes(p, msgId, sizeof(msgId));
  p = extractStringFromQuotes(p, varName, sizeof(varName));
  sprintf(buf, String(F("/com.sap.iotservices.mms/v1/api/http/data/%s/%s/sync?%s=")).c_str(), devId, msgId, varName);
  storeToEE(EE_IOT_PATH_140B, buf, 140); // path
  //SERIAL_PORT << "IOT Path: " << buf << endl;
  printJSONConfig("");

  //heap("");
}

void SAP_HCP_IOT_Plugin::cfgHCPIOT2(const char *p) {
  char buf[140];
    p = extractStringFromQuotes(p, buf, sizeof(buf)); // token
  storeToEE(EE_IOT_TOKN_40B, buf, 40);     // token
  //SERIAL_PORT << "IOT OAuth Token: " << buf << endl;
  putJSONConfig(SAP_IOT_TOKEN, buf);

  p = extractStringFromQuotes(p, buf, sizeof(buf)); // button messageid
  //SERIAL_PORT << "-" << buf << "-" << endl;
  putJSONConfig(SAP_IOT_BTN_MSGID, buf);
  printJSONConfig("");


  //heap("");
}

void SAP_HCP_IOT_Plugin::sndHCPIOT(const char *line) {
  char host[60], path[140], token[40];
  EEPROM.get(EE_IOT_HOST_60B, host);
  EEPROM.get(EE_IOT_PATH_140B, path);
  EEPROM.get(EE_IOT_TOKN_40B, token);

  sprintf(path, "%s%s", path, &line[7]);
  if (DEBUG) SERIAL_PORT << F("Sending to HCP: ") << path << endl;
//  SERIAL_PORT << "hcpiot, token: " << token << endl;

  HTTPClient http;
  http.begin(String(HTTPS_STR) + host + path);
  addHCPIOTHeaders(&http, token);
  int rc = AT_FW_Plugin::processResponseCodeATFW(&http, http.POST(""));
  //SERIAL_PORT << "IOT rc: " << http.errorToString(rc).c_str();
  //heap("");
}

void SAP_HCP_IOT_Plugin::addHCPIOTHeaders(HTTPClient *http, const char *token) {
  http->addHeader("Content-Type",  "application/json;charset=UTF-8");
  http->addHeader("Authorization", String("Bearer ") + token);
}

void processMessage(String msgIn) {
  StaticJsonBuffer<200> jsonBuffer;
  char msg[301];
  strncpy(msg, msgIn.c_str(), 300);
  JsonArray& root = jsonBuffer.parseArray(msg);
  //[{"messageType":"42c3546a088b3ef8b8d3","sender":"IoT App","messages":[{"command":"switch on"}]}]

  if (!root.success()) {
    //if server has returned empyy response
    if (DEBUG) SERIAL_PORT << F("parseObject() failed: ") << msgIn << endl;
    return;
  }
  //SERIAL_PORT.print(root[0].is<JsonObject&>());
  //SERIAL_PORT  << "type:" << root.get(0) << endl;
  if (root[0].is<JsonObject&>()) {
    //SERIAL_PORT << root[0].asObject().containsKey("messages") << endl;
    //SERIAL_PORT << root[0]["messages"].is<JsonArray&>() << endl;
    //SERIAL_PORT << root[0]["messages"][0].is<JsonObject&>() << endl;
    //SERIAL_PORT << root[0]["messages"][0].asObject().containsKey("color") << endl;
    //SERIAL_PORT << " Received cmd: " << root[0]["messages"][0]["color"].asString() << endl;
    if (root[0]["messages"][0]["color"].asString()) {
      SERIAL_PORT << F("to process command") << endl;
      //processCommand(root[0]["messages"][0]["color"].asString());
    } else {
      SERIAL_PORT << F("command not recognized") << endl;
     }
  }
//  else {
//    SERIAL_PORT << "no cmd" << endl;
//  }

}
