#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>

#include <model.h>

class ModelLoader {
public:
    virtual ~ModelLoader() {};
    virtual Model loadModel(const std::string& path) = 0;
};

#endif // MODEL_LOADER_H
