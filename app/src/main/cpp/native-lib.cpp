#include <jni.h>

#include <cstdlib>
#include <cstring>
#include <jni.h>
#include <errno.h>
#include <cassert>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android/fdsan.h>
//#include <android_native_app_glue.h>

//#define _ENABLE_GPU_
#ifdef _ENABLE_GPU_
#ifndef  HETCOMPUTE_HAVE_GPU
#define HETCOMPUTE_HAVE_GPU
#endif
#endif

//#define _CPU_TEST_

#define _ENABLE_DSP_
#ifdef _ENABLE_DSP_
// enable dsp support
#ifndef HETCOMPUTE_HAVE_QTI_DSP
#define HETCOMPUTE_HAVE_QTI_DSP
#endif
#include "hetcompute/hetcompute.hh"
#include "hetcompute_dsp.h"
#include "fastcv.h"
#define BLOCK_SIZE  2048

#endif

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

using namespace std;

static bool gSupportFastCV = true;

extern "C" JNIEXPORT void JNICALL Java_cn_qianzhengwei_libhc_MainActivity_TestQualcommComputeDsp(JNIEnv *env, jobject, int mode);

extern "C" void LoadLib(){

}

extern "C" JNIEXPORT void JNICALL Java_cn_qianzhengwei_libhc_MainActivity_InitLib(JNIEnv *env, jobject, jboolean supportFastCV){
    gSupportFastCV = supportFastCV;
}

/*
extern "C" int fastcvadsp_fcvDotProducts8Q( const int8_t* __restrict a,
                                            const int8_t* __restrict b,
                                            unsigned int             abSize );

extern "C" void dspCV_deinitQ6();
extern "C" void dspCV_initQ6();
extern "C" void dspCV_initQ6_with_attributes(int mode);


extern "C" void fastcvadsp_fcvQ6SessionInit();
extern "C" void fastcvadsp_fcvQ6SessionDeInit();
*/

void cpuMatrixMultiplyf32( const float32_t * __restrict   src1,
                      uint32_t                       src1Width,
                      uint32_t                       src1Height,
                      uint32_t                       src1Stride,
                      const float32_t * __restrict   src2,
                      uint32_t                       src2Width,
                      uint32_t                       src2Stride,
                      float32_t * __restrict         dst,
                      uint32_t                       dstStride )
{

    float32_t result[src1Height];
    for(int m=0;m<src1Height;m++){
        //src1[Mn] * src2[nM]
        dst[m] = 0;
        for(int l=0;l<src2Width;l++) {
            for (int n = 0; n < src1Width; n++) {
                int idxSrc1 = m * src1Width + n;
                int idxSrc2 = src2Width * n + l ;
                float32_t src1Value = src1[idxSrc1];
                float32_t src2Value = src2[idxSrc2];
                dst[m] += src1Value * src2Value;
            }
        }

    }

}

struct tData
{
    float32_t data;
    float32_t data1;
    float32_t data2;
    float32_t data3;
    float32_t data4;
    float32_t data5;
    float32_t data6;
    float32_t data7;
    float32_t data8;
    float32_t data9;
    int datax1;
    int datax2;
    int datax3;
};
//#pragma pack(pop)

typedef float32_t inputData;

//for pinvoke
extern "C" JNIEXPORT void JNICALL TestQualcommComputeDsp(int width, int height, int width1, float32_t* srcInput1, float32_t* srcInput2, float32_t* dstOut) {

    if(!gSupportFastCV)
        return;

    LOGI("$$$ init fcv");
    fcvMemInit();
    fcvMemInitPreAlloc(4 * 1024 * 1024);
    fcvSetOperationMode(fcvOperationMode::FASTCV_OP_LOW_POWER);

/*
    float32_t src1Data[width * width];
    float32_t src2Data[width * width];
    for(int i=0;i<width * width;i++) {
        src1Data[i] = 1;
        src2Data[i] = 1;
    }
    float32_t* src1 = (float32_t *)&src1Data;
    float32_t* src2 = (float32_t *)&src2Data;
*/

    LOGI("$$$ fcvMatrixMultiplyf32 time start");

    int dstSize = height * width1;
    float32_t dst[dstSize];


    fcvMatrixMultiplyf32((const float32_t *)srcInput1,
                         width,
                         height,
                         sizeof(float32_t) * width, //0,//
                         (const float32_t *)srcInput2,
                         width1,
                          sizeof(float32_t) * width1, //0,//
                         (float32_t *)dstOut,
                         0);


    fcvMemDeInit();
    fcvCleanUp();

    LOGI("$$$ fcvMatrixMultiplyf32 time end");

    for(int i=0;i<width * height;i++) {
        LOGI("$$$ fcvMatrixMultiplyf32 src1[%i]:%f", i, srcInput1[i]);
    }
    for(int i=0;i<width * height;i++) {
        LOGI("$$$ fcvMatrixMultiplyf32 src2[%i]:%f", i, srcInput2[i]);
    }

    for(int i=0;i<height * width1;i++) {
        LOGI("$$$ fcvMatrixMultiplyf32 dst[%i]:%f", i, dstOut[i]);
    }
}


//for jni
#ifdef _ENABLE_DSP_
extern "C" JNIEXPORT void JNICALL Java_cn_qianzhengwei_libhc_MainActivity_TestQualcommComputeDsp(JNIEnv *env, jobject, int mode){

/*
    hetcompute::runtime::init();

    std::vector<float_t> vin(1024, 0);
    LOGI("$$$ start ...");
    hetcompute::pfor_each(size_t(0), vin.size(), [&vin](size_t i) {
        vin[i] = cos(pow(sin(pow(i,2)), 2));
    });

    LOGI("$$$ end ...");

    for(int i=0;i<1024;i++) {
        float a = vin[i];
        LOGI("$$$ print %f", (float) a);
    }
    hetcompute::runtime::shutdown();
*/
    LOGI("$$$ init fcv");
    fcvMemInit();
    fcvMemInitPreAlloc(4 * 1024 * 1024);
    //setup fcv mode to dsp
//    fcvSetOperationMode(fcvOperationMode::FASTCV_OP_LOW_POWER);
//    fcvSetOperationMode(fcvOperationMode::FASTCV_OP_CPU_PERFORMANCE);
//    fcvSetOperationMode(fcvOperationMode::FASTCV_OP_CPU_OFFLOAD);
    fcvSetOperationMode((fcvOperationMode)mode);



    char versionInfoStr[16];
    fcvGetVersion((char*)&versionInfoStr, 16);
    LOGI("$$$ fastCV version: %s",(char*) versionInfoStr);
    if(strcmp("1.7.1", versionInfoStr) == 5){

    }

    /*
    size_t size = 4 * 8;
    int8_t a[16];
    int8_t b[16];
    for(int i=0;i<16;i++){
        a[i] = i * 6;
        b[i] = i * i * 2;
    }

//    auto res = fcvDotProducts8((const int8_t*)&a, (const int8_t*)&b, 16);

    LOGI("$$$ fastcvadsp_fcvDotProducts8Q");
    auto res = fcvDotProducts8((const int8_t*)&a, (const int8_t*)&b, 16);
    LOGI("$$$ fcvDotProducts8: %i", res);
*/

    size_t width = 10;
/*
   float32_t src1[width * width];
    float32_t src2[width * width];
    float32_t dst[width * width];
    for(int i=0;i<width * width;i++){
        //src1[i] = i * 2 * 0.1;
        //src2[i] = std::sqrt(i * 0.1);
        src1[i] = 1;
        src2[i] = 1;
    }
 */

       tData src1[width * width];
       tData src2[width * width];
       float32_t dst[width * width];
       for(int i=0;i<width * width;i++){
           float32_t* ptr = (float32_t*)&src1[i];
           float32_t* ptr1 = (float32_t*)&src2[i];
           for(int i=0;i<10;i++){
               ptr[i] = 1;
               ptr1[i] = 1;
           }
           src1[i].datax1 = i;
           src1[i].datax2 = i * 2;
           src1[i].datax3 = i * 3;
           src2[i].datax1 = i + 1;
           src2[i].datax2 = i * 2 + 1;
           src2[i].datax3 = i * 3 + 1;
       }

/*
    LOGI("$$$ will fcvElementMultiplyf32");
    fcvElementMultiplyf32(  (const float32_t *)&src1,
                                  width,
                                  width,
                         0,
            (const float32_t *)&src2,0,
                            (float32_t * __restrict)&dst,0 );

    for(int i=0;i<width * width;i++) {
        LOGI("$$$ fcvElementMultiplyf32 dst[%i]:%f", i, dst[i]);
    }
*/


#ifndef _CPU_TEST_
    LOGI("$$$ fcvMatrixMultiplyf32 time start");

    fcvMatrixMultiplyf32((const float32_t *)&src1,
            width,
            width,
            sizeof(tData), // 0,//
            (const float32_t *)&src2,
           width,
           sizeof(tData), //0,//
           (float32_t *)&dst,
              0);

    fcvMemDeInit();
    fcvCleanUp();

    LOGI("$$$ fcvMatrixMultiplyf32 time end");


    for(int i=0;i<width * width;i++) {
        LOGI("$$$ fcvMatrixMultiplyf32 dst[%i]:%f", i, dst[i]);
    }

#endif


#ifdef _CPU_TEST_

    LOGI("$$$ cpuMatrixMultiplyf32 time start");
    cpuMatrixMultiplyf32((const float32_t *)&src1,
                         width,
                         width,
                         0,
                         (const float32_t *)&src2,
                         width,
                         0,
                         (float32_t *)&dst1,
                         0);
    LOGI("$$$ cpuMatrixMultiplyf32 time end");
    LOGI("$$$ cpuMatrixMultiplyf32 dst1[%i]:%f", 100, dst1[100]);
    /*
    for(int i=0;i<width;i++) {
        LOGI("$$$ cpuMatrixMultiplyf32 dst1[%i]:%f", i, dst1[i]);
    }
*/
#endif

    //fastcvadsp_fcvQ6SessionDeInit();
}

#endif
extern "C" JNIEXPORT jstring JNICALL
Java_cn_qianzhengwei_libhc_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ FastCV";
    return env->NewStringUTF(hello.c_str());
}


//opencl

#ifdef _ENABLE_GPU_

#define OCL_KERNEL(name, k) std::string const name##_string = #k
OCL_KERNEL(vdouble_kernel, __kernel void vdouble(__global float* A, __global float* B) {
    unsigned int i = get_global_id(0);
    B[i] = 2.0 * A[i];
});

extern "C" JNIEXPORT jstring JNICALL
        Java_cn_qianzhengwei_libhc_MainActivity_Test(
                JNIEnv *env,
                jobject /* this */) {
    hetcompute::runtime::init();





    hetcompute::runtime::shutdown();
}
#endif

