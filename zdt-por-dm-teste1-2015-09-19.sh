#!/bin/bash


##### fluxo em bytes para os dt ################

captura="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/network-traffic-2015-09-19-testes-apenas-test1.pcap"

listadestinos="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/dt-teste1-2015-09-19.csv"

listadms="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/lista-ips-teste1-2015-09-19.csv"


for j in $(cat $listadms)
do
echo "dados do $j"
echo pressione enter para continuar
read
 
   argumentocomando="io,stat,30"
   for i in $(cat $listadestinos)
   do
   argumentocomando="$argumentocomando,BYTES()ip.src==$j and ip.dst==$i.0/24"

   done
#echo $argumentocomando

comando="tshark  -r $captura -q  -z '$argumentocomando'"

#comando=(tshark  -r $captura -q  -z '$argumentocomando')

#echo $comando

#### use eval otherwise the com tshark will fail with "-a invalid argument" 
eval $comando
echo dados do $j
echo pressione enter para continuar
read
done


