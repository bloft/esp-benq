#include <SoftwareSerial.h>
#include "config.h"
#include "EasyMqtt.h"

SoftwareSerial benqSerial(D7, D8, false, 256);

bool set(String cmd, String value) {
	  String command = ("\r*"+cmd+"="+value+"#\r");
		benqSerial.print(command);
	  delay(100);
	  int i = 0;
	  String result = "";
	  while (benqSerial.available() > 0 && i++ < command.length()) {
	    result += (char)benqSerial.read();
	  }
		return true;
}

String get(String cmd) {
  if(set(cmd, "?")) {
		String result = "";
		while (benqSerial.available() > 0) {
			result += (char)benqSerial.read();
		}
	  if(result.indexOf("*") > 0 && result.indexOf("#") > 0) {
	    result = result.substring(result.indexOf("*")+1, result.indexOf("#"));
	    if(result.indexOf("=") > 0) {
	      return result.substring(result.indexOf("=")+1);
	    }
	  }
	}
	return "";
}

EasyMqtt mqtt;

void setup() {
	Serial.begin(115200);
	benqSerial.begin(115200);

  mqtt.wifi(wifi_ssid, wifi_pass);
  mqtt.mqtt(mqtt_server, mqtt_port, mqtt_user, mqtt_password);

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
}
