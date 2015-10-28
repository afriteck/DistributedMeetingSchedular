all:
	g++ main.cc -Wl,-rpath /usr/local/lib/$(shell dpkg-architecture -qDEB_BUILD_MULTIARCH) -Llibical/build/lib -lical -licalss -licalvcal -lical_cxx -licalss_cxx -o agent

clean:
	rm -f *.o agent
