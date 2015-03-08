/*
  Autor: Javier Sanchez
  
  
  Fecha creación: 08/03/2015
  Fecha modificacion:--/--/----
  Descripcion del programa:
  Programa prueba para ver funcionamiento Ubidots 
  leemos temperatura del DS18b20 
  consumo placa a 6,65v 194,6/196,1mA
  Plublicado en:  
  http://app.ubidots.com/ubi/getchart/page/sBUoaRxrE68FrTv7sEvkH5RmNSU 

    */

    #include <SPI.h>
    #include <Ethernet.h>
    #include <OneWire.h>
    #include <DallasTemperature.h>


    #define ONE_WIRE_BUS 6
   // Enter a MAC address for your controller below.
   // Newer Ethernet shields have a MAC address printed on a sticker on the shield
   byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
   //damos la mac que queramos diferente a cualquiera que tengamos en el router 


   // Initialize the Ethernet client library
   // with the IP address and port of the server
   // that you want to connect to (port 80 is default for HTTP):
   EthernetClient client;

   String idvariable = "54fc8dde7625423dfdd1036f";      //token de la variable
   String token = "091WDLNwsrb6lh32A5h4pRxe5e1oSe";     //shortToken del usuario
   OneWire oneWire (ONE_WIRE_BUS);
   DallasTemperature sensors (&oneWire);

   float temp;

   void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    sensors.begin();

     // start the Ethernet connection:
     if (Ethernet.begin(mac) == 0) {        //me conecto a internet dandole la direccion mac 
       Serial.println("Failed to configure Ethernet using DHCP");
       Serial.println("Press Reset Button");
       for(;;);                             // si no me puedo conectar me quedo en un bluce infinito sin hacer nada
     }
     // doy un par de segundos para que se conecte el shield
     for(boolean i=0;i<=1;i++){
       delay(1000);
       Serial.println("connecting...");
     }
   }

   void loop()
   {
     sensors.requestTemperatures();   // "pregunto" por OneWire a los sensores 
     temp=sensors.getTempCByIndex(0); // guardo la variable del sensor  en ºC
     int entero=temp;                 //descompongo el float en dos variables
     int decimales=(temp-entero)*100 ;// descompongo los decimales en un entero

     Serial.print("float: ");         //paso los valores por Serial para ver que coinciden
     Serial.println(temp);
     Serial.print("int: ");
     Serial.println(entero);
     Serial.print("decimales: ");
     Serial.println(decimales);
     save_value(String(entero),String (decimales));   //llamo a la funcion que los envia por ethernet
   //  save_value(String (temp));
    delay(58500); // aprox cada 1minuto
 }


 void envio_serial(String value,String value2){
  //esta funcion envia lo mismo que enviamos por el ethernet para ver que todo esta como deberia
  int num=0;
  String var = "{\"value\":"+ String(value)+"."+String(value2)+"}";
  num = var.length();
  Serial.println("*************************\nSend To Ubidots:\n");
  Serial.println("POST /api/v1.6/variables/"+idvariable+"/values HTTP/1.1");
  Serial.println("Host: things.ubidots.com");
  Serial.println("Content-Type: application/json");
  Serial.print("X-Auth-Token: ");
  Serial.println(token);
  Serial.print("Content-Length: ");
  Serial.println(num);
  Serial.println();
  Serial.println(var); 
  Serial.println();                  
  Serial.println("*************************");
} 


void save_value(String value,String value2)
{
       // pasamos las dos variables a un String para enviarlo 
       int num=0;
       String var = "{\"value\":"+ String(value)+"."+String(value2)+"}";// juntamos las dos variables para tener un numero decimal
       num = var.length();//medimos la longitud del string para luego indicarselo a la web


       if (client.connect("http://things.ubidots.com",80)) {//me conecto al servidor de ubidots
         Serial.println("connected");
         client.println("POST /api/v1.6/variables/"+idvariable+"/values HTTP/1.1");//metemos el token de la variable que nos da la web
         client.println("Host: things.ubidots.com");
         client.println("Content-Type: application/json");
         client.println("X-Auth-Token: "+token); // nos autentificamos con nuestro token
         client.print("Content-Length: ");
         client.println(num);//enviamos la longitud del mensaje
         client.println();
         client.println(var); //enviamos la variable como nos marca la docu de la web
         client.stop(); // necesario para poder volver a enviar de nuevo los datos!!!
         envio_serial(String(value),String (value2));               
        
        
      }
      else {
         // if you didn't get a connection to the server:
         Serial.println("connection failed");
       }
/*
************pondremos esto si queremos que no intente entrar en caso de no poder conectarse************
       if (!client.connected()) {
         Serial.println();
         Serial.println("disconnecting.");
         client.stop();

       // do nothing forevermore:
       for(;;);
     }*/
     while (client.available()) {
       char c = client.read();
       Serial.print(c);
     }
     
   }
