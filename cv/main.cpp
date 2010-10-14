#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <time.h>
#include <MBSUtil.h>
#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int fps = 5;
int delayFrames = 1000 / fps;
CvCapture* capture = NULL;
CvVideoWriter* writer = NULL;
bool showWindows = true;
int webcamId = -1;
bool isWebcam = true;
bool getout = false;
time_t timestamp = 0;
FILE* outChapter = NULL;

std::vector<unsigned long> chapters;
unsigned long count = 0;

CvFont font;
CvScalar color = CV_RGB(255, 255, 0);

#define MAX_FILENAME 255
char inBase[MAX_FILENAME] = "";
char inFile[MAX_FILENAME] = "";
char outBase[MAX_FILENAME] = "";
char outFile[MAX_FILENAME] = "";

// signal handler
void catch_int(int)
{
    TRACE_INFO("INT signal received");
    getout = true;
}

int g_slider_position = 0;
void onTrackbarSlide(int pos)
{
    cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, pos);
    //TRACE_INFO("on slide %d", pos);
}

int g_slider_chapter = 0;
void onTrackbarSlideChapter(int pos)
{
    if(pos == 0)
        cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);
    else
        cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, chapters[pos-1]);
}

int main(int argc, char *argv[])
{
    int rc = 0;

    MBSTrace::getInstance().setFileName("cv",".log");
    MBSTrace::getInstance().openFile();

    time ( &timestamp );
    TRACE_INFO("Log started %s", fukctime(ctime(&timestamp)));

    signal(SIGINT, catch_int);

    for(int narg=1; narg<argc; narg++)
    {
        if(argv[narg][0]=='-') // option
        {
            switch(argv[narg][1])
            {
            case 'o':   // output file name
                narg++;
                if(narg >= argc)
                    TRACE_ERROR("Missing output filename");
                else
                {
                    strcpy(outBase, argv[narg]);

                    char strtime[20];
                    strftime(strtime, 20, "%Y%m%d%H%M%S", localtime(&timestamp));
                    snprintf(outFile, MAX_FILENAME, "%s%s.avi", outBase, strtime);
                    TRACE_INFO("Writing output to file %s", outFile);

                    char filename[MAX_FILENAME];
                    snprintf(filename, MAX_FILENAME, "%s%s.avi.idx", outBase, strtime);
                    outChapter = fopen(filename, "a+");
                    if(outChapter)
                        TRACE_INFO("Writing chapters to %s", filename);
                    else
                        TRACE_ERROR("Could not open %s", filename);
                }
                break;
            case 'h':
                TRACE_INFO("Batch mode");
                showWindows = false;
                break;
            default:
                TRACE_ERROR("Unknown option %s", argv[narg]);
                break;
            }
        }
        else if( isdigit(argv[narg][0]) ) // webcam system id
        {
            webcamId = atoi(argv[narg]);
            TRACE_INFO("Input from camera %d", webcamId);
        }
        else // input file name
        {
            strcpy(inBase, argv[narg]);
            TRACE_INFO("Input from file %s", inBase);
            isWebcam = false;
            capture = cvCreateFileCapture(inBase);
            fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
            delayFrames = 1000 / fps;
        }
    }

    if(showWindows)
    {
        cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX, 1, 1);
        cvNamedWindow("CurrentFrame", CV_WINDOW_AUTOSIZE);

        if(!isWebcam )
        {
            cvCreateTrackbar("Position", "CurrentFrame", &g_slider_position,
                             (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT),
                             onTrackbarSlide);

            char inChapter[MAX_FILENAME];
            strcpy(inChapter,inBase);
            strcat(inChapter,".idx");

            FILE *showChapter;
            if((showChapter = fopen(inChapter,"r")))
            {
                char line[80];
                while(fgets(line, 80, showChapter))
                {
                    TRACE_INFO("Chapter found: %ld",atoi(line));
                    chapters.push_back(atoi(line));
                }
                fclose(showChapter);
                if(chapters.size()>1)
                    cvCreateTrackbar("Chapter", "CurrentFrame", &g_slider_chapter,
                                    (chapters.size()-1),
                                    onTrackbarSlideChapter);
            }
        }
        cvMoveWindow("CurrentFrame", 0, 0);

        //cvNamedWindow("Controls", CV_WINDOW_AUTOSIZE);
        cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);
    }

    if( isWebcam )
        capture = cvCreateCameraCapture(-1);

    CvSize size = cvSize((int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH),
                      (int)cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT));

    if(outFile[0] != 0)
        writer = cvCreateVideoWriter(outFile, CV_FOURCC('D', 'I', 'V', 'X'), fps, size);

    IplImage* grayFrame = cvCreateImage(size, IPL_DEPTH_8U, 1);

    CvSize prcSize = cvSize(size.width/2,size.height/2);
    IplImage* prcImage = cvCreateImage(prcSize, IPL_DEPTH_8U, 1);
    IplImage* currImage = cvCreateImage(prcSize, IPL_DEPTH_8U, 1);
    IplImage* oldImage = cvCreateImage(prcSize, IPL_DEPTH_8U, 1);

    int kernelSize = 11;
    IplConvKernel *kernel=cvCreateStructuringElementEx(kernelSize,kernelSize,kernelSize/2+1,kernelSize/2+1,CV_SHAPE_RECT);

    // play it
    while(!getout)
    {
        // pause for user input
        //switch(cvWaitKey(1))
        switch(cvWaitKey(delayFrames))
        {
            case 27:    //ESC
                TRACE_INFO("ESC key pressed");
                getout = true;
            break;
        }

        time ( &timestamp );
        //TRACE_INFO("%ld",timestamp);

        // full resolution
        IplImage* frame = cvQueryFrame(capture);

        if( frame )
        {
            // convert to grayscale
            cvConvertImage(frame, grayFrame);
            //cvCvtColor(frame, currImage, CV_RGB2GRAY);

            // resize down
            cvPyrDown(grayFrame, currImage);

            // diff with previous frame
            cvAbsDiff(oldImage, currImage, prcImage);

            // conver to B&W
            cvThreshold(prcImage, prcImage, 15, 255, CV_THRESH_BINARY);

            // remove noise and tiny changes
            cvErode(prcImage, prcImage, kernel);

            if( showWindows )
            {
                if( !isWebcam )
                {
                    //int curr = (int) cvGetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES );
                    //TRACE_INFO("set track %d", curr);
                    //cvSetTrackbarPos("Position", "CurrentFrame", curr);
                }

                cvShowImage( "CurrentFrame", frame );
                cvShowImage( "Test", prcImage );
            }

            if( writer )
            {
                static int framesToSave = 0;

                if( cvCountNonZero(prcImage) )
                    framesToSave = 2 * fps;

                if ( framesToSave )
                {
                    framesToSave--;

                    // timestamp watermark
                    CvPoint pos = { 0, size.height - 5 };
                    cvPutText(frame, fukctime(ctime(&timestamp)), pos, &font, color);

                    cvWriteFrame(writer, frame);

                    count++;

                    // timeout
                    if ( ! framesToSave )
                    {   // close chapter
                        fprintf(outChapter, "%ld\n", count);
                    }
                }
            }

            // swap pointers (save currImage as oldImage)
            IplImage* tmpImage = currImage;
            currImage = oldImage;
            oldImage = tmpImage;
        }
    }

    // clean up and exit

    cvReleaseImage( &currImage );
    cvReleaseImage( &prcImage );
    cvReleaseImage( &oldImage );

    if( writer )
        cvReleaseVideoWriter( &writer );

    if( outChapter )
        fclose(outChapter);

    if( capture )
        cvReleaseCapture( &capture );

    if( showWindows )
    {
        cvDestroyWindow( "CurrentFrame" );
        cvDestroyWindow( "Test" );
        cvDestroyWindow( "Controls" );
    }

    time ( &timestamp );
    TRACE_INFO("Log finished %s", fukctime(ctime(&timestamp)));
    TRACE_INFO("Exit %d", rc);
    exit(rc);
}
