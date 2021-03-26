FILE1=intfMonitor.cpp
FILE2=networkMonitor.cpp


networkMon: $(FILE2)
	g++ $(FILE2) -o $@  

intfMon: $(FILE1)
	g++ $(FILE1) -o $@

all: networkMon intfMon


clean: 
	rm  networkMon intfMon

