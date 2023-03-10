#/bin/bash


BASE="/home/root/ultrafab"

cd ${BASE}

if [ -f gwscript ]; then
	source gwscript
fi

# Check for update
if [ -f gwupdate.tgz ] && [ -f gwupdate.tgz.md5 ]; then
	md5sum -s -c gwupdate.tgz.md5
	if [ $? -eq 0 ]; then
		echo --- UPDATE ARCHIEVE FOUND - EXTRACTING --------
		rm gwupdate.tgz.md5
		cd /
		tar xvzf $BASE/gwupdate.tgz
		cd $BASE
		rm gwupdate.tgz
		sync
		reboot
		exit
	else
		echo --- WARNING: invalid update archive found. DELETING IT! --------
		rm gwupdate.tgz gwupdate.tgz.md5
	fi
fi


sw_version=$(cat /etc/version)

#Keeping back up of last 3 logs
mv ulad_daemon.1.log ulad_daemon.2.log
mv ulad_daemon.0.log ulad_daemon.1.log
mv ulad_daemon.log ulad_daemon.0.log 

#starting the low level ultrafab application driver
./ulad_daemon config.json >> ulad_daemon.log 
