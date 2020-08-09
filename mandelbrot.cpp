#include <iostream>
#include <complex>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>
#include "ConsoleApplication1.h"


/*
Controls:
  r: Reset zoom
  mouse drag: Specify area to zoom in to. Mouse should be dragged from the bottom left of a zoom window to the top right.
  space: Zoom in
*/
using namespace std;

const int MAX_RADIUS = 2;
const int im_w = 10000, im_h = 10000;
long double lim[2][2] = { {-2.1, .9}, {-1.5, 1.5} };
long double rrange, irange, real0, imag0, real1, imag1, real2, imag2;
int iterations;
string name;
uint8_t canvas_arr[im_h][im_w];
cv::Mat canvas = cv::Mat(im_h, im_w, CV_8U, canvas_arr);
cv::Mat colormap = cv::Mat(im_h, im_w, CV_8U);
void mouseCallback(int event, int x, int y, int flags, void* userdata)
{
	switch (event)
	{
	case cv::EVENT_LBUTTONDOWN:
		real1 = (long double) x / im_w * rrange + lim[0][0];
		imag1 = (long double) (im_h - y) / im_h * irange + lim[1][0];
		break;
	case cv::EVENT_LBUTTONUP:
		real2 = (long double) x / im_w * rrange + lim[0][0];
		imag2 = (long double) (im_h - y) / im_h * irange + lim[1][0];

		if ((imag2 - imag1) > (real2 - real1))
			real2 = imag2 - imag1 + real1;
		else
			imag2 = real2 - real1 + imag1;

		imag2 = imag1 + real2 - real1;
		lim[0][0] = real1;
		lim[0][1] = real2;
		lim[1][0] = imag1;
		lim[1][1] = imag2;
	}
}
/*
int get_val(complex<long double> c, int iterations)
{
	int steps = 0;
	complex<long double> value(0., 0.);
	float radius = 0.;

	while ((radius <= MAX_RADIUS) && (steps < iterations))
	{
		++steps;
		value = value * value + c;
		radius = sqrt(real(value) * real(value) + imag(value) * imag(value));
	}
	return steps;
}
*/
int get_val(long double cr, long double ci, int iterations)
{
	int steps = 0;
	long double valr = 0;
	long double vali = 0;
	long double temp;
	float radius = 0.;

	while ((radius <= MAX_RADIUS) && (steps < iterations))
	{
		++steps;
		temp = valr;
		valr = valr * valr - vali * vali + cr;
		vali = 2 * temp * vali + ci;
		radius = sqrt(valr * valr + vali * vali);
	}
	return steps;
}

void output()
{
	cout << "range of " << fixed << rrange
		<< " and " << fixed << irange << endl
		<< "from " << fixed << lim[0][0] << " + " << lim[1][0] << 'i' << "  to  " << fixed << lim[0][1] << " + " << lim[1][1] << 'i' << endl
		<< iterations << " iterations" << endl;
}

int main()
{
	char start = '0';
	cout.precision(17);
	cv::namedWindow("mandelbrot");
	cv::setMouseCallback("mandelbrot", mouseCallback);
	while (true)
	{
		if ((cv::waitKey(1) & 0xff) == 'r')
		{
			lim[0][0] = -2.1;
			lim[0][1] = .9;
			lim[1][0] = -1.5;
			lim[1][1] = 1.5;
			cout << "reset" << endl;
			output();
		}

		if ((cv::waitKey(10) & 0xff) == ' ')
		{
			name = "images/mb";
			name += start;
			name += ".png";
			++start;

			rrange = lim[0][1] - lim[0][0];
			irange = lim[1][1] - lim[1][0];
			iterations = (int)(400 + pow(log10(((4 / abs(rrange)))), 4.3));
			output();

			for (int w = 0; w < im_w; ++w)
			{
				for (int h = 0; h < im_h; ++h)
				{
					real0 = (long double) w / im_w * rrange + lim[0][0];
					imag0 = (long double) (im_h - h) / im_h * irange + lim[1][0];
					int res = get_val(real0, imag0, iterations);
					if (res == iterations)
						canvas.at<uchar>(h, w) = 0;
					else
						canvas.at<uchar>(h, w) = (uint8_t) 255. - (float) res / iterations * 225.;
				}
			}

			cv::applyColorMap(canvas, colormap, cv::COLORMAP_BONE);
			cv::imshow("mandelbrot", colormap);
			cv::imwrite(name, colormap);
			cout << "done" << endl;
		}
	}
	cv::destroyWindow("mandelbrot");
}
