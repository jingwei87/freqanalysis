#!/bin/bash

# specify fsl trace
fsl="/dataset/fsl/2013/"
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
		./fsl-count tmp/${snapshot} "dbs/F_${date_of_latext}/"
		rm -rf ${snapshot}
	fi
done

# output db of latest backup 
./db-stat "dbs/F_${date_of_latext}/" > tmp/target-backup
# sort
sort -k2nr tmp/target-backup > tmp/sort-target-backup 
rm -f tmp/target-backup


for aux in ${date_of_aux[@]}; do
	# count auxiliary information (a prior backup)
	for user in ${users[@]}; do
		snapshot="fslhomes-user${user}-${aux}"
		if [ -f "${fsl}"/${snapshot}.tar.gz ]; then
			tar zxf "${fsl}"/${snapshot}.tar.gz  
			"${hasher}"/hf-stat -h ${snapshot}/${snapshot}.8kb.hash.anon > tmp/${snapshot} 
			./fsl-count tmp/${snapshot} "dbs/F_${aux}/"
			rm -rf ${snapshot}
		fi
	done

	# output db of auxiliary information 
	./db-stat "dbs/F_${aux}/" > tmp/aux-backup
	# sort
	sort -k2nr tmp/aux-backup > tmp/sort-aux-backup 
	rm -f tmp/aux-backup
	# rank  
	./db-rank tmp/sort-aux-backup "dbs/F_${aux}/"

	# attack   
	./freq-analysis tmp/sort-target-backup "dbs/F_${aux}/" > Result 
done

