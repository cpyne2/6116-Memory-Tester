// SPDX-License-Identifier: MIT

/*
 * 6116 Static RAM Tester - Tests SRAM by writing and reading patterns
 * 
 * Derived from the SRAM tester projects by Carsten Skjerk and Dennis Marttinen 
 * https://github.com/skjerk/Arduino-2114-SRAM-tester
 * https://github.com/twelho/sram-tester

 
 * Licensed under the MIT license
 */

struct WritePin {
  uint8_t pin;
  bool inverted;
};

// ------------------------------

/*
 * 6116 SRAM (ATMega 2560 pinout)
 * Pin 12 (Vss) is wired to GND
 * Pin 18 (CS) is wired to GND
 * Pin 20 (OE) is wired to GND
 * Pin 24 (Vcc) is wired to +5V
*/

//ccp
#define BTN_PIN_STOP 1
#define BTN_PIN 0
//ccp

// Address Pins
const uint8_t addressPins[] = {
  47, 45, 43, 41, 39, 37, 35, 33, 32, 34, 38
};

// Data Pins
const uint8_t dataPins[] = {
  49, 51, 53, 48, 46, 44, 42, 40
};

// Write Pins
const WritePin writePins[] = {
  { 36, true },  // (LOW = WRITE)
};

// The number of addressable memory locations
const uint32_t addressCount = 2048;

///*
// * 2114 SRAM (ATMega 2560 pinout)
// * Pin 12 (Vss) is wired to GND
// * Pin 18 (CS) is wired to GND
// * Pin 24 (Vcc) is wired to +5V
//*/
//
//// Address Pins
//const uint8_t addressPins[] = {
//  47, 49, 51, 45, 43, 41, 39, 38, 40, 42
//};
//
//// Data Pins
//const uint8_t dataPins[] = {
//  44, 46, 48, 50
//};
//
//// Write Pins
//const WritePin writePins[] = {
//  {52, true}, // (LOW = WRITE)
//};
//
//// The number of addressable memory locations
//const uint32_t addressCount = 1024;

// ------------------------------

#define NX(x) (sizeof(x) / sizeof((x)[0]))

const size_t NA = NX(addressPins);
const size_t ND = NX(dataPins);
const size_t NW = NX(writePins);

// Set Address pins to output
void setupAddressPins() {
  for (size_t i = 0; i < NA; i++) {
    pinMode(addressPins[i], OUTPUT);
  }
}

// Set Data pins to output
void setDataPinsOutput() {
  for (size_t i = 0; i < ND; i++) {
    pinMode(dataPins[i], OUTPUT);
  }
}

// Set Data pins to input
void setDataPinsInput() {
  for (size_t i = 0; i < ND; i++) {
    // Explicitly set them LOW or else this reads its own output
    digitalWrite(dataPins[i], LOW);
    pinMode(dataPins[i], INPUT);
  }
}

// Set Write pins to output
void setupWritePins() {
  for (size_t i = 0; i < NW; i++) {
    pinMode(writePins[i].pin, OUTPUT);
  }
}

// Initial setup of pins and serial monitor
void setup() {
  // Initialize all pins
  setupAddressPins();
  setDataPinsOutput();
  setupWritePins();

//ccp

   pinMode(11, OUTPUT);
   pinMode(10, OUTPUT);

   pinMode(9, OUTPUT);
   pinMode(8, OUTPUT);
   pinMode(7, OUTPUT);
   pinMode(6, OUTPUT);
   pinMode(5, OUTPUT);
   pinMode(4, OUTPUT);
   pinMode(3, OUTPUT);
   pinMode(2, OUTPUT);
   pinMode(BTN_PIN_STOP, INPUT_PULLUP); 
   pinMode(BTN_PIN, INPUT_PULLUP); 
  

  // Initialize Serial Port
  Serial.begin(230400);
  Serial.println("Universal Static RAM Tester by Dennis Marttinen");
  Serial.print(NA);
  Serial.print('/');
  Serial.print(ND);
  Serial.print('/');
  Serial.print(NW);
  Serial.println(" address/data/write pin(s) configured");
}

// Set the address pins to match the specified address
void setAddressBits(size_t address) {
  for (size_t i = 0; i < NA; i++) {
    digitalWrite(addressPins[i], bitRead(address, i));
  }
}

// Set the data pins to match the specified value
void setDataBits(size_t value) {
  for (size_t i = 0; i < ND; i++) {
    digitalWrite(dataPins[i], bitRead(value, i));
  }
}

void enableWritePins() {
  for (size_t i = 0; i < NW; i++) {
    const WritePin* p = &writePins[i];
    digitalWrite(p->pin, !p->inverted);
  }
}

void disableWritePins() {
  for (size_t i = 0; i < NW; i++) {
    const WritePin* p = &writePins[i];
    digitalWrite(p->pin, p->inverted);
  }
}

// Write data to the specified memory address
void writeData(size_t address, size_t data) {
  // Set data pins to output
  setDataPinsOutput();

  // Set address bits
  setAddressBits(address);

  // Set data bits
  setDataBits(data);

  // Enable Write pins
  enableWritePins();

  // Wait for the logic to be stabilized
  //delay(1);

  // Disable Write pins
  disableWritePins();

  // Wait a bit for the write to commit
  //delay(1);
}

// Read data from the specified memory address
// Note that the Write pins must already be in READ mode
size_t readData(size_t address) {
  // Set data pins to input
  setDataPinsInput();

  // Set address bits
  setAddressBits(address);

  // Wait for the logic to be stabilized
  //delay(1);

  // Read each data bit one by one
  size_t result = 0;
  for (size_t i = 0; i < ND; i++) {
    bitWrite(result, i, digitalRead(dataPins[i]));
  }
  return result;
}

// Output binary value from the ND bit data
void printBinary(size_t data) {
  for (size_t b = ND; b > 0; b--) {
    Serial.print(bitRead(data, b - 1));

    // ccp LED CODE - Add 1 to get the LED number and -1 to get the data postion, (ex 8 = Pin 9, and value 7)
    digitalWrite(b + 1, bitRead(data, b - 1));    
  }
}

// Print an unsigned 64-bit integer
void printU64(uint64_t value) {
  if (value == 0) {
    Serial.print('0');
    return;
  }

  unsigned char buf[20];
  uint8_t i = 0;

  while (value > 0) {
    uint64_t q = value / 10;
    buf[i++] = value - q * 10;
    value = q;
  }

  for (; i > 0; i--) {
    Serial.print((char)('0' + buf[i - 1]));
  }
}

void loop() {
  uint32_t firstError = 0;
  uint32_t lastError = 0;
  uint64_t errorCount = 0;




  // Use all possible values for data patterns
  for (size_t pattern = 0; pattern < bit(ND); pattern++) {
    Serial.print("Running test pattern ");
    printBinary(pattern);
    Serial.println();

    // Loop through all addresses in the SRAM
    for (uint32_t addr = 0; addr < addressCount; addr++) {
      // Write test pattern to the SRAM
      writeData(addr, pattern);

      // Read data from the SRAM
      size_t data = readData(addr);

      // Verify
      if (data != pattern) {
        lastError = addr;
        if (errorCount++ == 0) {
          firstError = addr;
        }

        Serial.print("Error at address 0x");
        Serial.print(addr, HEX);
        Serial.print(" - Got: ");
        printBinary(data);
        Serial.print(", expected: ");
        printBinary(pattern);
        Serial.println();
        digitalWrite(10, HIGH); //ccp
        exit(0); //ccp
      }




    }
  }



  Serial.println("Test complete");
  printU64(errorCount);
  Serial.print(" errors found (");
  Serial.print((100.f * errorCount) / ((float)bit(ND) * addressCount));
  Serial.println("% failed)");
  if (errorCount > 0) {
    Serial.print("Error span: 0x");
    Serial.print(firstError, HEX);
    Serial.print(" to 0x");
    Serial.println(lastError, HEX);
    digitalWrite(10, HIGH);
  }
  else{
    digitalWrite(11, HIGH);
    }




  // Nothing more to do, so loop indefinitely - or until Reset is pressed
  while (1) {};
}
