#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace sf;

#define PI 3.14159265
constexpr int minDist = 30;
constexpr int maxVel = 10;
constexpr int flockDist = 200;
constexpr int border = 25;

class Boid {
	
public:
		
	Boid(int width, int height, Texture &texture) {
		
		x = rand() % width;
		y = rand() % height;
		xVel = rand() % maxVel;
		yVel = rand() % maxVel;
		shape.setTexture(texture, true);
		shape.setScale(0.75, 0.75);
		
	}
	
	int x, y;
	double xVel, yVel;
	Sprite shape;
	
};

double getDistance(const Boid &b1, const Boid &b2) {
	
	return sqrt(pow(b2.x - b1.x, 2) + pow(b2.y - b1.y, 2));
	
}

vector<Boid> getCloseBoids(Boid &boid, vector<Boid> &boids, int dist) {
	
	vector<Boid> closeBoids;
	for (int i = 0; i < boids.size(); ++i) {
		
		if (&boid != &boids[i] && getDistance(boid, boids[i]) < dist)
			closeBoids.push_back(boids[i]);
		
	}
	return closeBoids;
	
}

void moveBoids(vector<Boid> &boids) {
	
	for (auto &boid:boids) {
		
		if (abs(boid.xVel) > maxVel || abs(boid.yVel) > maxVel) {
			
			double scaleFactor = maxVel / max(abs(boid.xVel), abs(boid.yVel));
			boid.xVel *= scaleFactor;
			boid.yVel *= scaleFactor;
			
		}
		
		boid.x += boid.xVel;
		boid.y += boid.yVel;
		boid.shape.setPosition(boid.x, boid.y);
		double angle = 0;
		if (boid.xVel > 0 && boid.yVel > 0) {
			angle = atan(abs(boid.yVel) / abs(boid.xVel)) * 180 / PI;
		} else if (boid.xVel < 0 && boid.yVel > 0) {
			angle = 180 - (atan(abs(boid.yVel) / abs(boid.xVel)) * 180 / PI);
		} else if (boid.xVel > 0 && boid.yVel < 0) {
			angle = -(atan(abs(boid.yVel) / abs(boid.xVel)) * 180 / PI);
		} else if (boid.xVel < 0 && boid.yVel < 0) {
			angle = -180 + (atan(abs(boid.yVel) / abs(boid.xVel)) * 180 / PI);
		}
		boid.shape.setRotation(angle);
		
	}
	
}

void moveTowardsCOM(vector<Boid> &boids) {
	
	for (auto &boid:boids) {
		
		double avgX = 0;
		double avgY = 0;
		
		vector<Boid> closeBoids = getCloseBoids(boid, boids, flockDist);
		if (closeBoids.size() == 0) return;
		
		for (auto &other:closeBoids) {
			
			avgX += boid.x;
			avgY += boid.y;
			
		}
		
		avgX /= closeBoids.size();
		avgY /= closeBoids.size();
		
		boid.xVel += (avgX - boid.x) / 100.0;
		boid.yVel += (avgY - boid.y) / 100.0;
		
	}

}

void keepAway(vector<Boid> &boids) {
	
	for (auto &boid:boids) {
		
		int xAway = 0;
		int yAway = 0;
		
		vector<Boid> closeBoids = getCloseBoids(boid, boids, flockDist);
		if (closeBoids.size() == 0) return;
		
		for (auto &other:closeBoids) {
			
			if (getDistance(boid, other) < minDist) {
				
				int xDiff = boid.x - other.x;
				int yDiff = boid.y - other.y;
				
				if (xDiff > 0) xDiff = sqrt(minDist) - xDiff;
				else if (xDiff < 0) xDiff = -sqrt(minDist) - xDiff;
				
				if (yDiff > 0) yDiff = sqrt(minDist) - yDiff;
				else if (yDiff < 0) yDiff = -sqrt(minDist) - yDiff;
				
				// xAway -= boid.x - other.x;
				// yAway -= boid.y - other.y;
				xAway += xDiff;
				yAway += yDiff;
				
			}
			
		}
		
		boid.xVel -= xAway / 5.0;
		boid.yVel -= yAway / 5.0;
		
	}
	
}

void matchVelocity(vector<Boid> &boids) {
	
	for (auto &boid:boids) {
		
		double avgXVel = 0;
		double avgYVel = 0;
		
		vector<Boid> closeBoids = getCloseBoids(boid, boids, flockDist);
		if (closeBoids.size() == 0) return;
		
		for (auto &other:closeBoids) {
			
			avgXVel += other.xVel;
			avgYVel += other.yVel;
			
		}
		
		avgXVel /= closeBoids.size();
		avgYVel /= closeBoids.size();
		
		boid.xVel += avgXVel / 8;
		boid.yVel += avgYVel / 8;
		
	}
	
}

void avoidBorder(vector<Boid> &boids, int width, int height) {
	
	for (auto &boid:boids) {
		
		if (boid.x < border && boid.xVel < 0) boid.xVel = -boid.xVel;
		if (boid.x > width - border && boid.xVel > 0) boid.xVel = -boid.xVel;
		if (boid.y < border && boid.yVel < 0) boid.yVel = -boid.yVel;
		if (boid.y > height - border && boid.yVel > 0) boid.yVel = -boid.yVel;
		
	}
	
}

int main(int argc, char** argv) {
	
	srand(time(NULL));
	unsigned int width = atoi(argv[1]);
	unsigned int height = atoi(argv[2]);
	int numBoids = atoi(argv[3]);
	Texture texture;
	texture.loadFromFile("arrow.png");
	
	vector<Boid> boids;
	for (int i = 0; i < numBoids; ++i) {
		
		boids.push_back(Boid(width, height, texture));
		
	}
	
	RenderWindow window({width, height}, "Boids");
	window.setFramerateLimit(60);
	
	while(window.isOpen()) {
		
		Event event;
		while (window.pollEvent(event)) {
			
			if (event.type == Event::Closed) window.close();
			
		}
				
		window.clear(Color::Black);
		
		moveTowardsCOM(boids);
		matchVelocity(boids);
		keepAway(boids);
		avoidBorder(boids, width, height);
		moveBoids(boids);
		
		for (auto &boid:boids) {
			
			window.draw(boid.shape);
			
		}
		
		window.display();
		
	}
	
	return 0;
	
}