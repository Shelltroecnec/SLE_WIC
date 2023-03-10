#/bin/bash


hwclock --set --date "2021-12-23"

echo "Mounting /dev/mmcblk1p1 to /mnt"
mount /dev/mmcblk1p1 /mnt


echo "Stopping doceker service"
/etc/init.d/docker.init stop

sleep 2

echo "Starting doceker service"
/etc/init.d/docker.init start


echo "Starting the docker-node-red image"
# docker run -d -p 1880:1880 --add-host="localhost:192.168.2.1" -v /mnt/docker/node_red_data:/data --name mynodered nodered/node-red /bin/bash
# docker run -d -p 1880:1880 -v /mnt/docker/node_red_data:/data --name mynodered nodered/node-red /bin/bash
docker rm -f mynodered
#for giving access to container
docker run --privileged -d -p 1880:1880 --group-add dialout --user=root -v /mnt/docker/node_red_data:/data --name mynodered nodered/node-red /bin/bash
