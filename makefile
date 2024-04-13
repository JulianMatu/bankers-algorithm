all: bankers

bankers:
	g++ -std=c++20 -o bankers.out main.cpp

debug:
	g++ -std=c++20 -o bankers_db.out main.cpp -g

clean:
	rm *.out