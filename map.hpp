#pragma once

#include <vector>

struct Vector2 {
	float x;
	float y;
};

struct RGB {
	float r;
	float g;
	float b;
};

struct Line {
	Vector2 pos1;
	Vector2 pos2;
	RGB color;

	Vector2 get_normal();
};

struct RayCastResult {
	bool hit;
	Vector2 hit_position;
	Vector2 normal;
	const Line* line;
	float distance;
};

constexpr RGB WHITE = { .r = 1.0f, .g = 1.0f, .b = 1.0f };
constexpr RGB AQUA_BLUE = { .r = 0.0f, .g = 0.5f, .b = 1.0f };
constexpr RGB RED = { .r = 1.0f, .g = 0.0f, .b = 0.0f };
constexpr RGB GREEN = { .r = 0.0f, .g = 1.0f, .b = 0.0f };

class Map {
public:
	Map(); //empty
	void add_tiles(size_t width, size_t height, const int* tiles);

	void clear();
	void add_line_segment(Vector2 pos1, Vector2 pos2, RGB color = AQUA_BLUE);
	bool cast(Vector2 position, Vector2 direction, RayCastResult& result);

private:
	std::vector<Line> _lines;
};