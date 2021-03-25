/*
Convert an image to grayscale.

The code is written for a mini-project of ITCS 5145 Parallel Programming at UNCC.(Q.C.)

To compile the code, we use
        gcc -g -Wall -o color2grapy stb_image/stb_image.h stb_image/stb_image_write.h color2gray.c -lm

To run the code, type
        ./color2gray ${input color image} ${output grayscale image} ${image type}

        The format of images depends on its types.
        To specify image type, we have ${image type} as follows:
            1 is for .png file
            2 is for .jpg file
        
        For example,
        ./color2grapy lena1.png lena2.png 1
        ./color2grapy lizard1.jpg lizard2.jpg 2
*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

using namespace std;

void colorToGrayscale(unsigned char *gray_img, unsigned char * color_img, int width, int height);

void colorToGrayscaleThreadFunction(int threadID, int numOfThreads, unsigned char *gray_img,
		unsigned char * color_img, int width, int height);

//void colorToGrayscaleThreadFunction(unsigned char *gray_img, unsigned char * color_img, int width, int height);

const int IS_PNG = 1;
const int IS_JPG = 2;
const int DESIRED_CHANNELS = 3;
const int MAX_NAME_LENGTH = 500;

int main(int argc, char *argv[]) {
    if (argc < 4){
        printf("Usage: color2Grayscale ${input color image file} ${output grayscale image file} ${image type}\n Image Types:\n\t1: PGN\n\t2: JPG");
	exit(-1);
    }

    int width, height, channels, type;
    char  in_name[MAX_NAME_LENGTH], out_name[MAX_NAME_LENGTH];
    strcpy(in_name, argv[1]);
    strcpy(out_name, argv[2]);
    type = atoi(argv[3]);

    unsigned char *color_img = stbi_load(in_name, &width, &height, &channels, 0); // load and conver the image to 3 channels (ignore the transparancy channel)
    if(color_img == NULL) {
        printf("Error in loading the image\n");
        exit(-1);
    }
    printf("Loaded image %s with a width of %dpx, a height of %dpx and %d channels\n", in_name, width, height, channels);

    // Convert the input image to gray
    int gray_channels = channels == 4 ? 2 : 1;
    size_t gray_img_size = width * height * gray_channels;
    
    unsigned char *gray_img = (unsigned char *)malloc(gray_img_size);

    if(gray_img == NULL) {
        printf("Unable to allocate memory for the gray image.\n");
        exit(1);
    }

    printf("Create a image array with a width of %dpx, a height of %dpx and %d channels\n", width, height, gray_channels);

   //colorToGrayscale(gray_img, color_img, width, height);
    int numOfThreads;
    printf("How many threads for calculation? Please type an integer number:");
    cin >> numOfThreads;

    vector<thread> thread_pool;
    time_t start = clock();
    for (int i = 0; i < numOfThreads; i++) {
    	thread th(colorToGrayscaleThreadFunction,i,numOfThreads,gray_img,color_img,width,height);// @suppress("Invalid arguments")
    	thread_pool.push_back(move(th));
    }
    for (thread & th : thread_pool) {
    	if (th.joinable())
    	    		th.join();
    }
    time_t end = clock();
    if (type == IS_PNG)
    	stbi_write_png(out_name, width, height, gray_channels, gray_img, width * gray_channels);
    else
        if (type == IS_JPG)
            stbi_write_jpg(out_name, width, height, gray_channels, gray_img, 100); //The last parameter of the stbi_write_jpg function is a quality parameter that goes from 1 to 100. Since JPG is a lossy image format, you can chose how much data is dropped at save time. Lower quality means smaller image size on disk and lower visual image quality.
    printf("Wrote image %s with a width of %dpx, a height of %dpx and %d channels\n", out_name, width, height, channels);

    stbi_image_free(gray_img); 

    cout << "Takes " << (end - start) << " clicks (" << ((end-start)*1000/(CLOCKS_PER_SEC + 0.0))
			<< " milliseconds)\n";
}

void colorToGrayscale(unsigned char *gray_img, unsigned char * color_img, int width, int height){
    unsigned char pixel[DESIRED_CHANNELS];
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
           //If the input image has a transparency channel this will be simply copied to the second channel of the gray image, while the first channel of the gray image will contain the gray pixel values. If the input image has three channels, the output image will have only one channel with the gray data.

            int greyOffset = row * width + col;
            int rgbOffset = greyOffset * DESIRED_CHANNELS;
            pixel[0] = color_img[rgbOffset];
            pixel[1] = color_img[rgbOffset + 1];
            pixel[2] = color_img[rgbOffset + 2];

            gray_img[greyOffset] = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
        }
    }
}
void colorToGrayscaleThreadFunction(int threadID, int numOfThreads, unsigned char *gray_img,
		unsigned char * color_img, int width, int height){
	int height_in_chunk = ceil(height / numOfThreads);
	int my_thread_first_row_index = threadID * height_in_chunk;
	int my_thread_last_row_index = my_thread_first_row_index + height_in_chunk - 1 > height ?
	                              height : my_thread_first_row_index + height_in_chunk - 1;
	for(int i = my_thread_first_row_index; i <= my_thread_last_row_index; i++){
		for(int j = 0; j < width;j++){
			int index_in_single_dimension = i * width  + j ;
			double r = color_img[3 * index_in_single_dimension];
			double g = color_img[3 * index_in_single_dimension + 1];
			double b = color_img[3 * index_in_single_dimension + 2];
			double L = r* 0.21 + g * 0.72 + b*0.07;
			gray_img[index_in_single_dimension] = L;
		}
	}
			//row from  my_thread_first_row_index  to my_thread_last_row_index
}
//void colorToGrayscaleThreadFunction(unsigned char *gray_img, unsigned char * color_img, int width, int height){


