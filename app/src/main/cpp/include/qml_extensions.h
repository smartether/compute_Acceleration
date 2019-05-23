
#ifndef QML_EXTENSIONS_H
#define QML_EXTENSIONS_H

#include <qml_types.h>
#include <qml_extensions_common.h>

#ifdef __cplusplus
extern "C" {
#endif


void sconv_mm(const bool biased, const float* src, const qml_int srcX, const qml_int srcY,
              const qml_int channels, const float* filters, const qml_int numFilters,
              const qml_int filterX, const qml_int filterY, const qml_int padX, const qml_int padY,
              const qml_int strideX, const qml_int strideY, float* output, const qml_int outputX, const qml_int outputY);

void i32i8conv(const QML_IMAGE_FORMAT inputFormat, const QML_IMAGE_FORMAT outputFormat, const bool biased, const qml_int8* src,
               const qml_int srcX, const qml_int srcY, const qml_int srcNumChannels,
               const qml_int8* filters, const qml_int numFilters, const qml_int filterX, const qml_int filterY, 
               const qml_int padLeft, const qml_int padTop, const qml_int strideX, const qml_int strideY, 
               qml_int* output, const qml_int outputX, const qml_int outputY);


#ifdef __cplusplus
}
#endif

#endif // QML_EXTENSIONS_H

