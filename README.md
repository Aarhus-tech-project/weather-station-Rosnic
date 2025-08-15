[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/XBO6NBqk)

# Weather Station 
Project for setting up an arduino weather station, sending data to a linux server with a mySQL database via MQTT.<br>
The ".ino" file is uploaded to the arduino, and the ".cpp" file is uploaded and run on the linux server.


The Linux server is a Debian 11 Server.
The server has an ip-adress of 192.168.111.11
The linux server can be accessed with ssh on a sudo user with the command: ssh niro@192.168.111.11 and the password: "Datait2025!".

It's running a MySQL server which can be accessed with the username: "root" and the password: "password".

The Linux server is also running a Grafana server, to show the data of the MySQL database in dashboards for a cleaner overlook of desired data.

The MQTT subscriber, which receives data from the Arduino publisher, also sends this data to the MySQL database.
The file with the c++ code is called Main.cpp, and is built with the command ./build.sh, which is a file containing a shell script for compiling the<br>
c++ file with g++ into an executable called "mqtt_mysql_logger".
To run the program and get the data logging running, simply enter "./mqtt_mysql_logger".
