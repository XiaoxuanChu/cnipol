#!/bin/csh

source /star/u/xchu/.login
source /star/u/xchu/.cshrc
source /star/u/xchu/run22/cnipol/script/r22setup.csh 22 > /dev/null

setenv CNIPOL_ONLINE_DIR /gpfs02/eic/eichome/cnipol/root

/star/u/xchu/run22/cnipol/script/mtest.csh run22_phys
#/star/u/xchu/run22/cnipol/script/mAlphaR22.csh
