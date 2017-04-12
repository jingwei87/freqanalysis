#!/bin/bash

training_trace='fsl-training'
target_trace='target-trace'
hasher='fs-hasher'
results='log'
hasher_outputs='tmp'
analysis_scripts='.'
users=('004' '007' '008' '012' '013' '015' '022' '026' '028')
months=('01' '02' '03' '04' '05' '06')
year=2013
dates=('01' '02' '03' '04' '05' '06' '07' '08' '09' '10'
	'11' '12' '13' '14' '15' '16' '17' '18' '19' '20'
	'21' '22' '23' '24' '25' '26' '27' '28' '29' '30' '31')

# generate training distribution
for month in ${months[@]}; do
	for date in ${dates[@]}; do  
		for user in ${users[@]}; do
			snapshot="fslhomes-user${user}-${year}-${month}-${date}"
			#snapshot="vm1-1"
				if [ -f "${training_trace}"/${snapshot}.tar.gz ]; then
					tar zxf "${training_trace}"/${snapshot}.tar.gz  
					"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > "${hasher_outputs}"/${snapshot} 
					#"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > "${hasher_outputs}"/${snapshot}.plain
					#"${analysis_scripts}"/k-minhash "${hasher_outputs}"/${snapshot}.plain > "${hasher_outputs}"/${snapshot} 
					echo "train ${snapshot}.8kb.hash.anon"
					"${analysis_scripts}"/fsl-count-local "${hasher_outputs}"/${snapshot} "./db-training/" "./left-1/" "./right-1/" 
					rm -rf ${snapshot}
					rm -rf "${hasher_outputs}"/${snapshot}
					rm -rf "${hasher_outputs}"/${snapshot}.plain
				fi
		done
	done
done


# generate target distribution
for month in ${months[@]}; do
	for date in ${dates[@]}; do  
		for user in ${users[@]}; do
			snapshot="fslhomes-user${user}-${year}-${month}-${date}"
			#snapshot="vm1-2"	
				if [ -f "${target_trace}"/${snapshot}.tar.gz ]; then
					tar zxf "${target_trace}"/${snapshot}.tar.gz  
					#"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > "${hasher_outputs}"/${snapshot} 
					"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > "${hasher_outputs}"/${snapshot}.plain
					"${analysis_scripts}"/k-minhash "${hasher_outputs}"/${snapshot}.plain > "${hasher_outputs}"/${snapshot} 
					echo "target ${snapshot}.8kb.hash.anon"
					"${analysis_scripts}"/mh-count "${hasher_outputs}"/${snapshot} "./db-target/" "./left-2/" "./right-2/" "${hasher_outputs}"/${snapshot}.plain "./db-relate/"
					rm -rf ${snapshot}
					rm -rf "${hasher_outputs}"/${snapshot}
					rm -rf "${hasher_outputs}"/${snapshot}.plain
				fi
		done
	done
done

"${analysis_scripts}"/mh-stat


