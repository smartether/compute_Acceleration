# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)
add_library(
        native-lib-qml
        SHARED
        native-lib-qml.cpp
)

add_library(hetCompute-1.0.0 SHARED IMPORTED)
add_library(fastcvadsp_stub SHARED IMPORTED)
add_library(fastcvopt SHARED IMPORTED)

## QML
add_library(qml SHARED IMPORTED)

set_property(TARGET hetCompute-1.0.0 PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libhetCompute-1.0.0.so")
set_property(TARGET fastcvadsp_stub PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libfastcvadsp_stub.so")
set_property(TARGET fastcvopt PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libfastcvopt.so")
set_property(TARGET qml PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libQML-1.1.0.so")

set(CMAKE_ANDROID_STL_TYPE c++_static)

# fastcv
target_include_directories(native-lib PRIVATE
            ${PROJECT_SOURCE_DIR}/include
            #E:/libs/android-sdk/ndk-bundle/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include
        )

# Qml
target_include_directories(native-lib-qml PRIVATE
            ${PROJECT_SOURCE_DIR}/include
        )


set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS} -DHETCOMPUTE_DISABLE_EXCEPTIONS
 -DHAVE_CONFIG_H=1
 -DHAVE_ANDROID_LOG_H=1
 -DHETCOMPUTE_HAVE_RTTI=1
 -DHETCOMPUTE_HAVE_OPENCL=1
 -DHETCOMPUTE_HAVE_GPU=1
 -DHETCOMPUTE_HAVE_GLES=1
 -DHETCOMPUTE_HAVE_QTI_DSP=1
 DHETCOMPUTE_THROW_ON_API_ASSERT=1
 -DHETCOMPUTE_LOG_FIRE_EVENT=1
")

# -DHETCOMPUTE_HAVE_OPENCL=1 -DHETCOMPUTE_HAVE_GPU=1 -DHETCOMPUTE_HAVE_GLES=1
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -fexceptions  -frtti -Ofast -Osize -DHETCOMPUTE_HAVE_QTI_DSP=1") # -fmerge-functions  -mfpu=neon  -std=gnu++11 -std=c++11  -stdlib=libstdc++

#if(ANDROID_ABI STREQUAL "arm64-v8a")
#    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate   -Wl,--allow-shlib-undefined -static-libstdc++")
#else()
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -Wl,--allow-shlib-undefined  -Wl,--hash-style=both -W1,-fuse-ld=bfd") # -u ANativeActivity_onCreate
#endif()


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib

        #fastcvadsp_stub
        fastcvopt
        #hetCompute-1.0.0

        android
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

target_link_libraries(
        native-lib-qml
        qml
        android
        ${log-lib}
)