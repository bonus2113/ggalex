#pragma once

class Point {
public:
	int X;
	int Y;
	Point(int _x, int _y) : X(_x), Y(_y) { }
	Point(const Point& _other) : X(_other.X), Y(_other.Y) { }
};