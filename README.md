# ProtMutLib-a-Lot
ProtMutLib-a-Lot is the specialization of Lib-a-Lot for handling predictors based on point mutations on sequences of proteins.

# Credits

ProtMutLib-a-Lot uses:

- the C parser for the MMTF file format,
- the above necessarily uses MsgPack for C,
- the CIF file parser made by the RCSB, and found at:

https://sw-tools.rcsb.org/apps/CIFPARSE-OBJ/source/source.html.

A copy of a version of the CIF file format parser working with the current version of ProtMutLib-a-Lot is included in this GitHub repository. All credits about the CIF file format parser go to the RCSB.

# Compatibility

ProtMutLib-a-Lot has been successfully used on Debian-based and Fedora.

# Prerequisite

MySQL should be installed on your server, as well as gcc.
The libraries needed for gcc are:
- cURL (development files),
- ncurses and its library menu (development files),
- libxml2-dev,
- libmysqlclient-dev,
- libpython-all-dev.

# Installation of the server

## Copy the repositories

1) clone the GitHub repository on your server. It can be done by executing

`git clone https://github.com/JulienFerte/ProtMutLib-a-Lot`

in the Terminal from the desired folder.

2) clone also the GitHub repository of MsgPack for the C language into any folder you fancy. It can be done by executing

`git clone https://github.com/msgpack/msgpack-c`

in the Terminal from the fancied folder.

## Installation of the MySQL database

3) Edit the file `mysql/db_libalot_create_users.sql` in order to choose the passwords for the two MySQL users `'LibALotEngine'@'%'` and `'LibALotUser'@'%'`.

4) On a server where you have MySQL installed, launch mysql with enough rights to execute the following scripts:
- `mysql/db_libalot.sql`
- `mysql/db_libalot_create_users.sql`

You can use the MySQL instruction `source __name_of_file__`.

## Installation of the server

5) From your local copy of ProtMutLib-a-Lot, in the folder `server_side/proteomics/lib/`, create a link named "msgpack" to the folder containing your local copy of MsgPack.

6) Execute the bash script `./compile.sh` in the folder server_side/proteomics/lib/

7) In the file server_side/configuration.c, at the line 114, change the path to the absolute path of your configuration file .LibALot.conf included in your local GitHub repository.

8) Configure your local copy of .LibALot.conf:

- open it as a text file, you notice it is an XML file,
- at the node `<mysql>` which is under the node `<servers>`, change the property ip to the IP address of the server where your MySQL database is stored,
- at the nodes `<client>` and `<engine>` underneath the above `<mysql>`, change the passwords to those you have selected,
- at the node `<core>` which is under the node `<servers>`, change the property ip to the IP address of the server where your copy of ProtMutLib-a-Lot is running.

9) Execute the script `./compile.sh` in the folder `server_side/`. An executable `LibALot.run` should appear in the folder `server_side/`

10) In the folder `server_side/modules/`, go into the folder `Module1/` and execute the script `./compile.sh`. A file `libmodule1.so` should appear.
Then go into the folder `Module2/` and execute the script `./compile.sh`. A file `libmodule2.so` should appear.

## Web page frontend

11) The files for interfacing with ProtMutLib-a-Lot are in the folder `client_side/`.

# Launch

From the Terminal, at the folder `server_side/`, execute `./LibALot.run`

# Bugs?

Feel free to report them.
