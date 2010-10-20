//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymROOT.cc
// 
//  Author    :   I. Nakagawa
//  Creation  :   7/11/2006
//                
/**
 *
 * Oct 18, 2010 - Dmitri Smirnov
 *    - Added SaveChannelTrees() and SaveEventTree(), PrintEventMap()
 *
 */

//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <math.h>
//#include <errno.h>
//#include <signal.h>
//#include <string.h>
//#include <iostream>
//#include <fstream>
//#include "rhicpol.h"
//#include "rpoldata.h"

#include "AsymROOT.h"

// defaults
const float MSIZE=1.2; // Marker size

StructHist Eslope;

// ROOT Histograms
TDirectory * Run;
TDirectory * Raw;
TDirectory * FeedBack;
TDirectory * Kinema;
TDirectory * Bunch;
TDirectory * ErrDet;
TDirectory * Asymmetry;

// Run Dir
TH2F * rate_vs_delim;

// FeedBack Dir
TH2F * mdev_feedback;  
TH1F * mass_feedback[TOT_WFD_CH];  // invariant mass for feedback

// Raw Directory
TH1F * bunch_dist_raw;              // counts per bunch (raw)
TH1F * strip_dist_raw;              // counts per strip (raw)
TH1F * tdc_raw;                     // tdc (raw)
TH1F * adc_raw;                     // adc (raw)
TH2F * tdc_vs_adc_raw;              // tdc vs. adc (raw)
TH2F * tdc_vs_adc_false_bunch_raw;  // tdc vs. adc (raw) for false bunch

// Kinema Direcotry
TH2F * t_vs_e[TOT_WFD_CH];
TH2F * t_vs_e_yescut[TOT_WFD_CH];
TH2F * mass_vs_e_ecut[TOT_WFD_CH];  // Mass vs. 12C Kinetic Energy 
TF1  * banana_cut_l[NSTRIP][2];     // banana cut low 
TF1  * banana_cut_h[NSTRIP][2];     // banana cut high
TLine  * energy_cut_l[NSTRIP];      // energy cut low 
TLine  * energy_cut_h[NSTRIP];      // energy cut high
TH1F  * energy_spectrum[NDETECTOR]; // energy spectrum per detector
TH1F  * energy_spectrum_all;        // energy spectrum for all detector sum
TH1F  * mass_nocut[TOT_WFD_CH];     // invariant mass without banana cut
TH1F  * mass_yescut[TOT_WFD_CH];    // invariant mass with banana cut


// Bunch Distribution
//TH1F * bunch_dist_raw;              // counts per bunch (raw)
TH1F * bunch_dist;                  // counts per bunch
TH1F * wall_current_monitor;        // wall current monitor
TH1F * specific_luminosity;         // specific luminosity

// ErrDet Direcotry
TH2F * mass_chi2_vs_strip;          // Chi2 of Gaussian Fit on Mass peak
TH2F * mass_sigma_vs_strip;         // Mass sigma width vs. strip 
TH2F * mass_e_correlation_strip;    // Mass-energy correlation vs. strip
TH2F * mass_pos_dev_vs_strip;       // Mass position deviation vs. strip
TH1I * good_carbon_events_strip;    // number of good carbon events per strip
TH2F * spelumi_vs_bunch;            // Counting rate vs. bunch
TH1F * bunch_spelumi;               // Counting rate per bunch hisogram
TH1F * asym_bunch_x45;              // Bunch asymmetry histogram for x45 
TH1F * asym_bunch_x90;              // Bunch asymmetry histogram for x90 
TH1F * asym_bunch_y45;              // Bunch asymmetry histogram for y45 

// Asymmetry Directory
TH2F * asym_vs_bunch_x45;           // Asymmetry vs. bunch (x45)
TH2F * asym_vs_bunch_x90;           // Asymmetry vs. bunch (x90)
TH2F * asym_vs_bunch_y45;           // Asymmetry vs. bunch (y45)
TH2F * asym_sinphi_fit;             // strip asymmetry and sin(phi) fit  
TH2F * scan_asym_sinphi_fit;        // scan asymmetry and sin(phi) fit  


/**
 *
 */
Root::Root()
{
   fRawEventTree = 0;
   fChannelEvent = new ChannelEvent();
}

/** Default destructor. */
Root::~Root()
{
   //delete fRawEventTree;
}

//
// Class name  : Root
// Method name : RootFile(char * filename)
//
// Description : Open Root File and define directory structure of histograms
//             : 
// Input       : char *filename
// Return      : 
//
int 
Root::RootFile(char *filename){

  rootfile = new TFile(filename,"RECREATE","ROOT Histogram file");

  // directory structure
  Run       = rootfile->mkdir("Run");
  Raw       = rootfile->mkdir("Raw");
  FeedBack  = rootfile->mkdir("FeedBack");
  Kinema    = rootfile->mkdir("Kinema");
  Bunch     = rootfile->mkdir("Bunch");
  ErrDet    = rootfile->mkdir("ErrDet");
  Asymmetry = rootfile->mkdir("Asymmetry");

  return 0;
}


/**
 *
 */
void Root::CreateTree()
{
   fRawEventTree = new TTree("RawEventTree", "Raw Event Tree");

   fRawEventTree->Branch("ChannelEvent", "ChannelEvent", &fChannelEvent, 32000, 99);
}


void Root::AddChannelEvent(processEvent &event)
{
   fChannelEvent->fEventId.fRevolutionId = event.delim*512 + event.rev*2 + event.rev0;
   fChannelEvent->fEventId.fBunchId      = event.bid;
   fChannelEvent->fEventId.fChannelId    = event.stN;
   fChannelEvent->fChannel.fAmpltd       = event.amp;
   fChannelEvent->fChannel.fTdc          = event.tdc;
   fChannelEvent->fChannel.fIntgrl       = event.intg;

   if (dproc.SAVETREES.test(2) || dproc.SAVETREES.test(3)) {
      fChannelEvents[fChannelEvent->fEventId] = *fChannelEvent;
   }
}


/**
 *
 */
void Root::FillTree()
{
   fRawEventTree->Fill();
}


/**
 *
 */
void Root::PrintEventMap()
{
   ChannelEventMap::const_iterator mi;
   ChannelEventMap::const_iterator mb = fChannelEvents.begin();
   ChannelEventMap::const_iterator me = fChannelEvents.end();

   for (mi=mb; mi!=me; mi++) {
      //mi->first.Print();
      mi->second.Print();
   }
}


/**
 *
 */
void Root::SaveChannelTrees()
{
   if (fChannelEvents.size() <= 0) {
      printf("No channels to save\n");
      return;
   }

   vector<TTree*> chEventTrees;
   TTree *chEventTree = 0;
   //ChannelEvent *chEvent = new ChannelEvent();
   ChannelData *chData = new ChannelData();
   char tmpCharStr[19]; 

   for (int i=0; i!=NSTRIP; i++) {
      sprintf(tmpCharStr, "ChannelEventTree%02d", i);
      chEventTree = new TTree(tmpCharStr, "Channel Event Tree");
      chEventTree->Branch("ChannelData", "ChannelData", &chData, 32000, 99);
      chEventTrees.push_back(chEventTree);
      //printf("size: %d\n", chEventTrees.size());
   }

   ChannelEventMap::iterator mi;
   ChannelEventMap::iterator mb = fChannelEvents.begin();
   ChannelEventMap::iterator me = fChannelEvents.end();

   for (mi=mb; mi!=me; mi++) {
      chData = &mi->second.fChannel;
      chEventTrees[mi->first.fChannelId]->Fill();
   }

   if (rootfile) {

      rootfile->cd();

      for (int i=0; i!=NSTRIP; i++) {
         chEventTrees[i]->Write();
         chEventTrees[i]->Delete();
      }
   }
}


/**
 *
 */
void Root::SaveEventTree()
{
   if (fChannelEvents.size() <= 0) {
      printf("No channels to save\n");
      return;
   }

   AnaEvent *anaEvent = new AnaEvent();
   char tmpCharStr[14]; 

   TTree *anaEventTree = new TTree("AnaEventTree", "Ana Event Tree");
   anaEventTree->Branch("AnaEvent", "AnaEvent", &anaEvent, 32000, 99);

   ChannelEventMap::iterator mi;
   ChannelEventMap::iterator mb = fChannelEvents.begin();
   ChannelEventMap::iterator me = fChannelEvents.end();
   ChannelEventMap::iterator nextmi;

   for (mi=mb; mi!=me; mi++) {
     
      //mi->second.Print();

      anaEvent->fEventId = mi->second.fEventId;
      anaEvent->fChannels[mi->first.fChannelId] = mi->second.fChannel;

      // Pointer to the next element, can be end of map
      nextmi = mi; nextmi++;

      if (anaEvent->fEventId < nextmi->second.fEventId || nextmi == me) {

         anaEventTree->Fill();
         //printf("Filled ana event\n");
         //if (anaEvent->fChannels.size() >= 2) {
         //   printf("XXX size: %d\n", anaEvent->fChannels.size());
         //}

         anaEvent->fChannels.clear();
      }
   }

   if (rootfile) {
      rootfile->cd();
      anaEventTree->Write();
      anaEventTree->Delete();
   }

   //delete anaEventTree;
}


//
// Class name  : Root
// Method name : RootHistBook()
//
// Description : Book ROOT Histograms
//             : 
// Input       : 
// Return      : 
//
int 
Root::RootHistBook(StructRunInfo runinfo)
{
  Char_t hname[100], htitle[100];

  Kinema->cd();
  // 1-dim Energy Spectrum
  Eslope.nxbin=100; Eslope.xmin=0; Eslope.xmax=0.03;
  for (int i=0; i<NDETECTOR; i++) {
    sprintf(hname,"energy_spectrum_det%d",i+1);
    sprintf(htitle,"%.3f : Energy Spectrum Detector %d ",runinfo.RUNID, i+1);
    energy_spectrum[i] = new TH1F(hname,htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
    energy_spectrum[i] -> GetXaxis() -> SetTitle("Momentum Transfer [-GeV/c]^2");
  }
  sprintf(htitle,"%.3f : Energy Spectrum (All Detectors)",runinfo.RUNID);
  energy_spectrum_all = new TH1F("energy_spectrum_all",htitle, Eslope.nxbin, Eslope.xmin, Eslope.xmax);
  energy_spectrum_all -> GetXaxis() -> SetTitle("Momentum Transfer [-GeV/c]^2");


  // Need to book for TOT_WFD_CH instead of NSTRIP to avoid seg. fault by filling histograms by
  // target events strip [73 - 76].
  for (int i=0; i<TOT_WFD_CH; i++) { 

    sprintf(hname,"t_vs_e_st%d",i+1);
    sprintf(htitle,"%.3f : t vs. Kin.Energy Strip-%d ",runinfo.RUNID, i+1);
    t_vs_e[i] = new TH2F(hname,htitle, 50, 200, 1500, 100, 20, 90);
    t_vs_e[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    t_vs_e[i] -> GetYaxis() -> SetTitle("Time of Flight [ns]");

    sprintf(hname,"t_vs_e_yescut_st%d",i+1);
    sprintf(htitle,"%.3f : t vs. Kin.Energy (with cut) Strip-%d ",runinfo.RUNID, i+1);
    t_vs_e_yescut[i] = new TH2F(hname,htitle, 50, 200, 1500, 100, 20, 90);
    t_vs_e_yescut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    t_vs_e_yescut[i] -> GetYaxis() -> SetTitle("Time of Flight [ns]");

    sprintf(hname,"mass_vs_e_ecut_st%d",i+1);
    sprintf(htitle,"%.3f : Mass vs. Kin.Energy (Energy Cut) Strip-%d ",runinfo.RUNID, i+1);
    mass_vs_e_ecut[i] = new TH2F(hname,htitle, 50, 200, 1000, 200, 6, 18);
    mass_vs_e_ecut[i] -> GetXaxis() -> SetTitle("Kinetic Energy [keV]");
    mass_vs_e_ecut[i] -> GetYaxis() -> SetTitle("Invariant Mass [GeV]");

    sprintf(hname,"mass_nocut_st%d",i+1);
    sprintf(htitle,"%.3f : Invariant Mass (nocut) for Strip-%d ",runinfo.RUNID, i+1);
    mass_nocut[i] = new TH1F(hname, htitle, 100, 0, 20);     
    mass_nocut[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");

    sprintf(hname,"mass_yescut_st%d",i+1);
    sprintf(htitle,"%.3f : Invariant Mass (w/cut) for Strip-%d ",runinfo.RUNID, i+1);
    mass_yescut[i] = new TH1F(hname, htitle, 100, 0, 20);     
    mass_yescut[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
    mass_yescut[i] -> SetLineColor(2);



  }


  // FeedBack Directory
  FeedBack->cd();
  for (int i=0; i<TOT_WFD_CH; i++) { 

    sprintf(hname,"mass_feedback_st%d",i+1);
    sprintf(htitle,"%.3f : Invariant Mass (feedback) for Strip-%d ",runinfo.RUNID, i+1);
    mass_feedback[i] = new TH1F(hname, htitle, 100, 0, 20);     
    mass_feedback[i] -> GetXaxis() -> SetTitle("Mass [GeV/c^2]");
    mass_feedback[i] -> SetLineColor(2);

  }



  // Raw Directory
  Raw->cd();
  sprintf(htitle,"%.3f : Raw Counts per Bunch ", runinfo.RUNID);
  bunch_dist_raw = new TH1F("bunch_dist_raw", htitle, NBUNCH, -0.5, NBUNCH-0.5);
  bunch_dist_raw -> GetXaxis() -> SetTitle("Bunch ID");
  bunch_dist_raw -> GetYaxis() -> SetTitle("Counts");
  bunch_dist_raw -> SetFillColor(17);

  sprintf(htitle,"%.3f : Raw Counts per Strip ", runinfo.RUNID);
  strip_dist_raw = new TH1F("strip_dist_raw", htitle, NSTRIP, -0.5, NSTRIP-0.5);
  strip_dist_raw -> GetXaxis() -> SetTitle("Strip ID");
  strip_dist_raw -> GetYaxis() -> SetTitle("Counts");
  strip_dist_raw -> SetFillColor(17);

  sprintf(htitle,"%.3f : Raw TDC (All Strips)", runinfo.RUNID);
  tdc_raw = new TH1F("tdc_raw", htitle, 100, 0, 100);
  tdc_raw -> GetXaxis() -> SetTitle("TDC [channel]");
  tdc_raw -> SetFillColor(17);

  sprintf(htitle,"%.3f : Raw ADC (All Strips)", runinfo.RUNID);
  adc_raw = new TH1F("adc_raw", htitle, 257, -0.5, 256.5);
  adc_raw -> GetXaxis() -> SetTitle("ADC [channel]");
  adc_raw -> SetFillColor(17);

  sprintf(htitle,"%.3f : Raw TDC vs. ADC (All Strips)", runinfo.RUNID);
  tdc_vs_adc_raw = new TH2F("tdc_vs_adc_raw", htitle, 100, -0.5, 256.6, 100, 0, 100);
  tdc_vs_adc_raw -> GetXaxis() -> SetTitle("ADC [channel]");
  tdc_vs_adc_raw -> GetYaxis() -> SetTitle("TDC [channel]");

  sprintf(htitle,"%.3f : Raw TDC vs. ADC (All Strips) false bunch", runinfo.RUNID);
  tdc_vs_adc_false_bunch_raw = new TH2F("tdc_vs_adc_false_bunch_raw", htitle, 100, -0.5, 256.6, 100, 0, 100);
  tdc_vs_adc_false_bunch_raw -> GetXaxis() -> SetTitle("ADC [channel]");
  tdc_vs_adc_false_bunch_raw -> GetYaxis() -> SetTitle("TDC [channel]");
  tdc_vs_adc_false_bunch_raw -> SetMarkerColor(2);


  // Bunch Directory
  Bunch->cd();
  sprintf(htitle,"%.3f : Counts per Bunch ", runinfo.RUNID);
  bunch_dist = new TH1F("bunch_dist", htitle, NBUNCH, -0.5, NBUNCH-0.5);
  bunch_dist -> GetXaxis() -> SetTitle("Bunch ID");
  bunch_dist -> GetYaxis() -> SetTitle("Counts");
  bunch_dist -> SetFillColor(13);

  sprintf(htitle,"%.3f : Wall Current Monitor", runinfo.RUNID);
  wall_current_monitor = new TH1F("wall_current_monitor", htitle, NBUNCH, -0.5, NBUNCH-0.5);
  wall_current_monitor -> GetXaxis() -> SetTitle("Bunch ID");
  wall_current_monitor -> GetYaxis() -> SetTitle("x10^9 protons");
  wall_current_monitor -> SetFillColor(13);

  sprintf(htitle,"%.3f : Specific Luminosity", runinfo.RUNID);
  specific_luminosity = new TH1F("specific_luminosity", htitle, NBUNCH, -0.5, NBUNCH-0.5);
  specific_luminosity -> GetXaxis() -> SetTitle("Bunch ID");
  specific_luminosity -> GetYaxis() -> SetTitle("x10^9 protons");
  specific_luminosity -> SetFillColor(13);

  // Error detectors
  ErrDet->cd();
  sprintf(htitle,"%.3f : Bunch Asymmetry X90", runinfo.RUNID);
  asym_bunch_x90 = new TH1F("asym_bunch_x90", htitle, 100, -0.1, 0.1);
  sprintf(htitle,"%.3f : Bunch Asymmetry X45", runinfo.RUNID);
  asym_bunch_x45 = new TH1F("asym_bunch_x45", htitle, 100, -0.1, 0.1);
  sprintf(htitle,"%.3f : Bunch Asymmetry Y45", runinfo.RUNID);
  asym_bunch_y45 = new TH1F("asym_bunch_y45", htitle, 100, -0.1, 0.1);
  sprintf(htitle,"%.3f : # of Events in Banana Cut per strip", runinfo.RUNID);
  good_carbon_events_strip = new TH1I("good_carbon_events_strip", htitle, NSTRIP, 0.5, NSTRIP+0.5);
  good_carbon_events_strip->SetFillColor(17);

  return 0;

}


//
// Class name  : Root
// Method name : RootFuncBook()
//
// Description : Book ROOT Functions and Histograms using Feedback infomations
//             : This routine shuould be called after Feedback operation
// Input       : 
// Return      : 
//
int 
Root::RootHistBook2(datprocStruct dproc, StructRunConst runconst, StructFeedBack feedback){

  Kinema->cd();
  char formula[100],fname[100];
  float low, high, sqrte, sigma;
  int Color=2;
  int Width=2;

  for (int i=0; i<NSTRIP; i++) {

    for (int j=0; j<2; j++) {
      
      sigma = j ? runconst.M2T*feedback.RMS[i]*dproc.MassSigmaAlt : runconst.M2T*feedback.RMS[i]*dproc.MassSigma;
      int Style = j + 1 ; 

      // lower limit 
      sprintf(formula,"%f/sqrt(x)+(%f)/sqrt(x)", runconst.E2T, sigma);
      sprintf(fname,"banana_cut_l_st%d_mode%d",i,j);
      banana_cut_l[i][j] = new TF1(fname, formula, dproc.enel, dproc.eneu);
      banana_cut_l[i][j] -> SetLineColor(Color); 
      banana_cut_l[i][j] -> SetLineWidth(Width); 
      banana_cut_l[i][j] -> SetLineStyle(Style); 

      // upper limit 
      sprintf(formula,"%f/sqrt(x)-(%f)/sqrt(x)", runconst.E2T, sigma);
      sprintf(fname,"banana_cut_h_st%d",i);
      banana_cut_h[i][j] = new TF1(fname, formula, dproc.enel, dproc.eneu);
      banana_cut_h[i][j] -> SetLineColor(Color); 
      banana_cut_h[i][j] -> SetLineWidth(Width); 
      banana_cut_h[i][j] -> SetLineStyle(Style); 

    }

    // energy cut low
    low  = runconst.E2T/sqrt(double(dproc.enel))-runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(double(dproc.enel));
    high = runconst.E2T/sqrt(double(dproc.enel))+runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(double(dproc.enel));
    energy_cut_l[i] = new TLine(dproc.enel, low, dproc.enel, high);
    energy_cut_l[i] ->SetLineColor(Color);
    energy_cut_l[i] ->SetLineWidth(Width);

    // energy cut high
    low  = runconst.E2T/sqrt(double(dproc.eneu))-runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(double(dproc.eneu));
    high = runconst.E2T/sqrt(double(dproc.eneu))+runconst.M2T*feedback.RMS[i]*dproc.MassSigma/sqrt(double(dproc.eneu));
    energy_cut_h[i] = new TLine(dproc.eneu, low, dproc.eneu, high);
    energy_cut_h[i] ->SetLineColor(Color);
    energy_cut_h[i] ->SetLineWidth(Width);

  }


  return 0;

}



//
// Class name  : Root
// Method name : DeleteHistogram
//
// Description : Delete Unnecessary Histograms
//             : 
// Input       : 
// Return      : 
//
int 
Root::DeleteHistogram(){

  // Delete histograms declared for WFD channel 72 - 75 to avoid crash. These channcles 
  // are for target channels and thus thes histograms wouldn't make any sense.
  for (int i=NSTRIP; i<TOT_WFD_CH; i++ ) {

    t_vs_e[i] -> Delete();
    t_vs_e_yescut[i] -> Delete();
    mass_vs_e_ecut[i] -> Delete();  // Mass vs. 12C Kinetic Energy 
    mass_nocut[i] -> Delete();
    //    mass_yescut[i] -> Delete();

  }

  
  return 0;

}




//
// Class name  : Root
// Method name : RootFile(char * filename)
//
// Description : Write out objects in memory and dump in rootfile before closing it
//             : 
// Input       : 
// Return      : 
//
int 
Root::CloseROOTFile(){
  

  Kinema->cd();
  TLine * l;
  for (int i=0;i<NSTRIP; i++){
    if (t_vs_e[i]) {
      for (int j=0; j<2; j++){
	if (banana_cut_l[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(banana_cut_l[i][j]);
	if (banana_cut_h[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(banana_cut_h[i][j]);
      }
      if (energy_cut_l[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(energy_cut_l[i]);
      if (energy_cut_h[i]) t_vs_e[i] -> GetListOfFunctions() -> Add(energy_cut_h[i]);
    }
  }

  // Write out memory before closing
  /*
  ErrDet->cd();
  if (mass_sigma_vs_strip)    mass_sigma_vs_strip      -> Write();
  if (mass_chi2_vs_strip)       mass_chi2_vs_strip       -> Write();
  if (mass_e_correlation_strip) mass_e_correlation_strip -> Write();
  if (bunch_rate)               bunch_rate    -> Write();
  if (rate_vs_bunch)            rate_vs_bunch -> Write();
  if (asym_bunch_x90)           asym_bunch_x90-> Write();
  if (asym_bunch_x45)           asym_bunch_x45-> Write();
  if (asym_bunch_y45)           asym_bunch_y45-> Write();


  Asymmetry->cd();
  if (asym_sinphi_fit)   asym_sinphi_fit   -> Write();
  if (asym_vs_bunch_x45) asym_vs_bunch_x45 -> Write();
  if (asym_vs_bunch_x90) asym_vs_bunch_x90 -> Write();
  if (asym_vs_bunch_y45) asym_vs_bunch_y45 -> Write();

  */

  if (fRawEventTree) {
     rootfile->cd();
     fRawEventTree->Write();
     fRawEventTree->Delete();
     //delete fRawEventTree;
  }

  rootfile->Write();


  // close rootfile
  rootfile->Close();

  return 0;

}


