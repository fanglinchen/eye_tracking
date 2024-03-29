#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include "methods.h"
#include <string.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>
#include <linux/usbdevice_fs.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>

#define  LOG_TAG    "TEST"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define IMG_WIDTH 640
#define IMG_HEIGHT 480

#define ERROR_LOCAL -1
#define SUCCESS_LOCAL 0

struct buffer {
        void *                  start;
        size_t                  length;
};

static char            dev_name[16];
static char package_path[255];
static char proc_path[50];
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;

int camerabase = -1;

int *rgb = NULL;
int *ybuf = NULL;

int yuv_tbl_ready=0;
int y1192_tbl[256];
int v1634_tbl[256];
int v833_tbl[256];
int u400_tbl[256];
int u2066_tbl[256];

int errnoexit(const char *s);

int xioctl(int fd, int request, void *arg);

int checkCamerabase(void);
int opendevice(int videoid);
int initdevice(void);
int initmmap(void);
int startcapturing(void);

int readframeonce(void);
int readframe(void);
void processimage (const void *p);

int stopcapturing(void);
int uninitdevice(void);
int closedevice(void);

void yuyv422toABGRY(unsigned char *src);

#ifdef __cplusplus
extern "C"{
#endif
JNIEXPORT jint JNICALL Java_com_camera_simplewebcam_CameraPreview_prepareCamera( JNIEnv* env,jobject thiz, jint videoid);
JNIEXPORT jint JNICALL Java_com_camera_simplewebcam_CameraPreview_prepareCameraWithBase( JNIEnv* env,jobject thiz, jint videoid, jint videobase);
JNIEXPORT void JNICALL Java_com_camera_simplewebcam_CameraPreview_processCamera( JNIEnv* env,jobject thiz);
JNIEXPORT void JNICALL Java_com_camera_simplewebcam_CameraPreview_stopCamera(JNIEnv* env,jobject thiz);
JNIEXPORT void JNICALL Java_com_camera_simplewebcam_CameraPreview_pixeltobmp( JNIEnv* env,jobject thiz,jobject bitmap);
JNIEXPORT void JNICALL Java_com_camera_simplewebcam_CameraPreview_pupilDetection(JNIEnv* env,jobject thiz);                                                  
#ifdef __cplusplus
}
#endif
