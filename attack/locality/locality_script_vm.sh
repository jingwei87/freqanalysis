#!/bin/bash

# path of fsl trace
vm='/dataset/ds_csci5470_4430_spring2014'

# prefix
pre='csci4430'
machines=('a' 'b' 'c')

# users considered in backups
users=('1' '2' '3' '4' '5' '6' '7' '8' '9' '10' '11' '12' '13' '14' '15' '16' '17' '18' '19' '20' '21' '22' '23' '24' '25' '26' '27' '28' '29' '30' '31' '32' '33' '34' '35' '36' '37' '38' '39' '40' '41' '42' '43' '44' '45' '46' '47' '48' '49' '50' '51' '52' '53' '54')

# auxiliary information
date_of_aux=('0217' '0223' '0302' '0309' '0316' '0323' '0330' '0406' '0413' '0420' '0427' '0504')

# target latest backup 
date_of_latest='0511'

# parameters
u=5
v=30
w=200000
leakage_rate=0

# count latest backup
for user in ${users[@]}; do
	for machine in ${machines[@]}; do
		snapshot="${pre}-${user}${machine}.fp.sha1"
		if [ -f ${vm}/${date_of_latest}/${snapshot} ]; then
			# remove zero chunks during transform
			./tran2vm_without_zero ${vm}/${date_of_latest}/${snapshot} > tmp/${snapshot}_${date_of_latest} 
			./count tmp/${snapshot}_${date_of_latest} "dbs/F_${date_of_latest}/" "dbs/L_${date_of_latest}/" "dbs/R_${date_of_latest}/" 
			rm -rf tmp/${snapshot}_${date_of_latest}
		fi
	done
done


# count prior backups (as auxiliary information) and launch frequency analysis
for aux in ${date_of_aux[@]}; do
	for user in ${users[@]}; do
		for machine in ${machines[@]}; do
			snapshot="${pre}-${user}${machine}.fp.sha1"
			if [ -f ${vm}/${aux}/${snapshot} ]; then
				# remove zero chunks during transform
				./tran2vm_without_zero ${vm}/${aux}/${snapshot} > tmp/${snapshot}_${aux} 
				./count tmp/${snapshot}_${aux} "dbs/F_${aux}/" "dbs/L_${aux}/" "dbs/R_${aux}/" 
				rm -rf tmp/${snapshot}_${aux}
			fi
		done
	done
	echo "===================Attack==================="
	echo "Auxiliary information: ${aux};  Target backup: ${date_of_latest}" 
	echo "Parameters: (u, v, w) = (${u}, ${v}, ${w})"
	# launch frequency analysis
	./attack ${u} ${v} ${w} ${leakage_rate} "dbs/F_${aux}" "dbs/L_${aux}" "dbs/R_${aux}" "dbs/F_${date_of_latest}" "dbs/L_${date_of_latest}" "dbs/R_${date_of_latest}"
	rm -rf inference-db/
done
