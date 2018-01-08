#include <IRremote.h>

int RECV_PIN = 10;
int RELAY1_PIN = 11;
int RELAY2_PIN = 12;

IRrecv irrecv(RECV_PIN);
decode_results results;

unsigned int key_codes[] = {0xFD08F7, 0xFD8877, 0xFD48B7, 0xFD28D7, 0xFDA857, 0xFD6897};

void setup(){

	Serial.begin(9600);
	irrecv.enableIRIn();
	pinMode(RELAY1_PIN, OUTPUT);
	pinMode(RELAY2_PIN, OUTPUT);
}

void loop(){

	if(irrecv.decode(&results)){
		
		Serial.print("Received data : 0x");
		Serial.println(results.value, HEX);

		if((results.value & 0xFFFF) == key_codes[0]){

			digitalWrite(RELAY1_PIN, HIGH);
		}
		else if((results.value & 0xFFFF) == key_codes[1]){

			digitalWrite(RELAY1_PIN, LOW);
		}
		else if((results.value & 0xFFFF) == key_codes[2]){

			digitalWrite(RELAY2_PIN, HIGH);
		}
		else if((results.value & 0xFFFF) == key_codes[3]){

			digitalWrite(RELAY2_PIN, LOW);
		}
		else if((results.value & 0xFFFF) == key_codes[4]){

			digitalWrite(RELAY1_PIN, HIGH);
			digitalWrite(RELAY2_PIN, HIGH);
		}
		else if((results.value & 0xFFFF) == key_codes[5]){

			digitalWrite(RELAY1_PIN, LOW);
			digitalWrite(RELAY2_PIN, LOW);
		}

		irrecv.resume();
	}
}