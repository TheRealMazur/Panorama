#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "A4988.h"
#define MOTOR_STEPS 200
#define DIR D1
#define STEP D2
#define ENBL D4
#define MS1 D5
#define MS2 D6
#define MS3 D7
#define RELAY D0
#define LEDPIN D8
A4988 stepper(MOTOR_STEPS, DIR, STEP, ENBL, MS1, MS2, MS3);

char speed;
char msteps;

bool panorama_flag;
bool revolution_flag;
bool shutter_flag;

int pano_angle;
int Revolution_speed;
int Revolution_number;
int Direction;
int delayTime;
int rpm;


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
String html = "";

void shutter(){		//close the relay for half a second
  digitalWrite(RELAY,1);
  delay(500);
  digitalWrite(RELAY,0);
}

void rewrite(){		//update the html site text using raw string literral to simplify the code
  html = R"=====(
<!DOCTYPE HTML>
<html>
   <head>
      <title>Panorama</title>
      <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
      <style>html{font-family: sans-serif; -webkit-text-size-adjust: 100%; -ms-text-size-adjust: 100%;}body{margin: 0;}hr{height: 0; -webkit-box-sizing: content-box; -moz-box-sizing: content-box; box-sizing: content-box;}button, input, select{margin: 0; font: inherit; color: inherit;}button{overflow: visible;}button, select{text-transform: none;}button, html input[type="button"], input[type="reset"], input[type="submit"]{-webkit-appearance: button; cursor: pointer;}button[disabled], html input[disabled]{cursor: default;}button::-moz-focus-inner, input::-moz-focus-inner{padding: 0; border: 0;}input{line-height: normal;}*:before, *:after{-webkit-box-sizing: border-box; -moz-box-sizing: border-box; box-sizing: border-box;}html{font-size: 10px; -webkit-tap-highlight-color: rgba(0, 0, 0, 0);}body{font-family: "Helvetica Neue", Helvetica, Arial, sans-serif; font-size: 14px; line-height: 1.42857143; color: #333; background-color: #fff;}input, button, select{font-family: inherit; font-size: inherit; line-height: inherit;}hr{margin-top: 20px; margin-bottom: 20px; border: 0; border-top: 1px solid #eee;}h2, h4{font-family: inherit; font-weight: 500; line-height: 1.1; color: inherit;}h2, h3{margin-top: 20px; margin-bottom: 10px;}h4{margin-top: 10px; margin-bottom: 10px;}h2{font-size: 30px;}h4{font-size: 18px;}.text-center{text-align: center;}.container{padding-right: 15px; padding-left: 15px; margin-right: auto; margin-left: auto;}@media (min-width: 768px){.container{width: 750px;}}@media (min-width: 992px){.container{width: 970px;}}@media (min-width: 1200px){.container{width: 1170px;}}label{display: inline-block; max-width: 100%; margin-bottom: 5px; font-weight: bold;}input[type="checkbox"]{margin: 4px 0 0; margin-top: 1px \9; line-height: normal;}input[type="file"]{display: block;}select[multiple], select[size]{height: auto;}input[type="checkbox"]:focus{outline: 5px auto -webkit-focus-ring-color; outline-offset: -2px;}.form-control{display: block; width: 100%; height: 34px; padding: 6px 12px; font-size: 14px; line-height: 1.42857143; color: #555; background-color: #fff; background-image: none; border: 1px solid #ccc; border-radius: 4px; -webkit-box-shadow: inset 0 1px 1px rgba(0, 0, 0, .075); box-shadow: inset 0 1px 1px rgba(0, 0, 0, .075); -webkit-transition: border-color ease-in-out .15s, -webkit-box-shadow ease-in-out .15s; -o-transition: border-color ease-in-out .15s, box-shadow ease-in-out .15s; transition: border-color ease-in-out .15s, box-shadow ease-in-out .15s;}.form-control:focus{border-color: #66afe9; outline: 0; -webkit-box-shadow: inset 0 1px 1px rgba(0,0,0,.075), 0 0 8px rgba(102, 175, 233, .6); box-shadow: inset 0 1px 1px rgba(0,0,0,.075), 0 0 8px rgba(102, 175, 233, .6);}.form-control::-moz-placeholder{color: #999; opacity: 1;}.form-control:-ms-input-placeholder{color: #999;}.form-control::-webkit-input-placeholder{color: #999;}.form-control::-ms-expand{background-color: transparent; border: 0;}@media(min-width: 768px){.form-inline .form-group{display: inline-block; margin-bottom: 0; vertical-align: middle;}.form-inline .form-control{display: inline-block; width: auto; vertical-align: middle;}.form-inline .form-control-static{display: inline-block;}.form-inline .input-group{display: inline-table; vertical-align: middle;}.form-inline .input-group .input-group-addon, .form-inline .input-group .input-group-btn, .form-inline .input-group .form-control{width: auto;}.form-inline .input-group > .form-control{width: 100%;}.form-inline .control-label{margin-bottom: 0; vertical-align: middle;}.form-inline .radio, .form-inline .checkbox{display: inline-block; margin-top: 0; margin-bottom: 0; vertical-align: middle;}.form-inline .radio label, .form-inline .checkbox label{padding-left: 0;}.form-inline .radio input[type="radio"], .form-inline .checkbox input[type="checkbox"]{position: relative; margin-left: 0;}.form-inline .has-feedback .form-control-feedback{top: 0;}}.btn{display: inline-block; padding: 6px 12px; margin-bottom: 0; font-size: 14px; font-weight: normal; line-height: 1.42857143; text-align: center; white-space: nowrap; vertical-align: middle; -ms-touch-action: manipulation; touch-action: manipulation; cursor: pointer; -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-image: none; border: 1px solid transparent; border-radius: 4px;}.btn:focus, .btn:active:focus, .btn:active.focus{outline: 5px auto -webkit-focus-ring-color; outline-offset: -2px;}.btn:hover, .btn:focus{color: #333; text-decoration: none;}.btn:active{background-image: none; outline: 0; -webkit-box-shadow: inset 0 3px 5px rgba(0, 0, 0, .125); box-shadow: inset 0 3px 5px rgba(0, 0, 0, .125);}.btn-primary{color: #fff; background-color: #337ab7; border-color: #2e6da4;}.btn-primary:focus{color: #fff; background-color: #286090; border-color: #122b40;}.btn-primary:hover{color: #fff; background-color: #286090; border-color: #204d74;}.btn-primary:active{color: #fff; background-color: #286090; border-color: #204d74;}.btn-primary:active:hover, .btn-primary:active:focus{color: #fff; background-color: #204d74; border-color: #122b40;}.btn-primary:active{background-image: none;}hr{border-color: black;}#Other{display: inline-block;}.btn-lg{background-color: #337ab7; color: white; font-size: 24px; padding: 10px 20px; margin: 5px 10px;}label{margin-left: 10px;}.form-control{padding: 10px 20px; text-align: center; font-size: 20px;}.form-check{background-color: #fafafa; border: 1px solid #cacece; box-shadow: 0 1px 2px rgba(0,0,0,0.05), inset 0px -15px 10px -12px rgba(0,0,0,0.05); padding: 9px; border-radius: 3px; display: inline-block; position: relative;}input[type=checkbox]{visibility: hidden;}.checkboxOne,.checkboxTwo{width: 80px; height: 40px; background: lightgray; margin: 0; display: inline-block; border-radius: 50px; position: relative; bottom: 8px;}.checkboxOne:before,.checkboxTwo:before{content: ''; position: absolute; top: 19px; left: 14px; height: 2px; width: 56px; background: #111;}.checkboxOne label,.checkboxTwo label{display: block; width: 22px; height: 22px; border-radius: 50%; transition: all .5s ease; cursor: pointer; position: absolute; top: 9px; z-index: 1; left: 8px; background: red;}.checkboxOne input[type=checkbox]:checked + label, .checkboxTwo input[type=checkbox]:checked + label{left: 40px; background-color: greenyellow;}</style>
   </head>
   <body>
      <div class="container text-center">
         <div class="form-inline">
            <h2>Motor </h2>
            <FORM action="/submit" method="post"> <input class="btn-primary btn-lg" name="MotorEnable" type="submit" value="ON"/> <input class="btn-primary btn-lg" name="MotorEnable" type="submit" value="OFF"/> </FORM>
         </div>
         <div class="form-inline">
            <h2>Steps</h2>
            <FORM action="/submit" method="post"> <input class="btn-primary btn-lg" name="Steps" type="submit" value="&lt;&lt;"/> <input class="btn-primary btn-lg" name="Steps" type="submit" value="&lt;"/> <input class="btn-primary btn-lg" name="Steps" type="submit" value="&gt;"/> <input class="btn-primary btn-lg" name="Steps" type="submit" value="&gt;&gt;"/> </FORM>
         </div>
         <div class="form-inline">
            <FORM action="/submit" method="post"> <input class="btn-primary btn-lg" name="Steps" type="submit" value="&lt;90"/> <input class="btn-primary btn-lg" name="Steps" type="submit" value="&lt;45"/> <input class="btn-primary btn-lg" name="Steps" type="submit" value="&lt;30"/> <input class="btn-primary btn-lg" name="Steps" type="submit" value="30&gt;"/> <input class="btn-primary btn-lg" name="Steps" type="submit" value="45&gt;"/> <input class="btn-primary btn-lg" name="Steps" type="submit" value="90&gt;"/> </FORM>
         </div>
         <div class="form-inline">
            <FORM action="/submit" method="post"> <input class="btn-primary btn-lg" name="Steps" type="submit" value="Shutter"/> </FORM>
         </div>
         <div class="form-inline">
            <h2>Panorama:</h2>
            <form action="/submit" method="post">
               <label>Degrees:</label> 
               <select class="form-control" name="Pano_angle" size="1" id="pano">
                  <option value="30">30</option>
                  <option value="45">45</option>
                  <option value="60">60</option>
                  <option value="90">90</option>
                  <option value="00">Other</option>
               </select>
               <div id="Other"> <label>Other:</label> <input class="form-control" maxlength="5" name="Pano_angle_other" size="3" type="number" value=")=====";
         html += String(EEPROM.read(31));
  html += R"=====(" min="1" max="360"/> </div>
               <label>Direction:</label> 
               <select class="form-control" name="Pano_direction" size="1">
                  <option value="1">Left</option>
                  <option value="-1">Right</option>
               </select>
               <label>Shutter:</label> 
               <div class="checkboxOne"> <input class="form-check" id="checkBox1" name="Panorama_shutter" type="checkbox"/> <label for="checkBox1"></label> </div>
               <input class="btn-primary btn-lg" name="Start" type="submit" value="Start"/> 
            </form>
         </div>
         <div class="form-inline">
            <h2>Rotation:</h2>
            <form action="/submit" method="post">
               <label>Revolution time (s)</label> 
         <input class="form-control" maxlength="5" name="Revolution_time" size="3" type="number" value="10" min="1" max="86400"/> 
         <label>Number of revolutions:</label> 
         <input class="form-control" maxlength="3" name="Revolution_number" size="3" type="number" value="1" min="1" max="86400"/>
         <label>Direction:</label> 
               <select class="form-control" name="Revolution_direction" size="1">
                  <option value="1">Left</option>
                  <option value="-1">Right</option>
               </select>
               <label>Shutter:</label> 
               <div class="checkboxTwo"> <input class="form-check" id="checkBox2" name="Revolution_shutter" type="checkbox"/> <label for="checkBox2"></label> </div>
               <input class="btn-primary btn-lg" name="Start" type="submit" value="Start"/> 
            </form>
         </div>
         <hr/>
         <div class="form-inline">
            <h2>CONFIGURATION:</h2>
            <FORM action="/submit" method="post">
               <h4>Revolution speed:</h4>
               <input class="form-control" maxlength="5" name="Conf_rpm" size="3" type="number" value=")=====";
  html += String(EEPROM.read(1));
  html += R"=====("min="1" max="200"/> RPM <input class="btn-primary btn-lg" name="Set" type="submit" value="Set"/> 
            </FORM>
         </div>
         <div class="form-inline">
            <FORM action="/submit" method="post">
               <h4>Delay time before shutter:</h4>
               <input class="form-control" maxlength="5" name="Conf_delay" size="3" type="number" value=")=====";
  html += String(EEPROM.read(11));
  html += R"=====(" min="1" max="86400"/> s <input class="btn-primary btn-lg" name="Set" type="submit" value="Set"/> 
            </FORM>
         </div>
     <div class="form-inline">
            <FORM action="/submit" method="post">
               <h4>Driver step resolution (current: 1/)=====";
         
  html += String(EEPROM.read(21));
  html += R"=====()</h4>
          <select class="form-control" name="Conf_steps" size="1">
          <option value="1">1/1</option>
          <option value="2">1/2</option>
          <option value="4">1/4</option>
          <option value="8">1/8</option>
          <option value="16">1/16</option>
          </select>
         <input class="btn-primary btn-lg" name="Set" type="submit" value="Set"/>
             </FORM>
         </div>
      </div>
      <script>document.getElementById('Other').style.display='none'; document.getElementById('pano').onchange=function (){if (document.getElementById('pano').value==='00'){document.getElementById('Other').style.display='inline-block';}else{document.getElementById('Other').style.display='none';}}</script> 
   </body>
</html>
  )=====";
}

void steps(int n){	//rotate the choosen angle, input above 10 changes the rotation direction
  int dir=1;
  if(n>10){
    dir=-1;
    n-=10;
  }
  switch(n){
    case 1:
    stepper.rotate(10*dir);
    break;
    case 2:
    stepper.rotate(dir);
    break;
    case 3:
    stepper.rotate(30*dir);
    break;
    case 4:
    stepper.rotate(45*dir);
    break;
    case 5:
    stepper.rotate(90*dir);
    break;
  }
}

void panorama(){	//do a cycle of moves and pauses according to choosen settings
	int i;
	Serial.print("panorama:");
	revolution_flag=0;
	int n=360/pano_angle;
	Serial.print("pano_angle:");
	Serial.print(pano_angle);
	if(360%pano_angle!=0)n+=1;
	Serial.print("Direction:");
	Serial.print(Direction);
	for(i=0;i<n;i++){
		stepper.rotate(pano_angle*Direction);
		delay(delayTime);
		if(shutter_flag)shutter();
	}
	shutter_flag=0;
	panorama_flag=0;
}

void revolution(){	//do a number of revolutions with the selected speed
	Serial.print("revolution:");
	panorama_flag=0;
	int old_rpm=rpm;
	rpm=60/Revolution_speed;
	Serial.print("Revolution_speed:");
	Serial.print(Revolution_speed);
	stepper.setRPM(rpm);
	Serial.print("Revolution_number:");
	Serial.print(Revolution_number);
	int n=Revolution_number;
	if(shutter_flag)shutter();
	delay(delayTime);
	Serial.print("stepper.rotate:");
	Serial.print(n*Direction*360);
	stepper.rotate(n*Direction*360);
	delay(delayTime);
	if(shutter_flag)shutter();
	rpm=old_rpm;
	stepper.setRPM(rpm);
	shutter_flag=0;
	revolution_flag=0;
}

void handleRoot() {	//print the html site if root adress is requested
  webServer.send(200, "text/html", html);
  }
  
void handleSubmit(){	//handle the data given via POST method and display the updated html site
  if (webServer.args() > 0 ) {
    for ( uint8_t i = 0; i < webServer.args(); i++ ) {
      if (webServer.argName(i) == "MotorEnable") {
         if(webServer.arg(i)=="ON")stepper.enable();
         if(webServer.arg(i)=="OFF")stepper.disable();
      }
      if (webServer.argName(i) == "Steps"){
        
        if(webServer.arg(i)=="<<")steps(1);
        if(webServer.arg(i)=="<")steps(2);
        if(webServer.arg(i)==">")steps(12);
        if(webServer.arg(i)==">>")steps(11);
        if(webServer.arg(i)=="<90")steps(5);
        if(webServer.arg(i)=="<45")steps(4);
        if(webServer.arg(i)=="<30")steps(3);
        if(webServer.arg(i)=="30>")steps(13);
        if(webServer.arg(i)=="45>")steps(14);
        if(webServer.arg(i)=="90>")steps(15);
        if(webServer.arg(i)=="Shutter")shutter();
      }
      if (webServer.argName(i) == "Pano_angle"){
        pano_angle=webServer.arg(i).toInt();
		panorama_flag=1;
      }
      if (webServer.argName(i) == "Pano_angle_other"){
        pano_angle =webServer.arg(i).toInt();
        EEPROM.put(31, pano_angle);
        rewrite();
        EEPROM.commit();
      }
      if (webServer.argName(i) == "Pano_direction"){
		  Serial.print("Pano_direction:");
        Serial.println(webServer.arg(i));
        Direction=webServer.arg(i).toInt();
      }
      if (webServer.argName(i) == "Panorama_shutter"){
		  Serial.print("Panorama_shutter:");
        Serial.println(webServer.arg(i));
        shutter_flag=1;
      }
      if (webServer.argName(i) == "Revolution_time"){
		  Serial.print("Revolution_time:");
        Serial.println(webServer.arg(i));
        Revolution_speed=webServer.arg(i).toInt();
		revolution_flag=1;
      }
      if (webServer.argName(i) == "Revolution_number"){
		  Serial.print("Revolution_number:");
        Serial.println(webServer.arg(i));
        Revolution_number=webServer.arg(i).toInt();
      }
      if (webServer.argName(i) == "Revolution_direction"){
		  Serial.print("Revolution_direction:");
        Serial.println(webServer.arg(i));
        Direction=webServer.arg(i).toInt();
      }
      if (webServer.argName(i) == "Revolution_shutter"){
		  Serial.print("Revolution_shutter:");
        Serial.println(webServer.arg(i));
        shutter_flag=1;
      }
      if (webServer.argName(i) == "Conf_rpm"){
        rpm =webServer.arg(i).toInt();
        EEPROM.put(1, rpm);
        rewrite();
        EEPROM.commit();
    stepper.setRPM(rpm);
      }
      if (webServer.argName(i) == "Conf_delay"){
        delayTime =webServer.arg(i).toInt();
        EEPROM.put(11, delayTime);
        rewrite();
    EEPROM.commit();
      }
      if (webServer.argName(i) == "Conf_steps"){
        int steps =webServer.arg(i).toInt();
        EEPROM.put(21, steps);
        rewrite();
        EEPROM.commit();
    stepper.setMicrostep(steps);
      }
      
   }
   if(revolution_flag)revolution();
	  if(panorama_flag)panorama();
}
webServer.send(200, "text/html", html);	
  }

void setup() {
  
  pinMode(LEDPIN, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY,0);
  EEPROM.begin(512);
  rewrite();
  
  rpm=EEPROM.read(1);
  stepper.begin(1);
  stepper.disable();
  stepper.setRPM(rpm);
  stepper.setMicrostep(EEPROM.read(21));
  delayTime=EEPROM.read(11);
  
  Serial.begin(115200);
  Serial.println("reboot?");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("SSID","12345678");

  //--------------------from dns example:
  
  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.on("/", handleRoot);
  webServer.on("/submit", handleSubmit);
  // simple HTTP server to see that DNS server is working
  webServer.onNotFound([]() {
    webServer.send(200, "text/html",html);
  });
  
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
