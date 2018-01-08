#define DO 2093
#define RE 2349
#define MI 2647
#define FA 2793
#define SO 3136
#define LA 3520
#define TI 3951
#define DO_h 4196

int trigPin = 8;
int pingPin = 9;
int speakerPin = 10;

int note = 0;
int music[] = {MI, RE, DO, RE, MI, MI, MI, RE, RE, RE, MI, SO, SO, MI, RE, DO, RE, MI, MI, MI, RE, RE, MI, RE, DO};

int microsecondsToCm(int sec){

	return (double)sec / 29.0 / 2.0;
}

void setup(){

	pinMode(trigPin, OUTPUT);
	pinMode(pingPin, INPUT);
}

void loop(){

	int duration;
	double cm;

	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	duration = pulseIn(pingPin, HIGH);

	cm = microsecondsToCm(duration);

	tone(speakerPin, music[note], 200.0 + 1.2 * cm);
	delay(200.0 + 1.2 * cm);
	note = (note + 1) % (sizeof(music) / sizeof(int));

	if(!note){

		delay(1000);
	}
}