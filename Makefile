CXX = g++
CXXFLAGS = -Wall -g -std=c++17
LIBS = -pthread


cyberlens: main.o Producer.o Consumer.o SharedQueue.o
	$(CXX) $(CXXFLAGS) -o cyberlens main.o Producer.o Consumer.o SharedQueue.o $(LIBS)


main.o: main.cpp Producer.hpp Consumer.hpp SharedQueue.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp $(LIBS)

SharedQueue.o: SharedQueue.hpp

Consumer.o: Consumer.hpp SharedQueue.hpp

Producer.o: Producer.hpp Consumer.hpp
