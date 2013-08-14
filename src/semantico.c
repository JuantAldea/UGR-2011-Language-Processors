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

#include <stdio.h>

extern int linea;

typedef enum { marca, procedimiento, variable, parametro_formal } tipoEntrada;

/* Descriptores para el tipo */
typedef enum {entero, real, caracter, booleano, desconocido, sin_tipo, conjunto_entero, conjunto_caracter} dtipo;

typedef union{
  struct{
    tipoEntrada entrada;
    char *nombre;
    dtipo tipoDato;
    unsigned int parametros;
    unsigned int dimensiones;
    int minRango;
    int maxRango;
  };
  struct {
    char *EtiquetaEntrada;
    char *EtiquetaSalida;
    char *EtiquetaElse;
    char *NombreVarControl;
  };
}entradaTS;

#define MAX_TS 3000

unsigned int TOPE = -1;	/*Tope de la pila*/

entradaTS TS[MAX_TS];	/*Pila de la tabla de simbolos*/

/* Atributos sintetizados */
typedef struct{
  int atrib;
  char  *lexema;
  dtipo tipo;
}atributos;

#define YYSTYPE atributos	/*A partir de ahora, cada simbolo tiene una estructura de tipo atributos*/

int enProcedimiento = 0;
int enParametrosActuales = 0;
YYSTYPE parametrosActuales[200];
/******************************************************************************/

/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
/************************************************************************************************/
//typedef enum {entero, real, caracter, booleano, desconocido, sin_tipo, conjunto_entero, conjunto_caracter} dtipo;
char* strTipoTIPO(dtipo tipo){
  switch(tipo){
    case 0:
      return "entero";
      break;
    case 1:
      return "real";
      break;
    case 2:
      return "caracter";
      break;
    case 3:
      return "booleano";
      break;
    case 4:
      return "desconocido";
      break;
    case 5:
      return "sin tipo";
      break;
    case 6:
      return "conjunto_entero";
      break;
    case 7:
      return "conjunto_caracter";
      break;
  }
}

void imprimirEntradaTS(int i){
  printf("%d ", i);
  if (TS[i].entrada == marca){
    printf ("Marca\n");
  }else if (TS[i].entrada == procedimiento){
    printf ("Procedimiento: %s, Numero de parametros: %d\n", TS[i].nombre, TS[i].parametros);
  }else if (TS[i].entrada == variable){
    printf ("Variable: %s, Tipo: %s\n", TS[i].nombre, strTipoTIPO(TS[i].tipoDato));
  }else if (TS[i].entrada == parametro_formal){
    printf ("Parametro formal: %s, Tipo: %s\n", TS[i].nombre, strTipoTIPO(TS[i].tipoDato));
  }
}
void imprimirTS () {
  unsigned int i;
  for (i=0; i <= TOPE; i++) {
    imprimirEntradaTS(i);
  }
  printf("\n--------------------------------------------------------\n");
}

void insertarTS (tipoEntrada entrada, char *nombre, dtipo tipo) {
  /* Inserta un nuevo elemento en la tabla de simbolos */
  TOPE++;
  TS[TOPE].entrada = entrada;
  TS[TOPE].nombre = strdup(nombre);
  TS[TOPE].tipoDato = tipo;
  if (entrada == procedimiento){
    TS[TOPE].parametros = 0;
  }
}

void insertarVariable(char *lexema){
  if (compruebaIDlibreBloque (lexema) == 1){
    insertarTS (variable, lexema, sin_tipo);
  } else {
    printf ("%d: Identificador duplicado: %s\n", linea, lexema);
  }
}

void accionesErrorDeclaracionTipo(){
  while (TOPE > 0 && TS[TOPE].entrada == variable && TS[TOPE].tipoDato == sin_tipo){
    TOPE--;
  }
}

void establecerTipo(dtipo tipo){
  int temp = TOPE;
  if (tipo == desconocido){
    printf("%d: Tipo invalido\n", linea);
    accionesErrorDeclaracionTipo();
    return;
  }
  while (temp >= 0 && (TS[temp].entrada == variable || TS[temp].entrada == parametro_formal )&& TS[temp].tipoDato == sin_tipo){
    TS[temp].tipoDato = tipo;
    temp--;
  }
  //imprimirTS();
}

void inicioBloque(){
  insertarTS(marca, "", sin_tipo);
  if (enProcedimiento){
    enProcedimiento++;
    int tope = TOPE;
    for(enProcedimiento; enProcedimiento < tope; enProcedimiento++){
      insertarVariable(TS[enProcedimiento].nombre);
      establecerTipo(TS[enProcedimiento].tipoDato);
    }
    enProcedimiento = 0;
  }
}

void finBloque(){
  while (TOPE > 0 && TS[TOPE].entrada != marca){
    TOPE--;
  }
  if (TOPE > 0){
    TOPE--;
  }
  if (enProcedimiento){
    enProcedimiento = 0;
  }
}

//falta mirar tambien en los parametros si es un procedimiento
int compruebaIDlibreBloque (char *nombre) {
  int temp = TOPE;
  while (TS[temp].entrada != marca && TS[temp].entrada != procedimiento){
    if ( strcmp(TS[temp].nombre, nombre) == 0){
      return 0;
    }
    temp--;
  }
  return 1;
}

dtipo buscarVar (YYSTYPE variable){
  int tmp = TOPE;
  while (tmp > 0 && strcmp(variable.lexema, TS[tmp].nombre) != 0){
    tmp--;
  }

  if (strcmp(variable.lexema, TS[tmp].nombre) == 0){
    return TS[tmp].tipoDato;
  } else {
    printf ("%d: Variable  %s  no declarada\n", linea, variable.lexema);
    return desconocido;
  }
}

dtipo comprobarIgualdadDeTiposSimples(YYSTYPE uno, YYSTYPE otro){
  if (uno.tipo == desconocido || otro.tipo == desconocido){
    return desconocido;
  } else if (uno.tipo == otro.tipo){
    return uno.tipo;
  } else {
    printf("%d: Tipos incompatibles: %s y %s\n", linea, strTipoTIPO(uno.tipo), strTipoTIPO(otro.tipo));
    return desconocido;
  }
}

dtipo comprobarMASMENOSbinario(YYSTYPE exp1, YYSTYPE op, YYSTYPE exp2){
  if (exp1.tipo == desconocido || exp2.tipo == desconocido){
    return desconocido;
  }else if (exp1.tipo == exp2.tipo){ /* mismo tipo */
    return exp1.tipo;/* suma, resta, union y diferencia */
  } else if (exp1.tipo == conjunto_entero && exp2.tipo == entero){
    return exp1.tipo;/* C +/- x */
  } else if (op.atrib == 0 && (exp1.tipo == entero && exp2.tipo == conjunto_entero)) {
    return exp2.tipo; /* x + C */
  } else {
    printf("%d: Tipos incompatibles: %s y %s\n", linea, strTipoTIPO(exp1.tipo), strTipoTIPO(exp2.tipo));
    return desconocido;
  }
}

comprobarMASMENOSunario (YYSTYPE exp1){
   if (exp1.tipo == desconocido){
    return desconocido;
  } else if (exp1.tipo == entero || exp1.tipo == real){
    return exp1.tipo;
  } else {
    printf("%d: Tipo incompatible: %s\n", linea, strTipoTIPO(exp1.tipo));
    return desconocido;
  }
}

dtipo comprobarOPMUL(YYSTYPE exp1, YYSTYPE op, YYSTYPE exp2){
  if (exp1.tipo == desconocido || exp2.tipo == desconocido){
    return desconocido;
  } else if (exp1.tipo == exp2.tipo){ /* mismo tipo */
    if(op.atrib == 0 && (exp1.tipo == entero || exp1.tipo == real || exp1.tipo == conjunto_entero || exp1.tipo == conjunto_caracter)){
      return exp1.tipo;/* C * C || x * x || */
    } else if(op.atrib == 1 && (exp1.tipo == entero || exp1.tipo == real)) {
      return exp1.tipo; /* x / x */
    }else{
      printf("%d: Tipos incompatibles: %s y %s\n", linea, strTipoTIPO(exp1.tipo), strTipoTIPO(exp2.tipo));
      return desconocido;
    }
  } else if (op.atrib == 0 && (exp1.tipo == entero && exp2.tipo == conjunto_entero) || (exp1.tipo == conjunto_entero && exp2.tipo == entero)) {

    return conjunto_entero; /* C * x || x * C */
  } else if (op.atrib == 1 && (exp1.tipo == conjunto_entero && exp2.tipo == entero)){
    return exp2.tipo;
  }else{
    printf("%d: Tipos incompatibles: %s y %s\n", linea, strTipoTIPO(exp1.tipo), strTipoTIPO(exp2.tipo));
    return desconocido;
  }
}

dtipo comprobarOPBINLOGICO (YYSTYPE exp1, YYSTYPE exp2){
  if (exp1.tipo == desconocido || exp2.tipo == desconocido){
    return desconocido;
  } else if (exp1.tipo == exp2.tipo && exp1.tipo == booleano){
    return exp1.tipo;
  } else {
    printf("%d: Tipos incompatibles: %s y %s\n", linea, strTipoTIPO(exp1.tipo), strTipoTIPO(exp2.tipo));
    return desconocido;
  }
}

dtipo comprobarOPNEGACION (YYSTYPE exp1){
  if (exp1.tipo == desconocido){
    return desconocido;
  } else if (exp1.tipo == booleano){
    return exp1.tipo;
  } else {
    printf("%d: Tipo incompatible: %s y %s\n", linea, strTipoTIPO(booleano), strTipoTIPO(exp1.tipo));
    return desconocido;
  }
}

dtipo comprobarOPPERTENENCIA (YYSTYPE elemento, YYSTYPE conjunto){
  if (elemento.tipo == desconocido || conjunto.tipo == desconocido){
    return desconocido;
  } else if ((elemento.tipo == entero && conjunto.tipo == conjunto_entero) || (elemento.tipo == caracter && conjunto.tipo == conjunto_caracter)){
    return booleano;
  } else {
    printf("%d: Tipos incompatibles: %s y %s\n", linea, strTipoTIPO(elemento.tipo), strTipoTIPO(conjunto.tipo));
    return desconocido;
  }
}

dtipo comprobarOPRELACIONAL (YYSTYPE exp1, YYSTYPE op, YYSTYPE exp2){
  if (exp1.tipo == desconocido || exp2.tipo == desconocido){
    return desconocido;
  } else if (exp1.tipo == exp2.tipo && (exp1.tipo == entero || exp1.tipo == real || exp1.tipo == conjunto_entero || exp1.tipo == conjunto_caracter)){
    return booleano;
  }else{
    printf("%d: Tipos incompatibles: %s y %s\n", linea, strTipoTIPO(exp1.tipo), strTipoTIPO(exp2.tipo));
    return desconocido;
  }
}

dtipo comprobarExpresionLogica(YYSTYPE exp){
  if (exp.tipo == booleano){
    return exp.tipo;
  }else if (exp.tipo != desconocido) {
    printf("%d: Se esperaba una expresion logica pero se encontro %s\n", linea, strTipoTIPO(exp.tipo));
    return desconocido;
  }
}

int buscarProcedimiento(char* proc){
  int i = TOPE;
  while ((i > 0) && ( strcmp(TS[i].nombre, proc) != 0 )){
    i--;
  }
  if (strcmp(TS[i].nombre, proc) == 0){
    return i;
  } else {
    return -1;
  }
}

void insertarProcedimiento (char *lexema){
  if (buscarProcedimiento (lexema) == -1) {
    insertarTS (procedimiento, lexema, sin_tipo);
    enProcedimiento = TOPE;
  }else{
    enProcedimiento = -1;
    printf("%d: Procedimiento %s declarado previamente\n", linea, lexema);
  }
}

void insertarParametroFormal (char *lexema){
  if (enProcedimiento > 0){
    //habra que mirar que no existe ya  y esas cosas
    insertarTS (parametro_formal, lexema, sin_tipo);
  }
}

void anotarParametroActual (YYSTYPE parametro){
  parametrosActuales[enParametrosActuales] = parametro;
  enParametrosActuales++;
}

dtipo comprobarLlamadaProcedimiento(YYSTYPE proc){
  int nParam = enParametrosActuales;
  dtipo retorno;
  enParametrosActuales = 0;
  int indice = buscarProcedimiento(proc.lexema);
  if (indice == -1){
    printf("%d: El procedimiento %s no esta declarado\n", linea, proc.lexema);
    retorno = desconocido;
  }
  if (TS[indice].parametros != nParam){
    printf("%d: Numero de parametros incorrecto: %s declarado con %d parametros formales e invocado con %d parametros actuales \n", linea, proc.lexema, TS[indice].parametros, nParam);
    retorno = desconocido;
  }
  //else { //comprobacion de tipos
    int i, j;
    for (i = (nParam - 1), j = indice + 1; i >= 0 && j <= (indice + nParam); i--, j++){
      if (parametrosActuales[i].tipo == desconocido) {
	retorno = desconocido;
      } else if (TS[j].tipoDato != parametrosActuales[i].tipo){
	printf("%d: Tipo de parametro %d incorrecto, declarado %d, invocado, %d\n", linea, nParam - i, TS[j].tipoDato,  parametrosActuales[i].tipo);
	retorno = desconocido;
      }
    }
    //retorno = sin_tipo;
  //}
  return retorno;
}

void accionesListaIdentificadores(YYSTYPE id){
  if (enProcedimiento == 0){
    insertarVariable(id.lexema);
  } else {
    insertarParametroFormal(id.lexema);
    TS[enProcedimiento].parametros++;
  }
}


dtipo tipoCONSTANTE(YYSTYPE constante){
  switch(constante.atrib){
    case 0:
      return entero;
      break;
    case 1:
      return real;
      break;
    case 2:
      return booleano;
      break;
    case 3:
      return caracter;
      break;
    default:
      return desconocido;
  }
}

dtipo tipoTSIMPLE (YYSTYPE tipo){
  switch (tipo.atrib){
    case 0:
      return real;
      break;
    case 1:
      return booleano;
      break;
    case 2:
      return entero;
      break;
    case 3:
      return caracter;
      break;
    default:
      return desconocido;
  }
}

dtipo tipoTCOMPUESTO (YYSTYPE tipo){
  switch (tipo.atrib){
    case 2:
      return conjunto_entero;
      break;
    case 3:
      return conjunto_caracter;
      break;
    default:
      return desconocido;
  }
}

dtipo tipoAgregadoDeConstantes (YYSTYPE constante){
  if (constante.tipo == entero){
    return conjunto_entero;
  } else if (constante.tipo = caracter) {
    return conjunto_caracter;
  } else {
    printf("%d: Agregado invalido\n", linea);
    return desconocido;
  }
}

dtipo tipoListaConstantes (YYSTYPE constante, YYSTYPE agregado){
  if (agregado.tipo == tipoAgregadoDeConstantes(constante)){
    return agregado.tipo;
  } else {
    printf("%d: Agregado con tipos incompatibles\n", linea);
    return desconocido;
  }
}

/* typedef enum {entero, real, caracter, booleano, desconocido, sin_tipo, conjunto_entero, conjunto_caracter} dtipo; */