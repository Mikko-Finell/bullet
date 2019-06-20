#include "coordinate.hpp"
#include "util.hpp"
#include <limits>
#include <cassert>

Position::Position() {
}

Position::Position(float _x, float _y) : x(_x), y(_y) {
}

Position::Position(int _x, int _y) : x(_x), y(_y) {
}

Position::Position(const sf::Vector2f & v) : Position(v.x, v.y) {
}

Position::Position(const sf::Vector2i & v) : Position(v.x, v.y) {
}

float Position::distance_to(const Position & other) const {
    const auto dx = x - other.x == 0 ? 0 : std::pow(x - other.x, 2);
    const auto dy = y - other.y == 0 ? 0 : std::pow(y - other.y, 2);
    return dx + dy == 0 ? 0 : std::sqrt(dx + dy);
}

Position Position::operator-(const Position & other) const {
    return {x - other.x, y - other.y};
}

Position Position::operator+(const Position & other) const {
    return {x + other.x, y + other.y};
}

Position Position::operator*(const float z) const {
    return {x * z, y * z};
}

Position Position::operator/(const float z) const {
    return {x / z, y / z};
}

void Position::operator+=(const Position & other) {
    x += other.x;
    y += other.y;
}

Position::operator sf::Vector2f() const {
    return {x, y};
}

Position::operator sf::Vector2i() const {
    return {static_cast<int>(x), static_cast<int>(y)};
}

bool Position::operator==(const Position & other) const {
    return x == other.x && y == other.y;
}

bool Position::operator!=(const Position & other) const {
    return !operator==(other);
}

std::string Position::info() const {
    return "Position(" + std::to_string(x) 
        + ", " + std::to_string(y) + ")";
}

/////////////////////////////////////////////////////////////////////// COORDINATE
namespace impl {
template<int W, int H>
std::uint64_t Coordinate<W, H>::Hash::operator()(const Coordinate<W, H> & coord) const {
    assert(std::numeric_limits<std::int32_t>::min() < coord.x
           and std::numeric_limits<std::int32_t>::max() > coord.x);
    assert(std::numeric_limits<std::int32_t>::min() < coord.y
           and std::numeric_limits<std::int32_t>::max() > coord.y);
    const std::int32_t x = coord.x;
    const std::int32_t y = coord.y;
    return ((uint64_t)x << 32) | (((uint64_t)y << 32) >> 32);
}

template<int W, int H>
Coordinate<W, H>::Coordinate() {
}

template<int W, int H>
Coordinate<W, H>::Coordinate(float _x, float _y) : x(_x), y(_y) {
}

template<int W, int H>
Coordinate<W, H>::Coordinate(int _x, int _y) : x(_x), y(_y) {
}

template<int W, int H>
Coordinate<W, H>::Coordinate(const sf::Vector2f & v) : Coordinate(v.x, v.y) {
}

template<int W, int H>
Coordinate<W, H>::Coordinate(const sf::Vector2i & v) : Coordinate(v.x, v.y) {
}

template<int W, int H>
Coordinate<W, H>::Coordinate(const Position & pos) : Coordinate(pos.to_logic<W, H>()) {
}

// snap to grid
template<int W, int H>
Coordinate<W, H> Coordinate<W, H>::to_grid() const {
    return {
        std::floor(x),
        std::floor(y)
    };
}

template<int W, int H>
Position Coordinate<W, H>::to_pixel() const {
    return {
        (x - y) * W * 0.5f,
        (x + y) * H * 0.5f
    };
}

template<int W, int H>
float Coordinate<W, H>::distance_to(const Coordinate & other) const {
    const auto dx = x - other.x == 0 ? 0 : std::pow(x - other.x, 2);
    const auto dy = y - other.y == 0 ? 0 : std::pow(y - other.y, 2);
    return dx + dy == 0 ? 0 : std::sqrt(dx + dy);
}

template<int W, int H>
Coordinate<W, H>::operator sf::Vector2f() const {
    return {x, y};
}

template<int W, int H>
Coordinate<W, H>::operator Position() const { return to_pixel(); }

template<int W, int H>
Coordinate<W, H>::operator sf::Vector2i() const {
    return {static_cast<int>(x), static_cast<int>(y)};
}

template<int W, int H>
bool Coordinate<W, H>::operator==(const Coordinate & other) const {
    return x == other.x && y == other.y;
}

template<int W, int H>
bool Coordinate<W, H>::operator!=(const Coordinate & other) const {
    return !operator==(other);
}

template<int W, int H>
std::string Coordinate<W, H>::info() const {
    return "Coordinate(" + std::to_string(x) 
        + ", " + std::to_string(y) + ")";
}

/////////////////////////////////////////////////////////////////////////////////// REGION

template<int W, int H>
Coordinate<W, H>::Region::Region(float x, float y, float width, float height)
    : _x(x),_y(y),_width(width),_height(height)
{
}

template<int W, int H>
Coordinate<W, H> Coordinate<W, H>::Region::top_left() const {
    return {_x, _y};
}

template<int W, int H>
Coordinate<W, H> Coordinate<W, H>::Region::top_right() const {
    return {_x + _width, _y};
}

template<int W, int H>
Coordinate<W, H> Coordinate<W, H>::Region::bottom_left() const {
    return {_x + _width, _y + _height};
}

template<int W, int H>
Coordinate<W, H> Coordinate<W, H>::Region::bottom_right() const {
    return {_x, _y + _height};
}

template<int W, int H>
float Coordinate<W, H>::Region::width() const {
    return _width;
}

template<int W, int H>
float Coordinate<W, H>::Region::height() const {
    return _height;
}

template<int W, int H>
bool Coordinate<W, H>::Region::intersects(const Region & other) const {
    if (_width == other._width and _height == other._height) {
        return std::abs(_x - other._x) < _width
           and std::abs(_y - other._y) < _height;
    }
    else {
        return contains(other.top_left())    or contains(other.top_right())
            or contains(other.bottom_left()) or contains(other.bottom_right())
            or other.contains(top_left())    or other.contains(top_right())
            or other.contains(bottom_left()) or other.contains(bottom_right());
    }
}

template<int W, int H>
bool Coordinate<W, H>::Region::contains(const Coordinate & coord) const {
    return coord.x >= _x         and coord.y >= _y 
       and coord.x < _x + _width and coord.y < _y + _height;
}

template class Coordinate<config::cellw, config::cellh>;
template class Coordinate<config::tilew, config::tileh>;
}// impl
