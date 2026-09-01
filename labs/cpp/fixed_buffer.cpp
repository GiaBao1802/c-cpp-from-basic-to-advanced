#include <array>
#include <cassert>
#include <cstddef>

template<typename T, std::size_t N>
class FixedBuffer {
public:
    bool push(const T& value) noexcept {
        if (size_ == N) return false;
        data_[size_++] = value;
        return true;
    }
    std::size_t size() const noexcept { return size_; }
    const T& operator[](std::size_t index) const noexcept { return data_[index]; }
private:
    std::array<T, N> data_{};
    std::size_t size_{};
};

int main()
{
    FixedBuffer<int, 2> buffer;
    assert(buffer.push(10));
    assert(buffer.push(20));
    assert(!buffer.push(30));
    assert(buffer.size() == 2u && buffer[1] == 20);
}
