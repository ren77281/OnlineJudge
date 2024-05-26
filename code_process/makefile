code_process_server:compiler.o run.o code_process.o code_process_server.o 
	g++ -o $@ $^ -std=c++17 -lpthread -ljsoncpp

compiler.o:compiler.cpp 
	g++ -c -o $@ $^ -std=c++17 

run.o:run.cpp 
	g++ -c -o $@ $^ -std=c++17 -lpthread

code_process.o:code_process.cpp 
	g++ -c -o $@ $^ -std=c++17 -lpthread -ljsoncpp

code_process_server.o:code_process_server.cpp 
	g++ -c -o $@ $^ -std=c++17 -lpthread -ljsoncpp

.PHONY:clean
clean:
	rm -f *.o