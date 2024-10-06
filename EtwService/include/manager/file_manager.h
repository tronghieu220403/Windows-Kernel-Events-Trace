#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include "../ulti/support.h"

namespace manager {
	inline std::map<std::wstring, std::wstring> kDosPath;
    std::wstring GetDosPath(const std::wstring* wstr);

	class FileManager {
	public:
		void AddFile(const std::wstring& file_path);
	private:
		std::map<size_t, std::wstring> file_map_;
	};
}
#endif  // FILE_MANAGER_H_
