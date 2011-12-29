/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include <ctime>

#include "EventConfig.h"

#include "AnaInfo.h"
#include "DbEntry.h"
#include "MeasInfo.h"
#include "AnaMeasResult.h"
#include "MseMeasInfo.h"

ClassImp(EventConfig)

using namespace std;

/**
 * This is a persistent class. It contains virtually all information about a
 * single measurement.
 *
 * Default constructor.
 * We have to allocate memory for fMeasInfo, fAnaInfo, and fDbEntry as they can be
 * read from a file with a streamer. The streamers do not allocate memory by
 * themselves.
 */
EventConfig::EventConfig() : TObject(), fRandom(new TRandom()), //fConfigInfo(0),
   fMeasInfo(new MeasInfo()), fAnaInfo(new AnaInfo()), // fDbEntry(new DbEntry()), // replace fDbEntry with Mse... objects?
   fCalibrator(new Calibrator(fRandom)), fAnaMeasResult(new AnaMeasResult()), fMseMeasInfoX(new MseMeasInfoX())
{
}


/** Default destructor. */
EventConfig::~EventConfig()
{
   // there should be more than that...
   //delete fConfigInfo;
}


MeasInfo*      EventConfig::GetMeasInfo()      { return fMeasInfo; }
AnaInfo*       EventConfig::GetAnaInfo()       { return fAnaInfo; }
Calibrator*    EventConfig::GetCalibrator()    { return fCalibrator; }
AnaMeasResult* EventConfig::GetAnaMeasResult() { return fAnaMeasResult; }
MseMeasInfoX*  EventConfig::GetMseMeasInfoX()  { return fMseMeasInfoX; }


/** */
void EventConfig::Print(const Option_t* opt) const
{
   printf("EventConfig:\n");
   PrintAsPhp();
}


/** */
void EventConfig::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "<?php\n");

   //fprintf(f, "\n// TRecordConfigRhicStruct data\n");
   //if (!fConfigInfo) {
   //   Error("PrintAsPhp", "fConfigInfo not defined");
   //} else {
   //   fprintf(f, "$rc['data']['NumChannels'] = %d;\n", fConfigInfo->data.NumChannels);
   //}

   fprintf(f, "\n// MeasInfo data\n");
   if (!fMeasInfo) {
      Error("PrintAsPhp", "fMeasInfo not defined");
   } else {
      fMeasInfo->PrintAsPhp(f);
   }

   fprintf(f, "\n// AnaInfo data\n");
   if (!fAnaInfo) {
      Error("PrintAsPhp", "fAnaInfo not defined");
   } else {
      fAnaInfo->PrintAsPhp(f);
   }

   //fprintf(f, "\n// DbEntry data\n");
   //if (!fDbEntry) {
   //   Error("PrintAsPhp", "fDbEntry not defined");
   //} else {
   //   fDbEntry->PrintAsPhp(f);
   //}

   fprintf(f, "\n// Calibrator data\n");
   if (!fCalibrator) {
      Error("PrintAsPhp", "fCalibrator not defined");
   } else {
      fCalibrator->PrintAsPhp(f);
   }

   fprintf(f, "\n// AnaMeasResult data\n");
   if (!fAnaMeasResult) {
      Error("PrintAsPhp", "fAnaMeasResult not defined");
   } else {
      fAnaMeasResult->PrintAsPhp(f);
   }

   fprintf(f, "\n// MseMeasInfoX data\n");
   if (!fMseMeasInfoX) {
      Error("PrintAsPhp", "fMseMeasInfoX not defined");
   } else {
      fMseMeasInfoX->PrintAsPhp(f);
   }
   
   fprintf(f, "?>\n\n");
} //}}}


/** */
void EventConfig::PrintAsConfig(FILE *f) const
{ //{{{
	fprintf(f, "* Strip t0 ec edead A0 A1 ealph dwidth pede C0 C1 C2 C3 C4\n");
	fprintf(f, "* for the dead layer and T0  : %s\n", fMeasInfo->fRunName.c_str());
	//fprintf(f, "* for the Am calibration     : %s\n", fDbEntry->alpha_calib_run_name.c_str());
	fprintf(f, "* for the Am calibration     : %s\n", fAnaInfo->GetAlphaCalibRun().c_str());
	fprintf(f, "* for the Integral/Amplitude : default\n");
	fprintf(f, "* \n");

   fCalibrator->PrintAsConfig(f);
} //}}}


/** */
float EventConfig::ConvertToEnergy(UShort_t adc, UShort_t chId)
{ 
   return 0; //fConfigInfo->data.chan[chId].acoef * adc;
}


/** */
void EventConfig::Streamer(TBuffer &R__b)
{ //{{{
   // Stream an object of class EventConfig.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fRandom;
      //R__b >> fConfigInfo;
      R__b >> fMeasInfo;
      R__b >> fAnaInfo;
      R__b >> fCalibrator;
      R__b >> fAnaMeasResult;
      R__b >> fMseMeasInfoX;
      R__b.CheckByteCount(R__s, R__c, EventConfig::IsA());
   } else {
      R__c = R__b.WriteVersion(EventConfig::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << fRandom;
      //R__b << fConfigInfo;
      R__b << fMeasInfo;
      R__b << fAnaInfo;
      R__b << fCalibrator;
      R__b << fAnaMeasResult;
      R__b << fMseMeasInfoX;
      R__b.SetByteCount(R__c, kTRUE);
   }
} //}}}


/** */
string EventConfig::GetSignature()
{ //{{{
   string strSignature = "";

   if (fAnaInfo) {

      //string strAnaEndTime(25, ' ');
      char strAnaEndTime[25];
      time_t anaEndTime = fAnaInfo->fAnaDateTime + (time_t) fAnaInfo->fAnaTimeReal;
      //printf("***anaEndTime: %lld\n", anaEndTime);
      tm *ltime = localtime(&anaEndTime);
      strftime(strAnaEndTime, 25, "%c", ltime);

      strSignature += fAnaInfo->fRunName + " @ " + strAnaEndTime + " by " + fAnaInfo->fUserGroup.fUser;
      //strSignature += fAnaInfo->fRunName + " @ " + strAnaEndTime + " by " + fAnaInfo->fUserGroup->fRealName;
   }

   if (fMeasInfo) {
      if (strSignature.size() != 0) 
         strSignature += ", ";

      strSignature += fMeasInfo->fAsymVersion;
   }

   return strSignature;
} //}}}


/** */
TBuffer & operator<<(TBuffer &buf, EventConfig *&rec)
{ //{{{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, EventConfig *rec) : \n");
   rec->Streamer(buf);
   return buf;
} //}}}


/** */
TBuffer & operator>>(TBuffer &buf, EventConfig *&rec)
{ //{{{
   //printf("operator>>(TBuffer &buf, EventConfig *rec) : \n");
   rec->Streamer(buf);
   return buf;
} //}}}
