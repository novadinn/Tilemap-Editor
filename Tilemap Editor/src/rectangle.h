#pragma once

#include <algorithm>

struct Rectangle {
	Rectangle(int x, int y, int width, int height) : x_(x), y_(y), initial_width_(width), height_(height) {}

	int left() const { return x_; }
	int right() const { return x_ + initial_width_; }
	int top() const { return y_; }
	int bottom() const { return y_ + height_; }
	int width() const { return initial_width_; }
	int height() const { return height_; }

	bool pointIntersection(int x, int y) const { return (left() <= x && right() >= x) && (top() <= y && bottom() >= y); };
private:
	int x_, y_, initial_width_, height_;
};