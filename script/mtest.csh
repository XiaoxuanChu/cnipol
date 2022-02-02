#!/bin/csh

set MY_HOME_DIR=/star/u/xchu

if ( -e ${MY_HOME_DIR}/MASYMLOCK ) then
    exit;
else
    touch ${MY_HOME_DIR}/MASYMLOCK
endif


#source /star/u/xchu/.cshrc
#source /star/u/xchu/.login

#source /star/u/xchu/run22/cnipol/script/r22setup.csh 22 > /dev/null

#set CNIPOL_ONLINE_DIR=/gpfs02/eic/eichome/cnipol/root

echo ${CNIPOL_RESULTS_DIR}
set file=$1
#if ( `diff $runlist $runlistcurr` != "") then
set args="--update-db -g -m $file"

echo $CNIPOL_DIR/build/masym ${args}
$CNIPOL_DIR/build/masym ${args}

rsync -av --exclude='*.root' ${CNIPOL_RESULTS_DIR}/$file ${CNIPOL_ONLINE_DIR}
#endif
rm ${MY_HOME_DIR}/MASYMLOCK
