
#define SECRET_SSID "ADS1299"
#define SECRET_PASS "fluidfluid"

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
