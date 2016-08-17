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
   #do
   argumentocomando="$argumentocomando,FRAMES()ip.src==$j"

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
#output salvo manualmente em  /home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/planilha-consolidada-teste1-com-programa-william-2016-01-03-com-zeros-apenas-10-top-and-lower-destinos-any-flag.csv


