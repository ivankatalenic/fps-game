#ifndef SERVICE_LOCATOR_H
#define SERVICE_LOCATOR_H

#include <memory>

#include <model-loader.h>
#include <model-renderer.h>

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

#endif // SERVICE_LOCATOR_H
