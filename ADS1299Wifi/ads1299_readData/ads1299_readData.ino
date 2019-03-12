
#include <SPI.h>
#include "wiring_private.h"
SPIClass mySPI (&sercom1, 12, 13, 11, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

const byte WAKEUP = 0b00000010;     // Wake-up from standby mode
const byte STANDBY = 0b00000100;   // Enter Standby mode
const byte RESET = 0b00000110;   // Reset the device
const byte START = 0b00001000;   // Start and restart (synchronize) conversions
const byte STOP = 0b00001010;   // Stop conversion
const byte RDATAC = 0b00010000;   // Enable Read Data Continuous mode (default mode at power-up)
const byte SDATAC = 0b00010001;   // Stop Read Data Continuous mode
const byte RDATA = 0b00010010;   // Read data by command; supports multiple read back

//Register Read Commands
const byte RREG = 0b00100000;
const byte WREG = 0b01000000;

const byte CH1 = 0x05;
const byte CH2 = 0x06;
const byte CH3 = 0x07;
const byte CH4 = 0x08;
const byte CH5 = 0x09;
const byte CH6 = 0x0A;
const byte CH7 = 0x0B;
const byte CH8 = 0x0C;
const byte CHn = 0xFF;

const int pCS = 10; //chip select pin
const int pDRDY = 6; //data ready pin
const int pPWDN = 4;
const int pSTART = 3;
const int pRESET = 2;
const int pCLKSEL = 9;
const int LED = 1 ;
//Over clocked spi of the ADS1299 to 40M Hz
const int SPI_CLK = 40000000 ;

boolean deviceIDReturned = false;
boolean continuousRead = false ;
boolean startRead = false ;

int ch[7] ;
int cnt = 0 ;
String spiData;



void setup() {

    Serial.begin(115200);

    //start the SPI library:
    mySPI.begin();
    pinPeripheral(11, PIO_SERCOM);
    pinPeripheral(12, PIO_SERCOM);
    pinPeripheral(13, PIO_SERCOM);
    delay(3000);

    //Setup the wifi
    


  
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
  ADS_WREG(0x01, 0x94); // Sample Rate 96 - 250 , 95 - 500, 90 - 16k
  ADS_WREG(0x02, 0xD1); // Test Signal 2Hz Square Wave
  //ADS_WREG(CHn,0x10); // Active channels
  ADS_WREG(CHn, 0x00); // Ch on Test Signals with Gain 0
  // ADS_WREG(CH1,0x00);
  //ADS_WREG(0x15,0x20);//SRB1 To Negative Inputs
  ADS_RREAD(0, 24);
  //Serial.println("Started");
  Serial.println("Press 1 to START, any key to STOP");
}

void loop() {
  if (Serial.available())
  { int chk = Serial.parseInt() ;
    if (chk == 1)
    { startRead = 1 ;

    }
    else if (chk == 2)
    { startRead = 0;

    }
  }

  if (startRead)
  { ADS_ReadContinuous();
    digitalWrite(LED, HIGH);
  }
  else
  { ADS_STOP();
    digitalWrite(LED, LOW);
  }
  digitalWrite(LED, LOW);

}

void ADS_INIT()
{
  pinMode(pDRDY, INPUT);
  pinMode(pCS, OUTPUT);
  pinMode(pPWDN, OUTPUT);
  pinMode(pCLKSEL, OUTPUT);
  pinMode(pRESET, OUTPUT);
  digitalWrite(pCLKSEL, HIGH);
  delay(1);
  digitalWrite(pPWDN, HIGH);
  digitalWrite(pRESET, HIGH);
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
                    Serial.println(spiData);
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
