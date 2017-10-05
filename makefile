make:
	g++ -Wall -std=c++0x parse_test.cpp -o test.exe
comparer:
	g++ -Wall -std=c++0x comparer.cpp -o compare.exe
copier:
	g++ -Wall -std=c++0x copier.cpp -o coper.exe
loader:
	g++ -Wall -std=c++0x loader.cpp -o loader.exe
clean:
	rm *.exe *.o