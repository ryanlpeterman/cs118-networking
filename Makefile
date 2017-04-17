CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
USERID=704269982

all: server client

server: server.cpp listener.cpp listener.h
	$(CXX) -o $@ $^ $(CXXFLAGS)

client: client.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM server client *.tar.gz

dist: tarball
tarball: clean
	tar -cvzf /tmp/$(USERID).tar.gz --exclude=./.vagrant . && mv /tmp/$(USERID).tar.gz .
