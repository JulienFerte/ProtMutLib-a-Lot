#!/bin/bash

gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../configuration.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../list.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../log_record.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../module_context.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../mysql_connection_common.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../sized_string.c &&

gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../proteomics/mutation.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../proteomics/point_mutation.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../proteomics/protein.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../proteomics/sequence.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c ../../proteomics/structure.c &&

gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` `python2.7-config --cflags` -fPIC -c libmodule1.c &&

gcc -fPIC -shared -o libmodule1.so configuration.o list.o log_record.o module_context.o mysql_connection_common.o sized_string.o libmodule1.o mutation.o point_mutation.o protein.o sequence.o structure.o `curl-config --libs` `mysql_config --libs` `xml2-config --libs` `python2.7-config --ldflags` `python2.7-config --libs` -ldl -Wl,-z,defs &&

rm *.o &&

echo "Has successfully compiled libmodule1.so"

