#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include "../ulti/support.h"
#include "../ulti/debug.h"
#include "../etw/event/file/file.h"

namespace manager {
	inline std::map<std::wstring, std::wstring> kDosPath;
    std::wstring GetDosPath(const std::wstring* wstr);

	class FileManager {
	public:
		void AddFile(const size_t file_object, const std::wstring& file_path);
		void RemoveFile(const size_t file_object);
		std::wstring GetFilePath(const size_t file_object) const;
	private:
		std::map<size_t, std::wstring> file_map_;
	};

	class FileIoManager {
		bool is_evaluated_;
		std::deque<etw::FileIoEventType> op_;
	};
}
#endif  // FILE_MANAGER_H_
