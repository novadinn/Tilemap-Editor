#pragma once

#include <algorithm>

struct Rectangle {
	Rectangle() : x_(0), y_(0), width_(0), height_(0) {}
	Rectangle(int x, int y, int width, int height) : x_(x), y_(y), width_(width), height_(height) {}

	int left() const { return x_; }
	int right() const { return x_ + width_; }
	int top() const { return y_; }
	int bottom() const { return y_ + height_; }
	int width() const { return width_; }
	int height() const { return height_; }

	void set_x(int x) { x_ = x; }
	void set_y(int y) { y_ = y; }
	void set_width(int width) { width_ = width; }
	void set_height(int height) { height_ = height; }

	bool pointIntersection(int x, int y) const { return (left() <= x && right() >= x) && (top() <= y && bottom() >= y); };
private:
	int x_, y_, width_, height_;
};