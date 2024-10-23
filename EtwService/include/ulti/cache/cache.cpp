#include "cache.h"

template<typename T>
Cache<T>::Cache() : limit_(0), hash_func_(nullptr)
{
}

// Constructor
template <typename T>
Cache<T>::Cache(size_t limit, HashFunc hash_func)
    : limit_(limit), hash_func_(hash_func) {}

// Đẩy một phần tử vào cache
template <typename T>
void Cache<T>::Push(const T& element) {
	size_t hash = 0;
	if (hash_func_ == nullptr) {
		throw std::runtime_error("Hash function is not set");
	}
    else
    {
        hash = hash_func_(element);
    }

    // Nếu phần tử đã nằm trong cache, di chuyển nó lên đầu
    if (map_.find(hash) != map_.end()) {
        lru_list_.erase(map_[hash]);
    }
    else if (lru_list_.size() >= limit_) {
        // Nếu cache đầy, xóa phần tử cuối cùng (least recently used)
        const T& last = lru_list_.back();
        size_t last_hash = hash_func_(last);
        lru_list_.pop_back();
        map_.erase(last_hash);
    }

    // Thêm phần tử mới vào đầu danh sách
    lru_list_.push_front(element);
    map_[hash] = lru_list_.begin();
}

// Kiểm tra phần tử có nằm trong cache không
template <typename T>
bool Cache<T>::Query(const T& element) const {
    size_t hash = hash_func_(element);
    return map_.find(hash) != map_.end();
}

// Explicit template instantiation nếu cần (bắt buộc khi làm việc với template)
template class Cache<int>;  // Ví dụ dành cho int
template class Cache<std::string>;  // Ví dụ dành cho std::string
template class Cache<std::pair<size_t, size_t>>;  // Ví dụ dành cho std::string
