#ifndef CACHE_H_
#define CACHE_H_

#include "../support.h"

// Template class Cache
template <typename T>
class Cache {
public:
	using HashFunc = std::function<size_t(const T&)>;

	Cache();  // Constructor mặc định

	// Constructor nhận vào limit và hàm hash
	Cache(size_t limit, HashFunc hash_func);

	// Đẩy một phần tử vào cache
	void Push(const T& element);

	// Kiểm tra phần tử có nằm trong cache không
	bool Query(const T& element) const;

	// Đặt giới hạn số lượng phần tử
	void SetLimit(const size_t& limit) { limit_ = limit; }

	// Đặt hàm hash
	void SetHashFunc(const HashFunc& hash_func) { hash_func_ = hash_func; }
	
	// Xóa tất cả các phần tử trong cache
	void Clear() {
		lru_list_.clear();
		map_.clear();
	}

private:
	size_t limit_ = 0;  // Giới hạn số lượng phần tử
	HashFunc hash_func_ = nullptr;  // Hàm hash
	std::list<T> lru_list_;  // Danh sách các phần tử theo thứ tự truy cập gần nhất
	std::unordered_map<size_t, typename std::list<T>::iterator> map_;  // Bản đồ hash -> vị trí trong danh sách

	// Cấm sao chép và gán
	Cache(const Cache&) = delete;
	Cache& operator=(const Cache&) = delete;
};

#endif  // CACHE_H_
