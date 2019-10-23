//works only with 2 lines, since in K-means clustering algorithm, we have used k=2
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

Mat image = imread("painted.jpg", CV_LOAD_IMAGE_COLOR);
Mat image_lines = imread("painted.jpg", CV_LOAD_IMAGE_COLOR);
Mat image_clustered_line = imread("painted.jpg", CV_LOAD_IMAGE_COLOR);
Mat blurred_image = Mat::zeros(image.rows, image.cols, CV_8UC1);
Mat detected_edges = Mat::zeros(image.rows, image.cols, CV_8UC1);

void drawLine(Mat image, int m, int c, int thicknessLine, Scalar colorLine)
{
	Point p1, p2;
	//int flag;
	
	p1.x = 0;
	p1.y = m*p1.x + c;
	
	if(p1.y>=0 && p1.y<image.rows)
	{
		p2.y = 0;
		p2.x = (p2.y - c)/(float)m;
		
		if(!(p2.x>0 && p2.x<image.cols))
		{
			p2.x = image.cols;
			p2.y = m*p1.x + c;

			if(!(p2.y>0 && p2.y<image.rows))
			{
				p2.y = image.rows;
				p2.x = (p2.y - c)/(float)m;
			}
		}
	}

	else
	{
		p1.x = image.cols;
		p1.y = m*p1.x + c;

		if(p1.y>=0 && p1.y<image.rows)
		{
			p2.x = image.cols;
			p2.y = m*p1.x + c;

			if(!(p2.y>0 && p2.y<image.cols))
			{
				p2.y = image.rows;
				p2.x = (p2.y - c)/(float)m;
			}
		}

		else
		{
			p1.y = 0;
			p1.x = (p1.y - c)/(float)m;

			p2.y = image.rows;
			p2.x = (p2.y - c)/(float)m;
		}
	}

	line(image, p1, p2, colorLine, thicknessLine);	
}

int main()
{
	int k_blur = 3; //for median blur	
	int lowThreshold = 130;//for canny
	int ratio = 3;//for canny
	int kernel_size = 3; //for canny

	medianBlur(image, blurred_image, k_blur);

	namedWindow("original image");
	//namedWindow("canny image");

	Canny( blurred_image, detected_edges, lowThreshold, (lowThreshold*ratio), kernel_size );

	imshow("original image", image);
	//imshow("canny image", detected_edges);

	//detected_edges mat is the canny edge image
	vector<Point> detected_m_c;//vectot of all the (m,c) values of the final lines obtained by RANSAC
	vector<Point> Pts;//vector of all the white detected pixels in the canny edge image
	//vector<struct dataset> lines;

	int r,c;
	for(r=0;r<detected_edges.rows;r++)
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
cout<<endl;

	int size = Pts.size();
	cout << "\nThe number of white pixels is: "<<size;
cout<<endl;

	int iterations = 50;//the number of times we want to randomly test for a line...ideally we want a big number like 100 or above, but becoming computationally expensive 
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
		int Th = 3;//Th is the threshold error bound
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
			Point poin;
			poin.x = m;
			poin.y = c;
			detected_m_c.push_back(poin);
			cout<<"selected";
		}
		cout<<endl;
	}
    
    int no_lines = detected_m_c.size();
    cout << "\nThe number of detected lines is: "<<no_lines<<endl;

    Scalar colorLine(0,255,0); // Green
    int thicknessLine = 4;
    Scalar colorLine_clustered1(0,0,255); // Red
    Scalar colorLine_clustered2(255,0,0); // Blue
    int thicknessLine_clustered = 2;
  	
  	Point p1, p2;

    for(count=0;count<no_lines;count++)
    { 
	  	int m = detected_m_c[count].x;
	  	int c = detected_m_c[count].y;
	  	cout<<count<<"("<<m<<","<<c<<")"<<endl;

	  	drawLine(image_lines, m, c, thicknessLine, colorLine);
	}	
	    
	//since we know the input image consists of 2 lines only, we now apply k means clustering using k=2
////////////////////Code for K-Means algorithm/////////////////////////
	
	/////////we have assumed we know the numer of clusters beforehand, that is 2 in this case
	//srand(time(0));
	int rand1 = rand()%(no_lines-1);
	int rand2 = rand()%(no_lines-1);
	if(rand2==rand1)
	{
		rand2 = rand1+1;
	}

 	cout << "\nThe value of c1 is: "<<rand1<<endl;
  	cout << "\nThe value of c2 is: "<<rand2<<endl;

	int	c1x = detected_m_c[rand1].x;
	int c1y = detected_m_c[rand1].y;
	int	c2x = detected_m_c[rand2].x;
	int c2y = detected_m_c[rand2].y;
	//struct dataset data;
	vector<struct dataset> lines;

//we use 100 iterations of the k-means algorithm to perform clustering
	int no_cluster1 = 0;
	int no_cluster2 = 0;

	struct dataset data;

	for(r=0;r<100;r++)
	{
	    no_cluster1 = 0;
	    no_cluster2 = 0;
	    lines.clear();

	    for(count=0;count<no_lines;count++)
	    {
	    	data.p = detected_m_c[count];
	    	//data.cluster_no = 1;

	    	float dist1, dist2;
	    	dist1 = pow((detected_m_c[count].y - c1y),2) + pow((detected_m_c[count].x - c1x),2);
	    	dist2 = pow((detected_m_c[count].y - c2y),2) + pow((detected_m_c[count].x - c2x),2);
	    	
	    	if(dist1<dist2)
	    		data.cluster_no = 1;
	    	else
	    		data.cluster_no = 2;

	    	lines.push_back(data);
	    }

	    c1x = c1y = c2x = c2y = 0;

	    for(count=0;count<no_lines;count++)
	    {
	    	if(lines[count].cluster_no == 1)
	    	{
	    		c1x = c1x + detected_m_c[count].x;
	    		c1y = c1y + detected_m_c[count].y;
	    		no_cluster1++;
	    		//cout<<"("<<c1x<<","<<c1y<<")"<<" "<<"("<<no_cluster1<<","<<no_cluster2<<")"endl;
	    	}
	    	
	    	else
	    	{
				c2x = c2x + detected_m_c[count].x;
	    		c2y = c2y + detected_m_c[count].y;
	    		no_cluster2++;
	    		//cout<<"("<<c1x<<","<<c1y<<")"<<" "<<"("<<no_cluster1<<","<<no_cluster2<<")"endl;
	    	}
	    }

	    c1x = (float)c1x/no_cluster1;
  		c1y = (float)c1y/no_cluster1;
  	
  		cout<<"("<<c1x<<","<<c1y<<")"<<" "<<"("<<no_cluster1<<","<<no_cluster2<<")"<<"   ";

  		c2x = (float)c2x/no_cluster2;
  		c2y = (float)c2y/no_cluster2;

  		cout<<"("<<c2x<<","<<c2y<<")"<<" "<<"("<<no_cluster1<<","<<no_cluster2<<")"<<endl;

	}

	cout<<"THe m value of the 1st cluster mean is: "<<c1x<<endl;
	cout<<"THe c value of the 1st cluster mean is: "<<c1y<<endl;
	cout<<"THe m value of the 2nd cluster mean is: "<<c2x<<endl;
	cout<<"THe c value of the 2nd cluster mean is: "<<c2y<<endl;

	int flag1, flag2;
	
	drawLine(image_clustered_line, c1x, c1y, thicknessLine_clustered, colorLine_clustered1);
	drawLine(image_clustered_line, c2x, c2y, thicknessLine_clustered, colorLine_clustered2);
	
	imshow("RANSAC lines waali image", image_lines);
	imshow("Final clustered lines", image_clustered_line);
	waitKey(0);
	destroyAllWindows();

}
