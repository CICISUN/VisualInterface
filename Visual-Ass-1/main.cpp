#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;


//The function returns the square of the euclidean distance between 2 points.
double dist(Point x,Point y)
{
    return (x.x-y.x)*(x.x-y.x)+(x.y-y.y)*(x.y-y.y);
}


//Rewrite comparator to sort convexity areas
bool wayToSort(int i, int j) { return i > j; }

//Main function call to check if a given sequence is correct input
int checkpassword(String argv , int number) {
    
    int thresh = 100;
    int max_thresh = 255;
    int defect_size=0;
    RNG rng(12345);
    int defect_thresh=0;
    int finger_num=0;
    double max_contourarea=0.0; // biggest contour area of all contours
    double second_max_contourarea=0.0; //second biggest contour area
    double sum_contourarea=0.0; // sum of all contour areas, for computing max/sum ratio of contour area
    double avg_depth=0.0; // sum of all defect points to convex hull
    int defect_num; //number of defect numbers, to compute average average depth
    Size s=Size(640,852); //size of resized image
    Point cm;
//    vector<int> pwd={1,2,3,4,5,0}; //original code for part 1-3
    vector<int> pwd={0,0,8,8,10,10}; //new code for part 4

    Mat im_gray = imread(argv,CV_LOAD_IMAGE_GRAYSCALE);
    
    if (im_gray.empty())
    {
        cout << "Could not read input image file: " << im_gray << endl;
        return -1;
    }
    
    //resize all input images
    resize(im_gray, im_gray, s);
    
    //blur gray scaled image
    blur( im_gray, im_gray, Size(3,3) );
    
    //convert image to binary - black white image
    Mat img_bw = im_gray > 128;
    Mat threshold_output;
    
    cvNamedWindow( "orig", CV_WINDOW_AUTOSIZE );
    imshow( "orig", im_gray );
    
    cvNamedWindow( "binary", CV_WINDOW_AUTOSIZE );
    imshow("binary", img_bw);
    
    
    //try to find countours
    vector<vector<Point>> contour;
    // find contours
    findContours(img_bw,contour,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    // draw contours
    Mat result(img_bw.size(),CV_8U,Scalar(0));
    drawContours(result,contour,-1,Scalar(255),2);
    
    namedWindow("contours");
    imshow("contours",result);
    
    
    //create image to show convex hull result
//    createTrackbar( " Threshold:", "Source", &thresh, max_thresh);
    
    //try to find convex hull
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    //detect threshold
    threshold( im_gray, threshold_output, thresh, 255, THRESH_BINARY );
    
    // Find contours
    
    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    
    // Find the convex hull object for each contour
    vector<vector<Point> >hull( contours.size() );
    vector<vector<Vec4i>> defects(contours.size() );
    vector<vector<int> > hullsI(contours.size());
    vector<int> contourarea(contours.size());
    
    //loop throught all contours and find convex hull for each one of them
    for( int i = 0; i < contours.size(); i++ )
    {
        convexHull( Mat(contours[i]), hull[i], false );
        //find convexity defects
        convexHull( Mat(contours[i]), hullsI[i], false );
        if(hull[i].size()>=3){
            convexityDefects( contours[i], hullsI[i], defects[i]);}
        else cout<<"cannot make convex defects with less than 3 points" <<endl;
    }
    
    
    // Get the moments
    vector<Moments> mu(contours.size());
    
    for(int i = 0; i < contours.size(); i++ )
    {
        mu[i] = moments( contours[i], false );
    }
    
    // Get the mass centers:
    vector<Point> mc(contours.size());

    for( int i = 0; i < contours.size(); i++ )
    {
        mc[i] = Point( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
    }
    
    // Draw contours + hull + center of mass results
    
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
//        cout <<"contour area:"<<contourArea(contours[i])<<" i= "<<i<< endl;
        
        //update countour area array
        contourarea[i]=contourArea(contours[i]);
        sum_contourarea+=contourarea[i];
        
        
        //filter out small hulls
        if(contourArea(contours[i])>5000) {
            //update center of mass
            cm= mc[i];
            
            cout<<" mc[i] x:"<< mc[i].x<<" mc[i] y:"<< mc[i].y<<endl;
            
            //draw contours on binary image
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            drawContours( drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            
            //draw center of mass
            circle( drawing, mc[i], 4, color, -1, 8, 0 );
            
            //filter out unwanted defect points
            defect_num=0;
            for (int j=0;j<defects[i].size();j++){
                
                defect_num++;
                
                Vec4i v=defects[i][j];
                
                int startidx=v[0];
                Point ptStart( contours[i][startidx] ); // point of the contour where the defect begins
                int endidx=v[1];
                Point ptEnd( contours[i][endidx] ); // point of the contour where the defect ends
                int faridx=v[2];
                Point ptFar( contours[i][faridx] );// the farthest from the convex hull point within the defect
                float depth = v[3] / 256; // distance between the farthest point and the convex hull
                
                avg_depth+=dist(mc[i], ptStart);
                
                //            cout <<"depth:"<<depth<< endl;
                Size s = img_bw.size();
                defect_thresh=s.height/6;
                
                line( drawing, ptStart, ptFar, CV_RGB(0,255,0), 1 );
                line( drawing, ptEnd, ptFar, CV_RGB(0,255,0), 1 );
                circle( drawing, ptStart,   4, Scalar(100,0,255), 1 );
                
                if(depth > defect_thresh)
                {   defect_size++;
                    line( drawing, ptStart, ptFar, CV_RGB(0,255,0), 5 );
                    line( drawing, ptEnd, ptFar, CV_RGB(0,255,0), 5 );
                    circle( drawing, ptStart,   4, Scalar(100,0,255), 5 );
                }
                
            }
            avg_depth=avg_depth/defect_num;
        }
    }
    
    //calculate finger numbers
    finger_num=defect_size+1;
    
    //get two highest contour area
    std::sort(contourarea.begin(), contourarea.end(),wayToSort);
    max_contourarea=contourarea[0];
    second_max_contourarea=contourarea[1];
    
    
    //calculate ratio to determine holes
    double tmp_ratio=max_contourarea/sum_contourarea;
    
    if(tmp_ratio>0.97){
       
        cout<<"depth avg: " <<avg_depth<<endl;
        if(avg_depth>50000){
            
            cout<<"finger number: " <<finger_num<<endl;
            
            if(pwd[number-1]==finger_num)
            {
                if(finger_num==1||finger_num==2){
                    cout<<"correct passcode number: "<<number<<endl;
                }
                
                if(finger_num==3){
                    if(cm.x>s.width/3 || cm.y<s.height/2){
                        cout<<"wrong passcode number: "<<number<<endl;
                    }
                    else {
                        cout<<"correct passcode number: "<<number<<endl;
                    }
                }
                if(finger_num==4){
                    if(cm.x<s.width/3 || cm.y<s.height/3 || cm.x>s.width*2/3 || cm.y>s.height*2/3){
                        cout<<"wrong passcode number: "<<number<<endl;
                    }
                    else {
                        cout<<"correct passcode number: "<<number<<endl;
                    }
                }
                if(finger_num==5){
                    if(cm.x<s.width/3 || cm.y<s.height/2){
                        cout<<"wrong passcode number: "<<number<<endl;
                    }
                    else {
                        cout<<"correct passcode number: "<<number<<endl;
                    }
                }
                
            }
            
            
            else
            {
                cout<<"wrong passcode number: "<<number<<endl;
            }
            
            
        }
        else if(pwd[number-1]==0){
//            cout<<"could be a fist"<< endl;
            cout<<"correct passcode number: "<<number<<endl;
        }
        
        else cout<<"wrong passcode number: "<<number<<endl;

    }
    
    else if(tmp_ratio>0.9){
        
        cout<<"could have hole inside,ok?" <<endl;
        
        if(pwd[number-1]==10){
            cout<<"correct passcode number: "<<number<<endl;
        }
        else {
            cout<<"wrong passcode number: "<<number<<endl;
        }
    }
    
    else
        //cout<<"could be heart" <<endl;
    {
        if(pwd[number-1]==8){
            cout<<"correct passcode number: "<<number<<endl;
        }
        else{
            cout<<"wrong passcode number: "<<number<<endl;
        }
    }
    

    
    namedWindow( "demo", CV_WINDOW_AUTOSIZE );
    imshow( "demo", drawing );
    
    
    
    waitKey(0);
    return 0;
    
}



//main system call

int main( int argc, char** argv ) {
    
    checkpassword("fist_0_2.jpg",1);
    checkpassword("fist_0_3.jpg",2);
    checkpassword("heart_0_2.jpg",3);
    checkpassword("heart_0.jpg",4);
    checkpassword("ok_0.jpg",5);
    checkpassword("ok_bad.jpg",6);
    
}	
