#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include "../ulti/support.h"
#include "../ulti/debug.h"
#include "../etw/event/file/file.h"

#define CREATE_FLAG					0x00000001
#define DELETE_FLAG					0x00000002
#define RENAME_FLAG					0x00000004
#define INFO_SET_FLAG				0x00000008
#define WRITE_FLAG					0x00000010
#define READ_FLAG					0x00000020

#define READ_WRITE_FLAG				(READ_FLAG | WRITE_FLAG)
#define OVERWRITE_FLAG				READ_WRITE_FLAG
#define READ_DELETE_FLAG			(READ_FLAG | DELETE_FLAG)
#define CREATE_WRITE_FLAG			(CREATE_FLAG | WRITE_FLAG)
#define OVERWRITE_RENAME			(READ_WRITE_FLAG | RENAME_FLAG)
#define OVERWRITE_RENAME_FLAG		OVERWRITE_RENAME

#define EVALUATATION_INTERVAL_MS 5000
#define EVALUATATION_INTERVAL_SEC (EVALUATATION_INTERVAL_MS / 1000)

#define MIN_TOTAL_SIZE_CHECK_PER_SEC (20 * 1024 * 1024)// 20MB
#define MIN_TOTAL_SIZE_CHECK (MIN_TOTAL_SIZE_CHECK_PER_SEC * EVALUATATION_INTERVAL_SEC)

#define MIN_FILE_COUNT_PER_SEC 2
#define MIN_FILE_COUNT (MIN_FILE_COUNT_PER_SEC * EVALUATATION_INTERVAL_SEC)
#define MAX_FILE_COUNT (MIN_FILE_COUNT * 3)

#define MIN_DIR_COUNT 2

#define FILE_MAX_SIZE_SCAN (16 * 1024) // 16KB

#define THRESHOLD_PERCENTAGE 80
#define BelowThreshold(part, total) (part <= total * THRESHOLD_PERCENTAGE / 100)

namespace manager {

	class FileNameObjMap {
	public:
		void MapObjectWithPath(const size_t file_object, const std::wstring& file_path);
		void RemoveObject(const size_t file_object);
		const std::wstring& GetPathByObject(const size_t file_object);
	private:
		std::unordered_map<size_t, std::wstring> obj_to_name_map_;
	};

	struct FileIoInfo {

		size_t featured_access_flags = 0;
		std::wstring file_path;
		size_t start_time_ms = 0;
		size_t pid = 0;

		struct WriteInfo {
			size_t size = 0;
		} write_info;
		struct RenameInfo {
			std::wstring file_path_old;
		} rename_info;
	};

	class FileIoManager {
	private:
		std::mutex file_io_mutex_;
		std::deque<FileIoInfo> file_io_queue_;
	public:
		void LockMutex();
		void UnlockMutex();

		FileIoInfo PopFileIoEvent();
		size_t GetQueueSize();

		void PushRenameFileEventToQueue(const std::wstring& file_path_new, size_t pid, size_t start_time_ms, const std::wstring& file_path_old);

		void PushWriteFileEventToQueue(const std::wstring& file_path, size_t pid, size_t start_time_ms, size_t io_size);
	};

	/*___________________________________________*/

	inline std::unordered_map<std::wstring, const std::wstring> kNativePath;
	inline std::unordered_map<std::wstring, const std::wstring> kWin32Path;

	/*_________________FUNCTIONS_________________*/

	// DOS path getter function
	std::wstring GetNativePath(const std::wstring& win32_path);

	// Win32 path getter function
	std::wstring GetWin32PathCaseSensitive(const std::wstring& path);
	std::wstring GetWin32Path(const std::wstring& path);

	bool FileExist(const std::wstring& file_path);

	size_t GetFileSize(const std::wstring& file_path);

	std::wstring GetFileExtension(const std::wstring& file_name);

	bool IsExecutableFile(const std::wstring& file_path);

	std::wstring CopyToTmp(const std::wstring& path, size_t size = FILE_MAX_SIZE_SCAN);

	void ClearTmpFiles();

	std::vector<std::pair<std::wstring, bool>> CheckTrID(const std::vector<std::wstring>& file_list);

	// Parse trid output and return test results for each file
	std::vector<std::pair<std::wstring, bool>> AnalyzeTridOutput(const std::wstring& output);

	// Hàm kiểm tra file có chứa ký tự in được không
	bool IsPrintableFile(const std::wstring& file_path, std::streamsize max_size = FILE_MAX_SIZE_SCAN);

}
#endif  // FILE_MANAGER_H_
