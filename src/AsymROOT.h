#ifndef ASYM_ROOT_H
#define ASYM_ROOT_H
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TText.h"
#include "TLine.h"



class Root
{
 private:
  
 public:
  int RootFile(char*filename);
  int RootHistBook();
  int DeleteHistogram();
  int CloseROOTFile();

  TFile * rootfile;
  


};



// ROOT Histograms
extern TDirectory * Kinema;
extern TDirectory * ErrDet;
extern TDirectory * Asymmetry;

// Kinema Direcotry
extern TH2F * t_vs_e[TOT_WFD_CH];
extern TH2F * mass_vs_e_ecut[TOT_WFD_CH];  // Mass vs. 12C Kinetic Energy 
extern TH2F * mass_vs_t_ecut[TOT_WFD_CH];  // Mass vs. ToF (w/ Energy Cut)
extern TH2F * mass_vs_t[TOT_WFD_CH];       // Mass vs. ToF (w/o Energy Cut)

// ErrDet Direcotry
extern TGraphErrors * mass_sigma_vs_strip;         // Mass sigma width vs. strip 
extern TGraphErrors * mass_chi2_vs_strip;          // Chi2 of Gaussian Fit on Mass peak
extern TGraphErrors * mass_e_correlation_strip;    // Mass-energy correlation vs. strip
extern TGraph * rate_vs_bunch;                     // Counting rate vs. bunch
extern TH1F * bunch_rate;                          // Counting rate per bunch hisogram
extern TH1F * asym_bunch_x45;                      // Bunch asymmetry histogram for x45 
extern TH1F * asym_bunch_x90;                      // Bunch asymmetry histogram for x90 
extern TH1F * asym_bunch_y45;                      // Bunch asymmetry histogram for y45 

// Asymmetry Directory
extern TH2F * asym_vs_bunch_x45;                   // Asymmetry vs. bunch (x45)
extern TH2F * asym_vs_bunch_x90;                   // Asymmetry vs. bunch (x90)
extern TH2F * asym_vs_bunch_y45;                   // Asymmetry vs. bunch (y45)
extern  TGraphErrors * asym_sinphi_fit;             // strip asymmetry and sin(phi) fit  






#endif /* ASYM_ROOT_H */

