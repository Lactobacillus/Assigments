#include <SPI.h>
#include <WiFi.h>
#include <SoftwareSerial.h>

#define NUM 1

#define CACTUS 0 //습도 10 온도 35
#define LETTUCE 1 // 습도 20 온도 17
#define LAVENDER 2 // 습도 20 온도 22

char ssid[] = "abc";													// 네트워크의 SSID
char pass[] = "12345678"; 												// AP의 패스워드
int status = WL_IDLE_STATUS;

int temperatures[NUM + 1];
int humidities[NUM + 1];

int id = 0;
int mode = 0;

WiFiServer server(80);
SoftwareSerial uart(2, 3);
//SoftwareSerial xbee(2, 3);

void setup() {

	Serial.begin(9600);
	uart.begin(9600);
	//xbee.begin(9600);

	
	if (WiFi.status() == WL_NO_SHIELD) {								// 와이파이 쉴드 검사

		Serial.println("** WiFi shield NOT present.");
		while (true);
	}
	
	while(status != WL_CONNECTED) {

		Serial.print("* Attempting to connect to \"");
		Serial.print(ssid);
		Serial.println("\"");

		status = WiFi.begin(ssid, pass);								// WPA/WPA2 네트워크에 연결

		delay(3000);													// 접속 대기
	}
	
	Serial.print("* You're connected to the network \"");
	Serial.print(ssid);
	Serial.println("\"");

	server.begin();														// 웹 서버 시작
	Serial.print("* Web Server Address : ");							// 웹 서버 주소출력
	Serial.println(WiFi.localIP());
}

void loop() {

	int humidity;
	int temperature;

	if (uart.available() > 6) {

		id = uart.read();
		mode = uart.read();

		char a = uart.read();
		char b = uart.read();
		temperature = 10 * (a - '0') + (b - '0');

		a = uart.read();
		b = uart.read();
		humidity = 10 * (a - '0') + (b - '0');

		temperatures[id] = temperature;
		humidities[id] = humidity;

		Serial.println(id);
		Serial.println(mode);
		Serial.println(temperature);
		Serial.println(humidity);
		Serial.println();
	}

	/*

	if (xbee.available() > 6) {
		
		id = xbee.read();
		mode = xbee.read();

		char a = xbee.read();
		char b = xbee.read();
		temperature = 10 * (a - '0') + (b - '0');

		a = xbee.read();
		b = xbee.read();
		humidity = 10 * (a - '0') + (b - '0');

		temperatures[id] = temperature;
		humidities[id] = humidity;

		Serial.println(id);
		Serial.println(mode);
		Serial.println(temperature);
		Serial.println(humidity);
		Serial.println();
	}

	*/

	// 아두이노의 웹 서버로 요청을 보낸 클라이언트 검사
	WiFiClient client = server.available();

	if(client) {														// 클라이언트가 존재하는 경우
		
		while(client.available()) {

			client.read();												// 클라이언트의 요청은 읽어서 버림
		}
		
		// HTTP 메시지 전송
		client.println("HTTP/1.1 200 OK");
		client.println("Content-Type: text/html");
		client.println("Connection: close");  							// 응답 전송 후 연결 종료
		client.println("Refresh: 3");   								// 3초 마다 웹 페이지 자동 갱신
		client.println();
		client.println("<!DOCTYPE HTML>");
		client.println("<html>");

		for (int idx = 0; idx < NUM; idx++) {
	
			client.print("Farm - ");
			client.println(idx);
			client.println("</br>");

			if (mode == CACTUS) {

				client.print("Type : Cactus");
				client.println("</br>");
				client.println("Optimal Temperature : 35℃");
				client.println("</br>");
				client.println("Optimal humidity : 10%%");
				client.println("</br>");
			}
			else if (mode == LETTUCE) {

				client.print("Type : Lettuce");
				client.println("</br>");
				client.println("Optimal Temperature : 17℃");
				client.println("</br>");
				client.println("Optimal humidity : 20%%");
				client.println("</br>");
			}
			else if (mode == LAVENDER) {

				client.print("Type : Lavender");
				client.println("</br>");
				client.println("Optimal Temperature : 22℃");
				client.println("</br>");
				client.println("Optimal humidity : 20%%");
				client.println("</br>");
			}
			
			client.print("Current temperature : ");
			client.println(temperatures[idx]);
			client.println("</br>");
			client.print("Current humidity : ");
			client.println(humidities[idx]);
			client.println("</br>");
		}

		client.println("</html>");
		client.println();

		delay(1);

		client.stop();
	}
}
