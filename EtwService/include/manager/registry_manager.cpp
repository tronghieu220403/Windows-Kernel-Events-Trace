#include "registry_manager.h"

namespace manager {

	void RegistryManager::AddRegistryName(const size_t registry_object, const std::wstring& registry_path) {
		registry_map_[registry_object] = registry_path;
	}

	void RegistryManager::RemoveRegistryName(const size_t registry_object) {
		auto it = registry_map_.find(registry_object);
		if (it != registry_map_.end()) {
			registry_map_.erase(it);
		}
	}

	std::wstring RegistryManager::GetRegistryPath(const size_t registry_object) const {
		auto it = registry_map_.find(registry_object);
		if (it != registry_map_.end()) {
			return it->second;
		}
		else {
			return L"";
		}
	}
}
