//
//  main.cpp
//  Visual-Ass-2
//
//  Created by sunbingjie on 3/14/15.
//  Copyright (c) 2015 sunbingjie. All rights reserved.
//

#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <tuple>
#include <stdio.h>
#include <stdarg.h>

using namespace cv;
using namespace std;

int IMAGE_SIZE=40;
int BIN_SIZE=52;
int histSize[3] = {BIN_SIZE, BIN_SIZE, BIN_SIZE};
typedef tuple<int,int,double,Mat> tuple_data; //id a, id b, diff, mat

bool new_compare (const tuple_data &lhs, const tuple_data &rhs){
  return get<2>(lhs) > get<2>(rhs);
}

double calc_l1norm(Mat a, Mat b){
    double diff=0.0;
    int nonblack_pixels=0,ii=0;
        //Iterate thru all bins using indexes, calculate l1-norm for each pair
    for (int i=0; i<histSize[0]; i++) {
        for (int j=0; j<histSize[1]; j++) {
            for (int k=0; k<histSize[2]; k++) {
                if(i<8 || j<8 || k<8){continue;}
                else{
                    diff+=abs(a.at<double>(i, j, k)-b.at<double>(i, j, k));
                    nonblack_pixels=nonblack_pixels+abs(a.at<double>(i, j, k))+abs(b.at<double>(i, j, k));
                    cout<<"diff: "<<diff<<"nb: "<<nonblack_pixels<<endl;
                }
            }
        }
    }
    
    return diff/nonblack_pixels;
}
  
void cvShowManyImages(tuple_data self,tuple_data a,tuple_data b,tuple_data c,tuple_data x,tuple_data y,tuple_data z) {
    // img - Used for getting the arguments 
    Mat disp_img(Size(750,80),CV_8UC3);

    Mat selff=get<3>(self);
    Mat aa=get<3>(a);
    Mat bb=get<3>(b);
    Mat cc=get<3>(c);
    Mat xx=get<3>(x);
    Mat yy=get<3>(y);
    Mat zz=get<3>(z);

    selff.copyTo(disp_img(Rect(0,0, aa.cols, aa.rows)));
    aa.copyTo(disp_img(Rect(150,0, aa.cols, aa.rows)));
    bb.copyTo(disp_img(Rect(250,0, aa.cols, aa.rows)));
    cc.copyTo(disp_img(Rect(350,0, aa.cols, aa.rows)));
    xx.copyTo(disp_img(Rect(450,0, aa.cols, aa.rows)));
    yy.copyTo(disp_img(Rect(550,0, aa.cols, aa.rows)));
    zz.copyTo(disp_img(Rect(650,0, aa.cols, aa.rows)));
    
    putText(disp_img, std::to_string(get<1>(self)),Point(45,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(a)) , Point(200,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(b)) , Point(300,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(c)) , Point(400,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(x)) , Point(500,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(y)) , Point(600,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(z)) , Point(700,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    namedWindow("result");
    imshow("result",disp_img);

    waitKey(0);
}
 

void find_best_worst(Mat self,Mat self_orig, int ID){\
    
    int channel_numbers[] = { 0, 1, 2 };
    float ch_range[] = { 0.0, 255.0 };
    const float* channel_ranges[] = {ch_range, ch_range, ch_range};
    
    vector<tuple_data> data;//to store all diff,id pairs
    vector<tuple_data> result;
    //Find nearest & furthest images by iterating thru all images
    for (int id=1; id<=IMAGE_SIZE; id++){
    //If not current image
    if( ID!=id){
        //Deal with filename
        String filename;
        if(id<10)
            {filename="i0"+std::to_string(id)+".ppm";}
        else
            {filename="i"+std::to_string(id)+".ppm";}
        //Calculate Histogram of the image comparing to
        Mat image_b = imread(filename, IMREAD_COLOR);
        Mat histogram_b;
        calcHist(&image_b, 1, channel_numbers, Mat(), histogram_b, 3, histSize, channel_ranges );
//        normalize(histogram_b, histogram_b, 0, image_b.rows, NORM_L2, -1, Mat() );

        double tmp_diff=calc_l1norm(self,histogram_b);

        data.push_back(make_tuple(ID,id,tmp_diff,image_b));

    }
  
} 
    //sort tuple array using new comparator, in decreasing order
    sort(data.begin(),data.end(),new_compare);

    result.push_back(make_tuple(ID,ID,0.0,self_orig));
    //iterate to get largest and smallest six
    for (int i=0; i<data.size(); i++) {
        if(i<3){
            result.push_back(data[i]);
//            cout <<get<0>(data[i]) << "\t" << get<1>(data[i]) << "\t" << get<2>(data[i]) << endl;
        }
        if(i>data.size()-4){
            result.push_back(data[i]);
//            cout <<get<0>(data[i]) << "\t" << get<1>(data[i]) << "\t" << get<2>(data[i]) << endl;
        }
    }

    cvShowManyImages(result[0],result[1],result[2],result[3],result[4],result[5],result[6]);

}

void calchistogram(Mat image, int ID){
    //histogram[red][green][blue] for image[ID]
    Mat histogram;
    int channel_numbers[] = { 0, 1, 2 };
    int* number_bins = new int[image.channels()];
    float ch_range[] = { 0.0, 255.0 };
    const float* channel_ranges[] = {ch_range, ch_range, ch_range};
    //The functions calcHist calculate the histogram of one or more arrays. 
    calcHist(&image, 1, channel_numbers, Mat(), histogram, 3, histSize, channel_ranges );
//    normalize(histogram, histogram, 0, image.rows, NORM_L2, -1, Mat() );
    

    find_best_worst(histogram, image, ID);

}

void readimages(int IMAGESIZE){
    //read in each file and compute the color histogram
    String filename="i01.ppm";
    Mat image = imread(filename, IMREAD_COLOR);
    calchistogram(image,1);  
}



int main(int argc, const char * argv[]) {

    readimages(40);
    
    // //Step 1
    // print "COLOR:"
    // colorhist = readfiles1()
    // Cvals = compimcolor(colorhist)
    
    // //Step 2
    // print "\nTEXTURE:"
    // pixels = readfiles2()
    // Tvals = compimtexture(pixels)
    
    // //Step 3
    // print "\nCOMBINED:"
    
    // S = combinecolortex(Tvals, Cvals)
    // cluster(S)
    
    // //Step 4:
    // step4plot(Tvals, Cvals, S)
    
    waitKey();

    std::cout << "Done.\n";
    return 0;
}
