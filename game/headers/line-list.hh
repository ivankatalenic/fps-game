#ifndef LINE_LIST_HH
#define LINE_LIST_HH

#include <string>

class LineList {
public:
	LineList() = default;
	~LineList() = default;

	void addLine(const std::string* line);
	
};

#endif // LINE_LIST_HH
