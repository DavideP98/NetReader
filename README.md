# NetReader

Parser dei file in formato .net (codifica testuale di una rete di Petri) del pacchetto TINA.
Può convertire la rete in un programma Promela equivalente (per ora solo per sistemi elementari).
Uno degli scopi del programma è quello di implementare un meccanismo di espansione della rete, in modo da
poter generare reti di Petri di grandi dimensioni (ancora da implementare).

# Utilizzo: 
./netreader -p rete.net

fornisce in output il programma Promela equivalente alla rete di Petri passata in input

./netreader -n rete.net

fornisce in output la rete in formato .net basata sulla struttura dati allocata internamente 
(sarà utile soltanto dopo l'implementazione del meccanismo di espansione).

