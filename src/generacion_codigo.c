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
#include <string.h>

int NUMERO_ETIQUETA_ACTUAL = -1;
int NUMERO_VARIABLE_ACTUAL = -1;

FILE *SALIDA_CODIGO_PRINCIPAL = NULL;
FILE *SALIDA_CODIGO_SUBPROGRAMAS = NULL;
FILE *SALIDA_ACTUAL = NULL;

char *VAR_LISTA_CONSTANTES = NULL;
char LISTA_VAR[256];
char CADENA_FORMATO[256];
char PARAMETROS_ACTUALES[255];
int EN_PROCEDIMIENTO = 0;

void codigoAbrirDescriptores(){
  SALIDA_CODIGO_PRINCIPAL = fopen("codigo.c", "w");
  SALIDA_CODIGO_SUBPROGRAMAS = fopen("subprogramas.c", "w");
  SALIDA_ACTUAL = SALIDA_CODIGO_PRINCIPAL;
}

void codigoCerrarDescriptores(){
    fclose(SALIDA_CODIGO_PRINCIPAL);
    fclose(SALIDA_CODIGO_SUBPROGRAMAS);
}

char *generarIdentificador(char *base, int numero){
  char *ident =(char*) malloc(sizeof(char) * 256);
  sprintf(ident, "%s%d", base, numero);
  return ident;
}

char *generarEtiqueta(char *base){
  NUMERO_ETIQUETA_ACTUAL++;
  return generarIdentificador(base, NUMERO_ETIQUETA_ACTUAL);
}

char *generarVariable(){
  NUMERO_VARIABLE_ACTUAL++;
  return generarIdentificador("tmp", NUMERO_VARIABLE_ACTUAL);
}

void codigoDeclaracionVariables(FILE *f){
  int i = TOPE;
  int j = i-1;
  int parametro = 0;
  //typedef enum {entero, real, caracter, booleano, desconocido, sin_tipo, conjunto_entero, conjunto_caracter} dtipo;

  while (i >= 0 && (TS[i].entrada == variable)){
    parametro = 0;
    j = i-1;
    while (j >= 0 && (TS[j].entrada != procedimiento)){
      if (strcmp (TS[i].nombre, TS[j].nombre) == 0){
	parametro = 1;
	break;
      }
      j--;
    }

    if (!parametro && cTipo(TS[i].tipoDato, f)){
      fprintf(f, "%s;\n", TS[i].nombre);
    }else{
      printf ("ERROR: %s %d %d\n", TS[i].nombre, cTipo(TS[i].tipoDato, f), TS[i].tipoDato);
    }
    i--;
  }
  return;
}

void codigoParametrosFormales(FILE *f) {
  int temp = TOPE;
  //typedef enum {entero, real, caracter, booleano, desconocido, sin_tipo, conjunto_entero, conjunto_caracter} dtipo;
  while (temp >= 0 && (TS[temp].entrada == parametro_formal)){
    if (temp < TOPE){
      fprintf(f, ", ");
    }
    if (cTipo(TS[temp].tipoDato, f)){
      fprintf(f, "%s", TS[temp].nombre);
    }
    temp--;
  }
  return;
}

//typedef enum {entero, real, caracter, booleano, desconocido, sin_tipo, conjunto_entero, conjunto_caracter} dtipo;

int cTipo (dtipo tipo, FILE *f){
  int correcto = 1;
  switch(tipo){
    case entero:
    case booleano:
      fprintf(f, "int ");
      break;
    case real:
      fprintf(f, "float ");
      break;
    case caracter:
      fprintf(f, "char ");
      break;
    case conjunto_entero:
    case conjunto_caracter:
      fprintf(f, "celda *");
      break;
    default:
      correcto = 0;
      break;
  }
  return correcto;
}

void codigoSentenciaAsignacion(YYSTYPE destino, YYSTYPE origen, FILE *f){
  switch (destino.tipo){
    case conjunto_entero:
    case conjunto_caracter:
      fprintf(f, "%s = copiar_lista(%s);\n", destino.lexema, origen.lexema);
      break;
    default:
      fprintf(f, "%s = %s;\n", destino.lexema, origen.lexema);
      break;
  }

  return;
}

char *codigoMASMENOSbinario(YYSTYPE exp1, YYSTYPE op, YYSTYPE exp2, FILE *f){
  char *var = generarVariable();
  char signo;
  switch (op.atrib){
    case 0:
      signo = '+';
      break;
    case 1:
      signo = '-';
      break;
  }

  cTipo(exp1.tipo, f);//solo en el caso de conjuntos podria necesitar el tipo de exp2

  if ((exp1.tipo == conjunto_entero && exp2.tipo == entero) || (exp2.tipo == conjunto_entero && exp1.tipo == entero)){
    if (exp1.tipo == conjunto_entero){
      fprintf(f, "%s = operacion_escalar(%s, \'%c\', %s);\n", var, exp2.lexema, signo, exp1.lexema);
    } else{
      cTipo(exp2.tipo, f);
      fprintf(f, "%s = operacion_escalar(%s, \'%c\', %s);\n", var, exp1.lexema, signo, exp2.lexema);

    }
  }else if ((exp1.tipo == exp2.tipo) && (exp1.tipo == conjunto_entero || exp1.tipo == conjunto_caracter)){
    switch (signo){
      case '+':
	fprintf(f, "%s = union_listas (%s, %s);\n", var, exp1.lexema, exp2.lexema);
	break;
      case '-':
	fprintf(f, "%s = diferencia (%s, %s);\n", var, exp1.lexema, exp2.lexema);
	break;
    }
  }else{
    fprintf(f, "%s = %s %c %s;\n", var, exp1.lexema, signo, exp2.lexema);
  }

  return var;
}

char *codigoMASMENOSunario(YYSTYPE exp, FILE *f){
  char *var = generarVariable();
  cTipo(exp.tipo, f);
  fprintf(f, "%s = - %s;\n", var, exp.lexema);
  return var;
}

char *codigoOPMUL(YYSTYPE exp1, YYSTYPE op, YYSTYPE exp2, FILE *f){
  char *var = generarVariable();
  char signo;
  switch (op.atrib){
    case 0:
      signo = '*';
      break;
    case 1:
      signo = '/';
      break;
  }

  cTipo(exp1.tipo, f);//solo en el caso de conjuntos podria necesitar el tipo de exp2

  if ((exp1.tipo == conjunto_entero && exp2.tipo == entero) || (exp2.tipo == conjunto_entero && exp1.tipo == entero)){
    if (exp1.tipo == conjunto_entero){
      fprintf(f, "%s = operacion_escalar(%s, \'%c\', %s);\n", var, exp2.lexema, signo, exp1.lexema);
    } else {
      cTipo(exp2.tipo, f);
      fprintf(f, "%s = operacion_escalar(%s, \'%c\', %s);\n", var, exp1.lexema, signo, exp2.lexema);

    }
  }else if ((exp1.tipo == exp2.tipo) && (exp1.tipo == conjunto_entero || exp1.tipo == conjunto_caracter)){
    if (signo == '*'){
      fprintf(f, "%s = interseccion_listas (%s, %s);\n", var, exp1.lexema, exp2.lexema);
    }
  } else {
    fprintf(f, "%s = %s %c %s;\n", var, exp1.lexema, signo, exp2.lexema);
  }
  return var;
}

char *codigoOPNEGACION(YYSTYPE exp, FILE *f){
  char *var = generarVariable();
  cTipo(exp.tipo, f);
  fprintf(f, "%s = !%s;\n", var, exp.lexema);
  return var;
}

char *codigoOPOR(YYSTYPE exp1, YYSTYPE exp2, FILE *f){
  char *var = generarVariable();
  cTipo(exp1.tipo, f);
  fprintf(f, "%s = %s || %s;\n", var, exp1.lexema, exp2.lexema);
  return var;
}

char *codigoOPAND(YYSTYPE exp1, YYSTYPE exp2, FILE *f){
  char *var = generarVariable();
  cTipo(exp1.tipo, f);
  fprintf(f, "%s = %s && %s;\n", var, exp1.lexema, exp2.lexema);
  return var;
}

char *codigoOPXOR(YYSTYPE exp1, YYSTYPE exp2, FILE *f){
  char *var = generarVariable();
  cTipo(exp1.tipo, f);
  fprintf(f, "%s = %s ^ %s;\n", var, exp1.lexema, exp2.lexema);
  return var;
}

char *codigoOPIGUALDAD(YYSTYPE exp1, YYSTYPE op, YYSTYPE exp2, FILE *f){
  char *var = generarVariable();
  cTipo(exp1.tipo, f);
  switch (op.atrib){
    case 0:
      fprintf(f, "%s = %s == %s;\n", var, exp1.lexema, exp2.lexema);
      break;
    case 1:
      fprintf(f, "%s = %s != %s;\n", var, exp1.lexema, exp2.lexema);
      break;
  }
  return var;
}

char *codigoOPRELACIONAL(YYSTYPE exp1, YYSTYPE op, YYSTYPE exp2, FILE *f){
  char *var = generarVariable();
  cTipo(entero, f);
  if (exp1.tipo == conjunto_entero || exp1.tipo == conjunto_caracter){
    switch (op.atrib){
      case 0:
	fprintf(f, "%s = inclusion_estricta(%s, %s);\n", var, exp1.lexema, exp2.lexema);
	break;
      case 1:
	fprintf(f, "%s = inclusion(%s, %s);\n", var, exp1.lexema, exp2.lexema);
	break;
      case 2:
	fprintf(f, "%s = superconjunto_estricto(%s, %s);\n", var, exp1.lexema, exp2.lexema);
	break;
      case 3:
	fprintf(f, "%s = superconjunto (%s, %s);\n", var, exp1.lexema, exp2.lexema);
	break;
    }
  } else {

    switch (op.atrib){
      case 0:
	fprintf(f, "%s = %s < %s;\n", var, exp1.lexema, exp2.lexema);
	break;
      case 1:
	fprintf(f, "%s = %s <= %s;\n", var, exp1.lexema, exp2.lexema);
	break;
      case 2:
	fprintf(f, "%s = %s > %s;\n", var, exp1.lexema, exp2.lexema);
	break;
      case 3:
	fprintf(f, "%s = %s >= %s;\n", var, exp1.lexema, exp2.lexema);
	break;
    }
  }
  return var;
}

char *codigoOPPERTENENCIA (YYSTYPE exp1, YYSTYPE exp2, FILE *f){
  char *var = generarVariable();
  cTipo(booleano, f);
  fprintf(f, "%s = pertenencia(%s, %s);\n", var, exp1.lexema, exp2.lexema);
  return  var;
}

char *codigoCONSTANTE(YYSTYPE cte){
  char *var = (char*) malloc (sizeof(char)*256);
  var[0] = '\0';
  //cTipo(cte.tipo, f);
  switch(cte.atrib){
    case 2:
      if (strcmp(cte.lexema, "verdadero") == 0){
	strcat(var, "1");
      }else if (strcmp(cte.lexema, "falso") == 0){
	strcat(var, "0");
      }
      break;
    default:
      var = cte.lexema;
      //fprintf(f, "%s = %s;\n", var, cte.lexema);
      break;
  }
  return var;
}


void codigoIF(int parte, char *varControl, FILE *f) {
  switch (parte){
    case 0:
      TOPE++;
      TS[TOPE].EtiquetaElse = generarEtiqueta("etiqueta_sino_if");
      TS[TOPE].EtiquetaSalida = generarEtiqueta("etiqueta_salida_if");
      TS[TOPE].NombreVarControl = varControl;
      fprintf(f, "if (!%s) goto %s;\n", TS[TOPE].NombreVarControl, TS[TOPE].EtiquetaElse);
      break;
    case 1:
      fprintf(f, "goto %s;\n", TS[TOPE].EtiquetaSalida);
      fprintf(f, "%s:;\n", TS[TOPE].EtiquetaElse);
      break;
    case 2:
      fprintf(f, "%s:;\n", TS[TOPE].EtiquetaSalida);
      TOPE--;
      break;
  }
}

void codigoWHILE(int parte, char *varControl, FILE *f){
  switch (parte){
    case 0:
      TOPE++;
      TS[TOPE].EtiquetaEntrada = generarEtiqueta("etiqueta_entrada_while");
      TS[TOPE].EtiquetaSalida  = generarEtiqueta("etiqueta_salida_while");
      fprintf(f, "%s:;\n", TS[TOPE].EtiquetaEntrada);
      break;
    case 1:
      TS[TOPE].NombreVarControl = varControl;
      fprintf(f, "if (!%s) goto %s;\n", TS[TOPE].NombreVarControl, TS[TOPE].EtiquetaSalida);
      break;
    case 2:
      fprintf(f, "goto %s;\n", TS[TOPE].EtiquetaEntrada);
      fprintf(f, "%s:;\n", TS[TOPE].EtiquetaSalida);
      TOPE--;
      break;
  }
}

void codigoFOR(int parte, char *varControl, YYSTYPE incdec, YYSTYPE tope, FILE *f){
  switch (parte){
    case 0:
      TOPE++;
      TS[TOPE].EtiquetaEntrada = generarEtiqueta("etiqueta_entrada_for");
      TS[TOPE].EtiquetaSalida  = generarEtiqueta("etiqueta_salida_for");
      fprintf(f, "%s:;\n", TS[TOPE].EtiquetaEntrada);
      break;
    case 1:
      TS[TOPE].NombreVarControl = varControl;
      fprintf(f, "if (%s == %s) goto %s;\n", TS[TOPE].NombreVarControl, tope.lexema, TS[TOPE].EtiquetaSalida);
      break;
    case 2:
      switch(incdec.atrib){
	case 0:
	  fprintf(f, "%s += %s;\n", TS[TOPE].NombreVarControl, tope.lexema);
	  break;
	case 1:
	  fprintf(f, "%s -= %s;\n", TS[TOPE].NombreVarControl, tope.lexema);
	  break;
      }
      fprintf(f, "goto %s;\n", TS[TOPE].EtiquetaEntrada);
      fprintf(f, "%s:;\n", TS[TOPE].EtiquetaSalida);
      TOPE--;
      break;
  }
}

void codigoCabeceraMain(FILE *f){
  fprintf(f, "#include <stdio.h>\n");
  fprintf(f, "#include \"lista.c\"\n");
  fprintf(f, "#include \"subprogramas.c\"\n");
  fprintf(f, "int main(void)\n");
}

void abrirBloque(FILE *f){
  fprintf(f, "{\n");
}

void cerrarBloque(FILE *f){
  fprintf(f, "}\n");
}

void codigoSentenciaEntrada(int parte, FILE *f){
//   switch(parte){
//     case 0:
//       CADENA_FORMATO[0] = '\0';
//       LISTA_VAR[0] = '\0';
//       fprintf(f, "scanf");
//       break;
//     case 1:
//       CADENA_FORMATO[strlen(CADENA_FORMATO) -1] = '\0';
//       LISTA_VAR[strlen(LISTA_VAR) - 2]= '\0';
//       fprintf(f, "(\"%s\", ", CADENA_FORMATO);
//       fprintf(f, "%s);\n", LISTA_VAR);
//       break;
//   }
}

void codigoListaVariables(YYSTYPE var){
  //typedef enum {entero, real, caracter, booleano, desconocido, sin_tipo, conjunto_entero, conjunto_caracter} dtipo;
  switch(var.tipo){
    case entero:
    case booleano:
      fprintf(SALIDA_ACTUAL, "scanf(\"%%d\", &%s);\n", var.lexema);
      break;
    case real:
      fprintf(SALIDA_ACTUAL, "scanf(\"%%f\", &%s);\n", var.lexema);
      break;
    case caracter:
      fprintf(SALIDA_ACTUAL, "scanf(\"%%c\", &%s);\n", var.lexema);
      break;
    case conjunto_entero:
    case conjunto_caracter:
      //fflush(STDIN);
      fprintf(SALIDA_ACTUAL, "%s = leer_lista();\n", var.lexema);
      break;
    default:
      break;
   }
}

void codigoSentenciaSalida(int parte, FILE *f){

}

void codigoListaExpresionCadena(int tipo, YYSTYPE var){
  if (tipo == 0){
    switch(var.tipo){
      case entero:
      case booleano:
	fprintf(SALIDA_ACTUAL, "printf(\"%%d\",%s);\n", var.lexema);
	break;
      case real:
	fprintf(SALIDA_ACTUAL, "printf(\"%%f\",%s);\n", var.lexema);
	break;
      case caracter:
	fprintf(SALIDA_ACTUAL, "printf(\"%%c\",%s);\n", var.lexema);
	break;
      case conjunto_entero:
	fprintf(SALIDA_ACTUAL, "imprimir(0, %s);\n", var.lexema);
	break;
      case conjunto_caracter:
	fprintf(SALIDA_ACTUAL, "imprimir(1, %s);\n", var.lexema);
	break;
      default:
	break;
    }
  }else{
    fprintf(SALIDA_ACTUAL, "printf(%s);\n", var.lexema);
  }
}

void codigoProcedimiento(int parte, char* nombre){
  switch (parte){
    case 0:
      EN_PROCEDIMIENTO++;
      SALIDA_ACTUAL = SALIDA_CODIGO_SUBPROGRAMAS;
      fprintf(SALIDA_ACTUAL, "void %s (", nombre);
      break;
    case 1:
      fprintf(SALIDA_ACTUAL, ")\n");
      break;
    case 2:
      //fprintf(SALIDA_ACTUAL, "}\n");
      EN_PROCEDIMIENTO--;
      if (EN_PROCEDIMIENTO == 0){
	SALIDA_ACTUAL = SALIDA_CODIGO_PRINCIPAL;
      }
      break;
  }
}

void codigoLlamadaProcedimiento (int tipo, int parte, char *id, FILE *f){
  switch (tipo){
    case 0:
      switch (parte){
	case 0:
	  fprintf(f, "%s(", id);
	  break;
	case 1:
	  fprintf(f, "%s", PARAMETROS_ACTUALES);
	  break;
	case 2:
	  fprintf(f, ");\n");
	  PARAMETROS_ACTUALES[0] = '\0';
	  break;
      }
      break;
    case 1:
      fprintf(f, "%s();\n", id);
      break;
  }
  return;
}

void codigoParametroActual (YYSTYPE parametro, int ultimo, FILE *f){
  if (ultimo){
    strcat(PARAMETROS_ACTUALES, parametro.lexema);
    //fprintf(f, "%s", parametro.lexema);
  } else {
    strcat(PARAMETROS_ACTUALES, ", ");
    strcat(PARAMETROS_ACTUALES, parametro.lexema);
    //printf("%s\n", PARAMETROS_ACTUALES);
    //fprintf(f, ", %s", parametro.lexema);
  }
  return;
}

char *codigoDeclaracionAgregado(YYSTYPE constante, int parte, FILE *f){
  switch (parte){
    case 0:;
      fprintf(f, "insertar (%s, %s);\n", constante.lexema, VAR_LISTA_CONSTANTES);
      break;
    case 1:
      VAR_LISTA_CONSTANTES = generarVariable();
      fprintf(f, "celda *%s = nueva (%s);\n", VAR_LISTA_CONSTANTES, constante.lexema);
      break;
  }
  return VAR_LISTA_CONSTANTES;
}