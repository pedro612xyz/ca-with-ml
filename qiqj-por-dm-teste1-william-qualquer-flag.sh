#!/bin/bash


##### qiqj para os qt teste ################

captura="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/network-traffic-2015-09-19-testes-apenas-test1.pcap"

listadestinos="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/qt-teste1-2015-09-19.csv"

listadms="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/lista-ips-teste1-2015-09-19.csv"

transforma="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/testeabc/transforma-q-qq-flag.sh"


rm q-qq-flag-output-test.txt


for j in $(cat $listadms)
#for j in "192.168.1.61"

do
echo "dados do $j"
#echo pressione enter para continuar
#read
rm qi-qj-output.txt
rm  qiqj-temp-30-segundos.txt
rm qiqj-temp.txt 

   argumentocomando="io,stat,1"
   for i in $(cat $listadestinos)
   do
 argumentocomando="$argumentocomando,FRAMES()ip.src==$j and ip.dst==$i.0/24"
   done 


comando="tshark  -r $captura -q  -z '$argumentocomando'"



#### use eval otherwise the com tshark will fail with "-a invalid argument" 
eval $comando | awk '/0 <>/,/Dur/'  |cut -d "|" -f 3-12 | sed  's/|//g' > qiqj-temp.txt

#echo dados do $j
#echo  pressione enter para continuar
./didj -H -w  -i qiqj-temp.txt > "/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/testeabc/q-output-qq-flag.txt"


$transforma

done




