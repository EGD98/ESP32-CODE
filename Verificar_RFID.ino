#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <MFRC522.h>
#include <SPI.h>
#define rst_pin 22  //Esp32
#define ss_pin 21
MFRC522 mfrc522(ss_pin, rst_pin);
//const char* ssid = "INFINITUMC433";
//const char* password = "1866383401";
const char* ssid = "DATA CENTER";
const char* password = "EligioGlez1007@";
String lectUid="";
int ledAccess = 4;  
int ledDeneg = 2;
int button=34;
void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(ledAccess, OUTPUT);
  pinMode(ledDeneg, OUTPUT);
  pinMode (button, INPUT);
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  Serial.print("Conectandose");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Conectado!");
  Serial.println("\n Direccion IP:");
  Serial.print(WiFi.localIP());
  
}

void loop() {
  int lec=digitalRead(button);
  if (lec == 0){
  enviardatos();  
    }     
}
void enviardatos(){
  String rfid = getrfid();
  if(rfid!="SinRFID"){
    Serial.println("RFID: "+rfid);
    if (WiFi.status() == WL_CONNECTED){
                HTTPClient client;
                client.begin("http://sistemasmicrocontrolados.com/api/datos.php?num="+rfid); //pagina a consultar con el metodo GET  
                int httpcode = client.GET();
                if (httpcode > 0){
                  String payload= client.getString();
                  Serial.println("\n Estado del codigo: " + String(httpcode));
                  //Serial.println(payload);//Se muestra en el MS la informacion obtenida de la API que proviene del servidor
                  char json[500];
                  payload.replace(" ","");
                  payload.replace("\n","");
                  payload.trim();
                  payload.remove(0,1);
                  payload.toCharArray(json,500);
                  StaticJsonDocument<200> doc;
                  deserializeJson(doc,json);                  
                  const char* rfid= doc["Rfid"];//obteniendo desde la api el valor de RFID
                  Serial.println(String(rfid) + "\n");//Mostrando en el MS el valor de la variable rfid
                  if(String(rfid)== "autorizado") {
                                  digitalWrite(ledAccess, HIGH);
                                  delay(5000);
                                  digitalWrite(ledAccess, LOW);
                                }
                                else {
                                  digitalWrite(ledDeneg, HIGH);
                                  delay(5000);
                                  digitalWrite(ledDeneg, LOW);
                                }
                  client.end();
                  lectUid="";
                  
                  }
                  else{Serial.println("Error en la peticion http");}
    }else{
      Serial.println("Conexion perdida");
      delay(10000);
      }
  }
}
String getrfid() {
     String Estado= "SinRFID";
     if(! mfrc522.PICC_IsNewCardPresent())
      return String(Estado);
     if(! mfrc522.PICC_ReadCardSerial())
      return String (Estado);
     for(byte i=0; i<mfrc522.uid.size; i++){
      if(mfrc522.uid.uidByte[i]<0x10){
      //Serial.print(" 0"); 
      lectUid += String ("0");
      } 
      else {
       Serial.print("");
       //lectUid += String (" ");  
        }
        //Serial.print(mfrc522.uid.uidByte[i], HEX ); 
        //lectUid[i]=mfrc522.uid.uidByte[i];
        //Serial.print(lectUid[i]);
        
        lectUid += String (mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println("");
      mfrc522.PICC_HaltA();
      return String(lectUid);
}
