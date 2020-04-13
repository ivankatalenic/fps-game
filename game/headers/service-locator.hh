#ifndef SERVICE_LOCATOR_HH
#define SERVICE_LOCATOR_HH

#include "game/headers/model/model-loader.hh"
#include "game/headers/renderer/model-renderer.hh"

#include <memory>

class ServiceLocator {
public:
	static ServiceLocator& getInstance();

	std::unique_ptr<ModelLoader> getModelLoader() const;
	std::unique_ptr<ModelRenderer> getModelRenderer() const;
	double getCurrentTime() const;
private:
	ServiceLocator() = default;
	~ServiceLocator() = default; // Do not create a custom destructor!

	ServiceLocator(const ServiceLocator&) = default;
	ServiceLocator& operator=(const ServiceLocator&) = default;

	ServiceLocator(ServiceLocator&&) = default;
	ServiceLocator& operator=(ServiceLocator&&) = default;
};

#endif // SERVICE_LOCATOR_HH
