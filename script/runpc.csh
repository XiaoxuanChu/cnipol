#!/bin/csh

echo ${CNIPOL_RESULTS_DIR}
echo ${CNIPOL_ONLINE_DIR}
set run_name=$1
set args="--update-db -g -r $run_name"
if ( $run_name =~ *alpha0* ) then
    set args="--alpha $args"
else
    set args="$args"
endif
echo $CNIPOL_DIR/build/asym $args
$CNIPOL_DIR/build/asym $args

mkdir -p ${CNIPOL_ONLINE_DIR}/${run_name}/images
cp -rf ${CNIPOL_RESULTS_DIR}/${run_name}/images ${CNIPOL_ONLINE_DIR}/${run_name}/. > /dev/null
cp ${CNIPOL_RESULTS_DIR}/${run_name}/stdoe.log ${CNIPOL_ONLINE_DIR}/${run_name} > /dev/null
cp ${CNIPOL_RESULTS_DIR}/${run_name}/config_calib.dat ${CNIPOL_ONLINE_DIR}/${run_name} > /dev/null
cp ${CNIPOL_RESULTS_DIR}/${run_name}/runconfig.php ${CNIPOL_ONLINE_DIR}/${run_name} > /dev/null
