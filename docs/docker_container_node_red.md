
+ Stop the docker with following command:
 - $ /etc/init.d/docker.init stop

+ Create the file /etc/docker/daemon.json with following contents:
{
   "data-root": "/mnt/docker"
}

+ Mount the uSD partition on /mnt. The partition should have around 1GB of space:
  - $ mount /dev/mmcblk1p1 /mnt

+ Set the date to the current date:
 - $ hwclock --set --date "2021-12-02"
 - $ hwclock -s
Check if the date is updated with 'date' command.

+ Start the docker:
  - /etc/init.d/docker.init start

+ Note down the id address of the board with ifconfig

+ Now, run the node red following the instructions at:
https://nodered.org/docs/getting-started/docker

 - $ docker run -it -p 1880:1880 -v node_red_data:/data --name mynodered nodered/node-red
  - Below command give few more features
  docker run -d -p 1880:1880 --add-host="localhost:192.168.2.1" -v /mnt/docker/node_red_data:/data --name mynodered nodered/node-red
  : -d: run as daemon 
  : --add-host: container can access the host
  
This should download the node red related files. The node red can be accessed by connecting to http://<board ip>:1880



### when there is a permission issue then use this command to create a volume
chown 1000 /mnt/docker/node_red_data