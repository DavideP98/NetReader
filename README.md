# NetReader

Parser dei file in formato .net (codifica testuale di una rete di Petri) del pacchetto TINA.
Può convertire la rete in un modello Promela equivalente.
Implementa un semplice meccanismo di copia di posti e transizioni per ingrandire una rete.

# Utilizzo: 
per generare il modello Promela relativo alla rete:   
./netreader rete.net  

Di default viene usata la strategia più veloce per la simulazione delle reti di Petri, in alternativa si può     
usare una strategia alternativa, che utilizza un processo per simulare ciascuna transizione:   
./netreader -pml2 rete.net     

Per avere in output la rete in formato .net (utile nel caso di espansione della rete):   
./netreader -net rete.net   

Esempio di ingrandimento della rete:   
Si crea in Nd un'annotazione contenente una direttiva di questo tipo: 10(p0, p1, t0, t1)     
10 è il numero di copie da creare e all'interno delle parentesi sono specificati i posti e le transizioni da copiare (selezione).  

Gli archi dagli elementi della selezione ad altri elementi della selezioni diventano archi da copie a copie all'interno dello stesso lotto.
Gli archi dagli elementi della selezione ad elementi al di fuori della selezione diventano archi da copie ad elementi della rete originale.   

#Gioco su reti di Petri:  

Per passare al programma le informazioni per la strategia del giocatore si usa questa sintassi:   
p2, p3, ... : t4;   
p2, p4, ... : t3;   

#Verifica con Spin   
spin -a -f "!goal" modello.pml   
gcc -o pan pan.c   
./pan -a -f   


