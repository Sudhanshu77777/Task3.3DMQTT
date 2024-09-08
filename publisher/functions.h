#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void initializeWiFi();
void reconnectMQTT();
int readUltrasonicSensor();
void publishMessage(const char* topic, const char* message);
void waitForMQTTConnection();

#endif
