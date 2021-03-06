#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>


Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, sizeof(Pixel))) != nullptr)
  {
    size = width * height * channels;
  }
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height ]{};

  if(data != nullptr)
  {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
  }
}

int Image::Save(const std::string &a_path)
{
  auto extPos = a_path.find_last_of('.');
  if(a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
  }
  else if(a_path.substr(extPos, std::string::npos) == ".jpg" || a_path.substr(extPos, std::string::npos) == ".JPG" ||
          a_path.substr(extPos, std::string::npos) == ".jpeg" || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  }
  else
  {
    std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path << "\n";
    return 1;
  }

  return 0;
}

Image::~Image()
{
  if(self_allocated)
    delete [] data;
  else
  {
    stbi_image_free(data);
  }
}

Pixel blend(Pixel oldPixel, Pixel newPixel)
{
	newPixel.r = newPixel.a / 255.0 * (newPixel.r - oldPixel.r) + oldPixel.r;
	newPixel.g = newPixel.a / 255.0 * (newPixel.g - oldPixel.g) + oldPixel.g;
	newPixel.b = newPixel.a / 255.0 * (newPixel.b - oldPixel.b) + oldPixel.b;
	newPixel.a = 255;

	return newPixel;
}

Pixel dark(Pixel oldPixel, int koef)
{
	Pixel newPixel;
	newPixel.r = oldPixel.r - oldPixel.r / (darkkoef - koef);
	newPixel.g = oldPixel.g - oldPixel.g / (darkkoef - koef);
	newPixel.b = oldPixel.b - oldPixel.b / (darkkoef - koef);
	newPixel.a = 255;

	return newPixel;
}

Pixel undark(Pixel srcPixel, Pixel oldPixel)
{
	Pixel newPixel;
	newPixel.r = oldPixel.r + srcPixel.r / (darkkoef);
	newPixel.g = oldPixel.g + srcPixel.g / (darkkoef);
	newPixel.b = oldPixel.b + srcPixel.b / (darkkoef);
	newPixel.a = 255;

	return newPixel;
}
