//RECEPTOR DE ESTACION METEREOLOGICA 

#include <SoftwareSerial.h>
#include <ThingSpeak.h>
#include <WiFi.h>

String lora_band = "915000000"; 
String lora_networkid = "5";    // Lora Network ID
String lora_address = "2";      // Lora address
String lora_RX_address = "1";   // Lora RX address (para respuesta)

const char* ssid = "ssid";                        //SSID de router.
const char* password = "password";                //Contraseña de router.

unsigned long channelID = 9999999;                //ID del canal.
const char* WriteAPIKey = "API Key";     //Write API Key del canal.

SoftwareSerial mySerial(18, 19); // RX, TX 

WiFiClient cliente;

// Variables para almacenar los datos del sensor
String YL69 = "";
String SHT20T = ""; //Temperatura
String SHT20H = ""; //Humedad

String SI7021T = ""; //Temperatura
String SI7021H = ""; //Humedad
String VelV = "";

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  // Configuracion del Wifi
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado!");

  ThingSpeak.begin(cliente); //Inicialiar el servidor

  // Configuración del módulo RYLR998
  Serial.println("Configurando el módulo RYLR998...");
  delay(1500);

  mySerial.println("AT+BAND=" + lora_band);
  delay(500);
  mySerial.println("AT+ADDRESS=" + lora_address);
  delay(500);
  mySerial.println("AT+NETWORKID=" + lora_networkid);
  delay(1500);
  mySerial.println("AT+IPR=9600");
  delay(1500);

  Serial.println("Configuración completada.");


}

void loop() {
  if (mySerial.available()) {
    String receivedData = mySerial.readString();
    //Serial.println("Datos recibidos: " + receivedData);  //CADENA COMPLETA DE DATOS 
    
    // Extraer y actualizar los datos del sensor
    int Sdata = 0; //StarIndex 
    int Edata = receivedData.indexOf(','); //EndIndex
    YL69 = receivedData.substring(Sdata, Edata);

    //SHT20
    Sdata = Edata + 1;
    Edata = receivedData.indexOf(',', Sdata);
    String SHT20 = receivedData.substring(Sdata, Edata);
    int Separador = SHT20.indexOf('|');
    SHT20T = SHT20.substring(0, Separador);
    SHT20H = SHT20.substring (Separador + 1);

    //SI7021
    Sdata = Edata + 1;
    Edata = receivedData.indexOf(',', Sdata);
    String SI7021 = receivedData.substring(Sdata, Edata);
    Separador = SI7021.indexOf('|');
    SI7021T = SI7021.substring(0, Separador);
    SI7021H = SI7021.substring (Separador + 1);

    //ANEMOMETRO
    Sdata = Edata + 1;
    VelV = receivedData.substring(Sdata);

    // Imprimir los datos del sensor
    Serial.println("YL69: " + YL69);
    Serial.println("SHT20: " + SHT20);
    Serial.println("SI7021: " + SI7021);
    Serial.println("Anemometro: " + VelV);

    ThingSpeak.writeFields(channelID,WriteAPIKey);
    Serial.println("Datos enviados a ThingSpeak!");
    //delay(14000); // 14 Segundo que soporta ThingSpeak en la version Free

    // Asigna a campos
    ThingSpeak.setField (1,YL69);
    ThingSpeak.setField (2,SHT20T);
    ThingSpeak.setField (3,SHT20H);
    ThingSpeak.setField (4,SI7021T);
    ThingSpeak.setField (5,SI7021H);
    ThingSpeak.setField (6,VelV);

  } else {
    Serial.println("Esperando datos...");
  }
  delay(1000);
}
