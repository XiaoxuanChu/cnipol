/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "AlphaCalibrator.h"
#include "AsymHeader.h"
#include <fstream>

ClassImp(AlphaCalibrator)

using namespace std;


/** Default constructor. */
AlphaCalibrator::AlphaCalibrator() : Calibrator()
{
}


/** Default destructor. */
AlphaCalibrator::~AlphaCalibrator()
{
}


/** */
void AlphaCalibrator::Calibrate(DrawObjContainer *c)
{
   Info("Calibrate", "Called...");

   string    sCh("  ");
   TH1F     *htemp     = 0;
   TF1      *fitfunc   = new TF1("fitfunc", "gaus(0) + gaus(3)");
   TFitResultPtr fitres;

   fitfunc->SetLineColor(kRed);
   fitfunc->SetLineWidth(3);
   fitfunc->SetNpx(500);

   for (UShort_t i = 1; i <= NSTRIP; i++) {
      sprintf(&sCh[0], "%02d", i);

      ChannelCalib *chCalib;
      ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(i);

      if (iChCalib != fChannelCalibs.end()) {
         chCalib = &iChCalib->second;
      }
      else {
         ChannelCalib newChCalib;
         fChannelCalibs[i] = newChCalib;
         chCalib = &fChannelCalibs[i];
      }

      // Amplitude
      htemp = (TH1F*) c->d["alpha"]->d["channel" + sCh]->o["hAmpltd_ch" + sCh];

      if (!htemp) {
         Error("Calibrate", "Histogram channel%02d/hAmpltd_ch%02d does not exist", i, i);
         continue;
      }

      fitres = Calibrate(htemp, fitfunc);

      if (fitres) {
         chCalib->fACoef    = (ALPHA_KEV / fitres->Value(1)) * ATTEN;
         chCalib->fACoefErr = chCalib->fACoef * fitres->FitResult::Error(1) / fitres->Value(1);
         chCalib->fAChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2() / fitres->Ndf() : -1;

         ((TH1F*) c->d["alpha"]->o["hAmpltdW"])->SetBinContent(i, 100 * fitres->Value(2) / fitres->Value(1));
         ((TH1F*) c->d["alpha"]->o["hAmpltdW"])->SetBinError(i, 100 * fitres->FitResult::Error(2) / (fitres->Value(1)));

         ((TH1F*) c->d["alpha"]->o["hACoef"])->SetBinContent(i, chCalib->fACoef);
         ((TH1F*) c->d["alpha"]->o["hACoef"])->SetBinError(i, chCalib->fACoefErr);

         ((TH1F*) c->d["alpha"]->o["hACoefDisp"])->Fill(chCalib->fACoef);
      }

      // Integral
      htemp = (TH1F*) c->d["alpha"]->d["channel" + sCh]->o["hIntgrl_ch" + sCh];

      if (!htemp) {
         Error("Calibrate", "Histogram channel%02d/hIntgrl_ch%02d does not exist", i, i);
         continue;
      }

      fitres = Calibrate(htemp, fitfunc);

      if (fitres.Get()) {
         chCalib->fICoef    = (ALPHA_KEV / fitres->Value(1)) * ATTEN;
         chCalib->fICoefErr = chCalib->fICoef * fitres->FitResult::Error(1) / fitres->Value(1);
         chCalib->fIChi2Ndf = fitres->Ndf() > 0 ? fitres->Chi2() / fitres->Ndf() : -1;

         ((TH1F*) c->d["alpha"]->o["hIntgrlW"])->SetBinContent(i, 100 * fitres->Value(2) / fitres->Value(1));
         ((TH1F*) c->d["alpha"]->o["hIntgrlW"])->SetBinError(i, 100 * fitres->FitResult::Error(2) / (fitres->Value(1)));

         ((TH1F*) c->d["alpha"]->o["hICoef"])->SetBinContent(i, chCalib->fICoef);
         ((TH1F*) c->d["alpha"]->o["hICoef"])->SetBinError(i, chCalib->fICoefErr);

         ((TH1F*) c->d["alpha"]->o["hICoefDisp"])->Fill(chCalib->fICoef);
      }
      else {
         Error("Calibrate", "Empty TFitResultPtr");
      }
   }

   CalibrateBadChannels(c);
}


/** */
TFitResultPtr AlphaCalibrator::Calibrate(TH1 *h, TF1 *f)
{
   TF1      f_amer("f_amer", "gaus");
   TFitResultPtr fitres = 0;

   h->Print();
   float xmin = h->GetXaxis()->GetXmin();
   float xmax = h->GetXaxis()->GetXmax();

   if (h->Integral() < 10) {
      Error("Calibrate", "Too few entries in histogram %s. Skipped", h->GetName());
      return 0;
   }

   // First fit is to find americium peak
   // Will start from guessing initial params
   int   mbin_amer = h->GetMaximumBin();
   float norm_amer = h->GetBinContent(mbin_amer);
   float mean_amer = h->GetBinCenter(mbin_amer);
   float expectedSigma = 0.7;

   f_amer.SetParameters(norm_amer, mean_amer, expectedSigma);
   f_amer.SetParLimits(0, 0.8 * norm_amer, 1.2 * norm_amer); // norm
   f_amer.SetParLimits(1, xmin, xmax); // mean
   f_amer.SetParLimits(2, 0.1, 3 * expectedSigma); // sigma

   h->Fit(&f_amer, "BM"); // B: use limits, M: improve fit

   Double_t par[6]; // params for more general fit
   f_amer.GetParameters(&par[0]); // copy found params there

   mean_amer = par[1];
   float sigma_amer = par[2];

   // Now let's find initial params for gadolinium peak
   float xmax_gad = mean_amer - 3 * sigma_amer;
   // Use predicted gad peak position to track the right peak
   float predicted_gad = mean_amer / AM_ALPHA_E * GD_ALPHA_E;
   float xmin_gad = predicted_gad - 4 * sigma_amer;
   h->GetXaxis()->SetRangeUser(xmin_gad, xmax_gad);
   int   mbin_gad = h->GetMaximumBin();
   h->GetXaxis()->SetRange(); // reset range
   float norm_gad = h->GetBinContent(mbin_gad);
   float mean_gad = h->GetBinCenter(mbin_gad);

   par[3] = norm_gad;
   par[4] = mean_gad;
   par[5] = expectedSigma;
   f->SetParameters(par);

   f->SetParLimits(3, 0.8 * norm_gad, 1.2 * norm_gad); // norm
   f->SetParLimits(4, xmin_gad, xmax_gad); // mean
   f->SetParLimits(5, 0.1, 3 * expectedSigma); // sigma

   // And make sure that our existing peak won't too far
   f->SetParLimits(0, 0.8 * norm_amer, 1.2 * norm_amer); // norm
   f->SetParLimits(1, mean_amer - sigma_amer, mean_amer + sigma_amer); // mean
   f->SetParLimits(2, 0.8 * sigma_amer, 1.2 * sigma_amer); // sigma

   fitres = h->Fit(f, "BMS"); // B: use limits, M: improve fit, S: return fitres

   return fitres;
}


/** */
void AlphaCalibrator::CalibrateBadChannels(DrawObjContainer *c)
{
   Double_t aMean = ((TH1F*) c->d["alpha"]->o["hACoefDisp"])->GetMean();
   Double_t aRMS  = ((TH1F*) c->d["alpha"]->o["hACoefDisp"])->GetRMS();
   //Double_t iMean = ((TH1F*) c->d["alpha"]->o["hICoefDisp"])->GetMean();
   //Double_t iRMS  = ((TH1F*) c->d["alpha"]->o["hICoefDisp"])->GetRMS();

   Double_t aEntries = 0;
   //Double_t iEntries = 0;

   Double_t Det1eRMS  = 0;
   Double_t Det2eRMS  = 0;
   Double_t Det3eRMS  = 0;
   Double_t Det4eRMS  = 0;
   Double_t Det5eRMS  = 0;
   Double_t Det6eRMS  = 0;
   Double_t Det1eMean = 0;
   Double_t Det2eMean = 0;
   Double_t Det3eMean = 0;
   Double_t Det4eMean = 0;
   Double_t Det5eMean = 0;
   Double_t Det6eMean = 0;

   int i = 1;

   ChannelCalibMap::iterator mi, mi2;
   ChannelCalibMap::iterator mb  = fChannelCalibs.begin();
   ChannelCalibMap::iterator me  = fChannelCalibs.end();

   string sSi("  ");

   for (i = 1; i <= NSTRIP; i++) {
      sprintf(&sSi[0], "%02d", i);
      aEntries = ((TH1F*) c->d["alpha"]->d["channel" + sSi]->o["hAmpltd_ch" + sSi])->GetEntries();

      if (i > 0 && i < 13) {
         ((TH1F*) c->d["alpha"]->o["Detector1_Events"])->SetBinContent(i, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector1_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 12 && i < 25) {
         ((TH1F*) c->d["alpha"]->o["Detector2_Events"])->SetBinContent(i - 12, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector2_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 24 && i < 37) {
         ((TH1F*) c->d["alpha"]->o["Detector3_Events"])->SetBinContent(i - 24, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector3_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 36 && i < 49) {
         ((TH1F*) c->d["alpha"]->o["Detector4_Events"])->SetBinContent(i - 36, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector4_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 48 && i < 61) {
         ((TH1F*) c->d["alpha"]->o["Detector5_Events"])->SetBinContent(i - 48, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector5_Events_Dsp"])->Fill(aEntries);

      }
      if (i > 60 && i < 73) {
         ((TH1F*) c->d["alpha"]->o["Detector6_Events"])->SetBinContent(i - 60, aEntries);
         ((TH1F*) c->d["alpha"]->o["Detector6_Events_Dsp"])->Fill(aEntries);

      }
   }

   Det1eMean = ((TH1F*) c->d["alpha"]->o["Detector1_Events_Dsp"])->GetMean();
   Det1eRMS  = ((TH1F*) c->d["alpha"]->o["Detector1_Events_Dsp"])->GetRMS();
   Det2eMean = ((TH1F*) c->d["alpha"]->o["Detector2_Events_Dsp"])->GetMean();
   Det2eRMS  = ((TH1F*) c->d["alpha"]->o["Detector2_Events_Dsp"])->GetRMS();
   Det3eMean = ((TH1F*) c->d["alpha"]->o["Detector3_Events_Dsp"])->GetMean();
   Det3eRMS  = ((TH1F*) c->d["alpha"]->o["Detector3_Events_Dsp"])->GetRMS();
   Det4eMean = ((TH1F*) c->d["alpha"]->o["Detector4_Events_Dsp"])->GetMean();
   Det4eRMS  = ((TH1F*) c->d["alpha"]->o["Detector4_Events_Dsp"])->GetRMS();
   Det5eMean = ((TH1F*) c->d["alpha"]->o["Detector5_Events_Dsp"])->GetMean();
   Det5eRMS  = ((TH1F*) c->d["alpha"]->o["Detector5_Events_Dsp"])->GetRMS();
   Det6eMean = ((TH1F*) c->d["alpha"]->o["Detector6_Events_Dsp"])->GetMean();
   Det6eRMS  = ((TH1F*) c->d["alpha"]->o["Detector6_Events_Dsp"])->GetRMS();

   i = 1;

   //ofstream andersout;
   //andersout.open("andersout.txt");
   for (mi = mb; mi != me; mi++) {

      sprintf(&sSi[0], "%02d", i);
      aEntries = ((TH1F*) c->d["alpha"]->d["channel" + sSi]->o["hAmpltd_ch" + sSi])->GetEntries();

      ChannelCalib &ch = mi->second;

      // Invalidate channels even if the fitter returned some results
      if (i > 0 && i <= 12) {
         if (fabs(aMean - ch.fACoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det1eMean - aEntries) > 2 * Det1eRMS && (aEntries - Det1eMean < 0)) {
               ch.fACoefErr = -1;
            }
         }
      }
      if (i > 12 && i <= 24) {
         if (fabs(aMean - ch.fACoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det2eMean - aEntries) > 2 * Det2eRMS && (aEntries - Det2eMean < 0)) {
               ch.fACoefErr = -1;
            }
         }
      }
      if (i > 24 && i <= 36) {
         if (fabs(aMean - ch.fACoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det3eMean - aEntries) > 2 * Det3eRMS && (aEntries - Det3eMean < 0)) {
               ch.fACoefErr = -1;
            }
         }
      }
      if (i > 36 && i <= 48) {
         if (fabs(aMean - ch.fACoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det4eMean - aEntries) > 2 * Det4eRMS && (aEntries - Det4eMean < 0)) {
               ch.fACoefErr = -1;
            }
         }
      }
      if (i > 48 && i <= 60) {
         if (fabs(aMean - ch.fACoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det5eMean - aEntries) > 2 * Det5eRMS && (aEntries - Det5eMean < 0)) {
               ch.fACoefErr = -1;
            }
         }
      }
      if (i > 60 && i <= 72) {
         if (fabs(aMean - ch.fACoef) > 2*aRMS ) {
            if (aEntries<1000 && aEntries >= 0 && fabs(Det6eMean - aEntries) > 2 * Det6eRMS && (aEntries - Det6eMean < 0)) {
               ch.fACoefErr = -1;
            }
         }
      }

      i++;
   }

   // Now assign detector average
   // Ampltitude
   for (mi = mb; mi != me; mi++) {

      ChannelCalib &ch = mi->second;
      if (ch.fACoefErr >= 0) continue;

      short iDet = (short) floor((mi->first - 1) / 12);
      //printf("iDet: %d\n", iDet);

      float detAve = 0;
      short nChDet = 0;

      for (mi2 = mb; mi2 != me; mi2++) {

         ChannelCalib &ch2 = mi2->second;
         short iDet2 = (short) floor((mi2->first - 1) / 12);
         if (iDet == iDet2 && ch2.fACoefErr >= 0) {

            //printf("iDet2: %d\n", iDet2);
            detAve += ch2.fACoef;
            nChDet++;
         }
      }

      detAve    /= nChDet;
      ch.fACoef  = detAve;
   }

   // Now assign detector average
   // Integral
   for (mi = mb; mi != me; mi++) {

      ChannelCalib &ch = mi->second;
      if (ch.fICoefErr >= 0) continue;

      short iDet = (short) floor((mi->first - 1) / 12);
      //printf("iDet: %d\n", iDet);

      float detAve = 0;
      short nChDet = 0;

      for (mi2 = mb; mi2 != me; mi2++) {
         ChannelCalib &ch2 = mi2->second;
         short iDet2 = (short) floor((mi2->first - 1) / 12);
         if (iDet == iDet2 && ch2.fICoefErr >= 0) {
            //printf("iDet2: %d\n", iDet2);
            detAve += ch2.fICoef;
            nChDet++;
         }
      }

      detAve    /= nChDet;
      ch.fICoef  = detAve;
   }
}


/** */
void AlphaCalibrator::Print(const Option_t* opt) const
{
   Info("Print", "");
   Calibrator::Print(opt);
}
