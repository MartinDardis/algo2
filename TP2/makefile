CC = gcc
FLAGS_COMPILE = -g -std=c99 -Wall -Wtype-limits -pedantic -Wconversion -Wno-sign-conversion
LIBRARY = hash.c heap.c abb.c strutil.c pila.c lista.c
MAIN = main.c
SORT_FILE = tp2.c
ADD_FILE = agregar_archivo.c
VIEW_VISIT = ver_visitantes.c
OBJDIR = bin

objects = $(addprefix $(OBJDIR)/,hash.o heap.o abb.o strutil.o pila.o lista.o main.o tp2.o agregar_archivo.o ver_visitantes.o)
build_lib:
	$(CC) -c $(FLAGS_COMPILE) $(LIBRARY)

all: $(objects)
	$(CC) $(FLAGS_COMPILE) $(objects) -o programa

$(OBJDIR)/%.o: %.c
	$(CC) $(FLAGS_COMPILE) -c  $< -o $@


prueba: programa
	cat pruebas/prueba3.txt | valgrind --leak-check=full --track-origins=yes --show-reachable=yes  ./programa 200

pruebas: programa
	cat pruebas/prueba1.txt | valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./programa 200
	cat pruebas/prueba2.txt | valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./programa 200
	cat pruebas/prueba3.txt | valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./programa 200
	cat pruebas/pruebavol.txt | valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./programa 200


pruebavol: programa
	 cat pruebas/pruebavol.txt | valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./programa 1000


gdb: programa
	gdb -tui ./programa
	cat gdb.txt

zip:
	zip "entrega.zip" *.c *.h deps.mk

.PHONY: clean
clean:
	rm $(objects)
	rm programa
	rm *.part
