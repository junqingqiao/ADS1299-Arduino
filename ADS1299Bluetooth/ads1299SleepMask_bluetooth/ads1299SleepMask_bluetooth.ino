
#include <SPI.h>
#include "ads1299.h"
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Ethernet.h>
#include <bluefruit.h>

BLEDis  bledis;
BLEUart bleuart;
BLEBas  blebas;

const int myMISO = 15;
const int mySCLK = 16;
const int myMOSI = 7;

SPIClass mySPI (NRF_SPI0, myMISO, mySCLK, myMOSI);

const int pCS = 11; //chip select pin
const int pDRDY = 31; //data ready pin

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

void drdyIRS()
{
    if (!continuousRead)
    {
        Serial.println("Continuous Read");
        continuousRead = true ;
        digitalWrite(pCS, LOW);
        //mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));
        mySPI.transfer(START);
        mySPI.transfer(RDATAC);
        digitalWrite(pCS, HIGH);
        //mySPI.endTransaction();
    }
    //When interrupt happends read new data from ADS1299 to buffer.
    //mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE2));
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
    //Serial.printf("interrupted %d\n", cnt);
    //Serial.println("interrupted" );
    //Serial.println(cnt);
    //mySPI.endTransaction();
}


void ADS_RREAD(byte r , int n) {
  if (r + n > 24)
    n = 24 - r ;
  digitalWrite(pCS, LOW);
  Serial.println("Register Data");
  //mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE2));
  mySPI.transfer(SDATAC);
  mySPI.transfer(RREG | r); //RREG
  mySPI.transfer(n); // 24 Registers
  for (int i = 0; i < n; i++)
  { byte temp = mySPI.transfer(0x00);
    Serial.println(temp, HEX);
  }
  //mySPI.endTransaction();
  digitalWrite(pCS, HIGH);
}

void ADS_WREG(byte n, byte t)
{ 
    Serial.println("Start writing REGS \n");
    if (n == 0 || n == 18 || n == 19)
    Serial.println("Error: Read-Only Register");
    else if (n == 0xFF)
    { 
        digitalWrite(pCS, LOW);
        //mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE2));
        mySPI.transfer(SDATAC);
    for (int i = 5; i < 13; i++)
    { mySPI.transfer(WREG | i); //RREG
      mySPI.transfer(0x00);
      mySPI.transfer(t);
    }
    //mySPI.endTransaction();
    digitalWrite(pCS, HIGH);
    Serial.println("Written All Channels");
  }
  else
  { digitalWrite(pCS, LOW);
    //mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE2));
    mySPI.transfer(SDATAC);
    mySPI.transfer(WREG | n); //RREG
    mySPI.transfer(0x00); // 24 Registers
    mySPI.transfer(t);
    //mySPI.endTransaction();
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


void ADS_INIT()
{
    pinMode(pDRDY, INPUT);
    pinMode(pCS, OUTPUT);

    delay(1);
    digitalWrite(pCS, HIGH);
    delay(1000);  //delay to ensure connection
    digitalWrite(pCS, LOW);
    
    //mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE2));
    mySPI.transfer(RESET);
    //mySPI.endTransaction();
    Serial.println("SPI Started \n");
    delay(100);
    digitalWrite(pCS, HIGH);

    delay(10);  //delay to ensure connection
    mySPI.transfer(ADS1299_OPC_SDATAC);
    mySPI.transfer(ADS1299_OPC_STOP);
    Serial.println("SPI Started2 \n");

    ADS_WREG(ADS1299_REGADDR_GPIO, ADS1299_REG_GPIO_GPIOC4_OUTPUT |
    ADS1299_REG_GPIO_GPIOD4_LOW    |
    ADS1299_REG_GPIO_GPIOC3_OUTPUT |
    ADS1299_REG_GPIO_GPIOD3_LOW    |
    ADS1299_REG_GPIO_GPIOC2_OUTPUT |
    ADS1299_REG_GPIO_GPIOD2_LOW    |
    ADS1299_REG_GPIO_GPIOC1_OUTPUT |
    ADS1299_REG_GPIO_GPIOD1_LOW    );

    ADS_WREG(ADS1299_REGADDR_CONFIG1, ADS1299_REG_CONFIG1_RESERVED_VALUE |
    ADS1299_REG_CONFIG1_FMOD_DIV_BY_2048);

    ADS_WREG(ADS1299_REGADDR_CONFIG3, ADS1299_REG_CONFIG3_RESERVED_VALUE|
    ADS1299_REG_CONFIG3_REFBUF_ENABLED |
    ADS1299_REG_CONFIG3_BIASREF_INT    |
    ADS1299_REG_CONFIG3_BIASBUF_ENABLED);

    ADS_WREG(ADS1299_REGADDR_BIAS_SENSP, ADS1299_REG_BIAS_SENSP_BIASP1);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSN, ADS1299_REG_BIAS_SENSN_BIASN1);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSP, ADS1299_REG_BIAS_SENSP_BIASP2);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSN, ADS1299_REG_BIAS_SENSN_BIASN2);


    ADS_WREG(ADS1299_REGADDR_BIAS_SENSP, 0xFF);
    ADS_WREG(ADS1299_REGADDR_BIAS_SENSN, 0xFF);

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


void connect_callback(uint16_t conn_handle)
{
  char central_name[32] = { 0 };
  Bluefruit.Gap.getPeerName(conn_handle, central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 * https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/cores/nRF5/nordic/softdevice/s140_nrf52_6.1.1_API/include/ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.println("Disconnected");
}

void startAdv(void)
{
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    
    Bluefruit.Advertising.addService(bleuart);
    Bluefruit.ScanResponse.addName();

    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 40); //in unit 0.625ms
    Bluefruit.Advertising.setFastTimeout(30);
    Bluefruit.Advertising.start(0);
}

void setupBLE() 
{

    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
//    Bluefruit.setConnLedInterval(8);
    Bluefruit.begin();

    Bluefruit.setTxPower(4);
    Bluefruit.setName("FluidBCI");
    Bluefruit.setConnectCallback(connect_callback);
    Bluefruit.setDisconnectCallback(disconnect_callback);
    
    bledis.setManufacturer("MIT Media Lab");
    bledis.setModel("V0.1");
    bledis.begin();
    
    bleuart.begin();

    startAdv();
}



void setup()
{   
    Serial.begin(115200);
    
    //start the SPI library:
    mySPI.begin();
    NRF_GPIO->DIRSET = (1 << mySCLK);
    NRF_GPIO->DIRSET = (1 << myMOSI);
    NRF_GPIO->DIRSET = (0 << myMISO);
    NRF_SPIM0->FREQUENCY = SPI_CLK;
    NRF_SPIM0->PSEL.SCK = mySCLK;
    NRF_SPIM0->PSEL.MOSI = myMOSI;
    NRF_SPIM0->PSEL.MISO = myMISO;
    
//    Serial.println("Started \n");
    mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE1));

    
//    Serial.println("SPI configured \n");
    delay(2000);

    

    //Setup ADS1299
    // initalize the  data ready and chip select pins:
    ADS_INIT();
//    Serial.println("ADS1299 Configured \n");

    //Setup interrupt
    attachInterrupt(digitalPinToInterrupt(pDRDY), drdyIRS, FALLING);
    delay(100);
    //mySPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE2));
    mySPI.transfer(START);
    mySPI.transfer(RDATAC);
    //mySPI.endTransaction();
//    Serial.println("Start Streaming");
    setupBLE();
    drdyIRS();
}

void loop() 
{   
     byte* toSend = (byte*)getSendBuf();

        if( toSend != 0)
        {
            //No interrupt
//            noInterrupts();

//            Serial.println(((long int*)(toSend))[0]);
//            Serial.println(((long int*)(toSend))[2]);
//            Serial.println(((long int*)(toSend))[4]);
            bleuart.write(toSend,PACKET_SIZE*SEND_SIZE);
//            interrupts();
        }

}
