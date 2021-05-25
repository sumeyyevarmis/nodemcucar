
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266WiFi.h>

//wifi ve şifreyi ayarladık
char ssid[] = "Skynet";
char pass[] = "Doritos0603";

//Pinleri tanımladık
int STBY = 2;
//Motor A
int PWMA = 5; //hız kontrol
int AIN2 = 0; //yön
int AIN1 = 4; //yön
//Motor B
int PWMB = 13; //hız kontrol
int BIN2 = 14; //yön 
int BIN1 = 12; //yön

ESP8266WebServer server (80);

//Bu işlev, URL'de iletilen parametreleri alır(joystik çubuğunun x ve y koordinatları)
//ve motor hızını bu parametrelere göre ayarlar.
void handleJSData(){
  boolean yDir;
  int x = server.arg(0).toInt() * 10;
  int y = server.arg(1).toInt() * 10;
  int aSpeed = abs(y);
  int bSpeed = abs(y);
  //y yönünü negatif veya pozitif olmasına göre ayarlar
  if ( y < 0 ){
    yDir = 0; 
  }
  else { 
    yDir = 1;
  }  
  
  //x eksenine bağlı olarak her motorun hızı ayarlanır
  //bir motoru yavaşlatrı ve diğerini dönüş miktarına bağlı olarak orantılı şekilde hızlandırır

  aSpeed = constrain(aSpeed + x/2, 0, 1023);
  bSpeed = constrain(bSpeed - x/2, 0, 1023);

 
  digitalWrite(STBY, HIGH);  
  //MotorA
  digitalWrite(AIN1, !yDir);
  digitalWrite(AIN2, yDir);
  analogWrite(PWMA, aSpeed);
  //MotorB
  digitalWrite(BIN1, !yDir);
  digitalWrite(BIN2, yDir);
  analogWrite(PWMB, bSpeed);

  //HTTP 200 döndürür
  server.send(200, "text/plain", "");   
}

void setup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // pinleri çıktı olarak ayarlarız
  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT); 
  
  // Hata ayıklama konsolu için
  Serial.begin(9600);
  
  //Statik HTML dosyalarını sunabilmek için SPIFFS'yi başlatırız.
  if (!SPIFFS.begin()){
    Serial.println("SPIFFS Mount failed");
  } 
  else {
    Serial.println("SPIFFS Mount succesfull");
    Serial.println(WiFi.localIP());
  }
  //html ve js için SPIFFS'deki statik sayfaları ayarladık
  server.serveStatic("/", SPIFFS, "/joystick.html"); 
  server.serveStatic("/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //Bu URL'ye html dosyasındaki js tarafından erişildiğinde handleJSData çağırdık
  server.on("/jsData.html", handleJSData);  
  server.begin();
}

void loop()
{
  server.handleClient();  
}
