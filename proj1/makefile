all: TreeTest

TreeTest: Tree.o TreeTest.o
	g++ -o TreeTest Tree.o TreeTest.o

Tree.o: Tree.cpp Tree.hpp MemoryManager.hpp
	g++ -c Tree.cpp

TreeTest.o: TreeTest.cpp Tree.hpp
	g++ -c TreeTest.cpp

clean:
	rm -f *.o TreeTest
