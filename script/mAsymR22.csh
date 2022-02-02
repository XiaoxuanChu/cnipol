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
set tag=run${RUN_ID}_phys
set runlistcurr=${CNIPOL_RESULTS_DIR}/runXX/lists/$tag.curr
set runlist=${CNIPOL_RESULTS_DIR}/runXX/lists/$tag
root -b -q /star/u/xchu/run22/cnipol/script/getRunList.C\(\"$runlistcurr\"\)

#if ( `diff $runlist $runlistcurr` != "") then
if (1) then
	cp $runlistcurr $runlist
	set args="--update-db -g -m $tag"

	echo $CNIPOL_DIR/build/masym ${args}
	$CNIPOL_DIR/build/masym ${args}

	# This is the rync command you will need to cp over to webserver.
	rsync -av --exclude='*.root' ${CNIPOL_RESULTS_DIR}/$tag ${CNIPOL_ONLINE_DIR}
endif
rm $runlistcurr
rm ${MY_HOME_DIR}/MASYMLOCK
