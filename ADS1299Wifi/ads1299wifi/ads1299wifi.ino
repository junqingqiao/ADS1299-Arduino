
#include <SPI.h>
#include "wiring_private.h"
#include "ads1299wifi.h"
#include <WiFi101.h>
#include <WiFiUdp.h>


SPIClass mySPI (&sercom1, 12, 13, 11, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

const int pCS = 10; //chip select pin
const int pDRDY = 6; //data ready pin
const int pCLKSEL = 9;
const int LED = 1 ;
//Over clocked spi of the ADS1299 to 40M Hz
const int SPI_CLK = 20000000;

boolean deviceIDReturned = false;
boolean continuousRead = false ;
boolean startRead = false ;

int ch[8] ;
int cnt = 0 ;
String spiData;


//For WIFI
int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
unsigned int localPort = 8899;      // local port to listen on
String sendingString;
char sendingBuf[40];
int sendingCounter = 10;

WiFiUDP Udp;







void setup()
{   
    Serial.begin(115200);

    //start the SPI library:
    mySPI.begin();
    pinPeripheral(11, PIO_SERCOM);
    pinPeripheral(12, PIO_SERCOM);
    pinPeripheral(13, PIO_SERCOM);
    delay(3000);

    //Setup the wifi
    
    //Configure pins for Adafruit ATWINC1500 Feather
    WiFi.setPins(8,7,4,2);
    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD) 
    {
        Serial.println("WiFi shield not present");
        // don't continue:
        while (true);
    }
    // attempt to connect to WiFi network:
    while ( status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }
    Serial.println("Connected to wifi");
    printWiFiStatus();
    Serial.println("\nStarting connection to server...");
    Udp.begin(localPort);



    //Setup ADS1299
    Serial.flush();
    Serial.println("ADS1299");
    // initalize the  data ready and chip select pins:
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    ADS_INIT();
    digitalWrite(LED, LOW);
    delay(10);  //delay to ensure connection
    ADS_RREAD(0, 24);
    //ADS_WREG(0x03,0xE0); // Enable Internal Reference Buffer 6 -OFF , E - ON
    ADS_WREG(0x03, 0xE8); // Enable Internal Reference Buffer 6 -OFF , E - ON
    delay(50);
    ADS_WREG(CHn, 0x01); // Input Shorted
    delay(1);
    for (int i = 0; i < 10; i++)
        ADS_ReadContinuous();
    ADS_STOP();
    delay(1);
    ADS_WREG(0x01, 0x92); // Sample Rate 96 - 250 , 95 - 500, 90 - 16k
    ADS_WREG(0x02, 0xD1); // Test Signal 2Hz Square Wave
    //ADS_WREG(CHn,0x10); // Active channels
    ADS_WREG(CHn, 0x00); // Ch on Test Signals with Gain 0
    // ADS_WREG(CH1,0x00);
    //ADS_WREG(0x15,0x20);//SRB1 To Negative Inputs
    ADS_RREAD(0, 24);

    Serial.println("Press 1 to START, any key to STOP");
}

void loop() 
{
    if (Serial.available())
    { 
        int chk = Serial.parseInt() ;
        if (chk == 1)
        { 
            startRead = 1 ;
        }
        else if (chk == 2)
        { 
            startRead = 0;
        }
    }

    if (startRead)
    { 
        if(sendingCounter == 0)
        {
            //send the data
//            Serial.println(sendingString);
//            sendingString.toCharArray(sendingBuf,1000);
            spiData.toCharArray(sendingBuf,20);

            sendingBuf[19] = '\n';
            Udp.beginPacket("192.168.0.101", 8899);
        
            Udp.write(sendingBuf);
            Udp.endPacket();
    
            //Clean the buffer
            sendingString = "";
            sendingCounter = 10;
        }
        ADS_ReadContinuous();   
        sendingCounter--; 
        sendingString += spiData + "\n";    
    }
    else
    { 
        ADS_STOP();
        digitalWrite(LED, LOW);
    }
    digitalWrite(LED, LOW);

}

















void ADS_INIT()
{
  pinMode(pDRDY, INPUT);
  pinMode(pCS, OUTPUT);
  pinMode(pCLKSEL, OUTPUT);
  digitalWrite(pCLKSEL, HIGH);
  delay(1);
  digitalWrite(pCS, HIGH);
  delay(1000);  //delay to ensure connection
  digitalWrite(pCS, LOW);
  mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
  mySPI.transfer(RESET);
  mySPI.endTransaction();
  delay(100);
  digitalWrite(pCS, HIGH);
}

void ADS_RREAD(byte r , int n) {
  if (r + n > 24)
    n = 24 - r ;
  digitalWrite(pCS, LOW);
  Serial.println("Register Data");
  mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
  mySPI.transfer(SDATAC);
  mySPI.transfer(RREG | r); //RREG
  mySPI.transfer(n); // 24 Registers
  for (int i = 0; i < n; i++)
  { byte temp = mySPI.transfer(0x00);
    Serial.println(temp, HEX);
  }
  mySPI.endTransaction();
  digitalWrite(pCS, HIGH);
}

void ADS_WREG(byte n, byte t)
{ if (n == 0 || n == 18 || n == 19)
    Serial.println("Error: Read-Only Register");
  else if (n == 0xFF)
  { digitalWrite(pCS, LOW);
    mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
    mySPI.transfer(SDATAC);
    for (int i = 5; i < 13; i++)
    { mySPI.transfer(WREG | i); //RREG
      mySPI.transfer(0x00);
      mySPI.transfer(t);
    }
    mySPI.endTransaction();
    digitalWrite(pCS, HIGH);
    Serial.println("Written All Channels");
  }
  else
  { digitalWrite(pCS, LOW);
    mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
    mySPI.transfer(SDATAC);
    mySPI.transfer(WREG | n); //RREG
    mySPI.transfer(0x00); // 24 Registers
    mySPI.transfer(t);
    mySPI.endTransaction();
    digitalWrite(pCS, HIGH);
    Serial.println("Written Register");
  }
}

void ADS_ReadContinuous()
{
    if (!continuousRead)
    {
        //Serial.println("Continuous Read");
        continuousRead = true ;
        digitalWrite(pCS, LOW);
        mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
        mySPI.transfer(START);
        mySPI.transfer(RDATAC);
        digitalWrite(pCS, HIGH);
    }
    
    while (digitalRead(pDRDY));

    
    digitalWrite(pCS, LOW);
    int j = 0 ;
    spiData = String();
    for (int i = 0; i < 27; i++)
    { 
        byte temp = mySPI.transfer(0x00);
        if (i < 2)
        { 
            spiData += temp ;
            spiData += "," ;
        }
        else if (i == 2)
        { 
            spiData += cnt ;
            spiData += "," ;
        }
        else
        {
            ch[j] += temp << (3 - (i % 3)) * 8 ;
            if (i % 3 == 2)
            {
                ch[j] = ch[j] >> 8;
                //Serial.print(ch[j]);
                spiData += ch[j] ;
                if (j < 7)
                {
                    spiData += ",";
                }
                else
                { 
//                    Serial.println(spiData);
                }
                j++ ;
            }
        }
    }
    digitalWrite(pCS, HIGH);
    cnt++ ;
}


void ADS_STOP()
{ if (continuousRead)
  { digitalWrite(pCS, LOW);
    mySPI.transfer(STOP);
    mySPI.transfer(SDATAC);
    digitalWrite(pCS, HIGH);
    continuousRead = false ;
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
