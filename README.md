# NetReader

Parser dei file in formato .net (codifica testuale di una rete di Petri) del pacchetto TINA.
Può convertire la rete in un modello Promela equivalente.
Implementa un semplice meccanismo di copia di posti e transizioni per ingrandire una rete.

# Utilizzo: 
per generare il modello Promela relativo alla rete:
./netreader rete.net 

Esempio di ingrandimento della rete:
Si crea in Nd un'annotazione contenente una direttiva di questo tipo: 10(p0, p1, t0, t1)
10 è il numero di copie da creare e all'interno delle parentesi sono specificati i posti e le transizioni da copiare (selezione).

Gli archi dagli elementi della selezione ad altri elementi della selezioni diventano archi da copie a copie all'interno dello stesso lotto.
Gli archi dagli elementi della selezione ad elementi al di fuori della selezione diventano archi da copie ad elementi della rete originale.

