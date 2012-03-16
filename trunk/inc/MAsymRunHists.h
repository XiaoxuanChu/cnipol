/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymRunHists_h
#define MAsymRunHists_h

#include "TDirectory.h"
#include "THStack.h"

#include "AnaGlobResult.h"
#include "AsymGlobals.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "RunConfig.h"


/**
 *
 */
class MAsymRunHists : public DrawObjContainer
{
public:

   Float_t fMinFill;
   Float_t fMaxFill;
   time_t  fMinTime;
   time_t  fMaxTime;

private:

   std::map<Double_t, UInt_t> fHTargetVsRun;
   std::map<Double_t, UInt_t> fVTargetVsRun;
   std::map<std::string, THStack*> fHStacks;

public:

   MAsymRunHists();
   MAsymRunHists(TDirectory *dir);
   ~MAsymRunHists();

   void BookHists(std::string sid="");
   void BookHistsByPolarimeter(DrawObjContainer &oc, EPolarimeterId polId, EBeamEnergy beamE);
   void BookHistsByRing(DrawObjContainer &oc, ERingId ringId, EBeamEnergy beamE);
   //void BookHistsPolarimeterGlobRslt(DrawObjContainer &oc, EPolarimeterId polId);
   //void BookHistsEnergy(EBeamEnergy beamE);
   void Fill(const EventConfig &rc);
   void Fill(EventConfig &rc, DrawObjContainer &oc);
   void PostFill();
   void PostFill(AnaGlobResult &agr);
   void PostFillByPolarimeter(AnaGlobResult &agr, AnaFillResultMapIter iafr, EPolarimeterId polId, EBeamEnergy beamE);
   void PostFillByRing(AnaGlobResult &agr, AnaFillResultMapIter afr, ERingId ringId, EBeamEnergy beamE);
   void Print(const Option_t* opt="") const;
   void UpdateLimits();
	void SetMinMaxFill(UInt_t fillId);
	void AdjustMinMaxFill();
	void SetMinMaxTime(UInt_t time);

   ClassDef(MAsymRunHists, 1)
};

#endif
