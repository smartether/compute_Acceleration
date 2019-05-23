



#ifndef QML_EXTENSIONS_COMMON_H
#define QML_EXTENSIONS_COMMON_H

/**
 * FORMAT_IMAGE_MAJOR: The data for all channels for the first pixel are stored together, then the next pixel, and so on.
 * FORMAT_CHANNEL_MAJOR: The data for all pixels in the first channel are stored together, then all pixels from the next channel, and so on.
 **/
enum QML_IMAGE_FORMAT {FORMAT_IMAGE_MAJOR = 0, FORMAT_CHANNEL_MAJOR = 1};

#endif // QML_EXTENSIONS_COMMON_H
