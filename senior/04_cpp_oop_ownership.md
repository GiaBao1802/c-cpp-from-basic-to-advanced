# C++ OOP, object model và ownership

## OOP đầy đủ

- Encapsulation bảo vệ invariant.
- Abstraction expose capability thay implementation.
- Inheritance biểu diễn substitutability, không chỉ reuse.
- Polymorphism chọn behavior runtime/compile time.
- Composition thường ít coupling hơn inheritance.

Liskov Substitution: derived object phải dùng được nơi base được mong đợi mà không phá contract. Nếu derived throw thêm, đổi precondition hoặc không thực hiện behavior base, hierarchy sai.

## Virtual dispatch và object slicing

Base có virtual function nên có virtual destructor nếu delete qua base pointer. Truyền derived by value thành base làm slicing. Constructor/destructor không dispatch virtual như object hoàn chỉnh; tránh gọi virtual dựa derived state trong đó.

## Rule of zero/five

Nếu class sở hữu raw resource, phải quyết định destructor, copy constructor/assignment, move constructor/assignment. Tốt hơn đóng resource trong RAII member để class business dùng Rule of Zero.

## Smart pointer

```cpp
auto p = std::make_unique<Driver>();          // một owner
std::shared_ptr<Node> shared = make_node();   // shared ownership
std::weak_ptr<Node> observer = shared;        // phá cycle/quan sát
```

- `unique_ptr`: mặc định cho heap ownership, movable không copy.
- `shared_ptr`: atomic refcount thường có overhead; cycle gây leak.
- `weak_ptr`: không sở hữu; `lock()` có thể fail.
- raw pointer/reference: non-owning view nếu lifetime contract rõ.

Smart pointer không sửa lifetime design mơ hồ. Trong hard real-time/no-heap, dùng static storage, object pool, arena hoặc fixed-capacity owner.

## Exception safety

Basic guarantee: invariant còn đúng/no leak. Strong: operation fail không đổi observable state. No-throw: cam kết không fail bằng exception. RAII là nền tảng. Nếu project tắt exception, vẫn cần transactional update/status và cleanup có cấu trúc.

## SOLID có điều kiện

SRP, OCP, LSP, ISP, DIP là công cụ hỏi về coupling; không phải lý do tạo interface/class cho mọi function. Embedded design phải cân code size, indirect calls, configuration và traceability.

## Ownership table trong design

| Object | Creator | Owner | Mutable by | Lifetime | Thread/core |
|---|---|---|---|---|---|
| FrameBuffer | pool | handle | DMA then stage | lease | producer/consumer |

Nếu không điền được bảng này, design concurrency chưa đủ rõ.
