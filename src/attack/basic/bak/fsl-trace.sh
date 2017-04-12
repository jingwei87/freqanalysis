#!/bin/bash

training_trace='fsl-training'
target_trace='target-trace'
hasher='fs-hasher'
results='log'
hasher_outputs='tmp'
analysis_scripts='.'
users=('004' '007' '012' '013' '015' '028')
months=('01' '02' '03' '04' '05' '06')
year=2013
dates=('01' '02' '03' '04' '05' '06' '07' '08' '09' '10'
	'11' '12' '13' '14' '15' '16' '17' '18' '19' '20'
	'21' '22' '23' '24' '25' '26' '27' '28' '29' '30' '31')

for user in ${users[@]}; do

make clean
make

			snapshot="fslhomes-user${user}-2013-05-11"
				if [ -f "${training_trace}"/${snapshot}.tar.gz ]; then
					tar zxf "${training_trace}"/${snapshot}.tar.gz  
					"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > "${hasher_outputs}"/${snapshot} 
					"${analysis_scripts}"/fsl-count "${hasher_outputs}"/${snapshot} "./db-training/"  
					rm -rf ${snapshot}
					rm -rf "${hasher_outputs}"/${snapshot}
				fi

# generate histogram of training distribution
"${analysis_scripts}"/db-stat "./db-training/" > ${hasher_outputs}/db-content
# sort
sort -k2nr "${hasher_outputs}"/db-content > ${hasher_outputs}/hist-training 
rm -f "${hasher_outputs}"/db-content
# update rank db 
"${analysis_scripts}"/db-rank "${hasher_outputs}"/hist-training "./db-training/"

# generate target distribution
			snapshot="fslhomes-user${user}-2013-05-18"
				if [ -f "${target_trace}"/${snapshot}.tar.gz ]; then
					tar zxf "${target_trace}"/${snapshot}.tar.gz  
					"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > "${hasher_outputs}"/${snapshot} 
					"${analysis_scripts}"/fsl-count "${hasher_outputs}"/${snapshot} "./db-target/"  
					rm -rf ${snapshot}
					rm -rf "${hasher_outputs}"/${snapshot}
				fi

# generate histogram of target distribution
"${analysis_scripts}"/db-stat "./db-target/" > ${hasher_outputs}/db-content
# sort
sort -k2nr "${hasher_outputs}"/db-content > ${hasher_outputs}/hist-target 
rm -f "${hasher_outputs}"/db-content
# frequency analysis  
"${analysis_scripts}"/freq-analysis "${hasher_outputs}"/hist-target "./db-training/" > ${user}

done

