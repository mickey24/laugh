#include <stdio.h>
#include <cv.h>
#include <highgui.h>

int main(int argc, const char* argv[]) {
  // get an image file name
  if (argc != 2 && argc != 3) {
    printf("Illegal arguments.\n");
    return -1;
  }
  const char* img_name = argv[1];
  const char* output_name = argc == 3 ? argv[2] : "output.png";

  // load a source image
  IplImage* img_target = cvLoadImage(img_name, CV_LOAD_IMAGE_COLOR);
  IplImage* img_laugh = cvLoadImage("laugh.png", CV_LOAD_IMAGE_COLOR);
  IplImage* img_gray = cvCreateImage(cvGetSize(img_target), IPL_DEPTH_8U, 1);

  // load a crassifier cascade
  char cascade_name[] = "haarcascade_frontalface_default.xml";
  CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);

  // preprocessing
  CvMemStorage* storage = cvCreateMemStorage(0);
  cvClearMemStorage(storage);
  cvCvtColor(img_target, img_gray, CV_BGR2GRAY);
  cvEqualizeHist(img_gray, img_gray);

  // detect faces
  CvSeq* faces = cvHaarDetectObjects(img_gray, cascade, storage, 1.11, 4, 0, cvSize(40, 40));

  for (int i = 0; i < (faces ? faces->total : 0); ++i) {
    // resize the laughing man image
    CvRect* r = (CvRect*)cvGetSeqElem(faces, i);
    IplImage* img_laugh_resized = cvCreateImage(cvSize(r->width, r->height), img_target->depth, img_target->nChannels);
    cvResize(img_laugh, img_laugh_resized, CV_INTER_CUBIC);

    int x = r->x, y = r->y;
    const int i_max = ((x + img_laugh_resized->width ) > img_target->width ) ? img_target->width  - x : img_laugh_resized->width;
    const int j_max = ((y + img_laugh_resized->height) > img_target->height) ? img_target->height - y : img_laugh_resized->height;

    // overwrite pixels
    for (int j = 0; j < j_max; ++j){
      for (int i = 0; i < i_max; ++i){
        int r = img_laugh_resized->imageData[img_laugh_resized->widthStep * j + i * 3];
        int g = img_laugh_resized->imageData[img_laugh_resized->widthStep * j + i * 3 + 1];
        int b = img_laugh_resized->imageData[img_laugh_resized->widthStep * j + i * 3 + 2];

        if (r || g || b) {
          img_target->imageData[img_target->widthStep * (y+j) + (x+i) * 3] = r;
          img_target->imageData[img_target->widthStep * (y+j) + (x+i) * 3 + 1] = g;
          img_target->imageData[img_target->widthStep * (y+j) + (x+i) * 3 + 2] = b;
        }
      }
    }
    cvReleaseImage(&img_laugh_resized);
  }

  // output an image
  cvSaveImage(output_name, img_target);

  return 0;
}
