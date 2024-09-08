#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void initializeWiFi();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void blinkLED(int count, int onTime, int offTime);

#endif