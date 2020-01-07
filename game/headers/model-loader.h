#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <memory>
#include <string>

#include <model.h>

class ModelLoader {
public:
    virtual ~ModelLoader() {};
    virtual std::shared_ptr<Model> loadModel(const std::string& path) = 0;
};

#endif // MODEL_LOADER_H
