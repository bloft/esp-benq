#include <SoftwareSerial.h>
#include "config.h"
#include <EasyMqtt.h>
#include <RemoteDebug.h>

SoftwareSerial benqSerial(D7, D8, false, 256);

RemoteDebug Debug;

String readContent(int maxLength) {
	String result = "";
	boolean ascii = true;
  int i = 0;
	while (benqSerial.available() > 0 && i++ < maxLength) {
		char c = (char)benqSerial.read();
		if(!isAscii(c)) ascii = false;
		result += c;
	}
	if(ascii && result.indexOf("*") > 0 && result.indexOf("#") > 0) {
		result = result.substring(result.indexOf("*"), result.indexOf("#")+1);
		return result;
	} else {
    Debug.print("Result: ");
    Debug.println(result);
		Debug.println("Failed - Reading avalible data");
		// Invalid data, empth serial buffer
		while (benqSerial.available() > 0) {
			char c = (char)benqSerial.read();
			Debug.print(c);
		}
    Debug.println();
	}
	return "";
}

/**
 * Write a command to the projector
 * using ? as value for request a read
 */
bool set(String cmd, String value) {
	String command = ("\r*"+cmd+"="+value+"#\r");
	Debug.println(command);
	benqSerial.print(command);
	delay(10);
	String result = readContent(command.length());
	return result != "";
}

/**
 * Read out a value from the projector
 * Failed to read the data will result in a empty string
 */
String get(String cmd) {
	if(set(cmd, "?")) {
		String result = readContent(256);
		if(result.indexOf("*") >= 0 && result.indexOf("#") > 0) {
			result = result.substring(result.indexOf("*")+1, result.indexOf("#"));
			if(result.indexOf("=") > 0) {
        Debug.print("Success: ");
        Debug.println(result.substring(result.indexOf("=")+1));
				return result.substring(result.indexOf("=")+1);
			} else {
        Debug.println("Failed - Missing =");
        Debug.println(result);
			}
		} else {
      Debug.println("Failed - Missing */#");
      Debug.println(result);
		}
	}
	return "";
}

EasyMqtt mqtt;

void setup() {
	Serial.begin(115200);
	benqSerial.begin(115200);

  Debug.begin("Telnet_HostName");
  Debug.setResetCmdEnabled(true);

	mqtt.wifi(wifi_ssid, wifi_pass);
	mqtt.mqtt(mqtt_server, mqtt_port, mqtt_user, mqtt_pass);

	mqtt.setInterval(15);

	mqtt["model"] << [](){ return get("modelname"); };

	mqtt["power"] << [](){ return get("pow"); };
	mqtt["power"]["set"] >> [](String value){ set("pow", value); };
  
	mqtt["source"] << [](){ return get("sour"); };
	mqtt["source"]["set"] >> [](String value){ set("sour", value); };


	mqtt["blank"] << [](){ return get("blank"); };
	mqtt["blank"]["set"] >> [](String value){ set("blank", value); };
 
	mqtt["picture"]["mode"] << [](){ return get("appmod"); };
	mqtt["picture"]["mode"]["set"] >> [](String value){ set("appmod", value); };
  
	mqtt["picture"]["contrast"] << [](){ return get("con"); };
	mqtt["picture"]["contrast"]["set"] >> [](String value){ set("con", value); };
	mqtt["picture"]["brightness"] << [](){ return get("bri"); };
	mqtt["picture"]["brightness"]["set"] >> [](String value){ set("bri", value); };
	mqtt["picture"]["color"] << [](){ return get("color"); };
	mqtt["picture"]["color"]["set"] >> [](String value){ set("color", value); };
	mqtt["picture"]["sharpness"] << [](){ return get("sharp"); };
	mqtt["picture"]["sharpness"]["set"] >> [](String value){ set("sharp", value); };
	mqtt["picture"]["aspect"] << [](){ return get("asp"); };
	mqtt["picture"]["aspect"]["set"] >> [](String value){ set("asp", value); };
  

	mqtt["audio"]["volume"] << [](){ return get("vol"); };
	mqtt["audio"]["volume"]["set"] >> [](String value){ set("vol", value); };
	mqtt["audio"]["mute"] << [](){ return get("mute"); };
	mqtt["audio"]["mute"]["set"] >> [](String value){ set("mute", value); };
  
	mqtt["lamp"]["time"] << [](){ return get("ltim"); };
	mqtt["lamp"]["mode"] << [](){ return get("lampm"); };
	mqtt["lamp"]["mode"]["set"] >> [](String value){ set("lampm", value); };
}

void loop() {
	mqtt.loop();
  Debug.handle();
}
