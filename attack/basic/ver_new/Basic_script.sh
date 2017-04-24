#!/bin/bash

# specify fsl trace
fsl='/dataset/fsl/2013'
# specify fs-hasher
hasher='fs-hasher'

hasher_outputs='tmp'
users=('004' '007' '012' '013' '015' '028')
#date_of_aux=('2013-01-22' '2013-02-22' '2013-03-22' '2013-04-21')
date_of_aux=('2013-01-22')
date_of_latest='2013-05-21'

# count latest backup
for user in ${users[@]}; do
	snapshot="fslhomes-user${user}-${date_of_latest}"
	if [ -f "${fsl}"/${snapshot}.tar.gz ]; then
		tar zxf "${fsl}"/${snapshot}.tar.gz  
		"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > tmp/${snapshot} 
		./Count tmp/${snapshot} "dbs/F_${date_of_latest}/"  
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
			"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > tmp/${snapshot} 
			./Count tmp/${snapshot} "dbs/F_${aux}"  
			rm -rf ${snapshot}
#			rm -rf tmp/${snapshot}
		fi
	done
	echo "Auxilliary information: ${aux};  Target backup: ${date_of_latest}" >> result
	# launch frequency analysis
	./Attack "dbs/F_${aux}" "dbs/F_${date_of_latest}"  >> Result
done
