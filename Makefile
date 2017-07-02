flags = -std=c++11 -Wall
comp = g++
prog  = OSAP

#El problema
instance = game.txt
tam_pob = 4
cant_gen = 4
pc = 0.3
pm = 0.15

comp:$(prog)

exe:$(prog)
	#valgrind --leak-check=full --show-reachable=yes 
	./$(prog) $(instance) $(tam_pob) $(cant_gen) $(pc) $(pm) 

$(prog): main.o
	$(comp) main.o -o $(prog) $(flags)

main.o: main.cpp includes.h individuals.h constraint.h entities.h rooms.h
	$(comp) -c main.cpp $(flags)

clean:
	rm -f *~
	rm -f *.o
	rm -f *.ps
	rm -f $(prog)

