/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef MAsymSingleFillHists_h
#define MAsymSingleFillHists_h

#include "TDirectory.h"

#include "AsymGlobals.h"
#include "AnaFillResult.h"
#include "ChannelEvent.h"
#include "DrawObjContainer.h"
#include "RunConfig.h"


/**
 *
 */
class MAsymSingleFillHists : public DrawObjContainer
{
protected:

   UInt_t fFillId;

public: 

   TH1* fhAsymVsBunchId_X_BLU;
   TH1* fhAsymVsBunchId_X_YEL;

public:

   MAsymSingleFillHists();
   MAsymSingleFillHists(TDirectory *dir);
   ~MAsymSingleFillHists();

   void BookHists();
   void BookHistsPolarimeter(EPolarimeterId polId);
   void SetSignature(const std::string signature);
   void Fill(EventConfig &rc);
   void PostFill();
   void PostFill(AnaFillResult &afr);
   //void UpdateLimits();

   ClassDef(MAsymSingleFillHists, 1)
};

#endif
