#include "Webcam.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <cmath>

using namespace std;
using namespace cv;

Webcam WC;

Mat Raw;
int thresh = 50, N = 5;
int minSize = 1200, maxSize = 2700, tolerance = 0, tolDist = 25; // positive means tighter squares;

int frontSide = 0, // White
upSide = 1; // Blue

double Webcam::angle(Point pt1, Point pt2, Point pt0)
{
    // finds a cosine of angle between vectors
    // from pt0->pt1 and from pt0->pt2

    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

Point Webcam::MiddlePoint(vector<Point> sq) // Calculates the center point of the square
{
    Point center;
    center.x = (sq[0].x + sq[3].x) / 2;
    center.y = (sq[0].y + sq[3].y) / 2;
    return center;
}

bool Webcam::isOverlapping(vector<Point> sq1, vector<Point> sq2) //Checks if the two squares is either above the other
{
    int l = (sq1[2].x - sq1[0].x) + tolerance;
    int xDist = abs(MiddlePoint(sq1).x - MiddlePoint(sq2).x),
        yDist = abs(MiddlePoint(sq1).y - MiddlePoint(sq2).y);

    return xDist <= l && yDist <= l;
}

// calculating and using the center point of the frame as point of reference
Point Webcam::sqPosition(Mat frame, vector<Point> square)
{
    int centerX = frame.size().width / 2,
        centerY = frame.size().height / 2;

    // Every square is by default a centerpiece until proven otherwise...
    Point pos; pos.x = 1; pos.y = 1;

    int l = abs(square[0].x - square[1].x) + tolDist;

    if (centerX - l >= MiddlePoint(square).x) pos.x--; // Left Col
    else if (centerX + l <= MiddlePoint(square).x) pos.x++; // Right Col

    if (centerY - l >= MiddlePoint(square).y) pos.y--; // Down Row
    else if (centerY + l <= MiddlePoint(square).y) pos.y++; // Up Row

    return pos;
}

enum Color
{
    White = 0,
    Blue = 1,
    Yellow = 2,
    Red = 3,
    Green = 5,
    Orange = 4
};

Mat Webcam::ColorFilter(Mat image, int color)
{

    Mat hsv, end;
    Scalar lowRange, highRange;
    switch (color)
    {
    case White: // Make it work!!! WHY IS WHITE SO HARD...?
    {
        int threshold = 10;
        cvtColor(image, hsv, COLOR_BGR2HLS);
        lowRange = Scalar(0, 180, 0);
        highRange = Scalar(255, 245, 70);
        break;
    }

    case Blue:
    {
        cvtColor(image, hsv, COLOR_BGR2HSV);
        lowRange = Scalar(100, 90, 80);
        highRange = Scalar(120, 255, 255);
        break;
    }

    case Yellow:
    {
        cvtColor(image, hsv, COLOR_BGR2HSV);
        lowRange = Scalar(20, 160, 160);
        highRange = Scalar(30, 255, 255);
        break;
    }

    case Red: // Needs more refining...?
    {
        cvtColor(image, hsv, COLOR_RGB2HSV);
        lowRange = Scalar(118, 130, 70);
        highRange = Scalar(140, 255, 205);
        break;
    }

    case Green:
    {
        cvtColor(image, hsv, COLOR_BGR2HSV);
        lowRange = Scalar(50, 120, 70);
        highRange = Scalar(80, 255, 255);
        break;
    }

    case Orange:
    {
        cvtColor(image, hsv, COLOR_BGR2HSV);
        lowRange = Scalar(5, 205, 65);
        highRange = Scalar(20, 255, 255);
        break;
    }
    }
    inRange(hsv, lowRange, highRange, end);

    cvtColor(end, end, COLOR_GRAY2BGR);
    bitwise_and(image, end, end);

    return end;
}

Mat Webcam::CombineColorFilterResults(Mat frame, vector<int> colorToFilter)
{
    Mat end(frame.size().height, frame.size().width, CV_8UC3, Scalar(0, 0, 0));
    for (int i : colorToFilter)
    {
        Mat sq2 = ColorFilter(frame, i), sq_gray, mask, bg, fg;
        cvtColor(sq2, sq_gray, COLOR_BGR2GRAY);
        threshold(sq_gray, mask, 120, 255, THRESH_BINARY);

        bitwise_or(end, end, mask);

        Mat maskinv; bitwise_not(sq_gray, maskinv);
        bitwise_and(sq2, sq2, fg, maskinv);

        add(end, fg, end);
    }

    return end;
}

//static void findSquares(const Mat& image, vector<vector<Point>>& squares)
//{
//    squares.clear();
//
//    Mat timg(image);
//    //medianBlur(image, timg, 3);
//    Mat gray0(timg.size(), CV_8U), gray;
//
//    vector<vector<Point> > contours;
//
//    // find squares in every color plane of the image
//    for (int c = 0; c < 3; c++)
//    {
//        int ch[] = { c, 0 };
//        mixChannels(&timg, 1, &gray0, 1, ch, 1);
//        for (int l = 0; l < N; l++)
//        {
//            // hack: use Canny instead of zero threshold level.
//            // Canny helps to catch squares with gradient shading
//            if (l == 0)
//            {
//                // apply Canny. Take the upper threshold from slider
//                // and set the lower to 0 (which forces edges merging)
//                Canny(gray0, gray, 5, thresh, 5);
//                // dilate canny output to remove potential
//                // holes between edge segments
//                dilate(gray, gray, Mat(), Point(-1, -1));
//            }
//            else
//            {
//                gray = gray0 >= (l + 1) * 255 / N;
//            }
//
//            // find contours and store them all as a list
//            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
//
//            vector<Point> approx;
//
//            // test each contour
//            for (size_t i = 0; i < contours.size(); i++)
//            {
//                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);
//
//                if (approx.size() == 4 &&
//                    fabs(contourArea(Mat(approx))) > minSize &&
//                    fabs(contourArea(Mat(approx))) < maxSize &&
//                    isContourConvex(Mat(approx)))
//                {
//                    double maxCosine = 0;
//
//                    for (int j = 2; j < 5; j++)
//                    {
//                        // find the maximum cosine of the angle between joint edges
//                        double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
//                        maxCosine = MAX(maxCosine, cosine);
//                    }
//
//                    // if cosines of all angles are small
//                    // (all angles are ~90 degree) then write quandrange
//                    // vertices to resultant sequence
//                    if (maxCosine < 0.3)
//                        squares.push_back(approx);
//                }
//            }
//        }
//    }
//}

void Webcam::findSquaresAlt(const Mat image, vector<vector<Point>>& squares, vector<int>& colorType)
{
    int N2 = 2;
    squares.clear();

    vector<vector<Point> > contours;

    // find squares in every rubiks cube's color possible;
    for (int c = 1; c < 6; c++)
    {
        for (int l = 0; l < N2; l++)
        {
            Mat end = WC.ColorFilter(image, c);
            cvtColor(end, end, COLOR_BGR2GRAY);
            //medianBlur(end, end, 5);
            dilate(end, end, Mat(), Point(-1, -1));

            findContours(end, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for (size_t i = 0; i < contours.size(); i++)
            {
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);

                if (approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > minSize &&
                    fabs(contourArea(Mat(approx))) < maxSize &&
                    isContourConvex(Mat(approx)))
                {
                    double maxCosine = 0;

                    for (int j = 2; j < 5; j++)
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if (maxCosine < 0.3)
                    {
                        squares.push_back(approx);
                        colorType.push_back(c);
                    }
                }
            }
        }
    }
}

void Webcam::debugPos(const vector<Point> square)
{
    cout << endl << "{";
    for (int i = 0; i < square.size(); i++)
        cout << "[" << square[i].x << "," << square[i].y << "]" << ", ";
    cout << "}" << endl;
}

void Webcam::filterExtraContours(vector<vector<Point>>& sq, vector<int>& ct)
{
    if (sq.size() < 2) return;

    vector<int> validsq;

    for (int i = 0; i < sq.size(); i++) validsq.push_back(1);

    cout << "Filtering out the colliding squares... ";

    for (int i = 0; i < sq.size() - 1; i++)
    {
        if (validsq[i] == 0) continue;

        for (int j = i + 1; j < sq.size(); j++)
            if (WC.isOverlapping(sq[i], sq[j])) validsq[i] = 0;
    }

    vector<vector<Point>> nsq;
    vector<int> nct;
    for (int i = 0; i < validsq.size(); i++)
    {
        if (validsq[i] == 1)
        {
            nsq.push_back(sq[i]);
            nct.push_back(ct[i]);
        }
    }

    cout << nsq.size() << " / " << sq.size() << endl;

    sq.clear(); sq = nsq;
    ct.clear(); ct = nct;
}

void Webcam::drawSquares(Mat& image, const vector<vector<Point>>& squares)
{
    for (size_t i = 0; i < squares.size(); i++)
    {
        const Point* p = &squares[i][0];

        int n = (int)squares[i].size();
        //dont detect the border
        if (p->x > 3 && p->y > 3)
            polylines(image, &p, &n, 1, true, Scalar((i * 27 % 255), 255, (i * 47 % 255)), 2, LINE_AA);
    }
}

//void SortListSQ(vector<vector<Point>> &sq)
//{
//    if (sq.size() < 2) return;
//    int l = sq[0][1].x - sq[0][0].x + tolerance;
//    int ok = 0;
//    while (ok == 0) // Sort by height
//    {
//        ok = 1;
//        for (int i = 0; i < sq.size() - 1; i++)
//            if (MiddlePoint(sq[i]).y < MiddlePoint(sq[i+1]).y)
//            {
//                vector<Point> temp = sq[i+1];
//                sq[i+1] = sq[i];
//                sq[i] = temp;
//                ok = 0;
//            }
//    }
//
//    ok = 0;
//    while (ok == 0)
//    {
//        ok = 1;
//        for (int i = 0; i < sq.size() - 1; i++)
//            if (abs(MiddlePoint(sq[i]).y - MiddlePoint(sq[i + 1]).y) <= l)
//            {
//                if (MiddlePoint(sq[i]).x > MiddlePoint(sq[i + 1]).x)
//                {
//                    vector<Point> temp = sq[i + 1];
//                    sq[i + 1] = sq[i];
//                    sq[i] = temp;
//                    ok = 0;
//                }
//            }
//    }
//}

void Webcam::translateFaceColor(Mat& frame, vector<vector<Point>> square, vector<int> colorType, int(&ma)[3][3])
{
    // White being the default color for all squares
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) ma[i][j] = 0;

    if (square.size() < 1) return;

    for (int i = 0; i < square.size(); i++)
    {
        Point pos = sqPosition(frame, square[i]);
        ma[pos.y][pos.x] = colorType[i];
    }

    int debugLines = 1;

    if (debugLines)
    {
        int centerY = frame.size().height / 2,
            centerX = frame.size().width / 2,
            l = abs(square[0][0].x - square[0][1].x) + tolDist;

        //Center lines
        line(frame, Point(frame.size().width, centerY), Point(0, centerY), Scalar(0, 0, 0), 1, 0);
        line(frame, Point(centerX, frame.size().height), Point(centerX, 0), Scalar(0, 0, 0), 1, 0);

        //Offset Lines
        line(frame, Point(frame.size().width, centerY - l), Point(0, centerY - l), Scalar(0, 0, 255), 1, 0);
        line(frame, Point(frame.size().width, centerY + l), Point(0, centerY + l), Scalar(0, 0, 255), 1, 0);
        line(frame, Point(centerX - l, frame.size().height), Point(centerX - l, 0), Scalar(0, 0, 255), 1, 0);
        line(frame, Point(centerX + l, frame.size().height), Point(centerX + l, 0), Scalar(0, 0, 255), 1, 0);
    }
}