#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define controlPin 15
#define statusPin 13

const char *ssid = "HOME";
const char *password = "T20030617";

ESP8266WebServer server(80);

enum ServerStatus
{
	PowerOff = 0x00,
	PowerOn = 0x01,
	Unknown = 0x02,
};
ServerStatus serverStatus = Unknown;

void shortPress(){
	digitalWrite(controlPin, 1);
	delay(1000);
	digitalWrite(controlPin, 0);
}

void longPress(){
	digitalWrite(controlPin, 1);
	delay(8000);
	digitalWrite(controlPin, 0);
}

void updateStatus()
{
	serverStatus = (ServerStatus)digitalRead(statusPin);
}

void handleRoot()
{

	server.send(200, "text/plain", "Hello from HomeServerManager");
}

void handlePowerOn()
{
	if (serverStatus == PowerOff)
	{
		server.send(200, "text/plain", "Power On");
		shortPress();
	}
	else if(serverStatus == PowerOn){
		server.send(200, "text/plain", "Already Power On");
	}
	else{
		server.send(200, "text/plain", "Error");
	}
}

void handlePowerOff()
{

	if (serverStatus == PowerOn)
	{
		server.send(200, "text/plain", "Power Off");
		shortPress();
	}
	else if(serverStatus == PowerOff){
		server.send(200, "text/plain", "Already Power Off");
	}
	else{
		server.send(200, "text/plain", "Error");
	}
}
void handleForcePowerOff()
{
	if (serverStatus == PowerOn)
	{
		server.send(200, "text/plain", "Force Power Off");
		longPress();
	}
	else if(serverStatus == PowerOff){
		server.send(200, "text/plain", "Already Power Off");
	}
	else{
		server.send(200, "text/plain", "Error");
	}
}
void handleNotFound()
{

	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++)
	{
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

void setup(void)
{
	pinMode(controlPin, OUTPUT);
	pinMode(statusPin, INPUT);

	Serial.begin(115200);
	WiFi.begin(ssid, password);
	Serial.println("");

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	if (MDNS.begin("esp8266"))
	{
		Serial.println("MDNS responder started");
	}

	server.on("/", handleRoot);
	server.on("/on", handlePowerOn);
	server.on("/off", handlePowerOff);
	server.on("/forceoff", handleForcePowerOff);



	server.onNotFound(handleNotFound);

	server.begin();
	Serial.println("HTTP server started");
}

void loop(void)
{
	updateStatus();
	server.handleClient();
}