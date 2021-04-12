//add notifications and emails
//dht 5v
//soil hum 3.3V

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <TimeLib.h>
#include <WidgetRTC.h>
#include <DHT.h>

char auth[] = "L1FhebeIGzlkrjG7HruJISt54T_gIU_8";

char ssid[] = "MEZON_8EB306";
char pass[] = "20715318";

BlynkTimer timer;
WidgetRTC rtc;

#define DHTPIN D7

#define DHTTYPE DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);

int sensorPin = A0;
int sensorValue;

const int RELAY_PIN = D0; //LED
const int RELAY_PIN1 = D1;//pump
const int RELAY_PIN2 = D2; //fan
const int RELAY_PIN3 = D3; //heater

int pinModeValue1 = 0;

int Shour;
int Smin;
int Ss;
int Ehour;
int Emin;
int Es;

int p = 0;
int i = 0;
int j = 0;

// Digital clock display of the time
void clockDisplay()
{
  if (pinModeValue1 == 0) {
    String currentTime = String(hour()) + ":" + minute() + ":" + second();
    String currentDate = String(day()) + " " + month() + " " + year();
    int h = hour();
    int m = minute();
    Serial.print("Current time: ");
    Serial.print(currentTime);
    Serial.print(" ");
    Serial.print(currentDate);
    Serial.println();

    //works only if shour is less than end hour and for all days (make two if blocks comparing start and end hours and wrap code below in it), if want to do different days, add function(top of programme) to see which day is it and then compare,
    if (h > Shour && h < Ehour) {
      digitalWrite(RELAY_PIN, LOW);
    }
    if (h < Shour && h < Ehour) {
      digitalWrite(RELAY_PIN, HIGH);
    }
    if (h > Shour && h > Ehour) {
      digitalWrite(RELAY_PIN, HIGH);
    }
    if (h == Ehour && h == Shour ) {
      if (m >= Emin || m < Smin) {
        digitalWrite(RELAY_PIN, HIGH);
      }
      if (m < Emin && m >= Smin) {
        digitalWrite(RELAY_PIN, LOW);
      }
    }
    if (h == Ehour && h > Shour ) {
      if (m >= Emin) {
        digitalWrite(RELAY_PIN, HIGH);
      }
      if (m < Emin) {
        digitalWrite(RELAY_PIN, LOW);
      }
    }
    if (h < Ehour && h == Shour ) {
      if ( m < Smin) {
        digitalWrite(RELAY_PIN, HIGH);
      }
      if ( m >= Smin) {
        digitalWrite(RELAY_PIN, LOW);
      }
    }
  }
}

BLYNK_WRITE(V1) {

  TimeInputParam t(param);

  if (t.hasStartTime())
  {
    Serial.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
    delay(500);
    Shour = t.getStartHour();
    //  Blynk.virtualWrite(V7, Shour);
    Smin = t.getStartMinute();
    //  Blynk.virtualWrite(V8, Smin);
    Ss =  t.getStartSecond();
    //  Blynk.virtualWrite(V9, Ss);
    delay(1000);
  }
  else if (t.isStartSunrise())
  {
    Serial.println("Start at sunrise");
  }
  else if (t.isStartSunset())
  {
    Serial.println("Start at sunset");
  }
  else
  {
  }

  if (t.hasStopTime())
  {
    Serial.println(String("Stop: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());
    delay(500);
    Ehour = t.getStopHour();
    //  Blynk.virtualWrite(V10, Ehour);
    Emin = t.getStopMinute();
    //Blynk.virtualWrite(V11, Emin);
    Es = t.getStopSecond();
    // Blynk.virtualWrite(V12, Es);
  }
  else if (t.isStopSunrise())
  {
    Serial.println("Stop at sunrise");
  }
  else if (t.isStopSunset())
  {
    Serial.println("Stop at sunset");
  }
  else
  {
    // Do nothing: no stop time was set
  }

  Serial.println(String("Time zone: ") + t.getTZ());

  Serial.println(String("Time zone offset: ") + t.getTZ_Offset());

  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      Serial.println(String("Day ") + i + " is selected");
    }
  }
  Serial.println();
}

void sendSensor()
{

  int average = 0;
  for (int i = 0; i < 10; i++) {
    sensorValue = analogRead(sensorPin);
    average = average + sensorValue;
    delay(400);
  }
  int  sensorValue1 = average / 10;

  Blynk.virtualWrite(V2, sensorValue1);


  if (pinModeValue1 == 0) {

     if (sensorValue1 <= 500) {
      digitalWrite (RELAY_PIN1, HIGH);
      i = 0;
    }
    else if (sensorValue1 > 800) {
      digitalWrite (RELAY_PIN1, LOW);
      i = i + 1;
    }
  }
  Serial.println(i);
  if (i > 10) {
    i = 0;
    Blynk.notify("tooo dry!!");
  }

}

void sendSensor1()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V3, h);
  Serial.println(h);
  Blynk.virtualWrite(V4, t);

  Serial.println(t);
  Serial.println("----------------");

  if (pinModeValue1 == 0) {

    if (t >= 28) {
      digitalWrite (RELAY_PIN2, LOW);
      digitalWrite (RELAY_PIN3, HIGH);
    }
    else if (t > 23 && t < 28) {
      digitalWrite (RELAY_PIN2, HIGH);
      digitalWrite (RELAY_PIN3, HIGH);
    }
    else if (t <= 23) {
      digitalWrite (RELAY_PIN2, HIGH);
      digitalWrite (RELAY_PIN3, LOW);
      j = j + 1;
    }
  }

  if (j > 20) {
    j = 0;
    Blynk.notify("tooo cold!!");
  }


}

BLYNK_WRITE(V0) {
  switch (param.asInt())
  {
    case 1: { // Item 1
        pinModeValue1 = 0;
            Serial.println("auto");
        break;
      }
    case 2: { // Item 2
        pinModeValue1 = 1;
            Serial.println("manual");
        break;
      }
  }
}

BLYNK_WRITE(V5)
{
  int pinValue = param.asInt();
  if ( pinValue = 1) {
    if (int(pinModeValue1)  % 2 == 1) {
           if (p == 1) {
        digitalWrite(RELAY_PIN, LOW);
        
      }
     else if (p == 2) {
        digitalWrite(RELAY_PIN1, LOW);
      }

      else if (p == 3) {
        digitalWrite(RELAY_PIN2, LOW);
      }
      else if (p == 4) {
        digitalWrite(RELAY_PIN3, LOW);
      }
    }
  }
}

BLYNK_WRITE(V6)
{
  int pinValue = param.asInt();
  if ( pinValue = 1) {
    if (int(pinModeValue1)  % 2 == 1) {
      if (p == 1) {
        digitalWrite(RELAY_PIN, HIGH);
      }
    if (p == 2) {
        digitalWrite(RELAY_PIN1, HIGH);
      }

      else if (p == 3) {
        digitalWrite(RELAY_PIN2, HIGH);
      }
      else if (p == 4) {
        digitalWrite(RELAY_PIN3, HIGH);
      }
    }
  }
}

BLYNK_WRITE(V7)
{
  switch (param.asInt()) {
    case 1:
      {
        p = 1;
        break;
      }
    case 2:
      {
        p = 2;
        break;
      }
    case 3:
      {
        p = 3;
        break;
      }
    case 4:
      {
        p = 4;
        break;
      }

  }
}


void notifyUptime()
{
  long uptime = millis() / 60000L;


  //


}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

void setup()
{
  // Debug console
  Serial.begin(9600);
 Serial.println("9600");
 
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  pinMode(RELAY_PIN1, OUTPUT);
  digitalWrite(RELAY_PIN1, HIGH);
  pinMode(RELAY_PIN2, OUTPUT);
  digitalWrite(RELAY_PIN2, HIGH);
  pinMode(RELAY_PIN3, OUTPUT);
  digitalWrite(RELAY_PIN3, HIGH);

  dht.begin();
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)

  // Display digital clock every 10 seconds
  timer.setInterval(60000L, clockDisplay);//1NULIS
  timer.setInterval(10000L, notifyUptime);
  timer.setInterval(3600000L, sendSensor); //2 NULIAI
  timer.setInterval(1800000L, sendSensor1);
}

void loop()
{
  Blynk.run();
  timer.run();
}
