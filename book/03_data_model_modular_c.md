# 03 — Struct, enum, bit và modular C

## Struct và padding

```c
typedef struct {
    uint8_t status;
    uint32_t timestamp;
} Sample;
```

Compiler có thể chèn padding trước `timestamp` để alignment; `sizeof(Sample)` có thể lớn hơn 5. Không dùng layout struct làm network/NvM format nếu chưa kiểm soát ABI/version/endian.

## Enum

Enum diễn đạt state tốt hơn magic number, nhưng underlying size có thể phụ thuộc compiler/options. Với protocol/register, serialize sang fixed-width type rõ ràng.

## Bit operations

```c
#define CTRL_ENABLE_MASK  (1u << 3)
#define CTRL_MODE_MASK    (3u << 4)

reg |= CTRL_ENABLE_MASK;
reg = (reg & ~CTRL_MODE_MASK) | ((uint32_t)mode << 4);
```

Luôn mask input nếu range chưa được đảm bảo. Read-modify-write có thể nguy hiểm với register có write-one-to-clear hoặc bị ISR/hardware đổi.

## Memory-mapped I/O

```c
#define REG32(addr) (*(volatile uint32_t *)(addr))
```

`volatile` yêu cầu access thật nhưng không tạo atomicity, ordering giữa core, cache coherence hay lock. Production MCAL dùng vendor register definitions và sequence theo manual, không tự hard-code tùy ý.

## Module C

```text
Adc_Service.h        public contract
Adc_Service.c        private state + implementation
Adc_Service_Cfg.h    generated/project configuration
Adc_Service_Types.h  shared public types
```

Header cần include guard, chỉ expose cần thiết, không định nghĩa mutable global. Implementation giữ helper/state là `static`.

## Dependency injection kiểu C

```c
typedef struct {
    uint16_t (*read)(uint8_t channel);
    void (*report_error)(uint16_t event);
} SensorDeps;
```

Truyền dependency table cho module giúp unit test dùng fake hardware. Đây là cách C đạt loose coupling tương tự interface trong C++.

## AUTOSAR application

- MCAL config object: struct/array `const`, thường generated.
- Register definition: struct/union/bit mask vendor-specific.
- PDU buffer: byte array với pack/unpack rõ endian.
- OS config: task/ISR/resource table.
- DEM/DET callback: function pointer hoặc configured call.
- MemMap section: macro đặt code/data vào region theo linker.
