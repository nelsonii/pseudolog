//SwitchPseudoLog_v1A

//=================================================================================

#include "Joystick.h"

//=================================================================================

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
                   JOYSTICK_TYPE_JOYSTICK, 0, 0,
                   true, true, false, false, false, false,
                   false, false, false, false, false);

// constructor: use default id, it's a joystick, there are no buttons,
//              there are no hat buttons, there is an X and Y analog
//              and no other controls.

//=================================================================================

const boolean debug = false; // if true, output info to serial monitor or plotter.
const boolean debugVerbose = false; // if true, output more info to serial monitor.
const boolean debugPlot = false; // if true, output data for plotter/graph viewing.

//=================================================================================

// Joystick pins
#define SW_UP A0
#define SW_DN A1  
#define SW_LT A2
#define SW_RT A3

//pot for adjusting sensitivity
#define SENSE A10

//=================================================================================

// values from switches
long rawLT, rawRT, rawUP, rawDN;

// value from sensitivity pot
long rawSense;
long adjSense;

//=================================================================================

// converted switch values
long adjHorz, adjVert; // data from joystick (cleaned/adjusted)
long mapHorz, mapVert; // mapped values, based on raw
long accHorz, accVert; // acceleration for pseudo analog

// invert if needed
const bool invHorz = false;
const bool invVert = false;

//=================================================================================

// Mapping to XBOX Adaptive Controller
// X and Y will auto map to Axis 0/1 or 2/3 depending on which side you plug
// the joystick into (left=0/1 or right=2/3).

// The XBOX Adapative Controller (XAC) likes joystick values from -127 to + 127
// adjust these min/max based on your upstream device. Same values for both X and Y.
const int joyMin = -127;
const int joyMax = +127;

//=================================================================================

const bool isDigital = true;

//=================================================================================


// set the default min/max values.
// Adjust values based on what you actually get from the stick being used.

int minHorz=0;
int centeredHorz=512;
int maxHorz=1023;

int minVert=0;
int centeredVert=512;
int maxVert=1023;

int threshold = 1023;

//hard limits, to catch out-of-range readings
const int limitMinHorz=0;
const int limitMaxHorz=1023;
const int limitMinVert=0;
const int limitMaxVert=1023;

//=================================================================================


void setup() {

  // Startup the joystick object. 
  Joystick.begin(false); // false indicates that sendState method call required (so we do all changes at once).

  // I'm using Map below, but setting range Just In Case.
  // Also setting Axis to zero at startup.
  Joystick.setXAxisRange(joyMin, joyMax); Joystick.setXAxis(0);
  Joystick.setYAxisRange(joyMin, joyMax); Joystick.setYAxis(0);
  Joystick.sendState();

  // Set U/D/L/R (from joystick) to input
  pinMode(SW_UP, INPUT);
  pinMode(SW_DN, INPUT);
  pinMode(SW_LT, INPUT);
  pinMode(SW_RT, INPUT);
  // Set sensitivity to input
  pinMode(SENSE, INPUT);

  if (debug) { 
    Serial.begin(9600);
    Serial.println("SwitchPseudolog_v1A");
  }

  if (!isDigital) { centerAnalogStick; } //WIP WIP WIP

}//setup


//=================================================================================


void loop() {

  // ----------------------------------------------------

  // Read pot on unit to determine sensitivity (0-1023).
  rawSense = analogRead(SENSE); delay(10);
  if (rawSense==0) {rawSense=1;} // disallow zero

  // Read analog values from input pins.
  rawUP = analogRead(SW_UP); delay(10);
  rawDN = analogRead(SW_DN); delay(10);
  rawLT = analogRead(SW_LT); delay(10);
  rawRT = analogRead(SW_RT); delay(10);

  // Toss out invalid ranges
  if (rawUP<limitMinVert || rawUP>limitMaxVert) {rawUP=centeredVert;}
  if (rawDN<limitMinVert || rawDN>limitMaxVert) {rawDN=centeredVert;}
  if (rawLT<limitMinHorz || rawLT>limitMaxHorz) {rawLT=centeredHorz;}
  if (rawRT<limitMinHorz || rawRT>limitMaxHorz) {rawRT=centeredHorz;}

  if (isDigital) { processDigitalReadings(); }

  // set mins and maxes, if different from previous/default values
  if (adjVert<minVert) {minVert=adjVert;}
  if (adjVert>maxVert) {maxVert=adjVert;}
  if (adjHorz<minHorz) {minHorz=adjHorz;}
  if (adjHorz>maxHorz) {maxHorz=adjHorz;}

  // Map values to a range the XAC likes
  mapVert = map(adjVert, minVert, maxVert, joyMin, joyMax);
  mapHorz = map(adjHorz, minHorz, maxHorz, joyMin, joyMax);

  // Invert value if requested (if "up" should go "down" or "left" to "right")
  if (invVert) {mapVert = -mapVert;}
  if (invHorz) {mapHorz = -mapHorz;}

  // Send the values to the joystick object
  Joystick.setYAxis(mapVert);
  Joystick.setXAxis(mapHorz);

  // Send updated joystick state to HID upstream
  Joystick.sendState();

  // ----------------------------------------------------

  if (debug) {serialDebug();}

}//loop


//=================================================================================


void processDigitalReadings() {

    // if digital switches are used with analog read, they will float
    // look for anything with a full up (1023) on the analog read.

    // The amount of movement is dependent on:
    // (a) how long the switch is held (longer-bigger jumps)
    // (b) multiplied by the sensitivity set (via sliding pot)

    adjSense = rawSense/50; //rawSense will be 1 to 1023 (zero disallowed above)
    if (adjSense<1) {adjSense=1;} // set a lower limit that's not zero
    
    //vertical - up/down
    if (rawUP>=threshold) {
      //toward min 
      if (accVert<threshold) {accVert=accVert+adjSense;} 
      if (adjVert-accVert>minVert) {adjVert=adjVert-accVert;} else {adjVert=minVert;}
      } 
    else if (rawDN>=threshold) {
      //toward max
      if (accVert<threshold) {accVert=accVert+adjSense;} 
      if (adjVert+accVert<maxVert) {adjVert=adjVert+accVert;} else {adjVert=maxVert;}
      }
    else { 
      //reset / re-center
      accVert=0; //reset
      adjVert=centeredVert; // re-center
      }
    
    //horizontal -- left/right
    if (rawLT>=threshold) {
      //toward min   
      if (accHorz<threshold) {accHorz=accHorz+adjSense;} 
      if (adjHorz-accHorz>minHorz) {adjHorz=adjHorz-accHorz;} else {adjHorz=minHorz;}
      } 
    else if (rawRT>=threshold) {
      //toward max
      if (accHorz<threshold) {accHorz=accHorz+adjSense;} 
      if (adjHorz+accHorz<maxHorz) {adjHorz=adjHorz+accHorz;} else {adjHorz=maxHorz;}
      } 
    else {
      //reset / re-center 
      accHorz=0; //reset
      adjHorz=centeredHorz; //re-center
      }

}//processDigitalReadings


//=================================================================================


void centerAnalogStick(){

/*
  // On startup, take some readings from the (hopefully) 
  // centered joystick to determine actual center for X and Y. 
  int iNumberOfSamples = 25;
  long lSampleSumHorz = 0;
  long lSampleSumVert = 0;
  for (int iSample = 1; iSample<=iNumberOfSamples; iSample++) {
    lSampleSumHorz += analogRead(HORZ_PIN); delay(10);
    lSampleSumVert += analogRead(VERT_PIN); delay(10);
  }
  centeredHorz=int(lSampleSumHorz/iNumberOfSamples);
  centeredVert=int(lSampleSumVert/iNumberOfSamples);
 */
 
}//centerAnalogStick


//=================================================================================


void serialDebug() {
  
    if (!debugPlot && debugVerbose) {
        Serial.print("V MinCenMax: ");
        Serial.print(minVert);
        Serial.print(" ");
        Serial.print(centeredVert);
        Serial.print(" ");
        Serial.print(maxVert);
        Serial.print(" ");
        Serial.print("H MinCenMax: ");
        Serial.print(minHorz);
        Serial.print(" ");
        Serial.print(centeredHorz);
        Serial.print(" ");
        Serial.print(maxHorz);
        Serial.print(" ");
        Serial.print("Raw U/D/L/R: ");
        Serial.print(rawUP);
        Serial.print(" ");
        Serial.print(rawDN);
        Serial.print(" ");
        Serial.print(rawLT);
        Serial.print(" ");
        Serial.print(rawRT);
        Serial.print(" ");
        Serial.print("Sense: ");
        Serial.print(rawSense);
        Serial.print(" ");
      }//!debugPlot && debugVerbose

      if (!debugPlot) {
        Serial.print("Acc V/H: ");
        Serial.print(accVert);
        Serial.print(" ");
        Serial.print(accHorz);
        Serial.print(" ");
        Serial.print("Adj Sense: ");
        Serial.print(adjSense);
        Serial.print(" ");
        Serial.print("Adj V/H: ");
        Serial.print(adjVert);
        Serial.print(" ");
        Serial.print(adjHorz);
        Serial.print(" ");
    }//!debugPlot

    if (!debugPlot) {Serial.print("Map V/H: ");}
    
    Serial.print(mapVert);
    if (invVert && !debugPlot) { Serial.print("i"); } // i indicates value was inverted
    Serial.print(" ");
    Serial.print(mapHorz);
    if (invHorz && !debugPlot) { Serial.print("i"); } // i indicates value was inverted 
    Serial.print(" ");
    
    Serial.println();

}//serialDebug

//=================================================================================
