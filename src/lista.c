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

#include "lista.h"

celda *nueva(int dato){
	celda *c = (celda *) malloc(sizeof(celda));
	c->dato = dato;
	c->siguiente = NULL;
	c->anterior = NULL;
	return c;
}

celda *insertar(int dato, celda *c){
	if (c == NULL){
		return nueva(dato);
	}
	celda *actual = c;
	celda *anterior = c->anterior;
	int insertada = 0;
	while (actual != NULL){
		if (actual->dato == dato){
			return actual;
		}
		anterior = actual;
		actual = actual->siguiente;
	}
	anterior->siguiente = nueva(dato);
	anterior->siguiente->anterior = anterior;
	return actual;
}

int igualdad(celda *l1, celda *l2){
	celda *actual1 = l1;
	celda *actual2 = l2;
	while (actual1 != NULL){
		actual2 = l2;
		while (actual2 != NULL){
			if (actual1->dato == actual2->dato){
				break; //si lo encontramos hemos acabado con ese elemento.
			}
			actual2 = actual2->siguiente;
		}
		if (actual2 == NULL){
			return 0; // si se llega al final de la lista es que no se ha encontrado y por tanto las listas no son iguales
		}
		actual1 = actual1->siguiente;
	}
	//si se completan los bucles es que las listas son iguales
	return 1;
}

celda* union_listas (celda *l1, celda *l2){
	celda *nueva;
	if (l1 != NULL){
		nueva = copiar_lista(l1);

		if (l2 == NULL){
			return nueva;
		}
	}else if (l2 != NULL){
		return copiar_lista(l2);
	}else if (l1 == NULL && l2 == NULL){
		return NULL;
	}

	celda *actual_l2 = l2;
	while (actual_l2 != NULL){
		insertar(actual_l2->dato, nueva);
		actual_l2 = actual_l2->siguiente;
	}
	return nueva;
}

int pertenencia (int dato, celda *c){
	if (c == NULL){
		return 0;
	}
	celda *actual = c;
	while (actual != NULL){
		if (actual->dato == dato){
			return 1;
		}
		actual = actual->siguiente;
	}
	return 0;
}

celda *copiar_lista (celda *c){
	celda *copia = NULL;
	celda *actual = c;
	celda *actual_copia;
	celda *anterior_copia = NULL;
	if (actual == NULL){
		return NULL;
	}
	copia = nueva(0);
	copia->anterior = NULL;
	actual_copia = copia;
	while (actual != NULL){
		actual_copia->dato = actual->dato;
		actual_copia->anterior = anterior_copia;
		actual = actual->siguiente;
		actual_copia->siguiente = nueva(-1);
		actual_copia->siguiente->anterior = actual_copia;
		anterior_copia = actual_copia;
		actual_copia = actual_copia->siguiente;

	}
	anterior_copia->siguiente = NULL;
	free(actual_copia);
	return copia;
}

celda *operacion_escalar (int numero, char op, celda *c){
	if (c == NULL){
		return 0;
	}
	celda *copia = copiar_lista (c);
	celda *actual = copia;
	while (actual != NULL){
		switch(op){
			case '+':
				actual->dato += numero;
				break;
			case '-':
				actual->dato -= numero;
				break;
			case '*':
				actual->dato *= numero;
				break;
			case '/':
				actual->dato /= numero;
				break;
		}
		actual = actual->siguiente;
	}
	return copia;
}

void borrar(celda *c){
	if (c == NULL){
		return;
	}
	if (c->anterior == NULL && c->siguiente == NULL){
		//free(c);
		return;
	}
	if (c->anterior != NULL){
		c->anterior->siguiente = c->siguiente;
	}else{
		//el primero elemento es una putada, copia el segundo en el primero y cargate el segundo
		c->dato = c->siguiente->dato;
		borrar(c->siguiente);
		return;
	}
	if (c->siguiente != NULL){
		c->siguiente->anterior = c->anterior;
	}
	free(c);
	return;
}

void imprimir (int tipo, celda *l){
	if (l == NULL){
		return;
	}
	celda *actual = l;
	printf("[ ");
	while (actual != NULL){
		switch(tipo){
		  case 0: //entero
		    printf("%d ", actual->dato);
		    break;
		  case 1:
		    printf("%c ", actual->dato);
		    break;
		}
		actual = actual->siguiente;
	}
	printf("]");
	return;
}

celda *leer_lista(){
}

void imprimir_inverso(celda *l){
	if(l == NULL){
		return;
	}
	celda *actual = l;
	while (actual->siguiente != NULL){
		actual = actual->siguiente;
	}
	while (actual != NULL){
		printf("%d ", actual->dato);
		actual = actual->anterior;
	}
	printf("\n");
	return;
}

celda *interseccion_listas (celda *l1, celda *l2){
	if (l1 ==NULL || l2 == NULL){
		return NULL;
	}
	int primera_vez = 1;
	celda *salida = NULL;
	celda *actual_l1 = l1;
	while (actual_l1 != NULL){
		if (pertenencia (actual_l1->dato, l2)){
			if (primera_vez){
				salida = insertar(actual_l1->dato, salida);
				primera_vez = 0;
			}else{
				insertar(actual_l1->dato, salida);
			}
		}
		actual_l1 = actual_l1->siguiente;
	}
	return salida;
}

celda *buscar (int dato, celda *l){
	celda *actual = l;
	while (actual != NULL) {
		if(actual->dato == dato){
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

celda *diferencia (celda *l1, celda *l2){
	celda *salida = NULL;
	celda *actual_l1 = l1;
	int primera_vez = 1;
	while (actual_l1 != NULL){
		if (buscar(actual_l1->dato, l2) == NULL){
			if (primera_vez){
				salida = insertar(actual_l1->dato, salida);
				primera_vez = 0;
			}else{
				insertar(actual_l1->dato, salida);
			}
		}
		actual_l1 = actual_l1->siguiente;
	}
	return salida;
}

int inclusion(celda *l1, celda *l2){
	celda *actual_l1 = l1;
	while (actual_l1 != NULL){
		if (buscar(actual_l1->dato, l2) == NULL){
			return 0;
		}
		actual_l1 = actual_l1->siguiente;
	}
	return 1;
}

int superconjunto (celda *l1, celda *l2){
	return inclusion (l2, l1);
}

int inclusion_estricta (celda *l1, celda *l2){
	if (inclusion (l1, l2)){
		celda *actual_l2 = l2;
		while (actual_l2 != NULL){
			if (!buscar(actual_l2->dato, l1)){
				return 1;
			}
			actual_l2 = actual_l2->siguiente;
		}
	}
	return 0;
}

int superconjunto_estricto (celda *l1, celda *l2){
	return inclusion_estricta (l2, l1);
}
