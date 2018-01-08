#include <SPI.h>
#include <dht11.h>
#include <SoftwareSerial.h>

#define LED_R 9
#define LED_G 10
#define LED_B 11
#define DHT11PIN 13
#define CACTUS 0 //습도 10 온도 35
#define LETTUCE 1 // 습도 20 온도 17
#define LAVENDER 2 // 습도 20 온도 22

dht11 DHT11;
SoftwareSerial uart(4, 5);
//SoftwareSerial xbee(4, 5);

char id = 1;
char mode = 0;
char MODENUM = 3;

int good_temp;
int good_humi;

volatile int state = HIGH;

void setup() {

	Serial.begin(9600);
	uart.begin(9600);
	//xbee.begin(9600);
	attachInterrupt(0, change_mode, RISING);
}

void change_mode() {

	state = !state;
	mode = (mode + 1) % MODENUM;
}

void loop() {

	int humidity;
	int temperature;

	if (mode == CACTUS) {

		good_temp = 35;
		good_humi = 10;
	}
	else if (mode == LETTUCE) {

		good_temp = 17;
		good_humi = 20;
	}
	else if (mode == LAVENDER) {

		good_temp = 22;
		good_humi = 20;
	}

	if (DHT11.read(DHT11PIN) == DHTLIB_OK) {

		humidity = DHT11.humidity;
		temperature = DHT11.temperature;

		if (mode == CACTUS) {

			Serial.println("mode : Cactus");
		}
		else if (mode == LETTUCE) {

			Serial.println("mode : Lecttuce");
		}
		else if (mode == LAVENDER) {

			Serial.println("mode : Lavender");
		}

		Serial.print("Optimal temperature : ");
		Serial.println(good_temp);
		Serial.print("Optimal humidity : ");
		Serial.println(good_humi);
		Serial.print("Temperature : ");
		Serial.println(temperature);
		Serial.print("Humidity : ");
		Serial.println(humidity);
	}

	if (good_temp - 5.0 < temperature && temperature < good_temp + 5.0 \
		&& good_humi - 10.0 < humidity && humidity < good_humi + 10.0) {

		analogWrite(LED_R, 0);
		analogWrite(LED_G, 127);
		analogWrite(LED_B, 0);
		Serial.println("Status : Good");
	}
	else if (good_temp - 10.0 < temperature && temperature < good_temp + 10.0 \
		&& good_humi - 20.0 < humidity && humidity < good_humi + 20.0) {

		analogWrite(LED_R, 127);
		analogWrite(LED_G, 127);
		analogWrite(LED_B, 0);
		Serial.println("Status : Warning");
	}
	else {

		analogWrite(LED_R, 127);
		analogWrite(LED_G, 0);
		analogWrite(LED_B, 0);
		Serial.println("Status : Danger");
	}

	Serial.println();
	
	uart.print(id);
	uart.print(mode);
	uart.print((temperature / 10));
	uart.print((temperature % 10));
	uart.print((humidity / 10));
	uart.print((humidity % 10));

	/*

	xbee.print(id);
	xbee.print(mode);
	xbee.print((temperature / 10));
	xbee.print((temperature % 10));
	xbee.print((humidity / 10));
	xbee.print((humidity % 10));
	
	*/

	delay(1000);
}
