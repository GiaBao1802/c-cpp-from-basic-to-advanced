# 05 — Robust C, MISRA và testing

## Defensive design

Validate tại trust boundary: public API, network/diagnostic input, persistent data, hardware result. Không lặp kiểm tra vô nghĩa ở mọi dòng; xác định invariant sau khi validation.

Các phép toán cần xét range, overflow, division by zero, conversion signed/unsigned và shift.

```c
bool add_u16(uint16_t a, uint16_t b, uint16_t *out)
{
    uint32_t sum;
    if (out == NULL) return false;
    sum = (uint32_t)a + (uint32_t)b;
    if (sum > UINT16_MAX) return false;
    *out = (uint16_t)sum;
    return true;
}
```

## MISRA C

MISRA là coding guideline nhằm giảm undefined/implementation-defined behavior và làm code dễ phân tích. Compliance không có nghĩa “zero warning bằng mọi giá”. Deviation hợp lệ cần rule, location, rationale, risk analysis, compensating measure và approval.

Ví dụ deviation thường gặp trong MCAL: cast địa chỉ integer thành pointer để truy cập SFR là cần thiết, nhưng phải giới hạn trong driver, dùng vendor definition và review địa chỉ/alignment.

## Static analysis và compiler warning

Compiler warning phát hiện một phần; static analyzer hiểu data/control flow rộng hơn; review tìm requirement/design/concurrency issue; dynamic test kiểm tra behavior. Không công cụ nào thay thế tất cả.

## Unit test

Pattern Arrange–Act–Assert:

```c
static void test_scale_rejects_zero_range(void)
{
    float output = 99.0f;
    bool ok = scale_adc(1u, 0u, &output);
    assert(!ok);
    assert(output == 99.0f);
}
```

Test normal, boundary, invalid, state transition và error injection. MCAL-dependent module nên mock/fake API, không test lại vendor driver trong unit test application.

## Coverage

- Statement: mỗi statement chạy chưa?
- Branch/decision: true/false branch chạy chưa?
- Condition: từng điều kiện nhận true/false chưa?
- MC/DC: từng condition được chứng minh ảnh hưởng độc lập tới decision.

Coverage cao không chứng minh requirement đúng; nó chỉ chỉ ra phần code đã kích hoạt theo metric.

## V-cycle evidence

Software requirement → architecture/design → unit implementation → unit test → integration test → software qualification test. Traceability nối requirement với design/code/test/result/defect.

## Error handling layers

DET thường cho development error/API misuse; DEM cho diagnostic event runtime; safety mechanism/SMU cho alarm/reaction; return code cho caller. Chọn đúng tầng theo requirement, không report mọi lỗi thành DTC.
