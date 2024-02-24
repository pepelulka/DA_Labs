#include <iostream>
#include <memory>
#include <algorithm>

// =================================== //
//               Utils                 //
// =================================== //

#pragma region Utils

// ****************************************************************************
// Vector
// ****************************************************************************

#pragma region VectorDeclaration

template <typename T>
class TVector {
public:

    TVector();
    TVector(size_t initSize);
    ~TVector();

    TVector(const TVector& other);
    TVector(TVector&& other);
    TVector& operator=(const TVector& other);
    TVector& operator=(TVector&& other);

    T* Data() const;
    size_t Size() const;
    size_t Capacity() const;
    void Resize(size_t newSize);
    void PushBack(const T& elem);

    T& operator[](size_t idx);
    const T& operator[](size_t idx) const;
protected:
    T* data;
    size_t capacity, size;

    static T* Allocate(size_t newCapacity);
    void ReallocateData(size_t newCapacity);
};

#pragma endregion VectorDeclaration

// ****************************************************************************
// String
// ****************************************************************************

#pragma region StringDeclaration

class TString : public TVector<char> {
public:
    using TVector<char>::TVector;

    TString(const char* cStr);
    TString operator=(const char* cStr);

};

TString operator+(const TString& lhs, const TString& rhs);

#pragma endregion StringDeclaration

// ****************************************************************************
// Implementations
// ****************************************************************************

#pragma region VectorImplementation

template <typename T>
T* TVector<T>::Allocate(size_t newCapacity) {
    try {
        return new T[newCapacity];
    } catch (std::bad_alloc) {
        std::cerr << "Not enough memory...\n";
        throw std::bad_alloc();
    }
}

template <typename T>
void TVector<T>::ReallocateData(size_t newCapacity) {
    T* temp = Allocate(newCapacity);
    std::copy(data, data + size, temp);
    capacity = newCapacity;
    delete[] data;
    data = temp;
}

template <typename T>
TVector<T>::TVector() : size(0), capacity(0), data(nullptr) { }

template <typename T>
TVector<T>::TVector(size_t initSize) : size(initSize), capacity(initSize) {
    data = Allocate(capacity);
}

template <typename T>
TVector<T>::~TVector() {
    if (data != nullptr) {
        delete[] data;
        data = nullptr;
    }
}

template <typename T>
TVector<T>::TVector(const TVector &other) : size(other.Size()), capacity(other.Capacity()) {
    data = Allocate(capacity);
    std::copy(other.Data(), other.Data() + size, data);
}

template <typename T>
TVector<T>::TVector(TVector &&other) : size(other.Size()), capacity(other.Capacity()), data(other.Data()) {
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

template <typename T>
TVector<T>& TVector<T>::operator=(const TVector &other) {
    size = other.Size();
    capacity = other.Capacity();
    data = Allocate(capacity);
    std::copy(other.Data(), other.Data() + size, data);
    return *this;
}

template <typename T>
TVector<T>& TVector<T>::operator=(TVector &&other) {
    size = other.Size();
    capacity = other.Capacity();
    data = other.Data();
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
    return *this;
}

template <typename T>
T *TVector<T>::Data() const {
    return data;
}

template <typename T>
size_t TVector<T>::Size() const
{
    return size;
}

template <typename T>
size_t TVector<T>::Capacity() const {
    return capacity;
}

template <typename T>
void TVector<T>::Resize(size_t newSize) {
    if (newSize <= capacity) {
        size = newSize;
    } else {
        size_t newCapacity = std::max(capacity, size_t(1));
        while (newCapacity < newSize) {
            newCapacity *= 4;
        }
        ReallocateData(newCapacity);
        size = newSize;
    }
}

template <typename T>
void TVector<T>::PushBack(const T &elem) {
    Resize(Size() + 1);
    (*this)[Size() - 1] = elem;
}

template <typename T>
T& TVector<T>::operator[](size_t idx) {
    if (idx >= Size()) {
        throw std::out_of_range("Vector index out of range");
    }
    return data[idx];
}

template <typename T>
const T &TVector<T>::operator[](size_t idx) const {
    if (idx >= Size()) {
        throw std::out_of_range("Vector index out of range");
    }
    return data[idx];
}

#pragma endregion VectorImplementation

#pragma region StringImplementation

TString::TString(const char* cStr) {
    *this = TString();
    char* ptr = const_cast<char*>(cStr);
    while (*ptr != '\0') {
        PushBack(*ptr);
        ptr++;
    }
}

TString TString::operator=(const char *cStr) {
    *this = std::move(TString(cStr));
    return *this;
}

TString operator+(const TString& lhs, const TString& rhs) {
    TString str(lhs.Size() + rhs.Size());
    std::copy(lhs.Data(), lhs.Data() + lhs.Size(), str.Data());
    std::copy(rhs.Data(), rhs.Data() + rhs.Size(), str.Data() + lhs.Size());
    return str;
}

#pragma endregion StringImplementation

#pragma endregion Utils

// =================================== //
//           Main program              //
// =================================== //

struct TPair {
    uint16_t key;
    uint64_t value;
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    //
    TVector<TPair> vec;
    TVector<size_t> cnt(UINT16_MAX + 1);

    // Init count array
    for (size_t i = 0; i < cnt.Size(); i++) {
        cnt[i] = 0;
    }

    size_t n = 0;
    bool flag = true;
    TPair p;
    // Array input
    while (std::cin >> p.key >> p.value) {
        n++;
        vec.PushBack(p);
        cnt[p.key]++;
    }

    // Prefix array from cnt array
    for (size_t i = 1; i < cnt.Size(); i++) {
        cnt[i] = cnt[i - 1] + cnt[i];
    }

    // Finally, the algorithm
    TVector<TPair> result(n);
    size_t ptr = 0;
    for (int i = static_cast<int>(vec.Size()) - 1; i >= 0; i--) {
        uint16_t key = vec[i].key;
        result[cnt[key] - 1] = vec[i];
        cnt[key]--;
    }

    // Output
    for (size_t i = 0; i < result.Size(); i++) {
        std::cout << result[i].key << "\t" << result[i].value << '\n';
    }
}
