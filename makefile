make:
	g++ -Wall -std=c++0x Wrestler.cpp Character.cpp loader.cpp -o loader
comparer:
	g++ -Wall -std=c++0x comparer.cpp -o compare.exe
copier:
	g++ -Wall -std=c++0x copier.cpp -o coper.exe
loader:
	g++ -Wall -std=c++0x Character.cpp loader.cpp -o loader
clean:
	rm *.exe *.o