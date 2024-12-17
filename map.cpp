#include "map.hpp"

#include <cmath>

Map::Map()
{
}

void Map::add_tiles(size_t width, size_t height, const int* tiles)
{
	constexpr float SCALE = 1.0f;

	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			int tile = tiles[i + j * width];
			if (tile) {
				int up = j == 0 ? 0 : tiles[i + (j - 1) * width];
				int down = j == height - 1 ? 0 : tiles[i + (j + 1) * width];
				int left = i == 0 ? 0 : tiles[(i - 1) + j * width];
				int right = i == width - 1 ? 0 : tiles[(i + 1) + j * width];

				if (up == 0) {
					add_line_segment(Vector2(i * SCALE, j * SCALE), Vector2((i + 1) * SCALE, j * SCALE), GREEN);
				}

				if (down == 0) {
					add_line_segment(Vector2((i + 1) * SCALE, (j + 1) * SCALE), Vector2(i * SCALE, (j + 1) * SCALE), GREEN);
				}

				if (left == 0) {
					add_line_segment(Vector2(i * SCALE, (j + 1) * SCALE), Vector2(i * SCALE, j * SCALE), RED);
				}

				if (right == 0) {
					add_line_segment(Vector2((i + 1) * SCALE, j * SCALE), Vector2((i + 1) * SCALE, (j + 1) * SCALE), RED);
				}
			}
		}
	}
}

void Map::clear()
{
	_lines.clear();
}

void Map::add_line_segment(Vector2 pos1, Vector2 pos2, RGB color)
{
	Line& line = _lines.emplace_back();
	line.pos1 = pos1;
	line.pos2 = pos2;
	line.color = color;
}

float determinant(float a, float b, float c, float d) {
	return a * d - b * c;
}

bool Map::cast(Vector2 origin, Vector2 direction, RayCastResult& result)
{
	float dirlength = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	direction.x /= dirlength;
	direction.y /= dirlength;

	float r_px = origin.x;
	float r_py = origin.y;
	float r_dx = direction.x;
	float r_dy = direction.y;

	result.hit = false;
	result.distance = 1e30f;
	result.line = nullptr;
	for (const Line& line : _lines) {
		float s_px = line.pos1.x;
		float s_py = line.pos1.y;
		float s_dx = line.pos2.x - line.pos1.x;
		float s_dy = line.pos2.y - line.pos1.y;

		float det = determinant(-r_dx, s_dx, -r_dy, s_dy);
		if (std::abs(det) < 1e-6) {
			// Parallel lives (sorry lines)
			continue;
		}

		// Calculate t and u
		float t = determinant(s_px - r_px, s_dx, s_py - r_py, s_dy) / det;
		float u = determinant(s_px - r_px, -r_dx, s_py - r_py, -r_dy) / det;

		// Check if there is an intersection along the ray and within the line segment
		if (t >= 0 && u >= 0 && u <= 1) {
			// Intersection point
			Vector2 intersection = { r_px + t * r_dx, r_py + t * r_dy };
			float new_distance2 = (intersection.x - origin.x) * (intersection.x - origin.x) + (intersection.y - origin.y) * (intersection.y - origin.y);
			if (new_distance2 < result.distance) {
				result.hit = true;
				result.distance = new_distance2;
				result.line = &line;
			}
		}
	}

	if (result.hit) {
		result.distance = std::sqrt(result.distance);
	}

	return result.hit;
}
