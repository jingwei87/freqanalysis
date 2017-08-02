#!/bin/bash

# path of fsl trace
fsl='/dataset/fsl/2013'

# users considered in backups
users=('004' '007' '012' '013' '015' '028')

# auxiliary information
date_of_aux=('2013-01-22' '2013-02-22' '2013-03-22' '2013-04-21' '2013-05-21')

hasher_outputs='tmp'

# count prior backups (as auxiliary information) and launch frequency analysis
for aux in ${date_of_aux[@]}; do
	for user in ${users[@]}; do
		snapshot="fslhomes-user${user}-${aux}"
		if [ -f "${fsl}"/${snapshot}.tar.gz ]; then
			tar zxf "${fsl}"/${snapshot}.tar.gz  
			fs-hasher/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > tmp/${snapshot} 
			./DDFS container/ index/ tmp/${snapshot} > result/${snapshot}    
			rm -rf ${snapshot}
#			rm -rf tmp/${snapshot}
		fi
	done
	# launch frequency analysis
done
