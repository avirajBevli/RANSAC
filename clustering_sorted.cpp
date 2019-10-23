//if threshold is kept high, the right lane is not detected
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

Mat image = imread("painted.jpg", CV_LOAD_IMAGE_COLOR);
Mat image_lines = imread("painted.jpg", CV_LOAD_IMAGE_COLOR);
Mat blurred_image = Mat::zeros(image.rows, image.cols, CV_8UC1);
Mat detected_edges = Mat::zeros(image.rows, image.cols, CV_8UC1);


int main()
{
	int k_blur = 3; //for median blur	
	int lowThreshold = 130;//for canny
	int ratio = 3;//for canny
	int kernel_size = 3; //for canny

	medianBlur(image, blurred_image, k_blur);

	namedWindow("original image");
	namedWindow("canny image");

	Canny( blurred_image, detected_edges, lowThreshold, (lowThreshold*ratio), kernel_size );

	imshow("original image", image);
	imshow("canny image", detected_edges);

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
			}
		}
	}

	int size = Pts.size();
	cout << "\nThe number of white pixels is: "<<size;

	int iterations = 200;//the number of times we want to randomly test for a line...ideally we want a big number like 100 or above, but becoming computationally expensive 
	int count;
	
	for(count=0;count<iterations;count++)
	{
		//srand(time(0));
		int pt1 = rand()%size;
		int pt2 = rand()%size;
		//cout << "\nThe first is: "<<pt1;
		//cout << "\nThe second is: "<<pt2;

		vector<Point> consensus_set;
		int m = (Pts[pt2].y - Pts[pt1].y)/(Pts[pt2].x - Pts[pt1].x);
		int c = Pts[pt2].y - (m*Pts[pt2].x);

		int counter;
		int Th = 15;//Th is the threshold error bound
		int Thresh_for_line = 1000;//Thresh_for_line is the threshold of inliers to classify as a line

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
		}
	}
    
    int no_lines = detected_m_c.size();
    cout << "\nThe number of detected lines is: "<<no_lines<<endl;

    Scalar colorLine(0,255,0); // Green
    int thicknessLine = 4;
    Scalar colorLine_clustered(0,0,255); // Red
    int thicknessLine_clustered = 2;
  
    for(count=0;count<no_lines;count++)
    { 
	  	Point p1, p2;
	  	int m = detected_m_c[count].x;
	  	int c = detected_m_c[count].y;

	  	p1.y = image.rows;
	  	p1.x = (int)((image.rows - c)/(float)m); 

	    p2.y = 0;
	  	p2.x = (int)(-c/(float)m);
/////////////the code to draw the line has been hard coded to draw only specific sort of lines/////////

	    line(image, p1, p2, colorLine, thicknessLine);
    }
	    
	//since we know the input image consists of 2 lines only, we now apply k means clustering using k=2
////////////////////Code for K-Means algorithm/////////////////////////
	

	/*struct dataset
	{
		Point p;
		int cluster_no;
	};*/

	/////////we have assumed we know the numer of clusters beforehand, that is 2 in this case
	//srand(time(0));
/*	int rand1 = rand()%no_lines;
	int rand2 = rand()%no_lines;*/
	int rand1 = 1;
	int rand2 = 2;
	////////////
	////////////we need to modify that such that it is ensured that the values of c1 and c2 are different
	////////////
 	cout << "\nThe value of c1 is: "<<rand1<<endl;
  	cout << "\nThe value of c2 is: "<<rand2<<endl;

	int	c1x = detected_m_c[rand1].x;
	int c1y = detected_m_c[rand1].y;
	int	c2x = detected_m_c[rand2].x;
	int c2y = detected_m_c[rand2].y;
	//struct dataset data;
	vector<struct dataset> lines;

//we use 10 iterations of the k-means algorithm to perform clustering
	int no_cluster1 = 0;
	int no_cluster2 = 0;

	for(r=0;r<100;r++)
	{
	    no_cluster1 = 0;
	    no_cluster2 = 0;

	    for(count=0;count<no_lines;count++)
	    {
	 		struct dataset data;
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
	    	}
	    	
	    	else
	    	{
				c2x = c2x + detected_m_c[count].x;
	    		c2y = c2y + detected_m_c[count].y;
	    		no_cluster2++;
	    	}
	    }

	    c1x = c1x/no_cluster1;
  		c1y = c1y/no_cluster1;
  	
  		c2x = c2x/no_cluster2;
  		c2y = c2y/no_cluster2;

	}

	Point P11, P21, P12, P22;
  	P11.y = image.rows;
  	P11.x = (int)((image.rows - c1y)/(float)c1x); 

    P21.y = 0;
  	P21.x = (int)(-c1y/(float)c1x);

  	P12.y = image.rows;
  	P12.x = (int)((image.rows - c2y)/(float)c2x); 

    P22.y = 0;
  	P22.x = (int)(-c2y/(float)c2x);

/////////////the code to draw the line has been hard coded to draw only specific sort of lines/////////

    line(image_lines, P11, P21, colorLine_clustered, thicknessLine_clustered);
	line(image_lines, P12, P22, colorLine_clustered, thicknessLine_clustered);
	
	imshow("RANSAC lines waali image", image);
	imshow("Final clustered lines", image_lines);
	waitKey(0);
	destroyAllWindows();

}
