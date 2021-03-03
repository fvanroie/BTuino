/* Touchscreen library for XPT2046 Touch Controller Chip
 * Copyright (c) 2015, Paul Stoffregen, paul@pjrc.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _XPT2046_Touchscreen_h_
#define _XPT2046_Touchscreen_h_

#include "Arduino.h"

#define Z_THRESHOLD 500

class TS_Point
{
public:
	TS_Point(void) : x(0), y(0), z(0) {}
	TS_Point(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z) {}
	bool operator==(TS_Point p) { return ((p.x == x) && (p.y == y) && (p.z == z)); }
	bool operator!=(TS_Point p) { return ((p.x != x) || (p.y != y) || (p.z != z)); }
	int16_t x, y, z;
};

/* https://dlbeer.co.nz/articles/tsf.html */
class median_filter
{
public:
	median_filter()
	{
		s.fill(0);
	}

	int operator()(int x)
	{
		s[0] = s[1];
		s[1] = s[2];
		s[2] = s[3];
		s[3] = s[4];
		s[4] = x;

		std::array<int, 5> t = s;

		cmp_swap(t[0], t[1]);
		cmp_swap(t[2], t[3]);
		cmp_swap(t[0], t[2]);
		cmp_swap(t[1], t[4]);
		cmp_swap(t[0], t[1]);
		cmp_swap(t[2], t[3]);
		cmp_swap(t[1], t[2]);
		cmp_swap(t[3], t[4]);
		cmp_swap(t[2], t[3]);

		return t[2];
	}

private:
	static inline void cmp_swap(int &a, int &b)
	{
		if (a > b)
			std::swap(a, b);
	}

	std::array<int, 5> s;
};

template <int N, int D>
class iir_filter
{
public:
	iir_filter() : s(0) {}

	int operator()(int x, bool reset = false)
	{
		if (reset)
		{
			s = x;
			return x;
		}

		s = (N * s + (D - N) * x + D / 2) / D;
		return s;
	}

private:
	int s;
};

template <int N, int D>
class channel_filter
{
public:
	int operator()(int x, bool reset = false)
	{
		return i(m(x), reset);
	}

private:
	median_filter m;
	iir_filter<N, D> i;
};

template <int P>
class debounce_filter
{
public:
	debounce_filter() : s(0) {}

	bool on() const
	{
		return s >= P;
	}

	bool operator()(bool x)
	{
		if (!x)
		{
			s = 0;
			return false;
		}

		if (s < P)
			s++;

		return on();
	}

private:
	int s;
};

template <int N, int D, int P>
class sample_filter
{
public:
	TS_Point operator()(const TS_Point &s)
	{
		const bool rst = !p.on();

		return TS_Point(x(s.x, rst), y(s.y, rst), p(s.z > Z_THRESHOLD));
	}

private:
	debounce_filter<P> p;
	channel_filter<N, D> x;
	channel_filter<N, D> y;
};

typedef sample_filter<7, 10, 6> default_sample_filter;

#if ARDUINO < 10600
#error "Arduino 1.6.0 or later (SPI library) is required"
#endif

class XPT2046_Touchscreen
{
public:
	constexpr XPT2046_Touchscreen(uint8_t cspin, uint8_t tirq = 255)
		: csPin(cspin), tirqPin(tirq) {}
	bool begin();
	TS_Point getPoint();
	bool tirqTouched();
	bool touched();
	void readData(uint16_t *x, uint16_t *y, uint8_t *z);
	bool bufferEmpty();
	uint8_t bufferSize() { return 1; }
	void setRotation(uint8_t n) { rotation = n % 4; }
	// protected:Hello1
	volatile bool isrWake = true;

private:
	void update();
	uint8_t csPin, tirqPin, rotation = 1;
	int16_t xraw = 0, yraw = 0, zraw = 0;
	uint32_t msraw = 0x80000000;
};

#endif
