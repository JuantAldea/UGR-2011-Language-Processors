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

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//extern int yyerror(char const *);

/* Tabla de simbolos */

#include "semantico.c"
#include "generacion_codigo.c"
YYSTYPE DUMMY;
%}
%defines
%error-verbose
%token CORIZQ
%token CORDER
%token PARIZQ
%token PARDER
%token DOSPUNTOS
%token PYC
%token ASIG
%token PUNTO
%token COMA
%token MASMENOS
%token SI
%token FIN
%token NEGACION
%token SINO
%token PARA
%token TSIMPLE
%token INICIO
%token HACER
%token INCDEC
%token SALIDA
%token ENTRADA
%token MIENTRAS
%token VARIABLES
%token ENTONCES
%token PROGRAMA
%token TCONJUNTO
%token FINVARIABLES
%token PROCEDIMIENTO
%token CADENA
%token CONSTANTE
%token ID

%left OPOR
%left OPAND
%left OPXOR
%left OPPERTENCIA
%left OPIGUALDAD
%left OPRELACIONAL
%left MASMENOS
%left OPMUL
%right NEGACION
%right SIGNO

%%

programa : { codigoAbrirDescriptores(); } PROGRAMA ID PYC { codigoCabeceraMain(SALIDA_ACTUAL); }  bloque PUNTO { codigoCerrarDescriptores(); };

bloque : INICIO
	  {
	    abrirBloque(SALIDA_ACTUAL);
	    inicioBloque ();
	  }
	  bloque_declaracion_de_variables declaracion_de_subprogs sentencias FIN
	  {
	    finBloque();
	    cerrarBloque(SALIDA_ACTUAL);
	  };

bloque_declaracion_de_variables : VARIABLES cuerpo_declaracion_variables FINVARIABLES PYC { codigoDeclaracionVariables(SALIDA_ACTUAL); }
				| /*vacio*/ ;

cuerpo_declaracion_variables : declaracion_variables PYC cuerpo_declaracion_variables
			     | declaracion_variables
			     | /*vacio*/ ;

declaracion_de_subprogs : declaracion_procedimiento declaracion_de_subprogs
			| /*vacio*/ ;

declaracion_variables : lista_identificadores DOSPUNTOS tipo { establecerTipo($3.tipo); }
		      | error;

sentencias : sentencia PYC sentencias
	   | sentencia
	   | /*vacio*/ ;

sentencia : bloque
	  | sentencia_asignacion
	  | sentencia_if
	  | sentencia_while
	  | sentencia_for
	  | sentencia_entrada
	  | sentencia_salida
	  | sentencia_procedimiento
	  | error ;

sentencia_asignacion :	ID ASIG
			{ abrirBloque(SALIDA_ACTUAL) }
			expresion
			{
			  $1.tipo = buscarVar($1);
			  $$.tipo = comprobarIgualdadDeTiposSimples($1, $4);
			  codigoSentenciaAsignacion($1, $4, SALIDA_ACTUAL);
			  cerrarBloque(SALIDA_ACTUAL);
			};

sentencia_if :	SI
		{ abrirBloque(SALIDA_ACTUAL) }
		expresion
		{ codigoIF(0, $3.lexema, SALIDA_ACTUAL); }
		ENTONCES sentencia
		{ codigoIF(1, NULL, SALIDA_ACTUAL); }
		si_no
		{
		  $$.tipo = comprobarExpresionLogica($3);
		  codigoIF(2, NULL, SALIDA_ACTUAL);
		  cerrarBloque(SALIDA_ACTUAL);
		}
	     /*| SI expresion ENTONCES sentencia SINO sentencia {$$.tipo = comprobarExpresionLogica($2); };*/

si_no : SINO sentencia
      | /*vacio*/;

sentencia_while : MIENTRAS
		  {
		    abrirBloque(SALIDA_ACTUAL);
		    codigoWHILE(0, NULL, SALIDA_ACTUAL);
		  }
		  expresion
		  { codigoWHILE(1, $3.lexema, SALIDA_ACTUAL); }
		  HACER sentencia
		  {
		    $$.tipo = comprobarExpresionLogica($3);
		    codigoWHILE(2, NULL, SALIDA_ACTUAL);
		    cerrarBloque(SALIDA_ACTUAL);
		  };

sentencia_for : PARA
		{
		  abrirBloque(SALIDA_ACTUAL);
		  codigoFOR(0, NULL, DUMMY, DUMMY, SALIDA_ACTUAL);
		}
		sentencia_asignacion INCDEC CONSTANTE
		{
		  codigoFOR(1, $3.lexema, $4, $5, SALIDA_ACTUAL);
		}
		HACER sentencia
		{
		  $$.tipo = comprobarIgualdadDeTiposSimples($3, $5);
		  codigoFOR(2, $3.lexema, $4, $5, SALIDA_ACTUAL);
		  cerrarBloque(SALIDA_ACTUAL);
		} ;

sentencia_entrada : ENTRADA { codigoSentenciaEntrada(0 ,SALIDA_ACTUAL); } lista_variables { codigoSentenciaEntrada(1 ,SALIDA_ACTUAL); };

sentencia_salida : SALIDA { codigoSentenciaSalida(0 ,SALIDA_ACTUAL); } lista_exp_cadena { codigoSentenciaSalida(1 ,SALIDA_ACTUAL); };

lista_exp_cadena : exp_cadena coma_e lista_exp_cadena
		 | exp_cadena ;

exp_cadena : expresion { codigoListaExpresionCadena(0, $1); }
	   | CADENA { codigoListaExpresionCadena(1, $1); } ;

expresion : PARIZQ expresion PARDER
	    {
	      $$.tipo = $2.tipo;
	      $$.lexema = $2.lexema;
	    }
	  | NEGACION expresion	{
				  $$.tipo = comprobarOPNEGACION($2);
				  $$.lexema = codigoOPNEGACION($2, SALIDA_ACTUAL);
				}
	  | MASMENOS expresion %prec SIGNO {
					    $$.tipo = comprobarMASMENOSunario($2);
					    $$.lexema = codigoMASMENOSunario($2, SALIDA_ACTUAL);
					   }
	  | expresion OPOR expresion  {
					$$.tipo = comprobarOPBINLOGICO($1, $3);
					$$.lexema = codigoOPOR($1, $3, SALIDA_ACTUAL);
				      }
	  | expresion OPAND expresion {
					$$.tipo = comprobarOPBINLOGICO($1, $3);
					$$.lexema = codigoOPAND($1, $3, SALIDA_ACTUAL);
				      }
	  | expresion OPXOR expresion {
					$$.tipo = comprobarOPBINLOGICO($1, $3);
					$$.lexema = codigoOPXOR($1, $3, SALIDA_ACTUAL);
				      }
	  | expresion OPMUL expresion {
					$$.tipo = comprobarOPMUL($1, $2, $3);
					$$.lexema = codigoOPMUL($1, $2, $3, SALIDA_ACTUAL);
				      }
	  | expresion MASMENOS expresion {
					    $$.tipo = comprobarMASMENOSbinario($1, $2, $3);
					    $$.lexema = codigoMASMENOSbinario($1, $2, $3, SALIDA_ACTUAL);
					 }
	  | expresion OPIGUALDAD expresion {
					      $$.tipo = ((comprobarIgualdadDeTiposSimples($1, $3) != desconocido) ? booleano : desconocido);
					      $$.lexema = codigoOPIGUALDAD($1, $2, $3,SALIDA_ACTUAL);
					   }
	  | expresion OPRELACIONAL expresion {
						$$.tipo = comprobarOPRELACIONAL ($1, $2, $3);
						$$.lexema = codigoOPRELACIONAL($1, $2, $3, SALIDA_ACTUAL);
					     }
	  | expresion OPPERTENCIA  expresion {
						$$.tipo = comprobarOPPERTENENCIA($1, $3);
						$$.lexema = codigoOPPERTENENCIA ($1, $3, SALIDA_ACTUAL);
					     }
	  | ID 	{
		  $$.tipo = buscarVar($1);
		  $$.lexema = $1.lexema;
		}
	  | CONSTANTE {
			$$.tipo = $1.tipo;
			$$.lexema = codigoCONSTANTE($1);
		      }
	  | agregado {
			$$.tipo = $1.tipo;
			$$.lexema = $1.lexema;
		     }
	  | PARIZQ expresion error
	  | error ;

declaracion_procedimiento : cabecera_procedimiento bloque { codigoProcedimiento(2, NULL); };

cabecera_procedimiento :PROCEDIMIENTO ID
			{
			  insertarProcedimiento($2.lexema);
			  codigoProcedimiento(0, $2.lexema);
			}
			PARIZQ cuerpo_declaracion_variables PARDER PYC
			{
			  codigoParametrosFormales(SALIDA_ACTUAL);
			  codigoProcedimiento(1, NULL);
			}

		       | PROCEDIMIENTO ID
			 {
			  insertarProcedimiento($2.lexema);
			  codigoProcedimiento(0, $2.lexema);
			  codigoProcedimiento(1, NULL);
			 }
			 PYC ;

sentencia_procedimiento : ID PARIZQ
			  {
			    PARAMETROS_ACTUALES[0] = '\0';
			  }
			  lista_expresiones
			  {

			    comprobarLlamadaProcedimiento($1);
			    codigoLlamadaProcedimiento(0, 0, $1.lexema, SALIDA_ACTUAL);
			    codigoLlamadaProcedimiento(0, 1, NULL, SALIDA_ACTUAL);
			    codigoLlamadaProcedimiento(0, 2, NULL, SALIDA_ACTUAL);
			  }
			  PARDER
			| ID PARIZQ PARDER
			  {
			    comprobarLlamadaProcedimiento($1);
			    codigoLlamadaProcedimiento(1, 0, $1.lexema, SALIDA_ACTUAL);
			  }
			| ID
			  {
			    comprobarLlamadaProcedimiento($1);
			    codigoLlamadaProcedimiento(1, 0, $1.lexema, SALIDA_ACTUAL);
			  } ;

lista_identificadores : ID coma_e lista_identificadores { accionesListaIdentificadores($1); }
		      | ID { accionesListaIdentificadores($1); } ;

lista_expresiones : expresion coma_e lista_expresiones
		    {
		      anotarParametroActual ($1);
		      codigoParametroActual ($1, 0, SALIDA_ACTUAL);
		    }
		  | expresion
		    {
		      anotarParametroActual ($1);
		      codigoParametroActual ($1, 1,SALIDA_ACTUAL);
		    } ;

lista_variables : ID coma_e
		  {
		    $1.tipo = buscarVar($1);
		    codigoListaVariables($1);
		  }
		  lista_variables
		| ID
		  {
		    $1.tipo = buscarVar($1);
		    codigoListaVariables($1);
		  } ;

agregado : CORIZQ CORDER  {
			    $$.tipo = sin_tipo;
			  }
	 | CORIZQ lista_constantes CORDER {
					    $$.tipo = $2.tipo;
					    $$.lexema = $2.lexema;
					  };

lista_constantes : CONSTANTE coma_e lista_constantes
		   {
		     codigoDeclaracionAgregado($1, 0, SALIDA_ACTUAL);
		     $$.tipo = tipoListaConstantes($1, $3);
		     $$.lexema = $3.lexema;
		   }
		 | CONSTANTE
		   {
		     $$.lexema = codigoDeclaracionAgregado($1, 1, SALIDA_ACTUAL);
		     $$.tipo = tipoAgregadoDeConstantes($1);
		   } ;

tipo : TSIMPLE { $$.tipo = tipoTSIMPLE($1); }
     | TCONJUNTO TSIMPLE { $$.tipo = tipoTCOMPUESTO($2);}
     | error { yyerrok; accionesErrorDeclaracionTipo(); };

coma_e : COMA
       | error ;
%%

#include  "lex.yy.c"

int yyerror(char * msg){
    printf("%d: %s en %s\n", linea, msg, yytext);
    return 0;
}
