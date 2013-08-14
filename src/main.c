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
#include <stdlib.h>

extern FILE *yyin ;
int yyparse(void) ;

FILE *abrir_entrada( int argc, char *argv[] )
{
  FILE *f= NULL ;
  if ( argc > 1 ){
    f= fopen(argv[1],"r");
    if (f==NULL){
      fprintf(stderr,"fichero ’%s’ no encontrado\n",argv[1]);
      exit(1);
    } else {
      //printf("leyendo fichero ’%s’.\n",argv[1]);
    }
  } else {
    printf("leyendo entrada estándar.\n");
  }
  return f ;
}
/************************************************************/
int main( int argc, char *argv[] )
{
  yyin= abrir_entrada(argc,argv) ;
  return yyparse() ;
}
