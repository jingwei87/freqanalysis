#!/bin/bash

# path of fsl trace
fsl='/dataset/fsl/2013'

# users considered in backups
users=('004' '007' '012' '013' '015' '028')

# auxiliary information
#date_of_aux=('2013-01-22' '2013-02-22' '2013-03-22' '2013-04-21')
date_of_aux=('2013-01-22')

# target latest backup 
date_of_latest='2013-05-21'

hasher_outputs='tmp'

# count latest backup
for user in ${users[@]}; do
	snapshot="fslhomes-user${user}-${date_of_latest}"
	if [ -f "${fsl}"/${snapshot}.tar.gz ]; then
		tar zxf "${fsl}"/${snapshot}.tar.gz  
		fs-hasher/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > tmp/${snapshot} 
		./count tmp/${snapshot} "dbs/F_${date_of_latest}/"  
		rm -rf ${snapshot}
#		rm -rf tmp/${snapshot}
	fi
done

# count prior backups (as auxiliary information) and launch frequency analysis
for aux in ${date_of_aux[@]}; do
	for user in ${users[@]}; do
		snapshot="fslhomes-user${user}-${aux}"
		if [ -f "${fsl}"/${snapshot}.tar.gz ]; then
			tar zxf "${fsl}"/${snapshot}.tar.gz  
			fs-hasher/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > tmp/${snapshot} 
			./count tmp/${snapshot} "dbs/F_${aux}"  
			rm -rf ${snapshot}
#			rm -rf tmp/${snapshot}
		fi
	done
	echo "==========================Attack=========================="
	echo "Auxiliary information: ${aux};  Target backup: ${date_of_latest}" 
	# launch frequency analysis
	./attack "dbs/F_${aux}" "dbs/F_${date_of_latest}" 
done
