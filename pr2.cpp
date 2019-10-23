//since the number of iterations is taken to be 10000 the program will take time...to improve the time reduce the number of iterations in line number 66
//Th, threshold_for_line, iterations need to be changed according to the input image
#include<opencv2/opencv.hpp>
#include <stdio.h>   
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>

#include <opencv2/core/core.hpp>

#include <opencv2/imgproc.hpp> // drawing shapes
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

int main()
{
	namedWindow("original image");
	
	imshow("original image", image);
	
	vector<Point> detected_m_c;//vectot of all the (m,c) values of the final lines obtained by RANSAC
	vector<Point> Pts;//vector of all the white detected pixels in the canny edge image

	int r,c;
	
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

	int iterations = 10000;//the number of times we want to randomly test for a line...ideally we want a big number like 100 or above, but becoming computationally expensive 
	int count;
	
	srand(time(0));
	for(count=0;count<iterations;count++)
	{
		//srand(time(0));
		int pt1 = rand()%size;
		int pt2 = rand()%size;
		cout<<count<<"("<<Pts[pt1].x<<","<<Pts[pt1].y<<")"<<"    "<<"("<<Pts[pt2].x<<","<<Pts[pt2].y<<")"<<"    ";
		int m,c;

		vector<Point> consensus_set;
		
		if(Pts[pt2].x - Pts[pt1].x != 0)
		{
			m = (Pts[pt2].y - Pts[pt1].y)/(Pts[pt2].x - Pts[pt1].x);
			c = Pts[pt2].y - (m*Pts[pt2].x);
		}

		int counter;
		int Th = 2;//Th is the threshold error bound
		int Thresh_for_line = 600;//Thresh_for_line is the threshold of inliers to classify as a line

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

