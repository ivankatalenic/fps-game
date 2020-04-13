#ifndef DRAWABLE_HH
#define DRAWABLE_HH

class Drawable {
public:
	virtual ~Drawable() {};
	virtual void draw() const = 0;
};

#endif // DRAWABLE_HH
