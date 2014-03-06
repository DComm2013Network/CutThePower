CXX 		= g++
CXXFLAGS	= -std=c++0x -c -g `sdl2-config --cflags`
LDFLAGS		= `sdl2-config --libs` -lSDL2_net -pthread 
OBJECTS		= ServerCommunication.o GameplayCommunication.o PipeUtils.o NetworkRouter.o SendSystem.o world.o

all: network
	
network: $(OBJECTS)
	$(CXX) -o network_client $(OBJECTS) $(LDFLAGS)

world.o: world.cpp
	$(CXX) $(CXXFLAGS) world.cpp

SendSystem.o: SendSystem.cpp
	$(CXX) $(CXXFLAGS) SendSystem.cpp

ServerCommunication.o: ServerCommunication.cpp
	$(CXX) $(CXXFLAGS) ServerCommunication.cpp

GameplayCommunication.o: GameplayCommunication.cpp
	$(CXX) $(CXXFLAGS) GameplayCommunication.cpp

PipeUtils.o: PipeUtils.cpp
	$(CXX) $(CXXFLAGS) PipeUtils.cpp

NetworkRouter.o: NetworkRouter.cpp
	$(CXX) $(CXXFLAGS) NetworkRouter.cpp

gametest: GameplayDriver.cpp
	$(CXX) $(CXXFLAGS) GameplayDriver.cpp $(LDFLAGS)

servtest: ServerDriver.cpp
	$(CXX) $(CXXFLAGS) ServerDriver.cpp $(LDFLAGS)

clean:
	rm -rf network_client *.o

