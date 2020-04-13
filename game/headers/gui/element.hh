#ifndef ELEMENT_HH
#define ELEMENT_HH

class Element {
public:
	virtual void draw() const = 0;
	virtual ~Element() {};
};

#endif // ELEMENT_HH
