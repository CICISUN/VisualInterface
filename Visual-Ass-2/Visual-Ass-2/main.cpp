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
#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include "dataanalysis.h" 
#include <fstream>

using namespace cv;
using namespace std;
using namespace alglib;


int IMAGE_SIZE=40;
int BIN_SIZE=5;
int BIN_SIZE_2=50;
int BIN_NUM=255/BIN_SIZE+1;
int PERM_SIZE=91391;
int histSize[3] = {BIN_NUM, BIN_NUM, BIN_NUM};
typedef tuple<int,int,double,Mat> tuple_data; //id a, id b, diff, mat
// typedef tuple<tuple<int,int,int,int>, double> tuple_combination;



bool new_compare (const tuple_data &lhs, const tuple_data &rhs){
  return get<2>(lhs) < get<2>(rhs);
}


/*************************************************************************
    calc_l1norm function: calc_l1norm for two images
*************************************************************************/

double calc_l1norm(vector<vector<vector<int>>> a, vector<vector<vector<int>>> b){
    double diff=0.0;
    int nonblack_pixels=0;
        //Iterate thru all bins using indexes, calculate l1-norm for each pair
    for (int i=0; i<histSize[0]; i++) {
        for (int j=0; j<histSize[1]; j++) {
            for (int k=0; k<histSize[2]; k++) {
                 
                diff+=abs(a[i][j][k]-b[i][j][k]);
                nonblack_pixels=nonblack_pixels+abs(a[i][j][k])+abs(b[i][j][k]);
            }
        }
    }
//    cout<<diff/nonblack_pixels<<endl;
    return diff/nonblack_pixels;
}
  
/*************************************************************************
    cvShowManyImages function: cvShowManyImages for seven images
*************************************************************************/


void cvShowManyImages(int ID,tuple_data self,tuple_data a,tuple_data b,tuple_data c,tuple_data x,tuple_data y,tuple_data z) {
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
    
    putText(disp_img, std::to_string(get<1>(self)), Point(45,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(a)) , Point(200,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(b)) , Point(300,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(c)) , Point(400,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(x)) , Point(500,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(y)) , Point(600,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    putText(disp_img, std::to_string(get<1>(z)) , Point(700,70), CV_FONT_NORMAL, 0.5, Scalar(255,255,255),1,1);
    
    namedWindow("result"+to_string(ID));
    imshow("result"+to_string(ID),disp_img);
    
    String filename=to_string(ID)+".jpg";

    imwrite(filename, disp_img);

    disp_img=Scalar(0,0,0);
    disp_img.release();
//    waitKey(0);
}


/*************************************************************************
    calchistogram function: calchistogram for each image
*************************************************************************/


vector<vector<vector<int>>> calchistogram(Mat image){
    
    vector<vector<vector<int>>> histogram (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));
    
    
    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++)
        {
            int b = image.at<Vec3b>(i,j)[0];
            int g = image.at<Vec3b>(i,j)[1];
            int r = image.at<Vec3b>(i,j)[2];
            // cout << r/BIN_SIZE << " " << g/BIN_SIZE << " " << b/BIN_SIZE << endl ;
            if (b < 40 && g < 40 && r < 40){
                continue;
            }
            else{
                histogram[r/BIN_SIZE][g/BIN_SIZE][b/BIN_SIZE]+=1;
                
            }
        }
    }
    return histogram;
}

/*************************************************************************
    find_best_worst function: find_best_worst for each image
*************************************************************************/

void find_best_worst(vector<vector<vector<int>>> self, Mat self_orig, int ID){
  
    vector<tuple_data> data;//to store all diff,id pairs
    vector<tuple_data> result;
    //Find nearest & furthest images by iterating thru all images
    for (int id=1; id<=IMAGE_SIZE; id++){
    //If not current image
    if( ID!=id){
        //Deal with filename
        String filename;
        if(id<10)
            {filename="i0"+to_string(id)+".ppm";}
        else
            {filename="i"+to_string(id)+".ppm";}
        //Calculate Histogram of the image comparing to
        Mat image_b = imread(filename, IMREAD_COLOR);
        vector<vector<vector<int>>> histogram_b (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));
        // calcHist(&image_b, 1, channel_numbers, Mat(), histogram_b, 3, histSize, channel_ranges );
//        normalize(histogram_b, histogram_b, 0, image_b.rows, NORM_L2, -1, Mat() );

        histogram_b=calchistogram(image_b);

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

    cvShowManyImages(ID, result[0],result[1],result[2],result[3],result[4],result[5],result[6]);

}

  
/*************************************************************************
    fillhis function: fillup his for each pair image
*************************************************************************/

vector<vector<double>> fillhis(int start, int end){
    vector<vector<double>> comb (40, vector<double>(40));
    vector<vector<vector<int>>> his_a (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));
    vector<vector<vector<int>>> his_b (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));

    for(int i=start-1; i<=end-1; i++){
        for (int j= i+1; j<=end-1; j++){
            String filename_a, filename_b;
            if(i+1<10)
                {filename_a="i0"+to_string(i+1)+".ppm";}
            if(i+1>=10)
                {filename_a="i"+to_string(i+1)+".ppm";} 
            if(j+1<10)
                {filename_b="i0"+to_string(j+1)+".ppm";}
            if(j+1>=10)
                {filename_b="i"+to_string(j+1)+".ppm";}

           
            Mat image_a = imread(filename_a, IMREAD_COLOR);
            Mat image_b = imread(filename_b, IMREAD_COLOR);
            his_a=calchistogram(image_a);
            his_b=calchistogram(image_b);

            comb[i][j]=calc_l1norm(his_a,his_b);
        }
    }

    return comb;
}


/*************************************************************************
    find_best_worst function: find_best_worst_overall for each image
*************************************************************************/
void find_best_worst_overall(int IMAGE_SIZE){
  
   vector<vector<double>> comb (40, vector<double>(40));
   int Max_a,Max_b,Max_c,Max_d;
   int Min_a,Min_b,Min_c,Min_d;
   double max_his_a=0.0;
   double min_his_a=1.0;

   comb=fillhis(1,40);

   vector<vector<vector<vector<double>>>> dp (40, vector<vector<vector<double>> >(40, vector<vector<double> >(40,vector<double>(40))));

   for(int i=0;i<39;i++){
        for(int j=i+1;j<40;j++){
            for(int k=j+1;k<40;k++){
                for(int l=k+1;l<40;l++){
                    dp[i][j][k][l]=(comb[i][j]+comb[i][k]+comb[i][l]+comb[j][k]+comb[j][l]+comb[k][l])/6;
                    if(dp[i][j][k][l]<min_his_a){
                        min_his_a=dp[i][j][k][l];
                        Min_a=i;
                        Min_b=j;
                        Min_c=k;
                        Min_d=l;
                    }
                    else if(dp[i][j][k][l]>max_his_a){
                        max_his_a=dp[i][j][k][l];
                        Max_a=i;
                        Max_b=j;
                        Max_c=k;
                        Max_d=l;
                    }
            }
        }
    }
   }



//    for(int i=0; i<39; i++){
//         for (int j= i+1; j<40; j++){
// //            cout<<", "<<comb[i][j];
//             if(comb[i][j]>max_his_a){
//                 max_his_a=comb[i][j];
//                 Max_a=i;
//                 Max_b=j;
//             }
//             else if(comb[i][j] >max_his_b){
//                 max_his_b=comb[i][j];
//                 Max_c=i;
//                 Max_d=j;
//             }
//             else if(comb[i][j] <min_his_a){
//                 min_his_a=comb[i][j];
//                 Min_a=i;
//                 Min_b=j;
//             }
//             else if(comb[i][j] <min_his_b){
//                 min_his_b=comb[i][j];
//                 Min_c=i;
//                 Min_d=j;
//             }
//         }
         
//     }
    
    //for printing out the nn matrix
    // for ( std::vector<std::vector<int>>::size_type i = 0; i < comb.size(); i++ )
    // {
    //     for ( std::vector<int>::size_type j = 0; j < comb[i].size(); j++ )
    //     {
    //         std::cout << comb[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }
//    cout<< min_his_a<< " "<<max_his_a<<endl;
//    cout<< Max_a<<" "<< Max_b<<" "<< Max_c<<" "<< Max_d<<endl;
//    cout<<Min_a<<" "<< Min_b<<" "<< Min_c<<" "<< Min_d<<endl;
//
    //  0.738717 1
    // 14 18 29 30
    // 5 6 8 10
}


/*************************************************************************
    part_one: find_best_worst for each image, find best_worst four-image groups
*************************************************************************/

void part_one(Mat image, int ID){
    //histogram[red][green][blue] for image[ID]
    vector<vector<vector<int>>> histogram (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));
    histogram=calchistogram(image);
    find_best_worst(histogram, image, ID);
}


/*************************************************************************
    readimages function: trigger comparison for each image
*************************************************************************/

void readimages_part_one(int IMAGE_SIZE){
    //read in each file and compute the color histogram
    
    for (int i=1; i<=IMAGE_SIZE;i++){
        String filename;
        if(i<10)
            {filename="i0"+to_string(i)+".ppm";}
        else
            {filename="i"+to_string(i)+".ppm";} 

        Mat image = imread(filename, IMREAD_COLOR);
        part_one(image,i);  
    }

    find_best_worst_overall(IMAGE_SIZE);
}


vector<vector<int>> calc_grey(Mat image, int ID){
    
    vector<vector<int>> grey_image (60,vector<int>(89));

    for(int i = 0; i < image.rows; i++){
            for(int j = 0; j < image.cols; j++)
            {
                
                int b = image.at<Vec3b>(i,j)[0];
                int g = image.at<Vec3b>(i,j)[1];
                int r = image.at<Vec3b>(i,j)[2];
                if(((r+g+b)/3)<40){continue;}
                else{grey_image[i][j]= (int) (b+g+r)/3;}
                
            }
        }

        return grey_image;
}

vector<int> calc_laplacian(vector<vector<int>> image, int ID){

vector<int> histogram (4000/BIN_SIZE_2);
vector<int> lap;

for(int r = 1; r < 60-1; r++){
    for(int c = 1; c < 89-1; c++)
        {
            int tmp_lap = 8 * image[r][c] - image[r-1][c] - image[r+1][c] - image[r][c-1]- image[r][c+1] - image[r-1][c-1] - image[r-1][c+1] - image[r+1][c-1] - image[r+1][c+1];
            lap.push_back(tmp_lap);     
        }
    }

   int min_elemnt=2000;
   int max_elemnt=2000;

   for(int i=0;i<lap.size();i++){
       
        int index_bin=(int) (lap[i]+min_elemnt)/BIN_SIZE_2;
        histogram[index_bin]+=1;
   
   }
 
    return histogram;
}


/*************************************************************************
    calc_l1norm_two function: calc_l1norm_two for two images
*************************************************************************/

double calc_l1norm_two(vector<int> a, vector<int> b){
   double diff=0.0;
   int nonblack_pixels=0;
       //Iterate thru all bins using indexes, calculate l1-norm for each pair
   for (int i=0; i<a.size(); i++) {
        
       diff+=abs(a[i]-b[i]);
       nonblack_pixels=nonblack_pixels+abs(a[i])+abs(b[i]);
   
   }
    
   return diff/nonblack_pixels;
}

/*************************************************************************
    find_best_worst_two function: find_best_worst_two for all images
*************************************************************************/

void find_best_worst_two(vector<int> self, Mat self_orig, int ID){
    
    vector<tuple_data> data;//to store all diff,id pairs
    vector<tuple_data> result;
    //Find nearest & furthest images by iterating thru all images
    for (int id=1; id<=IMAGE_SIZE; id++){
    //If not current image
    if( ID!=id){
        //Deal with filename
        String filename;
        if(id<10)
            {filename="i0"+to_string(id)+".ppm";}
        else
            {filename="i"+to_string(id)+".ppm";}
        //Calculate Histogram of the image comparing to
        Mat image_b = imread(filename, IMREAD_COLOR);
        
        vector<vector<int>> grey_image (60,vector<int>(89));
        
        vector<int> histogram_b (4000/BIN_SIZE_2);

        grey_image=calc_grey(image_b,id);
       
        //laplacian images
        histogram_b=calc_laplacian(grey_image,id);

        double tmp_diff=calc_l1norm_two(self,histogram_b);

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

    cvShowManyImages(ID, result[0],result[1],result[2],result[3],result[4],result[5],result[6]);

}


/*************************************************************************
    fillhis function: fillup his for each pair image
*************************************************************************/

vector<vector<double>> fillhis_tex(int start, int end){
    vector<vector<double>> comb (40, vector<double>(40));
    vector<int> his_a (4000/BIN_SIZE_2);
    vector<int> his_b (4000/BIN_SIZE_2);
    vector<vector<int>> grey_image_a (60,vector<int>(89));
    vector<vector<int>> grey_image_b (60,vector<int>(89));
    vector<int> histogram_a (4000/BIN_SIZE_2);
    vector<int> histogram_b (4000/BIN_SIZE_2);

    for(int i=start-1; i<=end-1; i++){
        for (int j= i+1; j<=end-1; j++){
            String filename_a, filename_b;
            if(i+1<10)
                {filename_a="i0"+to_string(i+1)+".ppm";}
            if(i+1>=10)
                {filename_a="i"+to_string(i+1)+".ppm";} 
            if(j+1<10)
                {filename_b="i0"+to_string(j+1)+".ppm";}
            if(j+1>=10)
                {filename_b="i"+to_string(j+1)+".ppm";}

           
            Mat image_a = imread(filename_a, IMREAD_COLOR);
            Mat image_b = imread(filename_b, IMREAD_COLOR);
         
            grey_image_a=calc_grey(image_a,i+1);
            grey_image_b=calc_grey(image_b,j+1);
            //laplacian images
            histogram_a=calc_laplacian(grey_image_a,i+1);
            histogram_b=calc_laplacian(grey_image_b,j+1);

            comb[i][j]=calc_l1norm_two(histogram_a,histogram_b);
        }
    }

    return comb;
}


/*************************************************************************
    find_best_worst function: find_best_worst_overall for each image
*************************************************************************/
void find_best_worst_overall_two(int IMAGE_SIZE){
  
   vector<vector<double>> comb (40, vector<double>(40));
   int Max_a,Max_b,Max_c,Max_d;
   int Min_a,Min_b,Min_c,Min_d;
   double max_his_a=0.0;
   double min_his_a=1.0;

   comb=fillhis_tex(1,40);

   vector<vector<vector<vector<double>>>> dp (40, vector<vector<vector<double>> >(40, vector<vector<double> >(40,vector<double>(40))));

   for(int i=0;i<39;i++){
        for(int j=i+1;j<40;j++){
            for(int k=j+1;k<40;k++){
                for(int l=k+1;l<40;l++){
                    dp[i][j][k][l]=(comb[i][j]+comb[i][k]+comb[i][l]+comb[j][k]+comb[j][l]+comb[k][l])/6;
                    if(dp[i][j][k][l]<min_his_a){
                        min_his_a=dp[i][j][k][l];
                        Min_a=i;
                        Min_b=j;
                        Min_c=k;
                        Min_d=l;
                    }
                    else if(dp[i][j][k][l]>max_his_a){
                        max_his_a=dp[i][j][k][l];
                        Max_a=i;
                        Max_b=j;
                        Max_c=k;
                        Max_d=l;
                    }
            }
        }
    }
   }

   //  cout<< min_his_a<< " "<<max_his_a<<endl;
   // cout<< Max_a<<" "<< Max_b<<" "<< Max_c<<" "<< Max_d<<endl;
   // cout<<Min_a<<" "<< Min_b<<" "<< Min_c<<" "<< Min_d<<endl;
   // 0.0623596 0.507399
   //  9 10 25 33
   //  28 29 34 36
}

/*************************************************************************
    find_best_worst function: find_best_worst_overall for each image
*************************************************************************/

void part_two(Mat image, int ID){
    //grayscale image
    vector<vector<int>> grey_image (60,vector<int>(89));
    vector<int> histogram;

    grey_image=calc_grey(image,ID);
   
    //laplacian images
    histogram=calc_laplacian(grey_image,ID);

    find_best_worst_two(histogram, image, ID);

}

/*************************************************************************
    readimages function for parttwo: Read file pixel information for histogram comparison
*************************************************************************/

void readimages_part_two(int IMAGE_SIZE){
//
   for (int i=1; i<=IMAGE_SIZE;i++){
       String filename;
       if(i<10)
           {filename="i0"+to_string(i)+".ppm";}
       else
           {filename="i"+to_string(i)+".ppm";} 

       Mat image = imread(filename, IMREAD_COLOR);
       part_two(image,i);  
   }

    find_best_worst_overall_two(IMAGE_SIZE);


}



/*************************************************************************
    // We have a set of points in 1D space:
    //     (P0,P1,P2,P3,P4) = (1, 3, 10, 16, 20)
    //
    // We want to perform Agglomerative Hierarchic Clusterization (AHC),
    // using either complete or single linkage and Euclidean distance
    // (default metric).
    //
    // First two steps merge P0/P1 and P3/P4 independently of the linkage type.
    // However, third step depends on linkage type being used:
    // * in case of complete linkage P2=10 is merged with [P0,P1]
    // * in case of single linkage P2=10 is merged with [P3,P4]
    //
*************************************************************************/

void cluster(double a[40][40]){
   real_2d_array xy;
    
    xy.setlength(40, 40);
    
    for ( int i = 0; i < 40; i++ )
    {
        for ( int j = 0; j < 40; j++ )
        {
            xy(i,j) = a[i][j];
        }
    }

    clusterizerstate s;
    ahcreport rep;
    // real_2d_array xy=a;
    // real_2d_array xy = "[[1],[3],[10],[16],[20]]";
    integer_1d_array cidx;
    integer_1d_array cz;

    clusterizercreate(s);
    clusterizersetpoints(s, xy, 2);

    // use complete linkage, reduce set down to 2 clusters.
    // print clusterization with clusterizergetkclusters(2).
    // P2 must belong to [P0,P1]
    clusterizersetahcalgo(s, 0);
    clusterizerrunahc(s, rep);
    clusterizergetkclusters(rep, 7, cidx, cz);
    printf("%s\n", cidx.tostring().c_str()); // EXPECTED: [1,1,1,0,0]

    // use single linkage, reduce set down to 2 clusters.
    // print clusterization with clusterizergetkclusters(2).
    // P2 must belong to [P2,P3]
    clusterizersetahcalgo(s, 1);
    clusterizerrunahc(s, rep);
    clusterizergetkclusters(rep, 7, cidx, cz);
    printf("%s\n", cidx.tostring().c_str()); // EXPECTED: [0,0,1,1,1]

}



/*************************************************************************
    readimages function for parttwo: Read file pixel information for histogram comparison
*************************************************************************/

void readimages_part_three(int IMAGE_SIZE, double r){
 
   double a[40][40];
   double T, C;
   vector<vector<int>> grey_image_a (60,vector<int>(89));
   vector<vector<int>> grey_image_b (60,vector<int>(89));
   vector<int> histogram_b (4000/BIN_SIZE_2);
   vector<int> histogram_a (4000/BIN_SIZE_2);
   vector<vector<vector<int>>> his_a (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));
   vector<vector<vector<int>>> his_b (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));
   
   for(int i=0;i<40;i++){
    for(int j=0;j<40;j++){

        String filename_a, filename_b;
        if(i+1<10)
            {filename_a="i0"+to_string(i+1)+".ppm";}
        if(i+1>=10)
            {filename_a="i"+to_string(i+1)+".ppm";} 
        if(j+1<10)
            {filename_b="i0"+to_string(j+1)+".ppm";}
        if(j+1>=10)
            {filename_b="i"+to_string(j+1)+".ppm";}

       
        Mat image_a = imread(filename_a, IMREAD_COLOR);
        Mat image_b = imread(filename_b, IMREAD_COLOR);

        grey_image_a=calc_grey(image_a,i+1);
        grey_image_b=calc_grey(image_b,j+1);

        histogram_a=calc_laplacian(grey_image_a,i+1);
        histogram_b=calc_laplacian(grey_image_b,j+1);

        T=calc_l1norm_two(histogram_a,histogram_b);

        his_a=calchistogram(image_a);
        his_b=calchistogram(image_b);

        C=calc_l1norm(his_a,his_b);



        a[i][j]=r * T +(1-r) * C;

    }
   }
   cluster(a);
}

/*************************************************************************
    main function for partfour: Read file pixel information for histogram comparison
*************************************************************************/


vector<vector<tuple_data>> find_best_worst_four(vector<vector<vector<int>>> self, Mat self_orig, vector<vector<tuple_data>> result,int ID){

    vector<tuple_data> data;//to store all diff,id pairs
    vector<tuple_data> tmp_result;
    //Find nearest & furthest images by iterating thru all images
    for (int id=1; id<=IMAGE_SIZE; id++){
    //If not current image
    if( ID!=id){
        //Deal with filename
        String filename;
        if(id<10)
            {filename="i0"+to_string(id)+".ppm";}
        else
            {filename="i"+to_string(id)+".ppm";}
        //Calculate Histogram of the image comparing to
        Mat image_b = imread(filename, IMREAD_COLOR);
        vector<vector<vector<int>>> histogram_b (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));
        // calcHist(&image_b, 1, channel_numbers, Mat(), histogram_b, 3, histSize, channel_ranges );
//        normalize(histogram_b, histogram_b, 0, image_b.rows, NORM_L2, -1, Mat() );

        histogram_b=calchistogram(image_b);

        double tmp_diff=calc_l1norm(self,histogram_b);

        data.push_back(make_tuple(ID,id,tmp_diff,image_b));

    }
  
} 
    //sort tuple array using new comparator, in decreasing order
    sort(data.begin(),data.end(),new_compare);

    tmp_result.push_back(make_tuple(ID,ID,0.0,self_orig));
    //iterate to get largest and smallest six
    for (int i=0; i<data.size(); i++) {
        if(i<3){
            tmp_result.push_back(data[i]);
//            cout <<get<0>(data[i]) << "\t" << get<1>(data[i]) << "\t" << get<2>(data[i]) << endl;
        }
        if(i>data.size()-4){
            tmp_result.push_back(data[i]);
//            cout <<get<0>(data[i]) << "\t" << get<1>(data[i]) << "\t" << get<2>(data[i]) << endl;
        }
    }

    result.push_back(tmp_result);
    return result;
}


/*************************************************************************
    main function for partfour: Read file pixel information for histogram comparison
*************************************************************************/


vector<vector<tuple_data>> find_best_worst_four_tex(vector<int> self, Mat self_orig, vector<vector<tuple_data>> result,int ID){
    
    vector<tuple_data> data;//to store all diff,id pairs
    vector<tuple_data> tmp_result;
    //Find nearest & furthest images by iterating thru all images
    for (int id=1; id<=IMAGE_SIZE; id++){
    //If not current image
    if( ID!=id){
        //Deal with filename
        String filename;
        if(id<10)
            {filename="i0"+to_string(id)+".ppm";}
        else
            {filename="i"+to_string(id)+".ppm";}
        //Calculate Histogram of the image comparing to
        Mat image_b = imread(filename, IMREAD_COLOR);
        
        vector<vector<int>> grey_image (60,vector<int>(89));
        
        vector<int> histogram_b (4000/BIN_SIZE_2);

        grey_image=calc_grey(image_b,id);
       
        //laplacian images
        histogram_b=calc_laplacian(grey_image,id);

        double tmp_diff=calc_l1norm_two(self,histogram_b);

        data.push_back(make_tuple(ID,id,tmp_diff,image_b));

    }
  
} 
    //sort tuple array using new comparator, in decreasing order
    sort(data.begin(),data.end(),new_compare);

    tmp_result.push_back(make_tuple(ID,ID,0.0,self_orig));
    //iterate to get largest and smallest six
    for (int i=0; i<data.size(); i++) {
        if(i<3){
            tmp_result.push_back(data[i]);
//            cout <<get<0>(data[i]) << "\t" << get<1>(data[i]) << "\t" << get<2>(data[i]) << endl;
        }
        if(i>data.size()-4){
            tmp_result.push_back(data[i]);
//            cout <<get<0>(data[i]) << "\t" << get<1>(data[i]) << "\t" << get<2>(data[i]) << endl;
        }
    }

      result.push_back(tmp_result);
    return result;

}

/*************************************************************************
    main function for partfour: Read file pixel information for histogram comparison
*************************************************************************/

double compare_raw(vector<vector<tuple_data>> sample,vector<vector<int>> input, int type){

double score=0.0;

if(type==1){
    for(int i=0;i<40;i++){
        for (int j=0;j<7;j++){

            int cur_ID=get<0>(sample[i][j]);
            int second_ID=get<1>(sample[i][j]);
            int input_alike=input[i][1];
            int input_unlike=input[i][2];

            if(j==1){
                if(input_alike==second_ID){score+=3;}
            }
            else if(j==2){
                if(input_alike==second_ID){score+=2;}
            }
            else if(j==3){
                if(input_alike==second_ID){score+=1;}
            }
            else if(j==4 || j==5 || j==6){
                if(input_alike==second_ID){score-=1;}
            }
        }
    }
}

if(type==2){
    for(int i=0;i<40;i++){
        for (int j=0;j<7;j++){

            int cur_ID=get<0>(sample[i][j]);
            int second_ID=get<1>(sample[i][j]);
            int input_alike=input[i][3];
            int input_unlike=input[i][4];

            if(j==1){
                if(input_alike==second_ID){score+=3;}
            }
            else if(j==2){
                if(input_alike==second_ID){score+=2;}
            }
            else if(j==3){
                if(input_alike==second_ID){score+=1;}
            }
            else if(j==4 || j==5 || j==6){
                if(input_alike==second_ID){score-=1;}
            }
        }
    }
}

    return score/40;

}


/*************************************************************************
    main function for partfour: Read file pixel information for histogram comparison
*************************************************************************/


void readimages_part_four(){

   vector<vector<tuple_data>> sys_result_color;
   vector<vector<tuple_data>> sys_result_tex;
   double c_score;
   double t_score;


   for (int i=1; i<=IMAGE_SIZE;i++){
       String filename;
       if(i<10)
           {filename="i0"+to_string(i)+".ppm";}
       else
           {filename="i"+to_string(i)+".ppm";} 

       Mat image = imread(filename, IMREAD_COLOR);

       vector<vector<vector<int>>> histogram (BIN_NUM,vector<vector<int> >(BIN_NUM,vector <int>(BIN_NUM)));
       histogram=calchistogram(image);
        vector<vector<int>> grey_image (60,vector<int>(89));
        vector<int> histogram_b;

        grey_image=calc_grey(image,i);
       
        //laplacian images
        histogram_b=calc_laplacian(grey_image,i);

       sys_result_color = find_best_worst_four(histogram, image, sys_result_color,i);
       sys_result_tex = find_best_worst_four_tex(histogram_b, image, sys_result_tex,i);
   }

    vector<vector<int>> input (40, vector<int>(5));
    ifstream in("rawdata_skylar.txt"); 
     
    for(int i = 0; i < 40; ++i){
        for(int j = 0; j < 5; ++j){
            in >> input[i][j];
        }
    }
    in.close();
 
    // for(int i = 0; i < 40; ++i){
    //     for(int j = 0; j < 5; ++j){
    //         cout<<input[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }

   for(int i=0;i<3;i++){
       c_score=compare_raw(sys_result_color,input,1);
       t_score=compare_raw(sys_result_tex,input,2);
   }
   
   cout<<c_score<<endl;
    cout<<t_score<<endl;


}

/*************************************************************************
    main function
*************************************************************************/
int main(int argc, const char * argv[]) { 
 
    //Step 1
    // cout<<"loading images for part one..."<<endl;
    // readimages_part_one(40);
    
    //Step 2
    // cout<<"loading images for part two..."<<endl;
    // readimages_part_two(40);
    
    //Step 3
    // cout<<"loading images for part three..."<<endl;
    // readimages_part_three(40, 0.5);
    
    //r==1
    // [3,5,3,6,6,5,6,3,6,3,3,6,6,5,6,6,2,4,2,4,4,4,2,2,2,0,1,2,4,4,6,4,4,4,4,4,4,2,5,5]
    // [2,6,2,3,3,6,3,2,3,2,2,6,3,6,6,3,4,4,4,4,4,4,4,4,4,0,1,4,4,4,3,4,4,4,4,4,4,4,5,5]
    //r==0
    // [5,2,5,5,3,3,3,5,3,5,3,1,1,1,3,5,0,0,5,2,2,2,3,5,0,3,4,3,6,6,4,6,3,2,0,5,6,5,2,3]
    // [4,6,4,4,6,6,6,4,6,4,6,6,6,6,6,4,6,6,4,6,6,6,6,4,6,6,0,6,5,5,1,2,6,6,6,4,3,4,6,6]
    //r==0.5
    // [2,5,2,2,3,5,3,2,3,2,3,1,1,1,5,2,4,6,4,4,6,6,4,4,4,6,0,4,6,6,3,6,6,6,6,4,6,4,5,5]
    // [4,5,4,4,6,6,6,4,6,4,6,5,0,5,6,4,6,6,6,6,6,6,6,6,6,1,2,6,6,6,3,6,6,6,6,6,6,6,5,6]
    //r==0.8
    // [4,5,4,6,6,5,6,4,6,4,4,6,4,6,6,4,2,3,2,3,3,3,2,2,2,0,1,2,3,3,6,3,3,3,3,3,3,2,5,5]
    // [5,0,5,4,4,6,4,5,4,5,5,4,4,4,4,4,3,3,3,3,3,3,3,3,3,1,2,3,3,3,4,3,3,3,3,3,3,3,6,6]

    //Step 4:
    cout<<"calculating results for part four..."<<endl;

    readimages_part_four();
    
//    waitKey();

    std::cout << "Done.\n";
    return 0;
}
