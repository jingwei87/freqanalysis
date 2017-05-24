#!/bin/bash

# path of fsl trace
fsl='/dataset/fsl/2013'

# users considered in backups
users=('004' '007' '012' '013' '015' '028')

# auxiliary information
#date_of_aux=('2013-01-22' '2013-02-21' '2013-03-21' '2013-04-22')
date_of_aux=('2013-05-21')

# parameters

# count prior backups (as auxiliary information) and launch frequency analysis
for aux in ${date_of_aux[@]}; do
	for user in ${users[@]}; do
		snapshot="fslhomes-user${user}-${aux}"
		if [ -f "${fsl}"/${snapshot}.tar.gz ]; then
			tar zxf "${fsl}"/${snapshot}.tar.gz  
			hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > tmp/${snapshot}
			./minhash tmp/${snapshot} > tmp/E${snapshot} 
			./Count tmp/E${snapshot} "dbs/F_${aux}" "dbs/L_${aux}" "dbs/R_${aux}"  
			rm -rf ${snapshot}
#			rm -rf tmp/${snapshot}
		fi
	done
	echo "Auxilliary information: ${aux};  Target backup: ${date_of_latest}" 
	# launch frequency analysis
done
