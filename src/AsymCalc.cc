//  Asymmetry Analysis of RHIC pC Polarimeter
//  End-of-Run routine
//  file name :   AsymCalc.cc
// 
//  Author    :   Itaru Nakagawa
//  Creation  :   01/21/2006
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include "TMinuit.h"
#include "TString.h"
#include "TMath.h"
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "WeightedMean.h"
#include "AsymCalc.h"


// =========================
// End of data process
// =========================
int end_process(recordConfigRhicStruct *cfginfo)
{
  if (Flag.feedback){

    //-------------------------------------------------------
    //                Feedback Mode
    //-------------------------------------------------------
    int FeedBackLevel=2; // 1: no fit just max and mean  
                         // 2: gaussian fit
    anal.TshiftAve = TshiftFinder(Flag.feedback, FeedBackLevel);

    // reset counters
    Nevtot = Nread = 0;
    for (int i=0;i<120;i++) Ntotal[i] = 0;
    return 0;

  }else if (!dproc.DMODE) {

    //-------------------------------------------------------
    //    Energy Yeild Weighted Average Analyzing Power
    //-------------------------------------------------------
      //    anal.A_N[0]=WeightAnalyzingPower(10040); // no cut in energy spectra
    anal.A_N[1]=WeightAnalyzingPower(10050); // banana cut in energy spectra


    //-------------------------------------------------------
    //                Normal Mode
    //-------------------------------------------------------
    int bid;
    asymStruct x90[120];  // x90[119] is total
    asymStruct x45[120];
    asymStruct y45[120];
    asymStruct cr45[120];
    asymStruct tx90[120][6];  
    asymStruct tx45[120][6];
    asymStruct ty45[120][6];
    float RL90[120],RL90E[120];
    float RL45[120],RL45E[120];
    float BT45[120],BT45E[120];
    float NL,NR;
    float tmpasym,tmpasyme;
    FILE *fp;
    float RU[120],RD[120],LU[120],LD[120];
    long SIU[6],SID[6];
    int tr,si;
    int gbid[120];    // if 1:good and used 0: be discarded
    float gtmin,gtmax,btmin,btmax;
    float X[HENEBIN],Y[HENEBIN],EX[HENEBIN],EY[HENEBIN];
    float fspinpat[120];
    int i,j,k;
    long Nsi[6]={0,0,0,0,0,0};

    
    //====================================================
    // Right-Left asymmetry
    //====================================================
    for (bid=0;bid<120;bid++){
        
        fspinpat[bid] = (float) spinpat[bid];
        
        // R-L X90
        if (Ncounts[2-1][bid]+Ncounts[5-1][bid]!=0) {

            NR = Ncounts[2-1][bid];
            NL = Ncounts[5-1][bid];
            RL90[bid] = (float) (NR-NL)/(NR+NL);
            RL90E[bid] = (float) 2*NL*NR*sqrt((1./NR)+(1./NL))/(NL+NR)/(NL+NR);

        } else {
            RL90[bid] = 0.;
            RL90E[bid] = 0.;
        }
        // R-L X45 
        if (Ncounts[1-1][bid]+Ncounts[3-1][bid]+
            Ncounts[4-1][bid]+Ncounts[6-1][bid]!=0){
            
            NR = Ncounts[1-1][bid]+Ncounts[3-1][bid];
            NL = Ncounts[4-1][bid]+Ncounts[6-1][bid];
            RL45[bid] = (float) (NR-NL)/(NR+NL);
            RL45E[bid] = (float) 2*NL*NR*sqrt((1./NR)+(1./NL))/(NL+NR)/(NL+NR);

        } else {
            RL45[bid] = 0.;
            RL45E[bid] = 0.;
        }  
        // B-T Y45
        if (Ncounts[3-1][bid]+Ncounts[4-1][bid]+
            Ncounts[1-1][bid]+Ncounts[6-1][bid]!=0){
            
            NR = Ncounts[3-1][bid]+Ncounts[4-1][bid];
            NL = Ncounts[1-1][bid]+Ncounts[6-1][bid];

            BT45[bid] = (float) (NR-NL)/(NR+NL);
            BT45E[bid] = (float) 2*NL*NR*sqrt((1./NR)+(1./NL))/(NL+NR)/(NL+NR);

        } else {
            BT45[bid] = 0.;
            BT45E[bid] = 0.;
        }  
    }

    // *** GOOD/BAD BUNCH CRITERIA 1

    gtmin = 0.0;
    gtmax = 1.0;
    btmin = 0.0;
    btmax = 1.00;

    for (bid=0;bid<120;bid++){
        gbid[bid] = 1;
        // good/total event rate
        if (Ntotal[bid]!=0){
            if (((float)Ngood[bid]/Ntotal[bid])<gtmin){
                fprintf(stdout,"BID: %d discarded (GOOD/TOTAL) %f \n",
                        bid,(float)Ngood[bid]/Ntotal[bid]);
                gbid[bid] = 0;
            }
            if (((float)Ngood[bid]/Ntotal[bid])>gtmax){
                fprintf(stdout,"BID: %d discarded (GOOD/TOTAL) %f \n",
                        bid,(float)Ngood[bid]/Ntotal[bid]);
                gbid[bid] = 0;
            }
        }
        // background / carbon event rate
        if (Ngood[bid]!=0){
            if (((float)Nback[bid]/Ngood[bid])<btmin){
                fprintf(stdout,"BID: %d discarded (BG/GOOD) %f \n",
                        bid,(float)Nback[bid]/Ngood[bid]);
                gbid[bid] = 0;
            }
            if (((float)Nback[bid]/Ngood[bid])>btmax) {
                fprintf(stdout,"BID: %d discarded (BG/GOOD) %f \n",
                        bid,(float)Nback[bid]/Ngood[bid]);
                gbid[bid] = 0;
            }
        }
    }   
    // Counts for each detector
    for (bid=0;bid<120;bid++){
        Nsi[0]+=Ncounts[0][bid];
        Nsi[1]+=Ncounts[1][bid];
        Nsi[2]+=Ncounts[2][bid];
        Nsi[3]+=Ncounts[3][bid];
        Nsi[4]+=Ncounts[4][bid];
        Nsi[5]+=Ncounts[5][bid];
    }


    HHPAK(31010, (float*)Ncounts[0]);
    HHPAK(31020, (float*)Ncounts[1]);
    HHPAK(31030, (float*)Ncounts[2]);
    HHPAK(31040, (float*)Ncounts[3]);
    HHPAK(31050, (float*)Ncounts[4]);
    HHPAK(31060, (float*)Ncounts[5]);


    float x90phys[2][120], x90acpt[2][120], x90lumi[2][120];
    float x45phys[2][120], x45acpt[2][120], x45lumi[2][120];
    float y45phys[2][120], y45acpt[2][120], y45lumi[2][120];
    float c45phys[2][120], c45acpt[2][120], c45lumi[2][120];


    // X90 (2-5) (C:1-4)
    for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + Ncounts[2-1][bid]*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + Ncounts[2-1][bid]*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + Ncounts[5-1][bid]*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + Ncounts[5-1][bid]*((spinpat[bid]==-1)?1:0);
        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x90[bid].phys = tmpasym; x90[bid].physE = tmpasyme; 
        x90phys[0][bid] = tmpasym;
        x90phys[1][bid] = tmpasyme;
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        x90[bid].acpt = tmpasym; x90[bid].acptE = tmpasyme; 
        x90acpt[0][bid] = tmpasym;
        x90acpt[1][bid] = tmpasyme;
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        x90[bid].lumi = tmpasym; x90[bid].lumiE = tmpasyme; 
        x90lumi[0][bid] = tmpasym;
        x90lumi[1][bid] = tmpasyme;
        //        printf("%d : %d %f %f %f %f \n",bid,spinpat[bid],
        //       RU[bid],RD[bid],LU[bid],LD[bid]);
    }
    fprintf(stdout,"si2 up :%10.0f down :%10.0f\n",RU[119],RD[119]);
    fprintf(stdout,"si5 up :%10.0f down :%10.0f\n",LU[119],LD[119]);

    // X45 (13-46) (C:02-35)
    for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[3-1][bid])*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[3-1][bid])*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[4-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[4-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x45[bid].phys = tmpasym; x45[bid].physE = tmpasyme; 
        x45phys[0][bid] = tmpasym;
        x45phys[1][bid] = tmpasyme;
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        x45[bid].acpt = tmpasym; x45[bid].acptE = tmpasyme; 
        x45acpt[0][bid] = tmpasym;
        x45acpt[1][bid] = tmpasyme;
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        x45[bid].lumi = tmpasym; x45[bid].lumiE = tmpasyme; 
        x45lumi[0][bid] = tmpasym;
        x45lumi[1][bid] = tmpasyme;
    }
    fprintf(stdout,"si1,3 up :%10.0f down :%10.0f\n",RU[119],RD[119]);
    fprintf(stdout,"si4,6 up :%10.0f down :%10.0f\n",LU[119],LD[119]);

    // Y45 (34-16) (C:23-05)
    for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[4-1][bid])*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[4-1][bid])*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        y45[bid].phys = tmpasym; y45[bid].physE = tmpasyme; 
        y45phys[0][bid] = tmpasym;
        y45phys[1][bid] = tmpasyme;
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        y45[bid].acpt = tmpasym; y45[bid].acptE = tmpasyme; 
        y45acpt[0][bid] = tmpasym;
        y45acpt[1][bid] = tmpasyme;
        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        y45[bid].lumi = tmpasym; y45[bid].lumiE = tmpasyme; 
        y45lumi[0][bid] = tmpasym;
        y45lumi[1][bid] = tmpasyme;
    }
    // CR45 (14-36) (C:03-25)
    for (bid=1;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[4-1][bid])*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (Ncounts[1-1][bid]+Ncounts[4-1][bid])*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (Ncounts[3-1][bid]+Ncounts[6-1][bid])*((spinpat[bid]==-1)?1:0);
        
        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        cr45[bid].phys = tmpasym; cr45[bid].physE = tmpasyme; 
        c45phys[0][bid]  = tmpasym; 
        c45phys[1][bid] = tmpasyme; 
        
        sqass(RU[bid],RD[bid],LD[bid],LU[bid],&tmpasym,&tmpasyme);
        cr45[bid].acpt = tmpasym; cr45[bid].acptE = tmpasyme; 
        c45acpt[0][bid]  = tmpasym; 
        c45acpt[1][bid] = tmpasyme; 

        sqass(RU[bid],LU[bid],RD[bid],LD[bid],&tmpasym,&tmpasyme);
        cr45[bid].lumi = tmpasym; cr45[bid].lumiE = tmpasyme; 
        c45lumi[0][bid]  = tmpasym; 
        c45lumi[1][bid] = tmpasyme; 
    }
    
    HHPAK(30000, x90phys[0]); HHPAKE(30000, x90phys[1]);
    HHPAK(30010, x90acpt[0]); HHPAKE(30010, x90acpt[1]);
    HHPAK(30020, x90lumi[0]); HHPAKE(30020, x90lumi[1]);

    HHPAK(30100, x45phys[0]); HHPAKE(30100, x45phys[1]);
    HHPAK(30110, x45acpt[0]); HHPAKE(30110, x45acpt[1]);
    HHPAK(30120, x45lumi[0]); HHPAKE(30120, x45lumi[1]);

    HHPAK(30200, y45phys[0]); HHPAKE(30200, y45phys[1]);
    HHPAK(30210, y45acpt[0]); HHPAKE(30210, y45acpt[1]);
    HHPAK(30220, y45lumi[0]); HHPAKE(30220, y45lumi[1]);

    HHPAK(30300, c45phys[0]); HHPAKE(30300, c45phys[1]);
    HHPAK(30310, c45acpt[0]); HHPAKE(30310, c45acpt[1]);
    HHPAK(30320, c45lumi[0]); HHPAKE(30320, c45lumi[1]);




    printf("*************** RESULT *******************\n");
    printf("        physics                luminosity             acceptance\n");
    printf("X90  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            x90[119].phys,x90[119].physE,
            x90[119].lumi,x90[119].lumiE,
            x90[119].acpt,x90[119].acptE);
    printf("X45  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            x45[119].phys,x45[119].physE,
            x45[119].lumi,x45[119].lumiE,
            x45[119].acpt,x45[119].acptE);
    printf("Y45  :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            y45[119].phys,y45[119].physE,
            y45[119].lumi,y45[119].lumiE,
            y45[119].acpt,y45[119].acptE);
    printf("CR45 :%10.6f+-%10.6f %10.6f+-%10.6f %10.6f+-%10.6f\n",
            cr45[119].phys,cr45[119].physE,
            cr45[119].lumi,cr45[119].lumiE,
            cr45[119].acpt,cr45[119].acptE);

    printf("*************** RESULT *******************\n");





    // Target Position Loop
    for (int i=0; i<=nTgtIndex; i++) {

    // X90 (2-5) (C:1-4)
      for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + NDcounts[2-1][bid][i]*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + NDcounts[2-1][bid][i]*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + NDcounts[5-1][bid][i]*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + NDcounts[5-1][bid][i]*((spinpat[bid]==-1)?1:0);
        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x90[bid].phys = tmpasym; x90[bid].physE = tmpasyme; 
        x90phys[0][bid] = tmpasym;
        x90phys[1][bid] = tmpasyme;

      }

     HHPAK(37000+i, x90phys[0]); HHPAKE(37000+i, x90phys[1]);

    // X45 (13-46) (C:02-35)
    for (bid=0;bid<120;bid++){
        RU[bid] = ((bid==0)?0:RU[bid-1])
            + (NDcounts[1-1][bid][i]+NDcounts[3-1][bid][i])*((spinpat[bid]==1)?1:0);
        RD[bid] = ((bid==0)?0:RD[bid-1])
            + (NDcounts[1-1][bid][i]+NDcounts[3-1][bid][i])*((spinpat[bid]==-1)?1:0);
        LU[bid] = ((bid==0)?0:LU[bid-1])
            + (NDcounts[4-1][bid][i]+NDcounts[6-1][bid][i])*((spinpat[bid]==1)?1:0);
        LD[bid] = ((bid==0)?0:LD[bid-1])
            + (NDcounts[4-1][bid][i]+NDcounts[6-1][bid][i])*((spinpat[bid]==-1)?1:0);

        sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
        x45[bid].phys = tmpasym; x45[bid].physE = tmpasyme; 
        x45phys[0][bid] = tmpasym;
        x45phys[1][bid] = tmpasyme;
    }

     HHPAK(37500+i, x45phys[0]); HHPAKE(37500+i, x45phys[1]);

    } // end-of-nTgtIndex loop



    // **** for different t range ****

    float txasym90[6][120], txasym90E[6][120];
    float txasym45[6][120], txasym45E[6][120];
    float tyasym45[6][120], tyasym45E[6][120];
    float tcasym45[6][120], tcasym45E[6][120];


    for (tr=0;tr<NTBIN;tr++){
      float SUM=0;
        // X90 (2-5)
        for (bid=0;bid<120;bid++){
            RU[bid] = ((bid==0)?0:RU[bid-1])
                + NTcounts[2-1][bid][tr]*((spinpat[bid]==1)?1:0)*gbid[bid];
            RD[bid] = ((bid==0)?0:RD[bid-1])
                + NTcounts[2-1][bid][tr]*((spinpat[bid]==-1)?1:0)*gbid[bid];
            LU[bid] = ((bid==0)?0:LU[bid-1])
                + NTcounts[5-1][bid][tr]*((spinpat[bid]==1)?1:0)*gbid[bid];
            LD[bid] = ((bid==0)?0:LD[bid-1])
                + NTcounts[5-1][bid][tr]*((spinpat[bid]==-1)?1:0)*gbid[bid];
            sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
            tx90[bid][tr].phys = tmpasym; tx90[bid][tr].physE = tmpasyme; 
            txasym90[tr][bid] = tmpasym; txasym90E[tr][bid] = tmpasyme; 
	    SUM+=NTcounts[2-1][bid][tr];
        }

        // X45 (13-46)
        for (bid=0;bid<120;bid++){
            RU[bid] = ((bid==0)?0:RU[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[3-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            RD[bid] = ((bid==0)?0:RD[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[3-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            LU[bid] = ((bid==0)?0:LU[bid-1])
                + (NTcounts[4-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            LD[bid] = ((bid==0)?0:LD[bid-1])
                + (NTcounts[4-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
            tx45[bid][tr].phys = tmpasym; tx45[bid][tr].physE = tmpasyme; 
            txasym45[tr][bid] = tmpasym; txasym45E[tr][bid] = tmpasyme; 
        }

        // Y45 (34-16)
        for (bid=0;bid<120;bid++){
            RU[bid] = ((bid==0)?0:RU[bid-1])
                + (NTcounts[3-1][bid][tr]+NTcounts[4-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            RD[bid] = ((bid==0)?0:RD[bid-1])
                + (NTcounts[3-1][bid][tr]+NTcounts[4-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            LU[bid] = ((bid==0)?0:LU[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            LD[bid] = ((bid==0)?0:LD[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
            ty45[bid][tr].phys = tmpasym; ty45[bid][tr].physE = tmpasyme; 
            tyasym45[tr][bid] = tmpasym; tyasym45E[tr][bid] = tmpasyme; 
        }
        // CROSS 45 (14-36)
        for (bid=0;bid<120;bid++){
            RU[bid] = ((bid==0)?0:RU[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[4-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            RD[bid] = ((bid==0)?0:RD[bid-1])
                + (NTcounts[1-1][bid][tr]+NTcounts[4-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            LU[bid] = ((bid==0)?0:LU[bid-1])
                + (NTcounts[3-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==1)?1:0)*gbid[bid];
            LD[bid] = ((bid==0)?0:LD[bid-1])
                + (NTcounts[3-1][bid][tr]+NTcounts[6-1][bid][tr])
                *((spinpat[bid]==-1)?1:0)*gbid[bid];
            sqass(RU[bid],LD[bid],RD[bid],LU[bid],&tmpasym,&tmpasyme);
            tcasym45[tr][bid] = tmpasym; tcasym45E[tr][bid] = tmpasyme; 
        }
        // Fill Histograms
        HHPAK(32000+tr+1, txasym90[tr]);  HHPAKE(32000+tr+1, txasym90E[tr]);
        HHPAK(32100+tr+1, txasym45[tr]);  HHPAKE(32100+tr+1, txasym45E[tr]);
        HHPAK(32200+tr+1, tyasym45[tr]);  HHPAKE(32200+tr+1, tyasym45E[tr]);
        HHPAK(32300+tr+1, tcasym45[tr]);  HHPAKE(32300+tr+1, tcasym45E[tr]);
    }   
    
    // Fill Histograms
    HHPAK(31000, fspinpat);
    HHPAK(31100, RL90);    HHPAKE(31100, RL90E);
    HHPAK(31110, RL45);    HHPAKE(31110, RL45E);
    HHPAK(31120, BT45);    HHPAKE(31120, BT45E);

    HHPAK(33000, (float*)Ngood);
    HHPAK(33010, (float*)Nback);
    HHPAK(33020, (float*)Ntotal);

    // Spin Sorted Strip Distribution  
    HHPAK(36000, (float*)NStrip[0]);
    HHPAK(36100, (float*)NStrip[1]);


    //-------------------------------------------------------
    // Bunch Asymmetries
    //-------------------------------------------------------
    /*
    BunchAsymmetry(-1, basym.Ax45[0], basym.Ax45[1]);
    BunchAsymmetry(0,  basym.Ax90[0], basym.Ax90[1]);
    BunchAsymmetry(1,  basym.Ay45[0], basym.Ay45[1]);
    HHPAK(31200, basym.Ax45[0]); HHPAKE(31200, basym.Ax45[1]);
    HHPAK(31300, basym.Ax90[0]); HHPAKE(31300, basym.Ax90[1]);
    HHPAK(31400, basym.Ay45[0]); HHPAKE(31200, basym.Ay45[1]);
    */

    //-------------------------------------------------------
    // Strip-by-Strip Asymmetries
    //-------------------------------------------------------
    if (dproc.RECONFMODE) CalcAsymmetry(anal.A_N[1]);
    

    //-------------------------------------------------------
    //  Check 12C Invariant Mass Possition
    //-------------------------------------------------------
    float MaxDev=TshiftFinder(Flag.feedback, 2);


    //-------------------------------------------------------
    //  Check for bunches with too few/many counts
    //-------------------------------------------------------
    //    checkForBadBunches();

  }//end-of-if(!dproc.DMODE)


    //-------------------------------------------------------
    // Specific Luminosity 
    //-------------------------------------------------------
    StructHistStat hstat;
    SpecificLuminosity(hstat.mean, hstat.RMS, hstat.RMSnorm);


    //-------------------------------------------------------
    // Run Informations
    //-------------------------------------------------------
    PrintWarning();
    PrintRunResults(hstat);



    return(0);


}


//
// Class name  :
// Method name : PrintWarning()
//
// Description : print warnings
// Input       : 
// Return      : 
//
void
PrintWarning(){

    printf("-----------------------------------------------------------------------------------------\n");
    printf("------------------------------  Error Detector Results ----------------------------------\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("===> Bunch distribution \n");
    printf(" Number of Problemeatic Bunches      : %d \n", anal.anomaly.nbunch);
    printf(" Problemeatic Bunch ID's             : ");
    for (int i=0; i<anal.anomaly.nbunch; i++) printf("%d ",anal.anomaly.bunch[i]+1) ; 
    printf("\n");
    printf(" Unrecognized Problematic Strips     : ");
    for (int i=0; i<anal.unrecog.anomaly.nbunch; i++) printf("%d ",anal.unrecog.anomaly.bunch[i]+1) ; 
    printf("\n");
    printf("===> Invariant Mass / strip \n");
    printf(" Maximum Mass Deviation [GeV]        : %6.2f   (%d)\n", strpchk.dev.max,  strpchk.dev.st);
    printf(" Maximum Mass fit chi-2              : %6.2f   (%d)\n", strpchk.chi2.max, strpchk.chi2.st);
    printf(" Maximum Mass-Energy Correlation     : %8.4f (%d)\n", strpchk.p1.max, strpchk.p1.st);
    printf(" Weighted Mean InvMass Sigma         : %6.2f \n", strpchk.average[0]);
    printf(" Good strip Mass-Energy Correlation  : %8.4f \n", strpchk.p1.allowance);
    printf(" Good Strip Mass Sigma Allowance[GeV]: %6.2f \n", strpchk.dev.allowance);
    printf(" Good Strip Mass Fit chi2 Allowance  : %6.2f \n", strpchk.chi2.allowance);
    printf(" Number of Problematic Strips        : %d \n", anal.anomaly.nstrip); 
    printf(" Problematic Strips                  : ");
    for (int i=0; i<anal.anomaly.nstrip; i++) printf("%d ", anal.anomaly.st[i]+1);
    printf("\n");
    printf(" Unrecognized Problematic Strips     : ");
    for (int i=0; i<anal.unrecog.anomaly.nstrip; i++) printf("%d ",anal.unrecog.anomaly.st[i]+1) ; 
    printf("\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("\n\n");

    return;
}



//
// Class name  :
// Method name : PrintRunResults()
//
// Description : print analysis results and run infomation
// Input       : StructHistStat hstat
// Return      : 
//
void
PrintRunResults(StructHistStat hstat){

    runinfo.RunTime = runinfo.StopTime - runinfo.StartTime;
    runinfo.EvntRate = float(Nevtot)/float(runinfo.RunTime);
    runinfo.ReadRate = float(Nread)/float(runinfo.RunTime);

    printf("-----------------------------------------------------------------------------------------\n");
    printf("-----------------------------  Operation Messages  --------------------------------------\n");
    printf("-----------------------------------------------------------------------------------------\n");
    if (Flag.feedback)        printf(" Feedback mode     : On \n");
    if (Flag.spin_pattern>=0) printf(" RHIC Spin Pattern : Recovered.\n");
    if (Flag.mask_bunch)      printf(" Applied a mask to the fill pattern in the data stream.\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("\n\n");

    printf("-----------------------------------------------------------------------------------------\n");
    printf("-----------------------------   Analysis Results   --------------------------------------\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf(" RunTime                 [s] = %10d\n",   runinfo.RunTime);
    printf(" Event Rate             [Hz] = %10.1f\n", runinfo.EvntRate);
    printf(" Read Rate              [Hz] = %10.1f\n", runinfo.ReadRate);
    printf(" Target                      =          %c\n",     runinfo.target);
    printf(" Target Operation            =      %s\n",     runinfo.TgtOperation);
    if (runinfo.Run>=6){
        printf(" Maximum Revolution #        = %10d\n", runinfo.MaxRevolution);
        printf(" Reconstructed Duration  [s] = %10.1f\n",runinfo.MaxRevolution/RHIC_REVOLUTION_FREQ);
        printf(" Target Motion Counter       = %10d\n",cntr.tgtMotion);
    }
    printf(" WCM Average                 = %10.1f\n", runinfo.WcmAve);
    printf(" WCM Average w/in rnge       = %10.1f\n", average.average);
    printf(" Specific Luminosity         = %10.2f%10.2f%10.4f\n",hstat.mean, hstat.RMS, hstat.RMSnorm);
    printf(" # of Filled Bunch           = %10d\n", NFilledBunch);
    printf(" bunch w/in WCM range        = %10d\n", average.counter);
    printf(" process rate                = %10.1f [%]\n",(float)average.counter/(float)NFilledBunch*100);
    printf(" Analyzing Power Average     = %10.4f \n", anal.A_N[1]);
    if (dproc.FEEDBACKMODE) 
      printf(" feedback average tshift     = %10.1f [ns]\n",anal.TshiftAve);
    printf(" Average Polarization        = %10.4f%9.4f\n",anal.P[0],anal.P[1]);
    printf(" Polarization (sinphi)       = %10.4f%9.4f\n",anal.sinphi.P[0],anal.sinphi.P[1]);
    printf(" Phase (sinphi)  [deg.]      = %10.4f%9.4f\n",anal.sinphi.dPhi[0]*R2D,anal.sinphi.dPhi[1]*R2D);
    printf(" chi2/d.o.f (sinphi fit)     = %10.4f\n",anal.sinphi.chi2);
    printf("-----------------------------------------------------------------------------------------\n");

    return;
}


//
// Class name  :
// Method name : WeightAnalysingPower()
//
// Description : Caluclate Energy Yeild weighted Analyzing power
//             : Histogram 34000 is filled only once, which is controled by CallFlag
//             : 
// Input       : int HID
// Return      : A_N
//
float
WeightAnalyzingPower(int HID){

    static int CallFlag=0;

      //----------------------------------------------------------------
    // analyzing power
    //
    // A_N values from L. Trueman (new for Run-04)
    //      t = e * 22.18 / 1000000.
    //      Emin = (0.0010-0.001/2.)*1e6/22.18 = 22.5
    //      Emax = (0.0255+0.001/2.)*1e6/22.18 = 1172.2

    float anth[25] = { // for 25 GeV
        0.0324757, 0.0401093, 0.0377283, 0.0339981, 0.0304917,
        0.0274323, 0.0247887, 0.0224906, 0.020473,  0.0186837,
        0.0170816, 0.0156351, 0.0143192, 0.0131145, 0.0120052,
        0.0109786, 0.0100245, 0.00913435, 0.00830108, 0.00751878,
        0.00678244, 0.00608782, 0.00543127, 0.00480969, 0.00422038};

    float anth100[25] = { // for 100 GeV
        0.0297964, 0.0372334, 0.0345393, 0.0306988, 0.0272192,
        0.0242531, 0.0217307, 0.0195634, 0.017677,  0.0160148,
        0.0145340, 0.0132021, 0.0119941, 0.0108907, 0.00987651,
        0.00893914, 0.00806877, 0.00725722, 0.00649782, 0.00578491,
        0.00511384, 0.00448062, 0.00388186, 0.00331461, 0.00277642};

    if (runinfo.BeamEnergy>50) {
        for (int i=0; i<25; i++) anth[i] = anth100[i] ;
    }


    float Emin = 22.5;
    float Emax = 1172.2;
    float DeltaE = (Emax - Emin)/25.;

    int nbin = HENEBIN;
    int fstbin = 1;
    int lstbin = HENEBIN;

    float X[HENEBIN];
    float Y[HENEBIN];
    float EX[HENEBIN];
    float EY[HENEBIN];

    hhrebin_(&HID, X, Y, EX, EY, &nbin, &fstbin, &lstbin);

    int j=0;
    float sum = 0.;
    float suma = 0.;
    for (int i=0;i<HENEBIN;i++){
        j = (int)((X[i] - Emin)/DeltaE);
        sum += Y[i];
        suma += Y[i]*anth[j];
        //printf("%d: Y %f AN %f X %f\n",i,Y[i],anth[j],X[i]);
    }

    float aveA_N = suma/sum;

    // Fill Analyzing power histogram only once.
    if (!CallFlag) HHF1(34000,0.5, aveA_N);
    CallFlag=1;

  return aveA_N;

}// End-of-WeightAnalyzingPower()




//
// Class name  :
// Method name : ExclusionList(int i, int j)
//
// Description : returns true if strip #j is in exclusion candidate
// Input       : int i, int j
// Return      : true/faulse
//
int
ExclusionList(int k, int j, int RHICBeam){

  int test=0;

  int i = k>35 ? k-36: k;
  if ((j==i)||(j==35-i)||(j==36+i)||(j==71-i)) test = 1;

  // This is because of dead channel 62. (Strip#63) in blue
  if (!RHICBeam) {
    if ((j==26)||(j==9)||(j==62)||(j==45)) test = 1;
  }

  return test;

}



//
// Class name  :
// Method name : calcAsymmetry
//
// Description : calculate Asymmetry
// Input       : int a, int b, int atot, int btot
// Return      : float Asym, float dAsym
//
int
calcAsymmetry(int a, int b, int atot, int btot, float &Asym, float &dAsym){

  float R = 0;
  float A = float(a);
  float B = float(b);
  float Atot = float(atot);
  float Btot = float(btot);
  
  if (Btot) R = Atot/Btot;
  if ((A+R*B)&&(A+B)){
    Asym  = (A-R*B) / (A+R*B);
    dAsym = sqrt(4*B*B*A + 4*A*A*B)/(A+B)/(A+B);
  }else{
    Asym = dAsym = 0;
  }

  return 0;
}



//
// Class name  : 
// Method name : WeightedMean(float A[N], float dA[N], int NDAT)
//
// Description : calculate weighted mean
// Input       : float A[N], float dA[N], int NDAT
// Return      : weighted mean
//
float
WeightedMean(float A[N], float dA[N], int NDAT){

  float sum1, sum2, dA2, WM;
  sum1 = sum2 = dA2 = 0;

  for ( int i=0 ; i<NDAT ; i++ ) {
    if (dA[i]){  // skip dA=0 data
      dA2 = dA[i]*dA[i];
      sum1 += A[i]/dA2 ;
      sum2 += 1/dA2 ;
    }
  }

  WM = dA2 == 0 ? 0 : sum1/sum2 ;
  return WM ;

} // end-of-WeightedMean()


//
// Class name  : 
// Method name : WeightedMeanError(float dA[N], int NDAT)
//
// Description : calculate weighted mean error 
// Input       : float dA[N], int NDAT
// Return      : weighted mean error
//
float
WeightedMeanError(float A[N], float dA[N], float Ave, int NDAT){

  float sum, dA2, dWM;
  sum = dA2 = dWM = 0;

  for ( int i=0 ; i<NDAT ; i++ ) {
    if (dA[i]){
      dA2 = dA[i]*dA[i];
      sum += 1/dA2 ;
    }
  }

  dWM = sum == 0 ? 0 : sqrt(1/sum);
  return dWM ;

} // end-of-WeightedMeanError()




//
// Class name  : 
// Method name : calcWeightedMean(float A[N], float dA[N], int NDAT, float &Ave, float &dAve)
//
// Description : call weighted mean and error 
// Input       : float A[N], float dA[N], float Ave, int NDAT
// Return      : Ave, dAve
//
void
calcWeightedMean(float A[N], float dA[N], int NDAT, float &Ave, float &dAve){

    Ave  = WeightedMean(A, dA, NDAT);
    dAve = WeightedMeanError(A,dA, Ave, NDAT);

  return ;

}




//
// Class name  : 
// Method name : SpecificLuminosity()
//
// Description : Handle Specific Luminosity
// Input       : Histograms 10033, 11033, 10034, 11034
// Return      : float &mean, float &RMS, float &RMS_norm
//
void 
SpecificLuminosity(float &mean, float &RMS, float &RMS_norm){


    //-------------------------------------------------------
    // Specific Luminosity 
    //-------------------------------------------------------
  int bid;
  float SpeLumi[120],dSpeLumi[120];
  float SpeLumi_norm[120], dSpeLumi_norm[120];

  for (bid=0; bid<120; bid++) {
    SpeLumi[bid] = wcmdist[bid] != 0 ? Ngood[bid]/wcmdist[bid] : 0 ;
    dSpeLumi[bid] = sqrt(SpeLumi[bid]);
  }
  HHPAK(10033, SpeLumi);    HHPAKE(11033, dSpeLumi);
  float ave = WeightedMean(SpeLumi,dSpeLumi,120);

  if (ave){
    for (bid=0; bid<120; bid++) {
      SpeLumi_norm[bid] = SpeLumi[bid]/ave;
      dSpeLumi_norm[bid] = dSpeLumi[bid]/ave;
    }
  }
  HHPAK(10034, SpeLumi_norm);    HHPAKE(11034, dSpeLumi_norm);


  // Book and fill histograms
  char hcomment[256];
  sprintf(hcomment,"Specific Luminosity");
  HHBOOK1(10035,hcomment,100,ave-ave/2,ave+ave/2.);
  for (bid=0;bid<120;bid++) HHF1(10035,SpeLumi[bid],1);

  // Get variables
  char CHOICE[5]="HIST";
  mean = HHSTATI(10035, 1, CHOICE, 0) ;
  RMS  = HHSTATI(10035, 2, CHOICE, 0) ;
  RMS_norm = (!mean) ? 0 : RMS/mean ;

  return;

}



//
// Class name  : 
// Method name : TshiftFinder
//
// Description : Find Time shift from 12C mass peak fit. Units are all in [GeV]
//             : ouotputs are converted to [keV]
// Input       : int Mode
//             : int FeedBackLevel 1) no fit, just max and mean
//             :                   2) gaussian fit
// Return      : average tshift [ns] @ 500keV
//
float
TshiftFinder(int Mode, int FeedBackLevel){

  char CHOICE[5]="HIST";
  const int np=3;
  float par[np], step[np], pmin[np], pmax[np], sigpar[np];
  char chfun[3]="G";
  char chopt[2]="Q";
  float chi2, hmax;
  float mdev,adev;

  for (int st=0; st<NSTRIP; st++){
    feedback.err[st]=1; // initialization
    int hid= Mode ? 16200+st+1 : 17200+st+1 ;

    switch(FeedBackLevel){

    case 1:     // Level 1 Histogram Maximum and Mean

      feedback.mdev[st] = HHSTATI(hid, 1, CHOICE, 0) - MASS_12C*k2G;
      feedback.RMS[st]  = HHSTATI(hid, 2, CHOICE, 0) ;
      break;

    case 2:   // Level 2 Gaussian Fit

      // parameter initialization 
      par[0] = HHMAX(hid);         // amplitude
      par[1] = MASS_12C*k2G;       // mean [GeV]
      par[2] = dproc.OneSigma*k2G; // sigma [GeV]
      if (par[0]) { // Gaussian Fit unless histogram isn't empty
	HHFITHN(hid, chfun, chopt, np, par, step, pmin, pmax, sigpar, chi2); 
      }else{
	par[2] = feedback.err[st] = 0; // set weight 0
      }
      feedback.err[st]  = sigpar[1] * chi2 * chi2;
      feedback.mdev[st] = par[1] - MASS_12C*k2G; 
      feedback.RMS[st]  = par[2]; 
      feedback.chi2[st] = chi2;

      break;

    }//end-of-switch(FinderLevel)

  }//end-of-st loop


  if (Mode) {
    // Fill summary histograms
    HHPAK(16300,feedback.mdev);  
    HHPAKE(16300,feedback.err);


  } else {

    HHPAK(16310,feedback.RMS);  
    HHPAKE(16310,feedback.err);
    HHPAK(16340,feedback.chi2);  
    StripAnomalyDetector();
    
  }

  mdev = WeightedMean(feedback.mdev,feedback.err,NSTRIP);
  printf("Average Mass Deviation  = %10.2f [GeV/c]\n",mdev);
  adev = mdev*G2k*runconst.M2T/sqrt(400.);
  printf("Tshift Average @ 400keV = %10.2f [ns]\n", adev);

  // Unit Conversion [GeV] -> [keV]
  for (int i=0; i<NSTRIP; i++) {
    feedback.tedev[i] = feedback.mdev[i]*G2k*runconst.M2T;
    feedback.RMS[i] *= G2k;
  }

  return adev ;

}



//
// Class name  : 
// Method name : int InvariantMassCorrelation()
//
// Description : Check the Mass vs. 12C Kinetic Enegy Correlation 
// Input       : int st
// Return      : 
//
int 
InvariantMassCorrelation(int st){

  char htitle[100],histname[100];

  // Function for Fitting
  TF1 * g1 = new TF1("g1","gaus",5,16);
  TF1 * f1 = new TF1("f1","pol1",200,1000);
  f1->SetLineColor(2);

  // Mass vs. Energy correlation
  sprintf(histname,"mass_vs_e_ecut_st%d",st);
  mass_vs_e_ecut[st]->Write();
  TH2F * hslice = (TH2F*) gDirectory->Get(histname);
  hslice->SetName("hslice");

  // slice histogram for fit
  hslice->FitSlicesY(g1);

  TH1D *hslice_1 = (TH1D*)gDirectory->Get("hslice_1");
  sprintf(histname,"mass_vs_energy_corr_st%d",st);
  hslice_1->SetName(histname);
  sprintf(htitle,"%8.3f:Invariant Mass vs. Energy Correlation Fit (Str%d)",runinfo.RUNID,st+1);
  hslice_1->SetTitle(htitle);
  hslice_1->GetXaxis()->SetTitle("12C Kinetic Energy [keV]");
  hslice_1->GetYaxis()->SetTitle("Invariant Mass [GeV]");
  hslice_1->Fit("f1","Q");

  //Get Fitting results
  for (int j=0;j<2; j++) {
    strpchk.ecorr.p[j][st]    = f1->GetParameter(j);
    strpchk.ecorr.perr[j][st] = strpchk.ecorr.p[j][st] != 0 ? f1->GetParError(j) : 0;
  }

  // Delete unnecessary histograms generated by FitSliceY();
  TH1D *hslice_0 = (TH1D*)gDirectory->Get("hslice_0");
  TH1D *hslice_2 = (TH1D*)gDirectory->Get("hslice_2");
  TH1D *hslice_chi2 = (TH1D*)gDirectory->Get("hslice_chi2");
  hslice_0->Delete(); hslice_2->Delete(); hslice_chi2->Delete();

  // Make graph of p1 paramter as a function of strip number when strip number is the last one
  if (st==NSTRIP-1) {
    float strip[NSTRIP],ex[NSTRIP];
    for (int k=0;k<NSTRIP;k++) {strip[k]=k+1;ex[k]=0;}
    TGraphErrors * ecorr = new TGraphErrors(NSTRIP, strip, strpchk.ecorr.p[1], ex, strpchk.ecorr.perr[1]);
    sprintf(htitle,"Run%8.3f : P[1] distribution for Mass vs. Energy Correlation", runinfo.RUNID);
    ecorr -> SetTitle(htitle);
    ecorr -> GetXaxis()->SetTitle("Strip Number");
    ecorr -> GetYaxis()->SetTitle("slope [GeV/keV]");
    ecorr -> SetMarkerStyle(20);
    ecorr -> SetMarkerColor(2);
    TLine * lp = new TLine(0, strpchk.p1.allowance, NSTRIP+1, strpchk.p1.allowance);
    TLine * ln = new TLine(0, strpchk.p1.allowance, NSTRIP+1, strpchk.p1.allowance);
    lp -> SetLineStyle(2);
    ln -> SetLineStyle(2);
    ecorr -> GetListOfFunctions()->Add(lp);
    ecorr -> GetListOfFunctions()->Add(ln);
    ecorr -> Write("ecorr");
  }

  return 0;

}


//
// Class name  : 
// Method name : StripErrorDetector()
//
// Description : fit banana with kinematic function. This routine is incomplete.
//             : 1. fix hard corded runconst.E2T consntant 1459.43. 
//             : 2. delete hbananan_1 histograms
// Input       : 
// Return      : 
//
void 
BananaFit(int st){

  TF1 * functof = new TF1("functof", "1459.43/sqrt(x)",200,1500);

  char hname[100];
  sprintf(hname,"t_vs_e_st%d",st);
  t_vs_e[st]->GetListOfFunctions()->Add(functof);
  t_vs_e[st]->Write();
  TH2F * hbanana = (TH2F*) gDirectory->Get(hname);
  hbanana->SetName("hbanana");

  // Get centers of banana
  hbanana->FitSlicesY();

  TH1D *hbanana_1 = (TH1D*) gDirectory->Get("hbanana_1");
  sprintf(hname,"banana_center_st%d",st);
  hbanana_1->SetName(hname);

  // Delete unnecessary histograms
  TH1D *hbanana_0    = (TH1D*) gDirectory->Get("hbanana_0");
  TH1D *hbanana_2    = (TH1D*) gDirectory->Get("hbanana_2");
  TH1D *hbanana_chi2 = (TH1D*) gDirectory->Get("hbanana_chi2");
  hbanana_0->Delete();  hbanana_2->Delete();  hbanana_chi2->Delete();

  return;
};



//
// Class name  : 
// Method name : StripErrorDetector()
//
// Description : find suspicious strips
// Input       : 
// Return      : 
//
int
StripAnomalyDetector(){


  int counter=0;
  float sigma=0;
  strpchk.average[0] = WeightedMean(feedback.RMS,feedback.err,NSTRIP);
  HHPAK(16320,strpchk.average);  
  
  TF1 *f1 = new TF1("f1","pol1",0,1000);

  strpchk.dev.max  = fabs(feedback.mdev[0]);
  strpchk.chi2.max = feedback.chi2[0];

  for (int i=0; i<NSTRIP; i++) {
    printf("Anomary Check for strip=%d ...\r",i);

    // t vs. Energy (this routine is incomplete)
    //    BananaFit(i);

    // MASS vs. Energy correlation
    InvariantMassCorrelation(i);
    if (!i) strpchk.p1.max   = fabs(strpchk.ecorr.p[1][i]);  // initialize max w/ strip 0
    if (fabs(strpchk.ecorr.p[1][i]) > strpchk.p1.max ) {
      strpchk.p1.max = fabs(strpchk.ecorr.p[1][i]);
      strpchk.p1.st  = i;
    }

    // Maximum devistion of peak from 12C_MASS
    if (fabs(feedback.mdev[i]) > strpchk.dev.max) {
      strpchk.dev.max  = fabs(feedback.mdev[i]);
      strpchk.dev.st   = i;
    }
    // Gaussian Mass fit Largest chi2
    if (feedback.chi2[i] > strpchk.chi2.max) {
      strpchk.chi2.max  = fabs(feedback.chi2[i]);
      strpchk.chi2.st   = i;
    }

    // Calculate one sigma of RMS distribution
    if (feedback.err[i]){
      sigma += (feedback.RMS[i]-strpchk.average[0])*(feedback.RMS[i]-strpchk.average[0])
	/feedback.err[i]/feedback.err[i];
      counter++;

    }

  }
  sigma=sqrt(sigma)/counter; 

  strpchk.p1.allowance   = errdet.MASS_ENERGY_CORR_ALLOWANCE;
  strpchk.dev.allowance  = errdet.MASS_DEV_ALLOWANCE;
  strpchk.chi2.allowance = errdet.MASS_CHI2_ALLOWANCE;
  float devlimit[1]={strpchk.dev.allowance+strpchk.average[0]};
  HHPAK(16330,devlimit);  
  float chi2limit[1]={strpchk.chi2.allowance};
  HHPAK(16350,chi2limit);

  // register and count suspicious strips 

  anal.anomaly.nstrip=0;
  for (int i=0;i<NSTRIP; i++) {
    if ((fabs(feedback.RMS[i])-strpchk.average[0]>strpchk.dev.allowance)   // large chi2 or deviation of peak position from average 
	||(feedback.chi2[i] > strpchk.chi2.allowance)           // chi2 of Gaussian fit on Inv. Mass peak
	||(fabs(strpchk.ecorr.p[1][i]) > strpchk.p1.allowance)) // mass vs. 12C kinetic energy correlation
      {
	anal.anomaly.st[anal.anomaly.nstrip]=i;
	++anal.anomaly.nstrip;
      }
  }
  
  UnrecognizedAnomaly(anal.anomaly.st,anal.anomaly.nstrip,runinfo.DisableStrip,runinfo.NDisableStrip,
		      anal.unrecog.anomaly.st, anal.unrecog.anomaly.nstrip);


  return 0;

};



//
// Class name  : 
// Method name : UnrecognizedAnomaly(int Mode)
//
// Description : Check whether anomalies are recognized or not.
// Input       : 
// Return      : unrecongnized (strip/bunch) ID in array z, and nz
//
int
UnrecognizedAnomaly(int *x, int nx, int *y, int ny, int *z, int &nz){

  int match[nx];
  for (int i=0;i<nx;i++) match[i]=x[i];

  // Check for mathing between two arrays x[nx], y[ny]
  for (int i=0; i<ny; i++){
    for (int j=0; j<nx; j++){
      if (y[i]==match[j]){ match[j]=-1; break;}
    }
  }

  nz=0;
  for (int i=0;i<nx; i++) {
    if (match[i]!=-1) { z[nz]=match[i] ;nz++ ;}
  }


  return 0;

}



//
// Class name  : 
// Method name : BunchAsymmetry
//
// Description : calculate asymmetries bunch by bunch
// Input       : int Mode -1[Ax45], 0[Ax90], 1[Ay45]
// Return      : Asym[NBUNCH], dA[NBUNCH]
//
int
BunchAsymmetry(int Mode, float A[], float dA[]){

  int error=0;
  int Rdet[2], Ldet[2];

  int RU[NBUNCH], RD[NBUNCH], LU[NBUNCH], LD[NBUNCH];
  int LumiRU[NBUNCH], LumiRD[NBUNCH], LumiLU[NBUNCH], LumiLD[NBUNCH];


  switch (Mode) {
  case -1: // Ax45
    Rdet[0]=0; Rdet[1]=2; Ldet[0]=3; Ldet[1]=5;
    break; 
  case 0:  // Ax90
    Rdet[0]=1; Rdet[1]=-1; Ldet[0]=4; Ldet[1]=-1;
    break; 
  case 1:  // Ay45
    Rdet[0]=0; Rdet[1]=5; Ldet[0]=2; Ldet[1]=3;
    break;
  default:
    cerr << "BunchAsymmetry: No muching mode is coded in for " << Mode << endl;
    error=-1;
  }

  // initiarize counters
  for (int i=0;i<NBUNCH;i++) {
    LumiRU[NBUNCH]=LumiRD[NBUNCH]=LumiLU[NBUNCH]=LumiLD[NBUNCH]=RU[i]=RD[i]=LU[i]=LD[i]=0;
    A[i] = dA[i] = 0;
  }


  if (!error){

    for (int bid=0;bid<NBUNCH;bid++){

      // Calculate Luminosity for Right and Left/Up and Down spin
      for (int i=0;i<3;i++) {
	LumiRU[bid] += Ncounts[i][bid]*((spinpat[bid]==1)?1:0);
	LumiRD[bid] += Ncounts[i][bid]*((spinpat[bid]==-1)?1:0);
	LumiLU[bid] += Ncounts[i+3][bid]*((spinpat[bid]==1)?1:0);
	LumiLD[bid] += Ncounts[i+3][bid]*((spinpat[bid]==-1)?1:0);
      }

      // Take sum of Up/Down for Right and Left detectors
      for (int i=0; i<2; i++) {
	RU[bid] += Ncounts[Rdet[i]][bid]*((spinpat[bid]==1)?1:0);
	RD[bid] += Ncounts[Rdet[i]][bid]*((spinpat[bid]==-1)?1:0);
	LU[bid] += Ncounts[Ldet[i]][bid]*((spinpat[bid]==1)?1:0);
	LD[bid] += Ncounts[Ldet[i]][bid]*((spinpat[bid]==-1)?1:0);
	if (!Mode) break;
      }

      if (spinpat[bid]==1) 
	calcAsymmetry(RU[bid],LU[bid],LumiRU[bid],LumiLU[bid],A[bid],dA[bid]);
      if (spinpat[bid]==-1)
	calcAsymmetry(RD[bid],LD[bid],LumiRU[bid],LumiLD[bid],A[bid],dA[bid]);

      cout << bid << " " << A[bid] << " " << dA[bid] << endl;
    } // end-of-for(bid)-loop

  }// end-of-(!error)

  return error;

}



//
// Class name  : 
// Method name : calcAsymmetry
//
// Description : calculate asymmetries
// Input       : aveA_N
// Return      : 
//
void
CalcAsymmetry(float aveA_N){

    //-------------------------------------------------------
    // Strip-by-Strip Asymmetries
    //-------------------------------------------------------

    int LumiSum[2][72]; // Total Luminosity [0]:Spin Up, [1]:Spin Down
    float LumiSum_r[2][72];//Reduced order Total luminosity for histograming
    float LumiRatio[72]; // Luminosity Ratio
    float Asym[72], dAsym[72]; // Raw Asymmetries strip-by-strip
    float P[72], dP[72]; // phi corrected polarization 
    float P_phi[62830],dP_phi[62830]; // phi corrected polarization with extended array
    float Pt[72], dPt[72]; // phi Trancated corrected polarization,

    for (int i=0;i<62830;i++) P_phi[i]=dP_phi[i]=0;


    for (int i=0; i<72; i++) {
      Asym[i] = dAsym[i] = RawP[i] = dRawP[i] = LumiSum_r[0][i] = LumiSum_r[0][i] = LumiRatio[i] = 0;
      LumiSum[0][i] = LumiSum[1][i] = 0;

      // Loop for Total Luminosity
      for (int j=0; j<72; j++) {

	// Calculate Luminosity. Own Strip and ones in cross geometry are excluded.
	if (!ExclusionList(i,j,runinfo.RHICBeam)){
	  for (int k=0; k<=1; k++) LumiSum[k][i]+=NStrip[k][j];
	}

      } // end-of-j-loop. 


      // Luminosity Ratio
      LumiRatio[i] = (float)LumiSum[0][i]/(float)LumiSum[1][i];
      // Calculate Raw Asymmetries for strip-i
      if ((LumiSum[1][i]) && ((NStrip[0][i]+NStrip[1][i])))
	calcAsymmetry(NStrip[0][i], NStrip[1][i], LumiSum[0][i], LumiSum[1][i], Asym[i], dAsym[i]);

      // Reduced Order Luminosity for histograms. Histogram scale is given in float, not double.
      // Cannot accomomdate large entry.
      LumiSum_r[0][i] = LumiSum[0][i]/1e3;
      LumiSum_r[1][i] = LumiSum[1][i]/1e3;

      // Since this is the recoil asymmetries, flip the sign of asymmetry
      Asym[i]*=-1;
      
      // Raw polarization without phi angle weighted A_N
      RawP[i]  =  Asym[i] / aveA_N;
      dRawP[i] = dAsym[i] / aveA_N;

      // Polarization with sin(phi) correction
      P[i]  = RawP[i] / sin(-phi[i]);
      dP[i] = fabs(dRawP[i] / sin(-phi[i]));

      // Dump Polarization to phi array
      int j = int(phi[i]*1e4);
      P_phi[j] = RawP[i];
      dP_phi[j]= dRawP[i];
      // Polarization with trancated sin(phi) correction
      Pt[i]  = RawP[i] / sin(-phit[i]);
      dPt[i] = fabs(dRawP[i] / sin(-phit[i]));



    } // end-of-i-loop


    // printing routine
    if (VERBOSE){
        printf("*========== strip by strip =============\n");
        for (int i=0;i<NSTRIP; i++){
            printf("%4d",i);
            printf("%7.3f", phi[i]);
            printf("%12.3e%12.3e", Asym[i],dAsym[i]);
            printf("%12.3e%12.3e", RawP[i],dRawP[i]);
            printf("%12.3e%12.4e",    P[i],   dP[i]);
            printf("%12.3e%12.4e",   Pt[i],  dPt[i]);
            printf("\n");
        }
        printf("*=======================================\n");
        printf("\n");
    } //end-of-if(VERBOSE)



    // Caluclate Weighted Average
    calcWeightedMean(P, dP, 72, anal.P[0], anal.P[1]);


    // Histrograming
    HHPAK(36010, LumiSum_r[0]);  HHPAK(36110, LumiSum_r[1]); 
    HHPAK(36200, LumiRatio); 
    HHPAK(36210, Asym);  HHPAKE(36210, dAsym);
    HHPAK(36220, RawP);  HHPAKE(36220, dRawP);
    HHPAK(36230, P_phi); HHPAKE(36230, dP_phi);
    HHPAK(36240, P);     HHPAKE(36240, dP);
    HHPAK(36250, phi); 

    // Fit phi-distribution
    AsymFit asymfit;
    asymfit.SinPhiFit(anal.P[0], RawP, dRawP, phi, anal.sinphi.P, anal.sinphi.dPhi, anal.sinphi.chi2);
    //asymfit.SinPhiFit(anal.P[0], anal.sinphi.P, anal.sinphi.dPhi, anal.sinphi.chi2);

    return;

}


//
// Class name  : 
// Method name : sin_phi(Float_t x, Double_t *par)
//
// Description : sin(x) fit. Amplitude and phase as parameters
// Input       : Double_t *x, Double_t *par
// Return      : par[0]*sin(x+par[1])
//
Double_t
sin_phi(Double_t *x, Double_t *par)
{
 Double_t value=par[0]*sin(-x[0]+par[1]);
 return value;
}



//
// Class name  : AsymFit
// Method name : SinPhiFit()
//
// Description : Master Routine for sin(phi) root-fit  
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
//             : Float_t *RawP, Float_t *dRawP, Float_t *phi (vectors to be fit)
// Return      : Float_t *P, Float_t *dphi, Float_t &chi2dof
//
void
AsymFit::SinPhiFit(Float_t p0, Float_t *RawP, Float_t *dRawP, Float_t *phi, 
		   Float_t *P, Float_t *dphi, Float_t &chi2dof)
{
  
  gStyle->SetOptFit(111);

  float dx[NSTRIP];
  for ( int i=0; i<NSTRIP; i++) dx[i] = 0;

  TGraphErrors * tg = new TGraphErrors(NSTRIP, phi, RawP, dx, dRawP);
  tg->SetTitle("sin(phi) fit");
  tg->GetXaxis()->SetTitle("phi [deg.]");
  tg->GetYaxis()->SetTitle("Asymmetry * A_N [%]");
  tg->SetMarkerStyle(20);
  tg->SetMarkerSize(1.5);
  tg->SetLineWidth(2);
  tg->SetMarkerColor(2);
  tg->SetDrawOption("P");

  // define sin(phi) fit function & initialize parmaeters
  TF1 *func = new TF1("sin_phi", sin_phi, 0, 2*M_PI, 2);
  func -> SetParameters(p0,0);
  func -> SetParLimits(0, -1, 1);
  func -> SetParLimits(1, -M_PI, M_PI);
  func -> SetParNames("P","dPhi");
  func -> SetLineColor(2);

  TText * txt = new TText(1,1,"sin(phi) fit");
  tg->GetListOfFunctions()->Add(txt);

  // Perform sin(phi) fit
  tg -> Fit("sin_phi","R");
  tg -> Write("tg");
    
  // Get fitting results
  P[0] = func->GetParameter(0);
  P[1] = func->GetParError(0);
  dphi[0] = func->GetParameter(1);
  dphi[1] = func->GetParError(1);

  // calculate chi2
  chi2dof = func->GetChisquare()/func->GetNDF();

  return ;

}// end-of-AsymFit::SinPhiFit()






/*       following 3 subroutines are unsuccessful MINUIT sin(phi) fit routines.  

//
// Class name  : 
// Method name : fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
//
// Description : Function returns chi2 for MINUIT
// Input       : 
// Return      : chi2
//
void 
fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{

  AsymFit asymfit;
   //calculate chisquare
   Float_t chisq = 0;
   Float_t delta;
   for (Int_t i=0;i<NSTRIP; i++) {
     if (dRawP[i]) delta  = (RawP[i]-asymfit.sinx(phi[i],par))/dRawP[i];
     chisq += delta*delta;
   }

   FitChi2 = chisq;
   f = chisq;
   
}


//
// Class name  : AsymFit
// Method name : sinx(Float_t x, Double_t *par)
//
// Description : sin(x) fit. Amplitude and phase as parameters
// Input       : Float_t x, Double_t *par
// Return      : par[0]*sin(x+par[1])
//
Float_t
AsymFit::sinx(Float_t x, Double_t *par)
{
 Float_t value=par[0]*sin(-x+par[1]);
 return value;
}

//
// Class name  : AsymFit
// Method name : SinPhiFit()
//
// Description : Master Routine for MINUIT call.
// Input       : Float_t p0 (1-par Polarization for par[0] initialization)
// Return      : Float_t *P, Float_t *phi, Float_t &chi2dof
//
void 
AsymFit::SinPhiFit(Float_t p0, Float_t *P, Float_t *phi, Float_t &chi2dof)
{
  

  const Int_t NPAR=2;
  TMinuit *gMinuit = new TMinuit(NPAR);  
  gMinuit->SetFCN(fcn);


   Double_t arglist[10];
   Int_t ierflg = 0;

   // By default, get rid of MINUIT busy messages
   if (!Flag.VERBOSE) {
     arglist[0]=-1;
     gMinuit->mnexcm("SET PRINT", arglist, 1, ierflg);
     arglist[0]=1;
     gMinuit->mnexcm("SET NOWarnings", arglist, 1, ierflg);
   }

// Initiarize par[0]=p0, par[1]=0
   static Double_t vstart[2] = {p0, 0.};
   static Double_t step[4] = {0.01 , 0.01};
   gMinuit->mnparm(0, "P",   vstart[0], step[0], 0,0,ierflg);
   gMinuit->mnparm(1, "phi", vstart[1], step[1], 0,0,ierflg);

// Now ready for minimization step
   arglist[0] = 1000; // call at least 1000 function calls
   arglist[1] = 0.01;  // tolerance
   gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
   gMinuit->mnexcm("MINOS", arglist ,2,ierflg);


// Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
   gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   //   if (!Flag.VERBOSE)  
   gMinuit->mnprin(3,amin);


   // Get Fitting Results
   Double_t val,eval,bnd1,bnd2;
   Int_t ivarbl;
   TString CHNAM;
   gMinuit->mnpout(0,CHNAM,val,eval,bnd1,bnd2,ivarbl);
   P[0]=(Float_t)val;
   //   P[1]=(Float_t)eval;
   P[1]=(Float_t)GetFittingErrors(gMinuit, 0);
   gMinuit->mnpout(1,CHNAM,val,eval,bnd1,bnd2,ivarbl);
   phi[0]=(Float_t)val;
   //   phi[1]=(Float_t)eval;
   phi[1]=(Float_t)GetFittingErrors(gMinuit, 1);


   Int_t NDATA=0;
   for (Int_t i=0;i<NSTRIP; i++) NDATA += dRawP[i] ? 1 : 0;
   chi2dof = FitChi2/Float_t(NDATA-NPAR);

   return;

}



//
// Class name  : AsymFit
// Method name : GetFittingErrors(TMinuit *gMinuit, Int_t NUM)
//
// Description : Retrun parameter errors for parameter NUM
// Input       : TMinuit *gMinuit, Int_t NUM
// Return      : error
//
Double_t 
AsymFit::GetFittingErrors(TMinuit *gMinuit, Int_t NUM){

  Double_t error;
  Double_t eplus, eminus, eparab, globcc;
  gMinuit->mnerrs(NUM,eplus,eminus,eparab,globcc);

  Int_t NoAverage = fabs(eplus)*fabs(eminus) ? 0 : 1;
  if (NoAverage) {
    error = eplus ? fabs(eplus) : fabs(eminus);
  }else{
    error =(fabs(eplus)+fabs(eminus))/2.;
  }

  cout << eplus << " " << eminus << " " << eparab << " " << globcc << " " << error << endl;

  return error;

}


*/


/*
//
// Class name  : RAMP
// Method name : CalcRAMP()
//
// Description : Not being use. 
// Input       : 
// Return      : 
//
void 
RAMP::CalcRAMP()
{

  if (dproc.RAMPMODE==1) {

    for (int dlm=0;dlm<RAMPTIME;dlm++){
      // not need for initialization for RUN,RD,LU,LD
      // they are initialized at bid=0
      memset(SIU,0,sizeof(SIU));
      memset(SID,0,sizeof(SID));
            
      for (bid=0;bid<120;bid++){
	for (si=0;si<6;si++){
	  SIU[si] += (NRcounts[si][bid][dlm])
	    *((spinpat[bid]==1)?1:0)*gbid[bid];
	  SID[si] += (NRcounts[si][bid][dlm])
	    *((spinpat[bid]==-1)?1:0)*gbid[bid];
	}
      }

      // fill the histograms
      for (si=0; si<6; si++) {
	HHF1(21000+si, (float)dlm, SIU[si]);
	HHF1(21100+si, (float)dlm, SID[si]);
      }
      
    }            
  }

  return;

}
*/


//
// Class name  : 
// Method name : checkForBadBunches()
//
// Description : check for bad bunches
// Input       : 
// Return      : 
//
void checkForBadBunches()
{

  // counter initiariztion
  anal.anomaly.nbunch=0;
  bnchchk.allowance=errdet.BUNCH_ALLOWANCE_SIGMA;


	printf("checking for bad bunches\n");
	
	double avg;
	double sigma;
	for(int i=0;i<NDETECTOR;i++)
	{
		avg=0.;
		for(int j=0;j<120;j++)
		{
			avg+=Ncounts[i][j];
		}
		avg=avg/120.;
		
		sigma=0.;
		for(int j=0;j<120;j++)
		{
			sigma+=((Ncounts[i][j]-avg)*(Ncounts[i][j]-avg));
		}
		sigma=sigma/120.;
		sigma=sqrt(sigma);
		
		for(int j=0;j<120;j++)
		{
			if((Ncounts[i][j]-avg)> bnchchk.allowance*sigma)
			{
			  anal.anomaly.bunch[anal.anomaly.nbunch]=j+1;
			  anal.anomaly.nbunch++;
			  printf("WARNING: bunch # %d has very many counts in detector # %d\n", j+1, i+1);

			}
		}
		
	}


	UnrecognizedAnomaly(anal.anomaly.bunch,anal.anomaly.nbunch,runinfo.DisableBunch,runinfo.NDisableBunch,
			    anal.unrecog.anomaly.bunch, anal.unrecog.anomaly.nbunch);


}


			
