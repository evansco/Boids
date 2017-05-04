boids:
	g++ -std=c++11 -g -c boids.cpp
	g++ boids.o -g -o boids -lsfml-graphics -lsfml-window -lsfml-system
	
.PHONY: boids