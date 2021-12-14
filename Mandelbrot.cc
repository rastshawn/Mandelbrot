#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <chrono>
#include <thread>

using namespace std;

int mandelbrot(float real, float imag) {
	int limit = 100;
	float zReal = real;
	float zImag = imag;

	for (int i = 0; i < limit; ++i) {
		float r2 = zReal * zReal;
		float i2 = zImag * zImag;
		
		if (r2 + i2 > 4.0) return i;

		zImag = 2.0 * zReal * zImag + imag;
		zReal = r2 - i2 + real;
	}
	return limit;
}

/* BELOW IS ALL JULIA STUFF */
float dist(float x1, float y1, float x2, float y2) {
	float result = sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
	return result;
}
class Complex {
	public: 
		float real;
		float imaginary;
		bool lessThan(float num) {
			return (dist(real, imaginary, 0, 0) < num);
		}
		Complex multiply(Complex x) {
			float newR = (this->real*x.real) - this->imaginary*x.imaginary;
			float newI = (this->real*x.imaginary) + this->imaginary*x.real;
			Complex ret(newR, newI);
			return ret;
		}
		Complex add(Complex x) {
			float newR = this->real + x.real;
			float newI = this->imaginary + x.imaginary;
			Complex ret(newR, newI);
			return ret;
		}
		Complex(float real, float imaginary) {
			this->real = real;
			this->imaginary = imaginary;
		}
};
int BAILOUT = 20;

float interpolate(float var, float min1, float max1, float min2, float max2) {
	float numerator = (var - min1) * (max2 - min2);
	float denominator = (max1 - min1);
	float ret =  min2 + (numerator / denominator);
	return ret;
};
//Complex c = Complex(-.66, -.71);
Complex c = Complex(0, 0);
Complex f(Complex x) {
	return x.multiply(x).add(c);
}

int intEscape(Complex c) {
	float num = 2;
	int loopCount = 0;
	while (c.lessThan(num) && loopCount < BAILOUT) {
		c = f(c);
		//std::cout << dist(c.imaginary, c.real, 0, 0) << std::endl;
		//std::cout << c.imaginary << " " << c.real << std::endl;
		loopCount++;
	}

	if (!c.lessThan(num)) {
		return loopCount;
	} else return -1;
};
float xx1 = -2;
float xx2 = 2;
float yx1 = -1.5;
float yx2 = 1.5;


float julia (float i, float j, int width, int height) {
	// notes, for audio we're really going to want to keep the real and imaginary components between -1 and 1
	// keep display between -1.5 and 1.5 or maybe 2 and -2
	float real = interpolate(i, 0, width-1, xx1, xx2);
	float imaginary = interpolate(j, 0, height-1, yx1, yx2);
	Complex complexSeed = Complex(real, imaginary);
	int esc = intEscape(complexSeed);
	if (esc == -1) {
		return 1; // black
	} else {
		return interpolate(esc, 0, BAILOUT, 0, 1);
	}


}
/* ABOVE IS ALL JULIA STUFF */

void testInterpolate() {
	float xmin = 0;
	float xmax = 10;
	float ymin = -5;
	float ymax = 5;
	std::cout << interpolate(0, xmin, xmax, ymin, ymax);
};
void drawJulia(int width, int heigth) {
	string char_ = "\u2588";

	string black = "\033[22;30m";
	string red = "\033[22;31m";
	string l_red = "\033[01;31m";
	string green = "\033[22;32m";
	string l_green = "\033[01;32m";
	string orange = "\033[22;33m";
	string yellow = "\033[01;33m";
	string blue = "\033[22;34m";
	string l_blue = "\033[01;34m";
	string magenta = "\033[22;35m";
	string l_magenta = "\033[01;35m";
	string cyan = "\033[22;36m";
	string l_cyan = "\033[01;36m";
	string gray = "\033[22;37m";
	string white = "\033[01;37m";

	std::stringstream frame;
	for (int j = 0; j < heigth; j++) {
		for (int i = 0; i < width; i++) {
			
			// float x = x_start + j*dx; // current real value
			// float y = y_fin - i*dy; // current imaginary value
			
			float value = 100*julia(i, j, width, heigth);
			
			if (value == 100) {frame << " ";}
			else if (value > 90) {frame << red << char_;}
			else if (value > 80) {frame << l_red << char_;}
			else if (value > 70) {frame << orange << char_;}
			else if (value > 60) {frame << yellow << char_;}
			else if (value > 50) {frame << l_green << char_;}
			else if (value > 40) {frame << green << char_;}
			else if (value > 30) {frame << l_cyan << char_;}
			else if (value > 20) {frame << cyan << char_;}
			else if (value > 15) {frame << l_blue << char_;}
			else if (value > 10) {frame << blue << char_;}
			else if (value > 5) {frame << magenta << char_;}
			else {frame << l_magenta << char_;}


			/*
			Testing Screen Grid
			float interpH = interpolate(i, 0, heigth-1, yx1, yx2);
			float interpW = interpolate(j, 0, width-1, xx1, xx2);
			cout << abs((int) interpW)+ abs((int) interpH);
			*/
			
			
			//cout << value << endl;
			
			frame << "\033[0m";
		}
		//cout << endl;
	}
	frame << "\033[2J\033[1;1H";
	cout << frame.str();
}
int main() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	//testInterpolate();
	//return 0;
	int width = w.ws_col; //number of characters fitting horizontally on my screen 
	int heigth = w.ws_row; //number of characters fitting vertically on my screen
		

	float dx = .005;
	float dy = -.007;
	while(1) {
		//cout << "\033[2J\033[1;1H";
		drawJulia(width, heigth);
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
		c = Complex(c.real+dx, c.imaginary+dy);
		if (c.real > 1 || c.real < -1) {
			dx = -dx;
		}
		if (c.imaginary > 1 || c.imaginary < -1) {
			dy = -dy;
		}
	}

	

	return 0;
}



int testComplex() {
	Complex a(-.66, 1);
	Complex b(12, .5);
	Complex c = a.multiply(b);
	Complex d = c.add(b);
	Complex e = a.multiply(b).add(b);

	std::cout << "a: " << a.real << " " << a.imaginary << std::endl;
	std::cout << "b: " << b.real << " " << b.imaginary << std::endl;
	std::cout << "c: " << c.real << " " << c.imaginary << std::endl;
	std::cout << "d: " << d.real << " " << d.imaginary << std::endl;
	std::cout << "e: " << e.real << " " << e.imaginary << std::endl;
}