online_judge:online_judge.o model.o view.o control.o
	g++ -o $@ $^ -std=c++17 -lpthread -lctemplate -ljsoncpp

model.o:model.cpp 
	g++ -c -o $@ $^ -std=c++17 -lpthread 

view.o:view.cpp 
	g++ -c -o $@ $^ -std=c++17 -lpthread -lctemplate

control.o:control.cpp 
	g++ -c -o $@ $^ -std=c++17 -lpthread -lctemplate -ljsoncpp

online_judge.o:online_judge.cpp 
	g++ -c -o $@ $^ -std=c++17 -lpthread -lctemplate -ljsoncpp

.PHONY:clean
clean:
	rm -f *.o
