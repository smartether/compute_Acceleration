#include <jni.h>

#include <cstdlib>
#include <cstring>
#include <jni.h>
#include <errno.h>
#include <cassert>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include "qml.h"
#include <android/sensor.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

using namespace std;

void blas_sgemm(const enum CBLAS_ORDER __Order, const enum CBLAS_TRANSPOSE __TransA,
        const enum CBLAS_TRANSPOSE __TransB, const int __M, const int __N, const int __K,
                const float __alpha, const float *__A, const int __lda, const float *__B,
                const int __ldb, const float __beta, float *__C, const int __ldc){
    for(int i=0;i<__M * __N; i++){

    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_cn_qianzhengwei_libhc_MainActivity_TestQML(
        JNIEnv *env,
        jobject /* this */) {

    if(QML_IS_SUPPORTED()) {
        qml_info info;
        QMLVersionInfo(&info);
        LOGI("qml type: %i", (int) info.type);


        const uint32_t matrixSize = 1024 * 1024;
        float *A = new float[matrixSize];
        float *B = new float[matrixSize];
        float *C = new float[matrixSize];
        for (uint32_t i = 0; i < matrixSize; i++) {
            A[i] = B[i] = C[i] = 1.0;
        }


        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 1024, 1024, 1024,
                    8.2, A, 1024, B, 1024, 2.6, C, 1024);
        // << "Value of C[0] is: " << C[0] << endl;
        char str[16];
        sprintf(str, "%f", C[0]);
        jstring jstr = env->NewStringUTF((const char *) &str);
        delete[] C;
        delete[] B;
        delete[] A;
        return jstr;
    }
    else{
        return nullptr;
    }
}



extern "C" JNIEXPORT void JNICALL
        Java_cn_qianzhengwei_libhc_MainActivity_TestNormal(
                JNIEnv *env,
                jobject /* this */) {
    const uint32_t matrixSize = 1024*1024;
    float *A = new float[matrixSize];
    float *B = new float[matrixSize];
    float *C = new float[matrixSize];
    for(uint32_t i=0; i < matrixSize; i++)
    {
        A[i] = B[i] = C[i] = 1.0;
    }

}