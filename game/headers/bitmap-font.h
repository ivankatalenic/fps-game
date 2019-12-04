#ifndef BITMAP_FONT_H
#define BITMAP_FONT_H

#include <filesystem>

class BitmapFont {
public:
	BitmapFont(std::filesystem::path bitmap_path);
	~BitmapFont();
private:
	int _texture_it{0};
};

#endif // BITMAP_FONT_H
