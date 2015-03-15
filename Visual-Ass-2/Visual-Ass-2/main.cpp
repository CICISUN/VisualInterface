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

using namespace cv;
using namespace std;

int IMAGE_SIZE=40;
int BIN_SIZE=64;
int histSize[3] = {BIN_SIZE, BIN_SIZE, BIN_SIZE};


void calchistogram(Mat image, int ID){
    //histogram: [image#-1][red][green][blue]
    Mat histogram;
    int channel_numbers[] = { 0, 1, 2 };
    int* number_bins = new int[image.channels()];
    float ch_range[] = { 0.0, 255.0 };
    const float* channel_ranges[] = {ch_range, ch_range, ch_range};
    //The functions calcHist calculate the histogram of one or more arrays. 
    calcHist(&image, 1, channel_numbers, Mat(), histogram, 3, histSize, channel_ranges );
    
    for (int id=1; id<=IMAGE_SIZE; id++){
    
    if( ID!=id){

        String filename;
        
        if(id<10)
            {filename="i0"+std::to_string(id)+".ppm";}
        else
            {filename="i"+std::to_string(id)+".ppm";}
        
        Mat image_b = imread(filename, IMREAD_COLOR);
        
        Mat histogram_b;

        calcHist(&image_b, 1, channel_numbers, Mat(), histogram_b, 3, histSize, channel_ranges );
        
        double diff=0.0;

        for (int i=0; i<histSize[0]; i++) {
        
            for (int j=0; j<histSize[1]; j++) {
                for (int k=0; k<histSize[2]; k++) {
                    diff+=abs(histogram.at<double>(i, j, k)-histogram_b.at<double>(i, j, k));
                }
            }
        }
        
        cout<<"diff: "<< diff<<"i: "<< id <<endl;
    }
    
} 

}



void readimages(int IMAGESIZE){
    //read in each file and compute the color histogram
    
    String filepre = "images/i";
    String filename="i01.ppm";
    String filename2="i02.ppm";

    Mat image = imread(filename, IMREAD_COLOR);
    
    Mat image2 = imread(filename2, IMREAD_COLOR);

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
