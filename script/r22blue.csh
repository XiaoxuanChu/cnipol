#!/bin/csh
set MY_HOME_DIR=/star/u/zchang

if ( -e ${MY_HOME_DIR}/BASYMLOCK ) then
    exit;
else
    touch ${MY_HOME_DIR}/BASYMLOCK
endif

source /star/u/zchang/.cshrc
source /star/u/zchang/.login

source /star/u/zchang/run22/cnipol/script/r22setup.csh 22 > /dev/null

setenv MY_REMOTE_HOME_DIR /home/cfsd/zchang
setenv CNIPOL_ONLINE_DIR /gpfs02/eic/eichome/cnipol/root

set CNIPOL_REMOTE_BLUE_DATA_DIR=/home/blue/20$RUN_ID/data
set RUNLIST=/gpfs02/eic/cnipol/runXX/lists/run${RUN_ID}_ball
touch ${RUNLIST}
#echo PATH=$PATH
#echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
    
# check bluepc tunnel
date > ${MY_HOME_DIR}/TSTTUNBLU
rsync -av ${MY_HOME_DIR}/TSTTUNBLU bluepc:${MY_REMOTE_HOME_DIR}/.
rm ${MY_HOME_DIR}/TSTTUNBLU
rsync -av bluepc:${MY_REMOTE_HOME_DIR}/TSTTUNBLU ${MY_HOME_DIR}/.

set blue_file_list="`rsync -av --min-size=500k --include='*.data' --exclude='*' bluepc:${CNIPOL_REMOTE_BLUE_DATA_DIR}/ ${CNIPOL_DATA_DIR}/ | tee -a /dev/fd/2`"
#echo "blue: $blue_file_list"
set file_list=`echo -n ${blue_file_list} | grep -oP '\S*.data(?=\s|$)'`

if ("$file_list" != "") then
	echo Processing following files:
	echo "$file_list"
        @ counter = 0
	foreach file ($file_list)
        	echo $file >> ${RUNLIST}
        	set run_name=`echo $file | grep -oP "\S*(?=.data)"`
                if ($counter == 2) then
		   wait
                   @ counter = 0
                 endif
		 /usr/bin/nohup  /star/u/zchang/run22/cnipol/script/runpc.csh ${run_name} >& /dev/null &
		#/star/u/zchang/run22/cnipol/script/runpc.csh ${run_name} >& /dev/null
                @ counter += 1
	end
	cat ${RUNLIST} | sort | uniq > ${RUNLIST}.bak
	mv ${RUNLIST}.bak ${RUNLIST}
endif

wait
rm ${MY_HOME_DIR}/BASYMLOCK

