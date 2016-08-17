#!/bin/bash


##### Tdn para os dt ################

captura="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/teste-edicap/network-traffic-2015-09-19-testes-apenas-test1.pcap"

listadestinos="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/qt-teste1-2015-09-19.csv"

listadms="/home/pedro/Documents/personal-data/mestrado/dissertacao/ideias/rogue-aps/revisao-qualificacao/testes/2015-09-19/captura-gateway/teste1/lista-ips-teste1-2015-09-19.csv"


#editcap  -i 300 network-traffic-2015-09-19-testes-apenas-test1.pcap  test





primeirarede="$(head -n1 $listadestinos)"
ultimarede="$(tail -n1 $listadestinos)"

rm -v listadestinotemporario
grep -Ev "$primeirarede|$ultimarede" $listadestinos > listadestinotemporario


for j in $(cat $listadms)
do

 echo trabalhando com o DM $j
 echo pressione enter para continuar
 read
 intervalo=300
 
 for i in test_00000_20150919132154 test_00001_20150919132654 test_00002_20150919133154 test_00003_20150919133654 test_00004_20150919134154 test_00005_20150919134654 test_00006_20150919135154   test_00007_20150919135656
do
 
rm -v output.txt

###
argumentocomando="conv,ip,ip.src==$j and ip.dst==$primeirarede.0/24 or"
echo $argumentocomando



 for k in $(cat  listadestinotemporario)
   do
  
    argumentocomando="$argumentocomando ip.dst==$k.0/24 or"
#echo $argumentocomando
#echo pausa
#read 
   done
#echo $argumentocomando

comando="tshark -r $i -q  -z '$argumentocomando ip.dst==$ultimarede.0/24'" 

#echo $comando

#### use eval otherwise the com tshark will fail with "-a invalid argument" 

eval $comando > output.txt 

###
echo dm $j
 for m in $(cat $listadestinos)
   do
      
 valor=$(grep $m output.txt | awk '{VALOR +=$11}; END {print VALOR}')
 
 echo colocar  $valor na planilha para a rede $m no intervalo $intervalo

   done

 intervalo=$[intervalo+300]


echo Enter para continuar

read
done
done


