build:
	g++ programm_1.cpp -o programm_1
	g++ -c integer.cpp -o integer.o
	g++ -c programm_2.cpp -o programm_2.o
	g++ integer.o programm_2.o -o programm_2
