CC = g++
CFLAGS = -DDEBUG -ggdb -Wall

all: sensitivity scan_sensitivity

sensitivity: sensitivity.cpp
		$(CC) $(CFLAGS) sensitivity.cpp -o sensitivity \
			-I./ \
			-I${ROOTSYS}/include \
		        `root-config --ldflags` \
	                `${ROOTSYS}/bin/root-config --glibs` \
	                -Wl,--no-as-needed \
		        -fdiagnostics-color \
		        -Wno-error

scan_sensitivity: scan_sensitivity.cpp
			$(CC) $(CFLAGS) scan_sensitivity.cpp -o scan_sensitivity \
			-I./ \
			-I${ROOTSYS}/include \
		        `root-config --ldflags` \
	                `${ROOTSYS}/bin/root-config --glibs` \
	                -Wl,--no-as-needed \
		        -fdiagnostics-color \
		        -Wno-error

clean:
	rm -f sensitivity
	rm -f scan_sensitivity

