#!/bin/bash


##### didj para os dt teste ################

captura="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/network-traffic-2015-09-19-testes-apenas-test1.pcap"

listadestinos="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/dt-teste1-2015-09-19.csv"

listadms="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/lista-ips-teste1-2015-09-19.csv"

transforma="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/testeabc/transforma.sh"


rm output-test.txt


for j in $(cat $listadms)
#for j in "192.168.1.61"

do
echo "dados do $j"
#echo pressione enter para continuar
#read
rm di-dj-output.txt
rm  didj-temp-30-segundos.txt
rm didj-temp.txt 

   argumentocomando="io,stat,1"
   for i in $(cat $listadestinos)
   do
   argumentocomando="$argumentocomando,FRAMES()ip.src==$j and   tcp.flags.syn == 1 and tcp.flags.ack != 1 and tcp.flags.fin != 1 and tcp.flags.ns != 1 and tcp.flags.push != 1 and tcp.flags.reset != 1 and tcp.flags.res != 1 and tcp.flags.urg != 1 and tcp.flags.cwr != 1 and tcp.flags.ecn != 1 and ip.dst==$i.0/24"

   done 


comando="tshark  -r $captura -q  -z '$argumentocomando'"



#### use eval otherwise the com tshark will fail with "-a invalid argument" 
eval $comando | awk '/0 <>/,/Dur/'  |cut -d "|" -f 3-12 | sed  's/|//g' > didj-temp.txt

#echo dados do $j
#echo  pressione enter para continuar
./didj -H -w  -i didj-temp.txt > "/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/testeabc/output.txt"


$transforma

done




