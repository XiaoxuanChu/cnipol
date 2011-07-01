/**
 *
 * 24 Dec, 2010 - Dmitri Smirnov
 *    - Created class
 *
 */

#ifndef RunInfo_h
#define RunInfo_h

#include <bitset>
#include <set>
#include <string>
#include <vector>

#include "TColor.h"
#include "TPRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TObject.h"
#include "TString.h"

#include "rpoldata.h"

#include "AsymHeader.h"
#include "RunConfig.h"
#include "DbEntry.h"

class MseRunInfoX;
class MseRunPeriodX;


/** */
class RunInfo : public TObject
{
protected:

   double       fBeamEnergy;
   Short_t      fExpectedGlobalTdcOffset;
   Float_t      fExpectedGlobalTimeOffset;

public:

   int          Run;
   double       RUNID;
   std::string  fRunName;
   time_t       StartTime;
   time_t       StopTime;
   float        RunTime;
   int          fDataFormatVersion;
   std::string  fAsymVersion;
   EMeasType    fMeasType;
   float        GoodEventRate;
   float        EvntRate;
   float        ReadRate;
	std::map<UShort_t, Float_t> fWallCurMon;
   Float_t      fWallCurMonAve;
   Float_t      fWallCurMonSum;
   Short_t      fPolId;
   UShort_t     fPolBeam;
   UShort_t     fPolStream;
   int          PolarimetryID;
   int          MaxRevolution;
   char         fTargetOrient;
   char         targetID;
   char         TgtOperation[16];
   int          ActiveDetector[N_DETECTORS];
   int          ActiveStrip[N_CHANNELS];
   int          NActiveStrip;
   int          NDisableStrip;
   int          fDisabledChannels[N_CHANNELS];
   std::vector<UShort_t>   fDisabledChannelsVec; // should rename to fDisabledChannels when get rid of the plain array
   std::set<UShort_t>   fActiveSiliconChannels;
   int          NFilledBunch;
   int          NActiveBunch;
   int          NDisableBunch;
   int          DisableBunch[N_BUNCHES];
	float        fProtoCutSlope;
	float        fProtoCutOffset;
	float        fProtoCutAdcMin;
	float        fProtoCutAdcMax;
	float        fProtoCutTdcMin;
	float        fProtoCutTdcMax;
	float        fPulserCutAdcMin;
	float        fPulserCutAdcMax;
	float        fPulserCutTdcMin;
	float        fPulserCutTdcMax;

public:

   RunInfo();
   ~RunInfo();

   std::string GetAlphaCalibFileName() const;
   std::string GetDlCalibFileName() const;
   //void        Streamer(TBuffer &buf);
   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintConfig();
   short       GetPolarimeterId();
   short       GetPolarimeterId(short beamId, short streamId);
   void        GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId);
   void        Update(DbEntry &rundb);
   void        Update(MseRunInfoX& run);
   void        Update(MseRunPeriodX& runPeriod);
   void        ConfigureActiveStrip(int mask);
   void        SetBeamEnergy(Float_t beamEnergy);
   Float_t     GetBeamEnergy();
   void        SetPolarimetrIdRhicBeam(const char* RunID);
   Float_t     GetExpectedGlobalTimeOffset();
   Short_t     GetExpectedGlobalTdcOffset();
   void        DisableChannels(std::bitset<N_DETECTORS> &disabled_det);
	Bool_t      IsDisabledChannel(UShort_t chId);
	void        SetDisabledChannel(UShort_t chId);
	Bool_t      IsSiliconChannel(UShort_t chId);
	Bool_t      IsHamaChannel(UShort_t chId);
	Bool_t      IsPmtChannel(UShort_t chId);

   ClassDef(RunInfo, 1)
};

//TBuffer & operator<<(TBuffer &buf, RunInfo *&rec);
//TBuffer & operator>>(TBuffer &buf, RunInfo *&rec);

#endif
