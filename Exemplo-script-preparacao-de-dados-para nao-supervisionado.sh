#!/bin/bash

arquivo="<arquivo csv>"

mkdir preparados

for i in ab11 ab7 ab9 tb4 ib2 ib3
do

echo "INTERVALO,SDT,SQT,RDT,RQT,ZDT,ZQT,TDT,TQT,FDT,FQT,DM" > preparados/$i-teste1-2019-09-19.csv

awk -F "," '{print $1","$12","$23","$34","$45","$56","$67","$78","$89","$300","$311","$312}' $arquivo |grep $i | head -n 62 >> preparados/$i-teste1-2019-09-19.csv

done

