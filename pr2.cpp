//Th, threshold_for_line, iterations need to be changed according to the input image
#include<opencv2/opencv.hpp>
#include <stdio.h>   
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>

#include <opencv2/core/core.hpp>

#include <opencv2/imgproc.hpp> // drawing shapes

//# define M_PI 3.14159265358979323846

using namespace std;
using namespace cv;

vector<Point> p1;
vector<Point> p2;

struct dataset
{
	Point p;
	int cluster_no;
};


int dist(vector<Point> Pts, int m, int c, int counter)
{
	int d;
	d = ( abs( ( -m*(Pts[counter].x) ) + Pts[counter].y - c ) )/( sqrt(pow(m,2) + 1) );
	return d; 
} 

Mat image = imread("pc.jpg", CV_LOAD_IMAGE_COLOR);
Mat image_lines = imread("pc.jpg", CV_LOAD_IMAGE_COLOR);
//Mat blurred_image = Mat::zeros(image.rows, image.cols, CV_8UC1);
//Mat detected_edges = Mat::zeros(image.rows, image.cols, CV_8UC1);

int main()
{
	/*int k_blur = 3; //for median blur	
	int lowThreshold = 130;//for canny
	int ratio = 3;//for canny
	int kernel_size = 3; //for canny

	medianBlur(image, blurred_image, k_blur);
*/
	namedWindow("original image");
	/*namedWindow("canny image");*/

	/*Canny( blurred_image, detected_edges, lowThreshold, (lowThreshold*ratio), kernel_size );
*/
	imshow("original image", image);
	/*imshow("canny image", detected_edges);
*/
	//detected_edges mat is the canny edge image
	vector<Point> detected_m_c;//vectot of all the (m,c) values of the final lines obtained by RANSAC
	vector<Point> Pts;//vector of all the white detected pixels in the canny edge image
	//vector<struct dataset> lines;

	int r,c;
	/*for(r=0;r<detected_edges.rows;r++)
	{
		for(c=0;c<detected_edges.cols;c++)
		{
			if(detected_edges.at<uchar>(r,c)==255)
			{
				Point pt_detected;
				pt_detected.x = c;
				pt_detected.y = r;
				Pts.push_back(pt_detected);//pushed the white pixel into Pts vector
				// /cout<<"("<<c<<","<<r<<")"<<"  ";
			}
		}
	}
*/
	for(r=0;r<image.rows;r++)
	{
		for(c=0;c<image.cols;c++)
		{
			if(image.at<Vec3b>(r,c)[0]<100 && image.at<Vec3b>(r,c)[1]<100 && image.at<Vec3b>(r,c)[2]<100)
			{
				Point pt_detected;
				pt_detected.x = c;
				pt_detected.y = r;
				Pts.push_back(pt_detected);//pushed the white pixel into Pts vector
				// /cout<<"("<<c<<","<<r<<")"<<"  ";
			}
		}
	}
	cout<<endl;

	int size = Pts.size();
	cout << "\nThe number of white pixels is: "<<size;
cout<<endl;

	int iterations = 20;//the number of times we want to randomly test for a line...ideally we want a big number like 100 or above, but becoming computationally expensive 
	int count;
	
	srand(time(0));
	for(count=0;count<iterations;count++)
	{
		//srand(time(0));
		int pt1 = rand()%size;
		int pt2 = rand()%size;
		cout<<count<<"("<<Pts[pt1].x<<","<<Pts[pt1].y<<")"<<"    "<<"("<<Pts[pt2].x<<","<<Pts[pt2].y<<")"<<"    ";

		//cout << "\nThe first is: "<<pt1;
		//cout << "\nThe second is: "<<pt2;
		int m,c;

		vector<Point> consensus_set;
		/*if(Pts[pt2].x - Pts[pt1].x == 0)
		{
			m = INT_MAX;
			c = Pts[pt2].y - (m*Pts[pt2].x);
		}
*/
		if(Pts[pt2].x - Pts[pt1].x != 0)
		{
			m = (Pts[pt2].y - Pts[pt1].y)/(Pts[pt2].x - Pts[pt1].x);
			c = Pts[pt2].y - (m*Pts[pt2].x);
		}

		int counter;
		int Th = 1;//Th is the threshold error bound
		int Thresh_for_line = 200;//Thresh_for_line is the threshold of inliers to classify as a line

		for(counter=0;counter<size;counter++)
		{
			if(dist(Pts, m,c,counter)<Th)//Th is the threshold error bound
			{
				consensus_set.push_back(Pts[counter]);
			}
		}

		if(consensus_set.size()>Thresh_for_line)//Thresh_for_line is the threshold of inliers to classify as a line
		{
			int consensus_set_size = consensus_set.size();
			int i;
			int row, col;

			for(i=0;i<consensus_set_size;i++)
			{
				row = consensus_set[i].y;
				col = consensus_set[i].x;
				image_lines.at<Vec3b>(row,col)[2] = 255;
				image_lines.at<Vec3b>(row,col)[1] = 0;
				image_lines.at<Vec3b>(row,col)[0] = 0;

			}

			Point poin;
			poin.x = m;
			poin.y = c;
			detected_m_c.push_back(poin);
			cout<<"selected";
		}

		cout<<endl;
	}
   
	imshow("RANSAC lines waali image", image_lines);
	waitKey(0);
	destroyAllWindows();

}

