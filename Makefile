TARGETS= serverM serverA serverB serverC clientA clientB
CC=g++ 
CFLAGS= -Wall -g  # Add any specific flags you want to use when building your executables, such as error flags (-Wall -Wextra), or flags for compiling with C++11 (-std=c++11)

all: $(TARGETS) 

clean:
	rm -f $(TARGETS) 


%: %.cpp
	$(CC) -o $@ $<