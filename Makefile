all:
	g++ main.cc Entity.cpp Agent.cpp -std=c++11 -Wl,-rpath /usr/local/lib/$(shell dpkg-architecture -qDEB_BUILD_MULTIARCH) -Llibical/build/lib -lical -licalss -licalvcal -lical_cxx -licalss_cxx -o agent

clean:
	rm -f *.o agent
