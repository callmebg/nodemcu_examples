#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_ssd1306syp.h>
#include <CN_SSD1306.h>
#include <string.h>
#include <SimpleDHT.h>

#define pinDHT11 4  //dht11 的2号脚与esp的4号角相连，板上标注为D2
#define SDA_PIN 14  //oled 上面的sda与esp的14号角相连，板上标注为D5
#define SCL_PIN 12  //oled 上面的scl与esp的12号角相连，板上标注为D6
#define STASSID "TP-LINK_465C3A"  //wifi的名字
#define STAPSK  "123456789"       //wifi密码

SimpleDHT11 dht11(pinDHT11);    //一个dht11的对象
ESP8266WebServer server(80);    //一个web对象
Adafruit_ssd1306syp display(SDA_PIN,SCL_PIN); //一个显示屏对象

unsigned char localIp[5] = "....";  //esp的ip
String msg = "test";                //显示文字内容
byte temperature = 0;               //温度 ℃
byte humidity = 0;                  //湿度 %
int err = SimpleDHTErrSuccess;      //dht11 错误信息

void change(uint32_t ip)  //翻译ip
{
  unsigned char* t = (unsigned char*)&ip;
  for(unsigned char i = 0; i < 4; i++)
  {
    localIp[i] = t[i]; 
  }
}
void homepage() 
{
  String content = "<html><head><title>hhh 我写出来了</title><meta charset='UTF-8'></head>";
    content += "<body><form action='/' method='GET'>项目地址：https://github.com/callmebg/nodemcu_examples<br>";
    content += "当前温度:" + String(temperature)+ "℃<br>";
    content += "当前湿度:" + String(humidity) + "%<br>";
    content += "输入想要显示的内容:<input type='text' name='words' placeholder='" + msg +"'><br>";
    content += "<input type='submit' name='SUBMIT' value='Submit'></form></body></html>";
  if (server.hasArg("words")) 
  {
    Serial.println("words");
    msg = server.arg("words");
  }
  server.send(200, "text/html", content);
  Serial.println("用户访问了主页");
}
void ifnotfound() 
{
  server.send(404, "text/html", "<html><head><title>见鬼了</title><meta charset='UTF-8'></head></html>嘤嘤嘤！你访问的页面被外星人劫持了哦");
  Serial.println("用户访问了一个不存在的页面");
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  
  //显示屏初始化
  display.initialize();
  display.setTextColor(WHITE); 
  display.println("wait a minute...");
  display.update();
  
  //wifi配置
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.print(WiFi.localIP());
  change(WiFi.localIP());
  
  //初始化WebServer
  server.on("/", homepage); //主页
  server.onNotFound(ifnotfound);  //404
  server.begin();
  Serial.println("HTTP server started");
  
}
void loop()
{
  unsigned char i;
  display.setCursor(0,0);
  display.setTextSize(1);
  //监听客户请求并处理
  server.handleClient();
  //读取温湿度
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  //画面
  display.clear();
  display.print("DHT11: ");
  display.print(temperature);
  display.print("*C ");
  display.print((int)humidity);
  display.println("%");
  display.print("IP: ");
  display.print(localIp[0]);
  display.print(".");
  display.print(localIp[1]);
  display.print(".");
  display.print(localIp[2]);
  display.print(".");
  display.println(localIp[3]);
  display.setTextSize(2);
  display.println(msg);
  display.update();
  delay(500);//2Hz的频率
}
