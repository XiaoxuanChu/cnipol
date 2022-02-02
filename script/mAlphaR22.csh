#!/bin/csh

set MY_HOME_DIR=/star/u/xchu

if ( -e ${MY_HOME_DIR}/MALPHALOCK ) then
    exit;
else
    touch ${MY_HOME_DIR}/MALPHALOCK
endif


#source /star/u/xchu/.cshrc
#source /star/u/xchu/.login

#source /star/u/xchu/run22/cnipol/script/r22setup.csh 22 > /dev/null

#set CNIPOL_ONLINE_DIR=/gpfs02/eic/eichome/cnipol/root

echo ${CNIPOL_RESULTS_DIR}
set runlistcurr=${CNIPOL_RESULTS_DIR}/runXX/lists/run${RUN_ID}_alpha.curr
root -b -q /star/u/xchu/run22/cnipol/script/getAlphaRunList.C\(\"$runlistcurr\"\)
set runlist=${CNIPOL_RESULTS_DIR}/runXX/lists/run${RUN_ID}_alpha

if ( `diff $runlist $runlistcurr` != "") then
	set args="-m run${RUN_ID}_alpha -o ${CNIPOL_RESULTS_DIR}/run${RUN_ID}_alpha/run${RUN_ID}_alpha.root -g --use-ssh"

	echo $CNIPOL_DIR/build/malpha ${args}
	$CNIPOL_DIR/build/malpha ${args}

	# This is the rync command you will need to cp over to webserver.
	rsync -av --exclude='*.root' ${CNIPOL_RESULTS_DIR}/run${RUN_ID}_alpha ${CNIPOL_ONLINE_DIR}
	cp $runlistcurr $runlist
endif
rm $runlistcurr
rm ${MY_HOME_DIR}/MALPHALOCK
