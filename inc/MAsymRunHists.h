/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymRunHists_h
#define MAsymRunHists_h

#include "TDirectory.h"
#include "THStack.h"

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

   UInt_t fMinFill;
   UInt_t fMaxFill;
   time_t fMinTime;
   time_t fMaxTime;

private:

   std::map<Double_t, UInt_t> fHTargetVsRun;
   std::map<Double_t, UInt_t> fVTargetVsRun;
   std::map<std::string, THStack*> fHStacks;

public:

   MAsymRunHists();
   MAsymRunHists(TDirectory *dir);
   ~MAsymRunHists();

   void BookHists(std::string sid="");
   void BookHistsPolarimeter(EPolarimeterId polId, EBeamEnergy beamE);
   //void BookHistsEnergy(EBeamEnergy beamE);
   void Fill(ChannelEvent *ch, std::string cutid="");
   //void Fill(Int_t n, Double_t* hData);
   void Fill(EventConfig &rc);
   void PostFill();
   void Print(const Option_t* opt="") const;
   void UpdateLimits();
	void SetMinMaxFill(UInt_t fillId);
	void SetMinMaxTime(UInt_t time);

   ClassDef(MAsymRunHists, 1)
};

#endif
