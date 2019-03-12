
#include <SPI.h>
#include "wiring_private.h"
#include "ads1299wifi.h"
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Ethernet.h>
#include "ButterHighPass.h"


SPIClass mySPI (&sercom1, 12, 13, 11, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

const int pCS = 10; //chip select pin
const int pDRDY = 9; //data ready pin

const int LED = 1 ;
//Over clocked spi of the ADS1299 to 40M Hz
const int SPI_CLK = 20000000;

boolean deviceIDReturned = false;
boolean continuousRead = false ;
boolean startRead = false ;

long ch[9];
int cnt = 0;

//There are two buffer used for wifi data sending.
char sendBuf[2][PACKET_SIZE*SEND_SIZE];

//Indicating which buffer is being written
int wBufIndex = 0;

//Indicates how much data packets are written into the current buffer
int wCount = 0;
bool isBufReady = false;



//For WIFI
int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
unsigned int localPort = 8899;      // local port to listen on


WiFiUDP Udp;

//Filters
struct ButterHighPassData HPfilter[8];


//Write one data packet into the buffer.
void pushToBuf(char* packet)
{
    //When current buffer is full
    if(wCount == SEND_SIZE)
    {
        //Switch buffer
        wBufIndex = (wBufIndex +1)%2;
        isBufReady = true;
        wCount = 0;
        //Serial.println("Switch Buffer");
    }

    //Write to buffer
    char* buf = getWriteBuf();
    memcpy(buf+wCount*PACKET_SIZE, packet, PACKET_SIZE  );
    wCount++;
    
}

char* getWriteBuf()
{
    return sendBuf[wBufIndex];
}

//Get the buffer for sending
//Have to be in non-interrupt context
char* getSendBuf()
{
    if(isBufReady == true)
    {
        isBufReady = false;
        return wBufIndex == 0 ? sendBuf[1] : sendBuf[0];
    }
    else
    {
        return 0;
    }
}

void drdyIRS()
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
    //When interrupt happends read new data from ADS1299 to buffer.
    digitalWrite(pCS, LOW);
    int j = 0 ;

    //Read ADS1299
    long packet[10];
    byte tempData[4] = {0,0,0,0};
    for (int i = 0; i < 27; i++)
    {
        byte temp = mySPI.transfer(0x00);

        tempData[2-i%3] = temp;

        if(i%3 == 2)
        {
            packet[i/3+1] = SIGN_EXT_24(*((int32_t *)tempData));
        }
    }
    packet[0] = cnt;

    //Push packet to buffer
    pushToBuf((char *)packet);
    
    digitalWrite(pCS, HIGH);
    cnt++ ;
}

void setupWifi()
{
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

        // wait 2 seconds for connection:
        delay(2000);
    }
    Serial.println("Connected to wifi");
    printWiFiStatus();
    Serial.println("\nStarting connection to server...");
    Udp.begin(localPort);

}




void setup()
{   
    Serial.begin(115200);

    //Init Filters
    for(int j=0; j < 8; j++)
    {
        initHPFilter(HPfilter+j,4,10,1000);
    }

    //start the SPI library:
    mySPI.begin();
    pinPeripheral(11, PIO_SERCOM);
    pinPeripheral(12, PIO_SERCOM);
    pinPeripheral(13, PIO_SERCOM);
    delay(3000);

    //Setup Wifi
    setupWifi();

    //Setup ADS1299
    // initalize the  data ready and chip select pins:
    ADS_INIT();


    //Setup interrupt
    attachInterrupt(digitalPinToInterrupt(pDRDY), drdyIRS, FALLING);
    delay(100);
    mySPI.transfer(START);
    mySPI.transfer(RDATAC);
    Serial.println("Start Streaming");
    drdyIRS();
    

}

void loop() 
{   
     byte* sendBuf = (byte*)getSendBuf();

        if( sendBuf != 0)
        {
            //Serial.println("Sending:"+ String(cnt));
            //No interrupt
//            noInterrupts();

            Udp.beginPacket("192.168.0.104", 8899);
        
            Udp.write(sendBuf, PACKET_SIZE*SEND_SIZE);
            Udp.endPacket();

//            interrupts();
//            Serial.println(((long int*)(sendBuf))[0]);
//            Serial.println(((long int*)(sendBuf))[4]);
//
//            Serial.println("Sent:"+ String(cnt));
        }

}





void ADS_INIT()
{
    pinMode(pDRDY, INPUT);
    pinMode(pCS, OUTPUT);

    delay(1);
    digitalWrite(pCS, HIGH);
    delay(1000);  //delay to ensure connection
    digitalWrite(pCS, LOW);
    
    mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
    mySPI.transfer(RESET);
    mySPI.endTransaction();
    delay(100);
    digitalWrite(pCS, HIGH);

    delay(10);  //delay to ensure connection
    mySPI.transfer(ADS1299_OPC_SDATAC);
    mySPI.transfer(ADS1299_OPC_STOP);
    

    ADS_WREG(ADS1299_REGADDR_GPIO, ADS1299_REG_GPIO_GPIOC4_OUTPUT |
    ADS1299_REG_GPIO_GPIOD4_LOW    |
    ADS1299_REG_GPIO_GPIOC3_OUTPUT |
    ADS1299_REG_GPIO_GPIOD3_LOW    |
    ADS1299_REG_GPIO_GPIOC2_OUTPUT |
    ADS1299_REG_GPIO_GPIOD2_LOW    |
    ADS1299_REG_GPIO_GPIOC1_OUTPUT |
    ADS1299_REG_GPIO_GPIOD1_LOW    );

    ADS_WREG(ADS1299_REGADDR_CONFIG1, ADS1299_REG_CONFIG1_RESERVED_VALUE |
    ADS1299_REG_CONFIG1_FMOD_DIV_BY_1024);

    ADS_WREG(ADS1299_REGADDR_CONFIG3, ADS1299_REG_CONFIG3_RESERVED_VALUE|
    ADS1299_REG_CONFIG3_REFBUF_ENABLED |
    ADS1299_REG_CONFIG3_BIASREF_INT    |
    ADS1299_REG_CONFIG3_BIASBUF_ENABLED);

    ADS_WREG(ADS1299_REGADDR_BIAS_SENSP, ADS1299_REG_BIAS_SENSP_BIASP1);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSN, ADS1299_REG_BIAS_SENSN_BIASN1);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSP, ADS1299_REG_BIAS_SENSP_BIASP2);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSN, ADS1299_REG_BIAS_SENSN_BIASN2);


//    ADS_WREG(ADS1299_REGADDR_BIAS_SENSP, 0xFF);
//    ADS_WREG(ADS1299_REGADDR_BIAS_SENSN, 0xFF);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSP, 0B00000001);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSN, 0B00000001);

    //Configure each channel
    ADS_WREG(CH1, ADS1299_REG_CHNSET_CHANNEL_ON          |
    ADS1299_REG_CHNSET_GAIN_24          |
    ADS1299_REG_CHNSET_SRB2_DISCONNECTED    |
    ADS1299_REG_CHNSET_NORMAL_ELECTRODE);

    ADS_WREG(CH2, ADS1299_REG_CHNSET_CHANNEL_ON          |
    ADS1299_REG_CHNSET_GAIN_24          |
    ADS1299_REG_CHNSET_SRB2_DISCONNECTED    |
    ADS1299_REG_CHNSET_NORMAL_ELECTRODE);

    ADS_WREG(CH3, ADS1299_REG_CHNSET_CHANNEL_ON          |
    ADS1299_REG_CHNSET_GAIN_24          |
    ADS1299_REG_CHNSET_SRB2_DISCONNECTED    |
    ADS1299_REG_CHNSET_NORMAL_ELECTRODE);

    ADS_WREG(CH4, ADS1299_REG_CHNSET_CHANNEL_ON          |
    ADS1299_REG_CHNSET_GAIN_24          |
    ADS1299_REG_CHNSET_SRB2_DISCONNECTED    |
    ADS1299_REG_CHNSET_NORMAL_ELECTRODE);

    ADS_WREG(CH5, ADS1299_REG_CHNSET_CHANNEL_ON          |
    ADS1299_REG_CHNSET_GAIN_24          |
    ADS1299_REG_CHNSET_SRB2_DISCONNECTED    |
    ADS1299_REG_CHNSET_NORMAL_ELECTRODE);

    ADS_WREG(CH6, ADS1299_REG_CHNSET_CHANNEL_ON          |
    ADS1299_REG_CHNSET_GAIN_24          |
    ADS1299_REG_CHNSET_SRB2_DISCONNECTED    |
    ADS1299_REG_CHNSET_NORMAL_ELECTRODE);

    ADS_WREG(CH7, ADS1299_REG_CHNSET_CHANNEL_ON          |
    ADS1299_REG_CHNSET_GAIN_24          |
    ADS1299_REG_CHNSET_SRB2_DISCONNECTED    |
    ADS1299_REG_CHNSET_NORMAL_ELECTRODE);

    ADS_WREG(CH8, ADS1299_REG_CHNSET_CHANNEL_ON          |
    ADS1299_REG_CHNSET_GAIN_24          |
    ADS1299_REG_CHNSET_SRB2_DISCONNECTED    |
    ADS1299_REG_CHNSET_NORMAL_ELECTRODE);

  
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
