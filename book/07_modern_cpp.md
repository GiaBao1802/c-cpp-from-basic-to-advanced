# 07 — Modern C++ nâng cao

## Value semantics và ownership

Ưu tiên object/value rõ lifetime. `std::unique_ptr` biểu diễn single ownership; `std::shared_ptr` chỉ dùng khi ownership thật sự shared và chấp nhận counter/timing overhead. Trên MCU không heap, có thể dùng static object, reference và fixed-capacity container.

## Template

```cpp
template<typename T, std::size_t N>
class FixedBuffer {
public:
    constexpr std::size_t capacity() const noexcept { return N; }
    T& operator[](std::size_t i) noexcept { return data_[i]; }
private:
    std::array<T, N> data_{};
};
```

Template tạo type-safe reusable code và compile-time capacity. Đổi lại có thể code bloat, error message dài và qualification khó hơn.

## STL và deterministic behavior

`std::array` có fixed size; `std::vector` thường heap/reallocation; `std::map` node allocation; `std::unordered_map` timing/memory khó dự đoán hơn. ADAS Linux/HPC có budget khác bare-metal/AUTOSAR Classic MCU.

## Lambda và callback

Lambda tiện cho algorithm/callback nhưng capture lifetime phải rõ. Không capture reference tới local nếu callback sống lâu hơn scope.

## Concurrency

C++ memory model định nghĩa data race là undefined behavior. Mutex, atomic và condition variable có semantics rõ, nhưng target OS/integration có thể cung cấp primitive riêng. `volatile` không thay `std::atomic`.

## Error handling

Options: exception, return/status, expected-like result, invariant termination. Automotive guideline/project quyết định. Nếu exception disabled, API phải làm error path rõ và test được; không giả vờ constructor luôn thành công nếu resource init có thể fail.

## ADAS relevance

ADAS pipeline thường dùng C++ cho sensor model, fusion, perception/control abstraction, fixed-size math, middleware adapter và test simulation. Kỹ năng quan trọng: ownership frame/image, zero-copy lifetime, concurrency, timestamp synchronization, latency và graceful degradation—không chỉ syntax class.
