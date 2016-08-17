#!/bin/bash


##### SYN para os dt ################

captura="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/network-traffic-2015-09-19-testes-apenas-test1.pcap"

listadestinos="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/dt-teste1-2015-09-19.csv"

listadms="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/lista-ips-teste1-2015-09-19.csv"


for j in $(cat $listadms)
do
echo "dados do $j"
echo pressione enter para continuar
read
 
   argumentocomando="io,stat,30"
  # for i in $(cat $listadestinos)
  # do
   argumentocomando="$argumentocomando,FRAMES()ip.src==$j and   tcp.flags.syn == 1 or tcp.flags.reset == 1  and tcp.flags.ack != 1 and tcp.flags.fin != 1 and tcp.flags.ns != 1 and tcp.flags.push != 1 and tcp.flags.res != 1 and tcp.flags.urg != 1 and tcp.flags.cwr != 1 and tcp.flags.ecn != 1"

   #done
#echo $argumentocomando

comando="tshark  -r $captura -q  -z '$argumentocomando'"

#comando=(tshark  -r $captura -q  -z '$argumentocomando')

#echo $comando

#### use eval otherwise the com tshark will fail with "-a invalid argument" 
eval $comando

echo pressione enter para continuar
read
done


