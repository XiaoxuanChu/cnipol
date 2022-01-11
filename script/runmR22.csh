#!/bin/csh

source /star/u/zchang/.login
source /star/u/zchang/.cshrc
source /star/u/zchang/run22/cnipol/script/r22setup.csh 22 > /dev/null

setenv CNIPOL_ONLINE_DIR /gpfs02/eic/eichome/cnipol/root

/star/u/zchang/run22/cnipol/script/mtest.csh run22_phys
#/star/u/zchang/run22/cnipol/script/mAlphaR22.csh
