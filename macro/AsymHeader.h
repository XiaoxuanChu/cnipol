#ifndef ASYM_HEADER_H
#define ASYM_HEADER_H

#define RHIC_REVOLUTION_FREQ 78e3  // RHIC Revolution Frequency 78 [kHz]
#define RHIC_MAX_FILL 120          // RHIC Maximum Fill Bunches
#define RAMPTIME 350               // duration of the ramp measurement (sec)
#define MAXDELIM 410               // maximum number of delimiter (dynamic motion only)
#define SEC_PER_DELIM 1            // second per deliminter
#define TARGETINDEX 1000           // maximum target full array size including static motion
#define NTBIN 14                   // number of -t bin

#define NSTRIP 72                  // Number of strip channels
#define NSTRIP_PER_DETECTOR 12     // Number of strips per detector
#define NTGTWFD 4                  // Number of target WFD channels
#define NDETECTOR 6                // Number of detectors
#define NBUNCH 120                 // Maximum bunch number
#define R2D 57.29577951            // [rad] -> [degree]
#define MHz 1e-6                   // [Hz]  -> [MHz]
#define G2k 1e6                    // [GeV] -> [keV]
#define k2G 1e-6                   // [keV] -> [GeV]
#define MASS_4He 3.728398e6        // Mass Alpha in [keV]
#define MASS_12C 11.187e6          // Mass Carbon in [keV]
#define C_CMNS 29.98               // Speed of Light in [cm/ns]
#define REC_BEAMMASK 0x00030000

#define CARBON_PATH_DISTANCE 18    // distance traveled by carbon, 18 cm in 2009 run

// 241 Am peak 5.486MeV (85%)
// 
//  21db = 0.0891251 
//  5db = 0.562341  
//  16db = 0.158489 
//  14db = 0.19953
//  14.?db = 0.2000  setting of attenuator board

#define AMPEAK 5486.0
#define ALPHA_KEV 5486.0
#define ATTEN 0.2000       // regular calibration  
#define CCONST 0.2000      // regular calibration  
//#define CCONST 0.1000    // regular calibration  
//#define CCONST 0.4000    // attenuation (x2) runs

const int TGT_OPERATION = 6;           // if nTgtIndex>TGT_OPERATION, then "scan", otherwise "fixed"
const int TOT_WFD_CH = NSTRIP + NTGTWFD; //Total WFD channels including target WFD channels.

#endif