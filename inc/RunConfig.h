
#ifndef RunConfig_h
#define RunConfig_h

#include <set>
#include <string>

#include "TColor.h"
#include "TObject.h"


enum EPolarimeterId {kB1U = 0, kY1D = 1, kB2D = 2, kY2U = 3, kUNKNOWN_POLID};

enum EBeamId {kBLUE_BEAM = 1, kYELLOW_BEAM = 2, kUNKNOWN_BEAM};

enum EStreamId {kUPSTREAM = 1, kDOWNSTREAM = 2, kUNKNOWN_STREAM};

enum EMeasType {kMEASTYPE_UNKNOWN = 0x00,
                kMEASTYPE_ALPHA   = 0x01,
                kMEASTYPE_SWEEP   = 0x02,
                kMEASTYPE_FIXED   = 0x04,
                kMEASTYPE_RAMP    = 0x08,
                kMEASTYPE_EMIT    = 0x10};

enum ETargetOrient {kTARGET_H = 0, kTARGET_V = 1, kUNKNOWN_ORIENT};

enum EBeamEnergy {kINJECTION = 24, kBEAM_ENERGY_100 = 100, kFLATTOP = 250};

enum ESpinState {kSPIN_DOWN = -1, kSPIN_NULL = 0, kSPIN_UP = +1};

typedef std::set<EBeamId>                  BeamIdSet;
typedef BeamIdSet::iterator                BeamIdSetIter;

typedef std::set<ETargetOrient>            TargetOrientSet;
typedef TargetOrientSet::iterator          TargetOrientSetIter;

typedef std::set<EBeamEnergy>::iterator    IterBeamEnergy;
typedef std::set<EPolarimeterId>::iterator IterPolarimeterId;
typedef std::set<ESpinState>::iterator     IterSpinState;

typedef std::set<EBeamEnergy>::iterator    BeamEnergyIter;
typedef std::set<EPolarimeterId>::iterator PolarimeterIdIter;
typedef std::set<ESpinState>::iterator     SpinStateIter;


typedef std::set<UShort_t>    ChannelSet;
typedef ChannelSet::iterator  ChannelSetIter;


ostream& operator<<(ostream &os, const ESpinState &ss);


class RunConfig : public TObject
{
private:

public:
   std::set<EPolarimeterId> fPolarimeters;
   std::set<EMeasType>      fMeasTypes;
   std::set<ETargetOrient>  fTargetOrients;
   std::set<EBeamId>        fBeams; //! for future releases
   std::set<EBeamEnergy>    fBeamEnergies;
   std::set<ESpinState>     fSpinStates;

   RunConfig();
   ~RunConfig();

   static std::string AsString(EPolarimeterId polId);
   static std::string AsString(EMeasType measType);
   static std::string AsString(ETargetOrient targetOrient);
   static std::string AsString(EBeamId beamId);
   static std::string AsString(EBeamEnergy beamEnergy);
   static std::string AsString(ESpinState spinState);
   static UShort_t    AsIndex(ESpinState spinState);
   static Color_t     AsColor(EPolarimeterId polId);
   static Color_t     AsColor(ESpinState spin);

   static EBeamId     GetBeamId(EPolarimeterId polId);
   static EStreamId   GetStreamId(EPolarimeterId polId);

   static UShort_t    GetDetectorId(UShort_t chId);
};

#endif
