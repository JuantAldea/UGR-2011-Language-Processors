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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <stdio.h>
typedef struct celda{
	struct celda *siguiente;
	struct celda *anterior;
	int dato;
}celda;

celda *nueva (int dato);
celda *buscar (int dato, celda *l);
void borrar (celda *c);
celda *copiar_lista (celda *c);

celda *operacion_escalar (int numero, char op, celda *c);

celda *union_listas (celda *l1, celda *l2);
celda *interseccion_listas (celda *l1, celda *l2);
celda *diferencia (celda *l1, celda *l2);


int inclusion (celda *l1, celda *l2);
int superconjunto (celda *l1, celda *l2);
int inclusion_estricta (celda *l1, celda *l2);
int superconjunto_estricto (celda *l1, celda *l2);


int pertenencia (int dato, celda *c);
celda *insertar (int dato, celda *c);
int igualdad (celda *l1, celda *l2);

celda *leer_lista(void);
void imprimir (int tipo, celda *l);
void imprimir_inverso (celda *l);
#endif
