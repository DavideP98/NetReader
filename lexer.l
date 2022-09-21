 /* Prima parte: Definizioni C */ 
%{
	#include"y.tab.h"	/* header generato da yacc, contiene le dichiarazioni
						   dei lessemi */
%}

%%	/* Seconda parte: Coppie Pattern-Azione  */ 

 /* Ignorare whitespace */
[ \t\n]							{}

 /* Ignorare le informazioni riguardanti le reti temporizzate */
\[[0-9]+,(w|[0-9]+)(\[|\])		{}

 /* Ignorare i '\\n' presenti nelle annotazioni */
\\\\n							{}

^tr								{ return TR; }
^pl								{ return PL; }
^an								{ return AN; }
^net							{ return NET; }

\->								{ return ARROW; }

([_\-0-9]*[a-zA-Z][_\-0-9]*)+	{ yylval.text = strdup(yytext); return NAME; }

 /* I numeri che soddisfano questo pattern sono quelli che denotano la
	marcatura dei posti e i pesi degli archi */ 

[0-9]+							{ yylval.ival = atoi(yytext); return NUM;}

 /* Pattern 'catch-all', utile per passare al parser caratteri come
	:,(,),ecc, senza dover definire appositi token */
.								{ return yytext[0]; }

%%

int yywrap(){
	return 1;
}
