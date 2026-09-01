# 06 — C++ core: class, ownership và RAII

## Từ struct C tới class

Class gom state và invariant với operation:

```cpp
class SpeedLimiter {
public:
    explicit SpeedLimiter(float maximum) : maximum_{maximum} {}
    float limit(float requested) const noexcept {
        return requested < maximum_ ? requested : maximum_;
    }
private:
    float maximum_;
};
```

Constructor thiết lập invariant; `private` ngăn sửa tùy ý; `const` nói method không đổi state; `noexcept` mô tả không phát exception.

## RAII

Resource Acquisition Is Initialization gắn lifetime resource với object. Destructor tự release lock/file/buffer khi rời scope, kể cả early return.

```cpp
class InterruptGuard {
public:
    InterruptGuard() noexcept { disable_interrupts(); }
    ~InterruptGuard() { restore_interrupts(); }
    InterruptGuard(const InterruptGuard&) = delete;
    InterruptGuard& operator=(const InterruptGuard&) = delete;
};
```

Target safety có thể tắt exception/dynamic allocation nhưng RAII vẫn hữu ích cho scoped lock/state restoration.

## Copy và move

Copy tạo resource/value thứ hai; move chuyển ownership. Rule of zero ưu tiên member tự quản lý lifetime. Với hardware handle singleton/non-copyable, xóa copy operation để tránh hai owner.

## Polymorphism

Runtime polymorphism dùng virtual function, có vtable/indirect call và cần policy rõ. Compile-time polymorphism dùng template, tránh virtual dispatch nhưng tăng code size/compile complexity. Không chọn theo khẩu hiệu; đo timing/memory và xét testability.

## C++ trong Automotive

- AUTOSAR Classic MCAL/BSW truyền thống chủ yếu C.
- Adaptive AUTOSAR và ADAS/HPC thường dùng C++ nhiều hơn.
- Tooling, simulation, SIL, log analysis có thể dùng đầy đủ STL hơn target safety MCU.
- Một ECU có thể có C driver dưới và C++ algorithm/service trên, nối qua C-compatible boundary.

## C interface cho C++

```cpp
extern "C" void Adas_MainFunction(void)
{
    controller.step();
}
```

`extern "C"` điều khiển linkage/name mangling, không biến C++ implementation thành C hay đảm bảo ABI của class/exception.
