# Advanced C — callback, preprocessor, OOP và ABI

## Function pointer

```c
typedef Std_ReturnType (*ReadFn)(uint16_t id, uint8_t *data, uint16_t *length);

typedef struct {
    uint16_t id;
    ReadFn read;
} DidEntry;
```

`ReadFn` là type của pointer tới function nhận ba parameter và trả status. Table-driven dispatch tránh `switch` dài và giống generated configuration trong DCM/MCAL.

```c
Std_ReturnType dispatch(const DidEntry *table, size_t count, uint16_t id,
                        uint8_t *data, uint16_t *length)
{
    for (size_t i = 0u; i < count; ++i) {
        if (table[i].id == id) {
            return table[i].read != NULL ? table[i].read(id, data, length) : E_NOT_OK;
        }
    }
    return E_NOT_OK;
}
```

Callback risks: signature mismatch, null pointer, callback lifetime, ISR/task context, recursion/reentrancy và indirect-call timing. Function pointer không mang object context; thường truyền thêm `void *context`.

## Preprocessor

Macro là token substitution, không có type. Luôn parenthesize parameter/result và tránh evaluate parameter nhiều lần.

```c
#define MIN_BAD(a,b) ((a) < (b) ? (a) : (b)) /* MIN_BAD(i++, j++) nguy hiểm */
```

Ưu tiên `static inline` khi cần type checking. Token-pasting `##`, stringification `#`, X-macro và conditional compilation hữu ích cho generated/config code nhưng làm debug khó.

## `#pragma`

`#pragma` là compiler-specific directive: warning control, packing, optimization, section placement. `#pragma pack(1)` có thể tạo unaligned access và ABI khác; không dùng chỉ để map protocol.

AUTOSAR MemMap thường dùng macro include pattern để chuyển code/data section theo compiler abstraction. Mục tiêu là mapping logical section sang linker section, không phải syntax đẹp.

## OOP bằng C

```c
typedef struct Sensor Sensor;
typedef struct {
    bool (*read)(Sensor *self, float *value);
    void (*destroy)(Sensor *self);
} SensorVTable;

struct Sensor {
    const SensorVTable *vptr;
    void *impl;
};
```

- Encapsulation: opaque struct trong header, implementation trong `.c`.
- Method: function nhận `self`.
- Interface/polymorphism: function-pointer table.
- Constructor/destructor: init/deinit API.
- Inheritance-like composition: base struct là member đầu hoặc explicit handle; phải kiểm soát layout.

Đây là pattern thường thấy trong driver abstraction, nhưng dynamic polymorphism tăng indirect call và test complexity. Generated static table thường phù hợp AUTOSAR Classic hơn heap object.

## Variadic, `setjmp`, signal và advanced feature policy

Variadic function mất type safety; format mismatch gây UB. `setjmp/longjmp` phá structured cleanup và thường bị hạn chế. Signal handler/POSIX có async-signal-safe rule. Senior cần biết feature tồn tại và lý do project cấm, không chỉ cách dùng.

## ABI

Application Binary Interface quy định calling convention, register/stack parameter, alignment, symbol naming và object layout. Lỗi ABI xuất hiện khi khác compiler flags, packing, C/C++ linkage hoặc shared-library version. Public binary interface không expose STL type tùy tiện nếu compiler/runtime compatibility không được kiểm soát.
