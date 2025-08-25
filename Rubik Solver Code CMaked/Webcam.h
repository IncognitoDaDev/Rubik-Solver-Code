#pragma once
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

class Webcam
{
public:
    enum Color
    {
        White = 0,
        Blue = 1,
        Yellow = 2,
        Red = 3,
        Green = 5,
        Orange = 4
    };

    static void drawSquares(Mat& image, const vector<vector<Point>>& squares);
    void translateFaceColor(Mat& frame, vector<vector<Point>> square, vector<int> colorType, int(&ma)[3][3]);
    static double angle(Point pt1, Point pt2, Point pt0);
    Point MiddlePoint(vector<Point> sq);
    bool isOverlapping(vector<Point> sq1, vector<Point> sq2);
    Point sqPosition(Mat frame, vector<Point> square);
    Mat ColorFilter(Mat image, int color);
    Mat CombineColorFilterResults(Mat frame, vector<int> colorToFilter);
    static void findSquaresAlt(const Mat image, vector<vector<Point>>& squares, vector<int>& colorType);
    void debugPos(const vector<Point> square);
    static void filterExtraContours(vector<vector<Point>>& sq, vector<int>& ct);
};

