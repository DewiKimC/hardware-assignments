#include <SPI.h>

const int sclk = 15;   // Serial Clock (SCLK) connected to 74HC595's Shift Register Clock Input (SRCLK)
const int mosi = 16;   // Master Out Slave In (MOSI) connected to 74HC595's Serial Data Input (SER)
const int hcCS = 10;   // Chip Select (CS) connected to 74HC595's Storage Register Clock Input (RCLK)
const int ledCount = 8; // Number of LEDs connected
const int MCP3008_CS_PIN = 9;//Chip select (CS) connected to MCP3008 
const int SN74HC95_OE_PIN = 3;

const byte DIFFMODE = 128;

SPISettings mySettings(2000000, MSBFIRST, SPI_MODE0);  

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome");

  SPI.begin();  // Initialize SPI communication

  // SN74HC595 pin setup
  pinMode(sclk, OUTPUT);
  pinMode(mosi, OUTPUT);
  pinMode(hcCS, OUTPUT);

  
  // Set all LEDs off initially
  updateLEDs(0b00000000);

  // MCP3008 pin setup
  pinMode(MCP3008_CS_PIN, OUTPUT);
  digitalWrite(MCP3008_CS_PIN, LOW);
  digitalWrite(MCP3008_CS_PIN, HIGH);
}

void loop() {
  // Shift LEDs on one by one
  int speed = map(readMCP3008(0), 0, 1023, 0, 255);  // Channel 0 for the first potentiometer
  int brightness = map(readMCP3008(2), 0, 1023, 255, 0); // Channel 1 for the second potentiometer

  // Print the values
  Serial.print("1: ");
  Serial.println(speed);

  Serial.print("2: ");
  Serial.println(brightness);

  ledPattern(speed);
  updateBrightness(brightness);

  delay(50);
}

void updateBrightness(int brightness) {
  analogWrite(SN74HC95_OE_PIN, brightness); // Adjust brightness
}

void ledPattern(byte speed){
  for (int i = 0; i < ledCount; i++) {
    updateLEDs(1 << i);  
    delay(speed);         
    //Serial.println(i);
  }

  // // Shift LEDs off one by one
  // for (int i = ledCount - 1; i >= 0; i--) {
  //   updateLEDs(1 << i);  // Shift '1' to the left to turn off the current LED
  //   delay(500);          
  // }
}

// Function to update the LEDs using the 74HC595 shift register
void updateLEDs(byte pattern) {
  digitalWrite(hcCS, LOW);       // Enable the shift register
  SPI.transfer(pattern);          // Send the pattern to the shift register
  digitalWrite(hcCS, HIGH);      // Disable the shift register, latch the data to the storage register
}

int readMCP3008(byte channel) {
  SPI.beginTransaction(mySettings);
  digitalWrite(MCP3008_CS_PIN, LOW);

  SPI.transfer(0x01); //start bit
  byte dataMSB = SPI.transfer( 128 + (channel << 4)) & 0x03;
  byte dataLSB = SPI.transfer(0);                // Push junk byte and get LSB byte on MISO return
  
  digitalWrite(MCP3008_CS_PIN, HIGH);
  SPI.endTransaction();

  return dataMSB << 8 | dataLSB; ;
}