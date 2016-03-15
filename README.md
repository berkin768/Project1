//please add split.h library into /usr/include


COMPILE = gcc project1.c -o project1.out -I/usr/include/libxml2 -lxml2 -lm
RUNNING = ./project1.out chartgen -v validation.xsd -i chartgendata.xml -o berkin.svg -t pie