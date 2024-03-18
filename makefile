all :  
	g++ -pedantic-errors ./*.cpp -lsqlite3 -o main

clean :
	rm main