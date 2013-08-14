# Copyright (C) 2011, Juan Antonio Aldea Armenteros
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

.SUFFIXES:
CC = gcc

all: clean practica

practica: main.o y.tab.o
	${CC} -o analizador main.o y.tab.o

y.tab.o: y.tab.c
	${CC} -c y.tab.c

main.o: main.c
	${CC} -c main.c

y.tab.c: sintactico.y lex.yy.c
	bison -d --verbose --report=all -o y.tab.c  sintactico.y

lex.yy.c: lexico.lex
	flex -l lexico.lex

clean:
	rm -f analizador main.o y.tab.o y.tab.c lex.yy.c
