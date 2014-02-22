CXX 		= g++
CXXFLAGS	= -std=c++11 -c `sdl2-config --cflags`
LDFLAGS		= `sdl2-config --libs` -lSDL2_net
OBJECTS		= ServerCommunication.o GameplayCommunication.o Functions.o 

all: network
	
network: $(OBJECTS)
	$(CXX) -o network $(OBJECTS) $(LDFLAGS)

ServerCommunication.o: ServerCommunication.cpp
	$(CXX) $(CXXFLAGS) ServerCommunication.cpp

GameplayCommunication.o: GameplayCommunication.cpp
	$(CXX) $(CXXFLAGS) GameplayCommunication.cpp

Functions.o: Functions.cpp
	$(CXX) $(CXXFLAGS) Functions.cpp

gametest: GameplayDriver.cpp
	$(CXX) $(CXXFLAGS) GameplayDriver.cpp $(LDFLAGS)

servtest: ServerDriver.cpp
	$(CXX) $(CXXFLAGS) ServerDriver.cpp $(LDFLAGS)

clean:
	rm -rf network gametest servtest *.o

