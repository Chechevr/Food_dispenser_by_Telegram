#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Servo.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Definir los pines para el sensor HC-SR04
const int trigPin = 2;
const int echoPin = 15;

// Definir la distancia mínima permitida
const int distanciaMinima = 8; // en centímetros

Servo myservo;  // crea el objeto servo

// GPIO de salida del servo
static const int servoPin = 13;

// Reemplazar con tus credenciales de Wifi
const char* ssid     = "NombreDeRed";
const char* password = "Contraseña";

// Inicializa Bot Telegram
#define BOTtoken "TuToken"  // Tu Bot Token (Obtener de Botfather)

#define CHAT_ID "123456"

WiFiClientSecure client;

UniversalTelegramBot bot(BOTtoken, client);

void handleNewMessages(int numNewMessages) {

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Usuario no autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;

    if (text == "/AlimentarGranja1") {
      // Leer la distancia del sensor HC-SR04
      long duration, distancia;
      digitalWrite(trigPin, LOW);  // Establecer el pin trigPin en LOW
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);  // Establecer el pin trigPin en HIGH por 10µs
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);  // Leer el tiempo de duración del pulso en echoPin
      distancia = duration / 58.2;  // Convertir el tiempo de duración a centímetros
      
      if (distancia > distanciaMinima) {
        bot.sendMessage(chat_id, "Nivel de alimento bajo", "");
      }
      
      bot.sendMessage(chat_id, "AlimentandoGranja1", "");
      myservo.write(90);             
      delay(200); // Tiempo que se tarda manteniendo para dispensar comida                      
      myservo.write(0);              
    }
  }
}

void setup() {
  Serial.begin(115200);

  myservo.attach(servoPin);  // vincula el servo en el servoPin

  // Configurar los pines del sensor HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Conecta a red WiFi con SSID y password
  Serial.print("Conectado a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Muestra IP local 
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot iniciado", "");
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while(numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
  
}
