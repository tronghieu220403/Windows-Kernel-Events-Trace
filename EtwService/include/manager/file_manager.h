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

#define MAX_TOTAL_SIZE_CHECK (150 * 1024 * 1024) // 150MB
#define FILE_MAX_SIZE_CHECK (512 * 1024) // 512KB

namespace manager {

	inline std::unordered_map<std::wstring, const std::wstring> kNativePath;
	inline std::unordered_map<std::wstring, const std::wstring> kWin32Path;

	/*_________________FUNCTIONS_________________*/

	// Hàm lấy đường dẫn DOS
	std::wstring GetNativePath(const std::wstring& win32_path);

	// Hàm lấy đường dẫn Win32
	std::wstring GetWin32Path(const std::wstring& path);

	// Hàm lấy kích thước file
	size_t GetFileSize(const std::wstring& file_path);
	
	// Hàm lấy đuôi file
	std::wstring GetFileExtension(const std::wstring& file_name);
	
	// Hàm kiểm tra file có phải là file thực thi không
	bool IsExecutableFile(const std::wstring& file_path);
	
	// Hàm copy file sang thư mục tạm
	std::wstring CopyToTmp(const std::wstring& path, const size_t& size = FILE_MAX_SIZE_CHECK);
	
	// Hàm xóa các file tạm
	void ClearTempFiles();

	// Hàm kiểm tra danh sách file
	std::vector<std::pair<std::wstring, bool>> CheckFileList(const std::vector<std::wstring>& file_list);

	// Hàm phân tích output của trid và trả về kết quả kiểm tra từng file
	std::vector<std::pair<std::wstring, bool>> AnalyzeTridOutput(const std::wstring& output);

	// Hàm kiểm tra file có chứa ký tự in được không
	bool IsPrintableFile(const std::wstring& file_path, std::streamsize max_size = FILE_MAX_SIZE_CHECK);

	/*___________________________________________*/

	class FileManager {
	public:
		void AddFileObject(const size_t file_object, const std::wstring& file_path);
		void RemoveFileObject(const size_t file_object);
		const std::wstring& GetFilePath(const size_t file_object);
	private:
		std::unordered_map<size_t, std::wstring> obj_to_name_map_;
	};

	struct FileIoManager {
		size_t featured_access_flags = 0;
		std::wstring current_path;
		std::wstring old_path;
		bool evaluation_needed = false; // TODO: Đổi thành true nếu có thay đổi file
		bool is_recognized = false; // File nhận diện được (không phải bị mã hóa)
	};
}
#endif  // FILE_MANAGER_H_
