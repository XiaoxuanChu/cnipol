#!/bin/bash

CHECKINGPERIOD=86400 # in secs 1 day 

source /direct/eic+u/schmidke/pCpro/pro2/cnipol/script/setup.sh 17 > /dev/null

CNIPOL_ONLINE_DIR=/home/cnipol/root

#while true; do

# You will need to change the following line to include all the measurements you want 
# ELIMINATE TILT SCAN FILLS 20629, 20631, 20658, 20659, 20660, 20690, 20691, 20714.XXX
# ELIMINATE APEX STM FILLS 20723
# ELIMINATE TILT SCAN FILLS 20757.XXX
# ELIMINATE APEX STM FILLS 20774
# ELIMINATE TILT SCAN MEAS. 20802.002-013, 202-214
# ELIMINATE APEX FILL 20842
# ELIMINATE WinBegin=5 MEASUREMENTS 20906.006-.009
# ELIMINATE APEX FILLS 20912, 20914
# ELIMINATE APEX FILLS 20934, 20935
# ELIMINATE APEX FILLS >21150
ls -1d /data1/run17/root/2* | grep -v alpha0 | \
    grep -v 20629 | grep -v 20631 | grep -v 20658 | grep -v 20659 | grep -v 20660 | grep -v 20690 | grep -v 20691 | \
    grep -v 20714.005 | grep -v 20714.006 | grep -v 20714.007 | grep -v 20714.008 | grep -v 20714.009 | \
    grep -v 20714.106 | grep -v 20714.107 | grep -v 20714.108 | grep -v 20714.109 | \
    grep -v 20714.205 | grep -v 20714.206 | grep -v 20714.207 | grep -v 20714.208 | grep -v 20714.209 | \
    grep -v 20714.304 | grep -v 20714.305 | grep -v 20714.306 | \
    grep -v 20723 | \
    grep -v 20757.005 | grep -v 20757.006 | grep -v 20757.007 | grep -v 20757.008 | \
    grep -v 20757.105 | grep -v 20757.106 | grep -v 20757.107 | grep -v 20757.108 | \
    grep -v 20757.206 | grep -v 20757.207 | grep -v 20757.208 | grep -v 20757.209 | \
    grep -v 20757.304 | grep -v 20757.305 | grep -v 20757.306 | grep -v 20757.307 | \
    grep -v 20774 | \
    grep -v 20802.002 | grep -v 20802.003 | grep -v 20802.004 | grep -v 20802.005 | grep -v 20802.006 | grep -v 20802.007 | \
    grep -v 20802.008 | grep -v 20802.009 | grep -v 20802.010 | grep -v 20802.011 | grep -v 20802.012 | grep -v 20802.013 | \
    grep -v 20802.202 | grep -v 20802.203 | grep -v 20802.204 | grep -v 20802.205 | grep -v 20802.206 | grep -v 20802.207 | \
    grep -v 20802.208 | grep -v 20802.209 | grep -v 20802.210 | grep -v 20802.211 | grep -v 20802.212 | grep -v 20802.213 | \
    grep -v 20802.214 | \
    grep -v 20842 | \
    grep -v 20906.006 | grep -v 20906.007 | grep -v 20906.008 | grep -v 20906.009 | \
    grep -v 20912 | grep -v 20914 | \
    grep -v 20934 | grep -v 20935 | \
    grep -v 21151 | grep -v 21152 | grep -v 21153 | grep -v 21154 | grep -v 21155 | \
    grep -v 21156 | grep -v 21157 | grep -v 21158 | grep -v 21159 | grep -v 21160 | \
    grep -v 21161 | grep -v 21162 | grep -v 21163 | grep -v 21164 | grep -v 21165 | \
    grep -v 21166 | grep -v 21167 | grep -v 21168 | grep -v 21169 | grep -v 21170 | \
    grep -v 21171 | grep -v 21172 | grep -v 21173 | grep -v 21174 | grep -v 21175 | \
    grep -v 21176 | grep -v 21177 | grep -v 21178 | grep -v 21179 | grep -v 21180 | \
    xargs -n 1 basename > /data1/runXX/lists/run17_phys
#This was needed in the past. May or may not need for run 17 but change it to your user name
#rm /tmp/cdev_cache-schmidke.sqlite # Dima K. SAYS DON'T DO
# Dima K. SAYS ADD THIS FOR MISSING FILL PROBLEM: (TAKEN OUT 18.04.17)
#sqlite3 /tmp/cdev_cache-schmidke.sqlite "DELETE FROM cache WHERE response LIKE '%Can''t%';"

args="--update-db -g -m run17_phys"

echo $CNIPOL_DIR/build/masym ${args}
$CNIPOL_DIR/build/masym ${args}

# This is the rync command you will need to cp over to webserver.
rsync -av --exclude='*.root' ${CNIPOL_RESULTS_DIR}/run17_phys cnipol@eicsrv1.phy.bnl.gov:${CNIPOL_ONLINE_DIR}

#sleep ${CHECKINGPERIOD}
#done
