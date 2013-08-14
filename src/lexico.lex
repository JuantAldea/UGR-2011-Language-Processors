/*
Copyright (C) 2011, Juan Antonio Aldea Armenteros
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/*** Seccion de declaraciones ***/

%{
  #define MAXLONBUFFER 200
  char buffer[MAXLONBUFFER];
  int linea = 1;
%}

%option noyywrap

%%

[ \t]	{}

\n	{ yylineno++; linea++;}

\[	{ return CORIZQ; }

\]	{ return CORDER; }

\(	{ return PARIZQ; }

\)	{ return PARDER; }

\:	{ return DOSPUNTOS; }

;	{ return PYC; }

:=	{ return ASIG; }

\.	{ return PUNTO; }

,	{ return COMA; }

\+	{yylval.atrib = 0; return MASMENOS; }

\-	{yylval.atrib = 1; return MASMENOS; }

or	{ return OPOR; }

and	{ return OPAND; }

xor	{ return OPXOR; }

\*	{ yylval.atrib = 0; return OPMUL; }

\/	{ yylval.atrib = 1; return OPMUL; }

"="	{ yylval.atrib = 0; return OPIGUALDAD; }

"<>"	{ yylval.atrib = 1; return OPIGUALDAD; }

"<"	{ yylval.atrib = 0; return OPRELACIONAL; }

"<="	{ yylval.atrib = 1; return OPRELACIONAL; }

">"	{ yylval.atrib = 2; return OPRELACIONAL; }

">="	{ yylval.atrib = 3; return OPRELACIONAL; }

si	{ return SI; }

fin	{ return FIN; }

not	{ return NEGACION; }

esta	{ return OPPERTENCIA; }

sino	{ return SINO; }

para	{ return PARA; }

real	{ yylval.atrib = 0; return TSIMPLE; }

logico	{ yylval.atrib = 1; return TSIMPLE; }

entero	{ yylval.atrib = 2; return TSIMPLE; }

caracter { yylval.atrib = 3; return TSIMPLE; }

inicio	{ return INICIO; }

hacer	{ return HACER; }

finvar	{ return FINVARIABLES; }

salida	{ return SALIDA; }

entrada	{ return ENTRADA; }

mientras	{ return MIENTRAS; }

variables	{ return VARIABLES; }

entonces	{ return ENTONCES; }

programa	{ return PROGRAMA; }

conjunto_de	{ return TCONJUNTO; }

procedimiento	{ yylval.lexema = strdup(yytext); return PROCEDIMIENTO; }

incrementando	{ yylval.atrib = 0; return INCDEC; }

decrementando	{ yylval.atrib = 1; return INCDEC; }

[0-9]+		{ yylval.atrib = 0; yylval.tipo = entero;   yylval.lexema = strdup(yytext); return CONSTANTE; }

[0-9]+\.[0-9]*	{ yylval.atrib = 1; yylval.tipo = real;     yylval.lexema = strdup(yytext); return CONSTANTE; }

"verdadero"	{ yylval.atrib = 2; yylval.tipo = booleano; yylval.lexema = strdup(yytext); return CONSTANTE; }

"falso"		{ yylval.atrib = 2; yylval.tipo = booleano; yylval.lexema = strdup(yytext); return CONSTANTE; }

\'[^\']\'	{ yylval.atrib = 3; yylval.tipo = caracter; yylval.lexema = strdup(yytext); return CONSTANTE; }

tabla	{ imprimirTS(); }

\/\/.*\n	{};

\"[^\"]+\"	{ yylval.lexema = strdup(yytext); return CADENA; }

[a-zA-Z_]+[0-9a-zA-Z_]*	{ yylval.lexema = strdup(yytext); return ID; }

.	{
	  sprintf(buffer, "error lexico, token no valido", linea);
	  yyerror(buffer);
	}
