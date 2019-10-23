////////////this program is without clustering, we are just coloring the particular pixels that vote for a valid line
#include<opencv2/opencv.hpp>
#include <stdio.h>   
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>

#include <opencv2/core/core.hpp>

#include <opencv2/imgproc.hpp> // drawing shapes

# define M_PI 3.14159265358979323846

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

Mat image = imread("pc2.jpg", CV_LOAD_IMAGE_COLOR);
Mat image_lines = imread("pc2.jpg", CV_LOAD_IMAGE_COLOR);

int main()
{
	imshow("original image", image);

	//detected_edges mat is the canny edge image
	vector<Point> detected_m_c;//vectot of all the (m,c) values of the final lines obtained by RANSAC
	vector<Point> Pts;//vector of all the white detected pixels in the canny edge image
	//vector<struct dataset> lines;

	int r,c;
	for(r=0;r<image.rows;r++)
	{
		for(c=0;c<image.cols;c++)
		{
			if(image.at<Vec3b>(r,c)[0]<50 && image.at<Vec3b>(r,c)[1]<50 && image.at<Vec3b>(r,c)[2]<50)
			{
				Point pt_detected;
				pt_detected.x = c;
				pt_detected.y = r;
				Pts.push_back(pt_detected);//pushed the white pixel into Pts vector
			}
		}
	}

	int size = Pts.size();
	cout << "\nThe number of white pixels is: "<<size;

	int iterations = 100;//the number of times we want to randomly test for a line...ideally we want a big number like 100 or above, but becoming computationally expensive 
	int count;
	srand(time(0));

	for(count=0;count<iterations;count++)
	{
		int pt1 = rand()%size;
		int pt2 = rand()%size;
		cout<<count<<"("<<pt1<<","<<pt2<<")"<<endl;
		//cout << "\nThe first is: "<<pt1;
		//cout << "\nThe second is: "<<pt2;

		vector<Point> consensus_set;//////this is the list of all the points that lie within the error bounds of the proposed line
		int m;

		if((Pts[pt2].x - Pts[pt1].x) == 0)
		{
			m = 30000;
		}
		
		else
			if(m<=30000 && m>=-30000)
				m = (Pts[pt2].y - Pts[pt1].y)/(Pts[pt2].x - Pts[pt1].x);
			else if(m>30000)
				m = 30000;
			else
				m = -30000;

		int c;
		
		if( (Pts[pt2].y - (m*Pts[pt2].x)) <= 30000 && (Pts[pt2].y - (m*Pts[pt2].x)) >= -30000 )
		{
			c = Pts[pt2].y - (m*Pts[pt2].x);
		}

		else if( (Pts[pt2].y - (m*Pts[pt2].x)) < -30000 )
		{
			c = -30000;
		}

		else if( (Pts[pt2].y - (m*Pts[pt2].x)) > 30000  )
		{
			c = 30000;
		}	
		

		int counter;
		int Th = 2;//Th is the threshold error bound
		int Thresh_for_line = 100;//Thresh_for_line is the threshold of inliers to classify as a line

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
		}
	}
	imshow("Final clustered lines", image_lines);
	waitKey(0);
	destroyAllWindows();

}
