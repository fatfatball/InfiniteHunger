#pragma once
#include <memory>
#include <stdexcept>
#include <utility>
#include <initializer_list>

template<typename T, typename Allocator = std::allocator<T>>
class List { // build a vector<> myself, still working
public:
	List() {
		size = 0;
		capacity = 16;
		data = alloc.allocate(capacity);
	}
#pragma region Construct&Deconstruct
	explicit List(int count) {
		if (count < 0) {
			throw std::invalid_argument("Size cannot be negative");
		}
		Reserve(count);
		for (int i = 0; i < count; ++i) {
			std::construct_at(data + i);
		}
		size = count;
	}
	
	List(int count, const T& value, int cap = -1) {
		if (count < 0) {
			throw std::invalid_argument("Size cannot be negative");
		}
		if (cap < count) {
			cap = count;
		}
		Reserve(cap);
		for (int i = 0; i < count; ++i) {
			std::construct_at(data + i, value);
		}
		size = count;
	}

	List(const List& other) requires std::is_copy_constructible_v<T> {
		Reserve(other.size);
		for (int i = 0; i < other.size; ++i) {
			std::construct_at(data + i, other[i]);
		}
		size = other.size;
	}

	List(List&& other) noexcept {
		data = other.data;
		size = other.size;
		capacity = other.capacity;
		other.data = nullptr;
		other.size = 0;
		other.capacity = 0;
	}

	List(std::initializer_list<T> initList) {
		int tmpSize = static_cast<int>(initList.size());
		Reserve(tmpSize);
		int i = 0;
		for (const T& element : initList) {
			std::construct_at(data + i, element);
			++i;
		}
		size = tmpSize;
	}

	~List() {
		Clear();
		if (data != nullptr) {
			alloc.deallocate(data, capacity);
		}
	}

	List& operator=(const List& other) requires std::is_copy_constructible_v<T> {
		if (this != &other) {
			List tmp(other);
			Swap(tmp);
		}
		return *this;
	}

	List& operator=(List&& other) {
		if (this != &other) {
			Clear();
			if (data != nullptr) {
				alloc.deallocate(data, capacity);
			}
			data = other.data;
			size = other.size;
			capacity = other.capacity;
			other.data = nullptr;
			other.size = 0;
			other.capacity = 0;
		}
		return *this;
	}
#pragma endregion

#pragma region Getter
	int GetSize() const { return size; }
	int GetCapacity() const { return capacity; }
	bool IsEmpty() const { return size == 0; }

	T& operator[](int index) {
		CheckIndex(index);
		return data[index];
	}

	const T& operator[](int index) const{
		CheckIndex(index);
		return data[index];
	}

	T& GetFront() {
		if (size == 0) {
			throw std::out_of_range("Out of range");
		}
		return data[0];
	}

	const T& GetFront() const {
		if (size == 0) {
			throw std::out_of_range("Out of range");
		}
		return data[0];
	}

	T& GetBack() {
		if (size == 0) {
			throw std::out_of_range("Out of range");
		}
		return data[size - 1];
	}

	const T& GetBack() const {
		if (size == 0) {
			throw std::out_of_range("Out of range");
		}
		return data[size - 1];
	}
#pragma endregion

#pragma region Modifier
	void Swap(List& other) noexcept {
		std::swap(data, other.data);
		std::swap(size, other.size);
		std::swap(capacity, other.capacity);
	}

	// change capacity, keep size
	void Reserve(int newCapacity) {
		if (newCapacity < 0) {
			throw std::invalid_argument("Capacity cannot be negative");
		}
		if (newCapacity <= capacity) {
			return;
		}

		T* newData = alloc.allocate(newCapacity);
		for (int i = 0; i < size; ++i) {
			std::construct_at(newData + i, std::move(data[i]));
			std::destroy_at(data + i);
		}
		
		if (data != nullptr) {
			alloc.deallocate(data, capacity);
		}
		data = newData;
		capacity = newCapacity;
	}

	void Resize(int newSize) {
		if (newSize < 0) {
			throw std::invalid_argument("Size cannot be negative");
		}
		if (newSize > size) {
			Reserve(newSize);
			for (int i = size; i < newSize; ++i) {
				std::construct_at(data + i);
			}
			size = newSize;
		}
		else {
			while (size > newSize) {
				--size;
				std::destroy_at(data + size);
			}
		}
	}

	void PushBack(const T& element) requires std::is_copy_constructible_v<T> {
		if (size == capacity) {
			T tmp = T(element);
			GrowIfNeeded(size + 1);
			std::construct_at(data + size, tmp);
		}
		else{
			std::construct_at(data + size, element);
		}
		++size;
	}

	void PushBack(T&& element) {
		if (size == capacity) {
			T tmp(std::move(element));
			GrowIfNeeded(size + 1);
			std::construct_at(data + size, std::move(tmp));
		}
		else {
			std::construct_at(data + size, std::move(element));
		}
		++size;
	}

	void PopBack() {
		if (size == 0) return;
		--size;
		std::destroy_at(data + size);
	}
	
	bool Erase(int index) {
		if (index < 0 || index >= size) {
			return false;
		}
		for (int i = index; i < size - 1; ++i) {
			data[i] = std::move(data[i + 1]);
		}
		--size;
		std::destroy_at(data + size);
		return true;
	}

	bool Insert(const T& element, int index) requires std::is_copy_constructible_v<T> {
		if (index < 0 || index > size) {
			return false;
		}
		if (index == size) {
			PushBack(element);
			return true;
		}

		T tmp(element);
		if (size == capacity) {
			GrowIfNeeded(size + 1);
		}

		std::construct_at(data + size, std::move(data[size - 1]));
		for (int i = size - 1; i > index; --i) {
			data[i] = std::move(data[i - 1]);
		}
		data[index] = std::move(tmp);

		++size;
		return true;
	}

	bool Insert(T&& element, int index) {
		if (index < 0 || index > size) {
			return false;
		}
		if (index == size) {
			PushBack(std::move(element));
			return true;
		}

		T tmp(std::move(element));
		if (size == capacity) {
			GrowIfNeeded(size + 1);
		}

		std::construct_at(data + size, std::move(data[size - 1]));
		for (int i = size - 1; i > index; --i) {
			data[i] = std::move(data[i - 1]);
		}
		data[index] = std::move(tmp);

		++size;
		return true;
	}

	void Clear() {
		for (int i = 0; i < size; ++i) {
			std::destroy_at(data + i);
		}
		size = 0;
	}
#pragma endregion

private:
	T* data = nullptr;
	int size = 0; 
	int capacity = 0;
	Allocator alloc{};

	void CheckIndex(int index) const {
		if (index < 0 || index >= size)
			throw std::out_of_range("Index out of range");
	}
	void GrowIfNeeded(int minCapacity) {
		if (minCapacity <= capacity) {
			return;
		}
		int newCapacity = (capacity == 0) ? 16 : capacity * 2;
		if (newCapacity < minCapacity) {
			newCapacity = minCapacity;
		}
		Reserve(newCapacity);
	}
};