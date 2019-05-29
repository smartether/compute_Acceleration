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
#include <random>
#include <cstdint>
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

//#define _BIT32_INT_
#define _BIT16_INT_


#ifdef _BIT32_INT_
#define _OFFSET_A_ 4
#define _SIZE_A_ 4
typedef int32_t INPUT_T;
#elif defined(_BIT16_INT_)
#define _SIZE_A_ 2
#define _OFFSET_A_ 2
typedef int16_t INPUT_T;
#endif

extern "C" JNIEXPORT void JNICALL OddqToCubeMatrix(int width, int height,INPUT_T* srcX_arg, INPUT_T* srcY_arg, INPUT_T* dst){
    fcvMemInit();
    fcvMemInitPreAlloc(4 * 1024 * 1024);
    fcvSetOperationMode(fcvOperationMode::FASTCV_OP_LOW_POWER);

    int blockSize = width * height;
    INPUT_T xAndOne[blockSize];
    std::memset((void*)xAndOne, 0, blockSize * _SIZE_A_);

    //_OFFSET_A_ -
    for(int i=0;i<blockSize;i++){
         xAndOne[i] = 1;
    }

    LOGI("$$$$$$$$  step1 ...");
    //fcvSetElementsc4u8((uint8_t*)&xAndOne, width, height, 0, 0, 0, 0, 1, nullptr, 0);

    INPUT_T* inputX32 = srcX_arg;
    INPUT_T* inputY32 = srcY_arg;
    uint8_t*  inputX = (uint8_t*)inputX32;
    //uint8_t* inputOne = nullptr;
    uint8_t* xAnd32One = (uint8_t*)&xAndOne;
    int16_t* outputXAndOne = (int16_t*)fcvMemAlloc(2 * blockSize, 16);



    LOGI("$$$$$$$$  step2 ...");

    fcvBitwiseAndu8(inputX, width * 2, height * 2, 0, xAnd32One, 0, (uint8_t*)outputXAndOne, 0);
    fcvMultiplyScalars16(inputX32, width, height, 0, -1, 0, inputX32, 0);

    LOGI("$$$$$$$$  step3 ...");

    fcvMultiplyScalars16(inputY32, width, height, 0, 2, 0, inputY32, 0);
    fcvAdds16(inputY32, width, height, 0, inputX32, 0, inputX32, 0);

    LOGI("$$$$$$$$  step4 ...");

    fcvAdds16(inputX32, width, height, 0, outputXAndOne, 0, inputX32, 0);
    //fcvMultiplyScalars16(inputX32, width, height, 0, 1, -1, dst, 0);
    fcvMultiplyScalars16(inputX32, width, height, 0, 1, 1, dst, 0);
    LOGI("$$$$$$$$  step5 ...");



    fcvMemFree(outputXAndOne);

    fcvMemDeInit();
    fcvCleanUp();
}

extern "C" JNIEXPORT void JNICALL Java_cn_qianzhengwei_libhc_MainActivity_OddqToCubeMatrix(JNIEnv* env, jobject obj){
    int width = 3;
    int height = 5000;
    int blockSize = width * height;
    int16_t srcX[blockSize];
    int16_t srcY[blockSize];
    int16_t dst[blockSize];

    for(int i=0;i<blockSize;i++){
        srcX[i] = 1;
        srcY[i] = 2;
        dst[i] = 0;
    }

    OddqToCubeMatrix(width, height, (int16_t*)&srcX, (int16_t*)&srcY, (int16_t*)&dst);
    LOGI("$$$ dst: %i", dst[0]);
   // for(int i=0;i< width;i++){
   //     LOGI("$$$ dst[%i]: %i", i, dst[i]);
   //}
}


#define _NEIGHBOR_MATRIX_STRIDE_

extern "C" JNIEXPORT void JNICALL NeighborMatrix(int width, int height,
        int32_t* srcX_arg, int32_t* srcY_arg, int32_t* srcZ_arg,
        int16_t* dst){
    fcvMemInit();
    fcvMemInitPreAlloc(4 * 1024 * 1024);
    fcvSetOperationMode(fcvOperationMode::FASTCV_OP_LOW_POWER);

    // 1000 * 3
    int32_t* srcX = srcX_arg;
    int32_t* srcY = srcY_arg;
    int32_t* srcZ = srcZ_arg;
    //fcvAbsDiffVs32(src, 0, width, height, 0, dst, 0);
    fcvAbsDiffVs32(srcX, 0,width, height, 0, srcX, 0);
    fcvAbsDiffVs32(srcY, 0,width, height, 0, srcY, 0);
    fcvAbsDiffVs32(srcZ, 0,width, height, 0, srcZ, 0);

#ifndef _NEIGHBOR_MATRIX_STRIDE_
    // type cast
    int16_t* i16X = (int16_t*)fcvMemAlloc(2 * width * height, 16);
    int16_t* i16Y = (int16_t*)fcvMemAlloc(2 * width * height, 16);
    int16_t* i16Z = (int16_t*)fcvMemAlloc(2 * width * height, 16);

    for(int i=0;i< width * height;i++){
        i16X[i] = (int16_t)srcX[i];
        i16Y[i] = (int16_t)srcY[i];
        i16Z[i] = (int16_t)srcZ[i];

    }


    fcvAdds16(i16X, width, height, 0, i16Y, 0, i16Y, 0);
    fcvAdds16(i16Y, width, height, 0, i16Z, 0, dst, 0);
#else
    // avoid tpye cast
    fcvAdds16((int16_t*)srcX, 1, width * height, 4, (int16_t*)srcY, 4, (int16_t*)srcY, 4);
    fcvAdds16((int16_t*)srcY, 1, width *height, 4, (int16_t*)srcZ, 4, dst, 0);


#endif

    fcvMemDeInit();
    fcvCleanUp();
}


extern "C" JNIEXPORT void JNICALL Java_cn_qianzhengwei_libhc_MainActivity_NeighborMatrix(JNIEnv* env, jobject obj){
    int width = 3;
    int height = 10000;
    int blockSize = width * height;
    int32_t srcX[blockSize];
    int32_t srcY[blockSize];
    int32_t srcZ[blockSize];
    int16_t dst[blockSize];

    for(int i=0;i<blockSize;i++){
        srcX[i] = 1;
        srcY[i] = 1;
        srcZ[i] = 1;
        dst[i] = 0;
    }

    NeighborMatrix(width, height, (int32_t*)&srcX, (int32_t*)&srcY, (int32_t*)&srcZ, (int16_t*)&dst);

    for(int i=0; i<blockSize; i++){
        LOGI("$$ dst[%i]: %i", i, dst[i]);
    }
}

extern "C" JNIEXPORT void JNICALL FarastHexogon(){
    int width = 3;
    int height = 5000;
    int32_t* srcDistance = nullptr;
    int32_t maxNum;
    int32_t miniMum;
    uint32_t maxX;
    uint32_t maxY;
    uint32_t  miniX;
    uint32_t miniY;
    fcvMinMaxLocs32(srcDistance, width, height, 0, &maxNum, &miniMum, &maxX, &maxY, &miniX, &miniY);

}


//miniMax
//fcvMinMaxLocf32

extern "C" JNIEXPORT void JNICALL RoleCul(int width, int height,
        float32_t* A_arg, float32_t* B_arg, float32_t* C_arg,
        float32_t* startPos_arg, float32_t* dstPos_arg,
        float32_t* time_arg, float32_t* lOrC_arg, float32_t* dst){
    fcvMemInit();
    fcvMemInitPreAlloc(4 * 1024 * 1024);
    fcvSetOperationMode(fcvOperationMode::FASTCV_OP_LOW_POWER);

    float32_t* lOrC = lOrC_arg;
    // start Pos
    float32_t* startPos = startPos_arg;
    // dst pos
    float32_t* dstPos = dstPos_arg;
    // time
    float32_t* time = time_arg;
    // 1 - startPos
    float32_t* invStartPos = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    // dst pos - start Pos
    float32_t* delta = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    // (d - s)
    float32_t* deltaMulTime = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    // num -1
    float32_t* invOne = (float32_t*) fcvMemAlloc(4 * width * height, 16);

    float32_t* tmpDst =  (float32_t*) fcvMemAlloc(4 * width * height, 16);
    float32_t* tmp = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    float32_t* tmp1 = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    float32_t* tmp2 = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    float32_t* tmpRes = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    float32_t* tPow2 = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    float32_t* invTime = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    float32_t* oneSubT = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    float32_t* oneSubTPow2 = (float32_t*)fcvMemAlloc(4 * width * height, 16);
    float32_t* timePow2 = (float32_t*)fcvMemAlloc(4 * width * height, 16);



    // -s
//    fcvElementMultiplyf32(startPos, width, height, 0, invOne, 0, invStartPos, 0);
    fcvMultiplyScalarf32(startPos, width, height, 0, -1, invStartPos, 0);

    // (d - s)
    fcvAddf32(dstPos, width, height, 0, invStartPos, 0, delta, 0);
    // (d - s)t
    fcvElementMultiplyf32(delta, width, height, 0, time, 0, deltaMulTime, 0);
    //s + (d - s)t
    fcvAddf32(startPos, width, height, 0, deltaMulTime, 0, tmpDst, 0);
    fcvElementMultiplyf32(tmpDst, width, height, 0, lOrC, 0, tmpDst, 0);

    float32_t* A = A_arg;
    float32_t* B = B_arg;
    float32_t* C = C_arg;



    fcvMultiplyScalarf32(time, width, height, 0, -1, invTime, 0);
    fcvAddScalarf32(invTime, width, height, 0, 1, oneSubT, 0);
    fcvElementMultiplyf32 (time, width, height, 0, time, 0, timePow2, 0);
    fcvElementMultiplyf32 (oneSubT, width, height, 0, oneSubT, 0, timePow2, 0);

    fcvElementMultiplyf32(A, width, height, 0, timePow2, 0, tmp, 0);
    fcvElementMultiplyf32(B, width, height, 0, oneSubT, 0, tmp1, 0);
    //TODO tmp1 input tmp1 output
    fcvElementMultiplyf32(tmp1, width, height, 0, time, 0, tmp1, 0);
    fcvElementMultiplyf32(C, width, height, 0, oneSubTPow2, 0, tmp2, 0);
    fcvAddf32(tmp, width, height, 0, tmp1, 0, tmpRes, 0);
    fcvAddf32(tmp2, width, height, 0, tmpRes, 0, tmpRes, 0);

    fcvMultiplyScalarf32(lOrC, width, height, 0, -1, lOrC, 0);
    fcvAddScalarf32(lOrC, width, height, 0, 1, lOrC, 0);
    fcvElementMultiplyf32(tmpRes, width, height, 0, lOrC, 0, tmpRes, 0);

    fcvAddf32(tmpDst, width, height, 0, tmpRes, 0, dst, 0);


    //dealloc
    fcvMemFree(invStartPos);
    fcvMemFree(delta);
    fcvMemFree(deltaMulTime);
    fcvMemFree(invOne);
    fcvMemFree(tmpDst);
    fcvMemFree(tmp);
    fcvMemFree(tmp2);
    fcvMemFree(tmpRes);
    fcvMemFree(tPow2);
    fcvMemFree(invTime);
    fcvMemFree(oneSubT);
    fcvMemFree(oneSubTPow2);
    fcvMemFree(timePow2);

    fcvMemDeInit();
    fcvCleanUp();
}


extern "C" JNIEXPORT void JNICALL Java_cn_qianzhengwei_libhc_MainActivity_RoleCul(JNIEnv* env, jobject obj){

    int width = 3;
    int height = 10000;
    int blockSize = height * width;

    float32_t A[blockSize];
    float32_t B[blockSize];
    float32_t C[blockSize];
    float32_t startPos[blockSize];
    float32_t dstPos[blockSize];
    float32_t time[blockSize];
    float32_t lOrC[blockSize];
    float32_t dst[blockSize];

    // c++ 11 random library
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float32_t> dist(1, 300);
    std::uniform_real_distribution<float32_t> dist_0_1(0, 1);

    for(int i=0;i< blockSize; i++){

        A[i] = dist(mt);
        B[i] = dist(mt);
        C[i] = dist(mt);
        startPos[i] = dist(mt);
        dstPos[i] = dist(mt);
        time[i] = dist_0_1(mt);
        lOrC[i] = dist_0_1(mt);

/*
        A[i] = 0;
        B[i] = 0;
        C[i] = 0;
        startPos[i] = 10;
        dstPos[i] = 20;
        time[i] =  (i) / float(blockSize);
        lOrC[i] = 1;
*/
    }


    LOGI("$$$ start ...");
    RoleCul(height, width, (float32_t*)&A, (float32_t*)&B, (float32_t*)&C, (float32_t*)&startPos,
            (float32_t*)&dstPos,(float32_t*)&time, (float32_t*)&lOrC,(float32_t*)&dst);
    LOGI("$$$ end ...");


    for(int i=0;i<blockSize;i++){
        LOGI("$$$ dst[%i]: %f", i, dst[i]);
    }

}



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

