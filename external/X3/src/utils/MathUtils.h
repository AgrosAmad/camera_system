#pragma once

// STL
#include <stdio.h>
#include <math.h>
#include <limits>

const float math_pi = 3.1415926536f;
const float math_2pi = 2 * math_pi;
const float math_radians = math_pi / 180.0f;
const float math_degrees = 180.0f / math_pi;
const float math_epsilon = 1e-6f;
const float math_maxfloat = (std::numeric_limits<float>::max)();
const float math_minfloat = -(std::numeric_limits<float>::max)();

template <class T> T math_fract(const T& x) { return x - floor(x); }
template <class T> T math_round(const T& x) { return floor(x + T(0.5)); }
template <class TA, class TB, class TC> TA math_clamp(const TA& x, const TB& minv, const TC& maxv) { if (x < (TA)minv) return (TA)minv; else if (x > (TA)maxv) return (TA)maxv; return x; }
template <class T> T math_sqrt(const T& x) { return x * x; }
template <class T> T math_cube(const T& x) { return x * x * x; }
template <class T> T math_sign(const T& x) { if (x > 0) return T(1); else if (x < 0) return T(-1); return T(0); }
template <class T> T math_deg2rad(const T& x) { return x * T(math_pi / 180.0); }
template <class T> T math_rad2deg(const T& x) { return x * T(180.0 / math_pi); }
template <class T> T math_min(const T& a, const T& b) { return (a < b ? a : b); }
template <class T> T math_min(const T& a, const T& b, const T& c) { return min(min(a, b), c); };
template <class T> T math_min(const T& a, const T& b, const T& c, const T& d) { return min(min(a, b), min(c, d)); };
template <class T> T math_max(const T& a, const T& b) { return (a > b ? a : b); }
template <class T> T math_max(const T& a, const T& b, const T& c) { return max(max(a, b), c); }
template <class T> T math_max(const T& a, const T& b, const T& c, const T& d) { return max(max(a, b), max(c, d)); }
template <class T> T math_smoothStep(const T& l, const T& u, const T& x) { T t = clamp((x - l) / (u - l), (T)0, (T)1); return t * t * (3 - 2 * t); }
template <class T> T math_lerp(const T& a, const T& b, const float& s) { T t = b * s + (1 - s) * a; return t; }
template <class T> void math_loop(T& a, const T& low, const T& high, const T& inc = 1.0) { if (a >= high) a = low; else a += inc; }
template <class T> T math_rand(const T& low, const T& high) { return rand() / (static_cast<float>(RAND_MAX) + 1.0) * (high - low) + low; }