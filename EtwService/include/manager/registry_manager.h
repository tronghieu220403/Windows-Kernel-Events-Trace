#ifndef REGISTRY_MANAGER_H_
#define REGISTRY_MANAGER_H_

#include "../ulti/support.h"
#include "../ulti/debug.h"

namespace manager {
	class RegistryManager {
	public:
		void AddRegistryName(const size_t registry_object, const std::wstring& registry_path);
		void RemoveRegistryName(const size_t registry_object);
		std::wstring GetRegistryPath(const size_t registry_object) const;
	private:
		std::map<size_t, std::wstring> registry_map_;
	};
}
#endif  // REGISTRY_MANAGER_H_
