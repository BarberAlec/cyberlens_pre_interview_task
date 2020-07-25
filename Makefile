CXX = g++
CXXFLAGS = -Wall -g -std=c++17
LIBS = -pthread


cyberlens: main.o Producer.o Consumer.o SharedQueue.o
	$(CXX) $(CXXFLAGS) -o cyberlens main.o Producer.o Consumer.o SharedQueue.o $(LIBS)


main.o: main.cpp Producer.h Consumer.h SharedQueue.h
	$(CXX) $(CXXFLAGS) -c main.cpp $(LIBS)

SharedQueue.o: SharedQueue.h

Consumer.o: Consumer.h SharedQueue.h

Producer.o: Producer.h Consumer.h
