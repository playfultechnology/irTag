/**
 * zTag
 * http://www.ztag.com/
 * IR LED on Pin 3 
 * IR detector/demodulator on Pin 2
 */

// DEFINES
// #define DEBUG
enum class Mode { Detector, Beacon, Player }; 
Mode mode = Mode::Beacon;
 
// INCLUDES
// IR library from https://github.com/z3t0/Arduino-IRremote
#include <IRremote.h>

// CONSTANTS
const int khz = 38; // 38kHz carrier frequency
// Obtained using Serial Monitor output from IRrecvDumpV2 example sketch
const unsigned int zombieSignal[55] = {400,1200,1200,400,400,1200,1200,800,800,400,400,1200,800,1200,1600,1200,800,400,800,400,400,400,400,1200,400,4800,400,4800,400,4800,400,400,2800,1200,3600,1200,400,4800,3800,1200,400,400,800,800,1200,1200,1200,400,2000,1200,800,800,400,1200,400};
const unsigned int doctorSignal[53] = {400,1200,1200,400,400,1200,1200,800,800,400,400,1200,800,1200,1600,1200,400,400,1200,400,400,400,400,1200,400,4800,400,4800,400,4800,400,400,2800,1200,3600,1200,400,4800,400,4800,400,4800,800,800,2000,1200,800,400,400,800,400,400,400};
const unsigned int infectedSignal[49] = {400,1200,1200,400,400,1200,1200,800,800,400,400,1200,800,400,400,400,1600,1200,400,800,800,400,400,400,400,1200,800,400,800,400,400,400,400,1200,400,4800,400,4800,400,4800,400,800,2400,1200,1200,800,400,400,400};

// GLOBALS
// IR Receiver on Pin 2
IRrecv irrecv(2);
// Always send on Pin 3
IRsend irsend;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void dumpCode (decode_results *results) {
  // Variable declaration
  Serial.print(F("unsigned int receivedData["));
  // Array size
  Serial.print(results->rawlen - 1, DEC);
  // Begin array
  Serial.print("] = {");
  // Dump data
  for(int i=1; i<results->rawlen; i++) {
    long val = results->rawbuf[i] * USECPERTICK;
    Serial.print(val, DEC);
    if(i<results->rawlen-1) { Serial.print(","); }
  }
  // End printout
  Serial.println("};");
}

bool CompareCode(decode_results *results, int code[], float tolerance){
  // Compare each time interval in the results (ignoring the first) to the supplied code
  for(int i=1; i<results->rawlen; i++) {
    long val = results->rawbuf[i] * USECPERTICK;
    if(code[i-1] * (1+tolerance) < val || val * (1+tolerance) < code[i-1]) {
      return false;
    }
  }
  return true;
}


void loop() {
  // Define object to store IR signal received
  decode_results results;

  // If a valid message can be decoded
  if(irrecv.decode(&results)) {
    #ifdef DEBUG
      // Dump the signal received
      dumpCode(&results);
    #endif
    
    if(CompareCode(&results, zombieSignal, 0.25)) {
      Serial.println("ZOMBIE!");
    }
    else if(CompareCode(&results, doctorSignal, 0.25)) {
      Serial.println("DOCTOR!");
    }
    else if(CompareCode(&results, infectedSignal, 0.25)) {
      Serial.println("INFECTED!");
    }
    
    irrecv.resume(); // Receive the next value
  }
  delay(500);

  
  irsend.sendRaw(doctorSignal, sizeof(doctorSignal) / sizeof(doctorSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.
  
  // Transmitting disables receiving, so need to enable it again
  irrecv.enableIRIn();
  delay(500);
}
