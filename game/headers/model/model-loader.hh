#ifndef MODEL_LOADER_HH
#define MODEL_LOADER_HH

#include "game/headers/model/model.hh"

#include <memory>
#include <string>

class ModelLoader {
public:
    virtual ~ModelLoader() {};
    virtual std::shared_ptr<Model> loadModel(const std::string& path) = 0;
};

#endif // MODEL_LOADER_HH
