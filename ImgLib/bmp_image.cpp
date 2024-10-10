#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    // поля заголовка Bitmap File Header
    char sign[2] = {'B', 'M'};
    uint32_t file_size;
    uint32_t reserved = 0;
    uint32_t file_offset_to_pixel_array = 54; // 14 + 40
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    uint32_t header_size = 40;
    int32_t width;
    int32_t height;
    uint16_t plains = 1;
    uint16_t bits_per_pixel = 24;
    uint32_t compression = 0;
    uint32_t image_size;
    int32_t horizontal_res = 11811; // pixels per meter
    int32_t vertical_res = 11811;
    int32_t used_colors = 0;
    int32_t important_colors = 0x1000000;
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image) {
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    file_header.file_size = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) +
        GetBMPStride(image.GetWidth()) * image.GetHeight();
    
    info_header.width = image.GetWidth();
    info_header.height = image.GetHeight();
    info_header.image_size = GetBMPStride(image.GetWidth()) * image.GetHeight();
    
    ofstream out(file, ios::binary);
    out.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    out.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));
    
    const int stride = GetBMPStride(image.GetWidth());
    vector<char> buff(stride);
    
    for (int y = image.GetHeight() - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);
        for (int x = 0; x < image.GetWidth(); ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(reinterpret_cast<const char*>(buff.data()), buff.size());
    }

    return out.good();
}

// напишите эту функцию
Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);
    int w, h;
    ifs.ignore(18); // пропускаем 18 байт полей файла до информации об изображении

    ifs.read(reinterpret_cast<char*>(&w), sizeof(w));
    ifs.read(reinterpret_cast<char*>(&h), sizeof(h));

    ifs.ignore(28);

    int stride = GetBMPStride(w);
    Image result(stride / 3, h, Color::Black());
    std::vector<char> buff(w * 3);

    for (int y = result.GetHeight() - 1; y >= 0; --y) {
        Color* line = result.GetLine(y);
        ifs.read(buff.data(), stride);

        for (int x = 0; x < w; ++x) {
            line[x].b = static_cast<byte>(buff[x * 3 + 0]);
            line[x].g = static_cast<byte>(buff[x * 3 + 1]);
            line[x].r = static_cast<byte>(buff[x * 3 + 2]);
        }
    }

    return result;
}

}  // namespace img_lib