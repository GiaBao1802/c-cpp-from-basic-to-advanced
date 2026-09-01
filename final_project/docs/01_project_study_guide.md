# Project Study Guide — học project từ số 0

## 1. Project đang mô phỏng điều gì?

Một xe có radar và camera cùng quan sát vật thể phía trước. Radar đo khoảng cách/tốc độ tương đối tốt; camera xác nhận vật thể và hỗ trợ phân loại. ECU nhận hai nguồn, kiểm tra dữ liệu, hợp nhất chúng, tính mức rủi ro rồi phát trạng thái ADAS.

```mermaid
flowchart LR
  CAR["Xe phía trước"] --> RADAR["Radar<br/>distance + relative speed"]
  CAR --> CAMERA["Camera<br/>object + distance"]
  RADAR --> ECU["ADAS ECU"]
  CAMERA --> ECU
  ECU --> OUT["Risk 0..3<br/>valid + distance"]
  ECU --> DIAG["Diagnostic events"]
```

Project không thực hiện computer vision thật. Nó tập trung vào software architecture bao quanh algorithm: data format, ownership, scheduling, error handling, shutdown và test.

## 2. Vì sao vừa C vừa C++?

```mermaid
flowchart TB
  subgraph PLATFORM["C — platform boundary"]
    BUS["Raw sensor bytes"] --> UNPACK["DLC check + unpack"]
    UNPACK --> CALLBACK["Function pointer callback"]
    DEM["DEM-like"]
    NVM["NvM-like"]
  end
  subgraph DOMAIN["C++ — ADAS domain"]
    ADAPTER["Adapter + physical conversion"] --> QUEUE["BoundedQueue"]
    QUEUE --> FUSION["Fusion Strategy"] --> DECISION["TTC decision"]
    DECISION --> OBSERVER["Observer / IPC"]
  end
  CALLBACK --> ADAPTER
  DECISION --> DEM
  DECISION --> NVM
```

C boundary tương tự nơi MCAL/BSW/generated interface thường xuất hiện trong AUTOSAR Classic. C++ domain giúp diễn đạt ownership, interface, algorithm strategy, threading và RAII rõ hơn. `extern "C"` giữ linkage tương thích, không biến C++ thành C.

## 3. Raw PDU được unpack thế nào?

Project dùng PDU học tập dài đúng 10 byte:

```text
Byte:   0         1         2..3          4..5             6..9
      +---------+---------+-------------+----------------+--------------+
      |Sensor ID| Counter |Distance [cm]|Rel speed [cm/s]|Timestamp [ms]|
      +---------+---------+-------------+----------------+--------------+
Size:    1 B       1 B         2 B             2 B              4 B
```

Các field nhiều byte dùng little-endian trong simulator. `platform_c.c` đọc từng byte bằng shift/or thay vì cast payload thành struct. Cast trực tiếp có thể sai vì alignment, padding và endian.

```mermaid
sequenceDiagram
  participant Test as Bus/Test source
  participant C as Platform_InjectRxPdu
  participant CB as SensorRxCallback
  participant A as CPlatformAdapter
  Test->>C: uint8_t payload[10]
  C->>C: check pointer, length=10, mode=RUN
  C->>C: unpack fixed-width SensorPdu
  C->>CB: callback(context, &pdu)
  CB->>A: static trampoline → object method
```

## 4. Function pointer nối C với object C++

C không biết class hay `this`. Khi đăng ký callback, C++ truyền hai thứ:

- địa chỉ function `rx_callback` có C-compatible signature;
- `void *context` trỏ tới object `CPlatformAdapter`.

Khi có PDU, static callback cast context về object và gọi `on_rx`. Pattern này xuất hiện ở driver callback, RTOS callback, C library và hardware abstraction.

```text
C storage: [callback address] [context address]
                    │              │
                    └──── call ────┘
                              ↓
          CPlatformAdapter::rx_callback(context, pdu)
                              ↓ cast
                     adapter->on_rx(*pdu)
```

Context phải sống lâu hơn thời gian callback có thể xảy ra. Project giữ adapter trong `main()` tới sau khi controller dừng.

## 5. Counter validation

Mỗi sensor có counter riêng. Sau frame counter `N`, frame hợp lệ tiếp theo mong đợi `N+1` theo modulo `uint8_t`. Nếu nhảy từ 0 sang 2, adapter báo `EVENT_BAD_COUNTER`.

```mermaid
stateDiagram-v2
  [*] --> FirstFrame
  FirstFrame --> Tracking: save N, expected=N+1
  Tracking --> Tracking: received expected / advance
  Tracking --> CounterError: received != expected
  CounterError --> Tracking: report DEM + resynchronize
```

Production E2E/rolling-counter behavior phải theo specification: window, duplicate, wrap, timeout, debounce và recovery. Demo chỉ minh họa invariant cơ bản.

## 6. Bounded queue và backpressure

Callback không chạy fusion trực tiếp. Nó đưa `Detection` vào queue capacity 8; worker thread lấy ra xử lý.

```mermaid
flowchart LR
  P["Producer<br/>C callback"] -->|try_push| Q["Ring buffer<br/>8 fixed slots"]
  Q -->|wait_pop| C["Consumer<br/>ADAS worker"]
  Q -->|full| DROP["Drop newest<br/>DEM overflow"]
  STOP["stop()"] -->|close + notify| Q
```

Queue fixed-size làm memory bound rõ. Khi full, policy hiện tại là drop frame mới và tăng event counter. Production phải chọn policy theo requirement: drop-oldest, drop-newest, block, coalesce hoặc chuyển degraded.

`condition_variable` cho worker ngủ khi queue rỗng. Predicate `closed || size != 0` chống spurious wakeup. Mutex bảo vệ đồng thời head, tail, size và slot ownership.

## 7. Ownership và lifetime

```mermaid
flowchart TD
  MAIN["main composition root"] -->|unique ownership| CTRL["AdasController"]
  CTRL -->|unique_ptr| STRAT["FusionStrategy"]
  MAIN -->|shared_ptr| METRIC["ConsoleMetrics"]
  CTRL -.->|weak_ptr observer| METRIC
  CTRL -->|owns + joins| THREAD["worker thread"]
  THREAD -->|local unique_ptr| IPC["IpcPublisher / file descriptor"]
```

- `unique_ptr<FusionStrategy>`: controller là owner duy nhất.
- `shared_ptr<Observer>`: composition root giữ observer sống.
- `weak_ptr`: controller quan sát nhưng không tạo ownership cycle.
- destructor controller gọi `stop()`, bảo đảm join thread.
- Unix publisher destructor đóng file descriptor theo RAII.

Smart pointer không thay design; sơ đồ ownership phải được quyết định trước khi chọn pointer.

## 8. Fusion Strategy

Radar và camera được lưu thành hai snapshot mới nhất. Khi đã có cả hai, `ConservativeFusion`:

1. kiểm tra chênh timestamp không quá 50 ms;
2. chọn khoảng cách nhỏ hơn để thận trọng;
3. chọn relative speed âm hơn;
4. đánh dấu radar valid và camera valid nếu synchronized.

```mermaid
flowchart LR
  R["Radar<br/>12 m, -6 m/s, t=1000"] --> F["ConservativeFusion"]
  C["Camera<br/>13 m, -5.5 m/s, t=1020"] --> F
  F --> O["Fused<br/>12 m, -6 m/s<br/>both valid"]
```

Strategy interface cho phép thay algorithm khác mà controller không đổi. Production cần association/object lists, uncertainty, calibration, coordinate transform và temporal filtering phức tạp hơn nhiều.

## 9. TTC và risk

Time To Collision đơn giản:

```text
closing_speed = max(-relative_speed, 0)
TTC = distance / closing_speed
```

| TTC | Risk |
|---:|---:|
| `< 1.0 s` | 3 — critical |
| `< 2.5 s` | 2 — high |
| `< 5.0 s` | 1 — warning |
| otherwise | 0 |

```mermaid
flowchart TD
  I["Fused object"] --> V{"valid, finite,<br/>distance >= 0?"}
  V -->|No| FAULT["Fault, valid=false"]
  V -->|Yes| CLOSE["Compute closing speed"]
  CLOSE --> TTC["Compute TTC"]
  TTC --> R3{"TTC < 1?"}
  R3 -->|Yes| L3["Risk 3"]
  R3 -->|No| R2{"TTC < 2.5?"}
  R2 -->|Yes| L2["Risk 2"]
  R2 -->|No| R1{"TTC < 5?"}
  R1 -->|Yes| L1["Risk 1"]
  R1 -->|No| L0["Risk 0"]
```

TTC demo không đủ để điều khiển phanh. Nó thiếu ego speed, acceleration, road curvature, object confidence, driver state và safety validation.

## 10. Output, DEM, NvM và IPC

Sau decision, worker thực hiện bốn output độc lập:

```mermaid
flowchart LR
  D["Decision"] --> COM["COM-like TX status"]
  D --> NVM["Pending last risk"]
  D --> OBS["Observer metrics"]
  D --> IPC["Unix datagram<br/>/tmp/adas_monitor.sock"]
```

IPC dùng datagram non-blocking để monitoring không chặn control pipeline. Nếu không có receiver, `publish()` fail nhưng decision vẫn tiếp tục. Đây là graceful degradation cho optional observer; production cần counter/log rate-limit.

## 11. Lifecycle và shutdown đúng

```mermaid
sequenceDiagram
  participant MAIN
  participant PLATFORM as EcuM-like C platform
  participant CTRL as AdasController
  participant Q as Queue/worker
  participant NVM as NvM-like
  MAIN->>PLATFORM: Platform_Init → STARTUP
  MAIN->>CTRL: construct + start worker
  MAIN->>PLATFORM: mode RUN
  PLATFORM->>Q: RX frames through callback
  MAIN->>PLATFORM: mode POST_RUN
  MAIN->>CTRL: stop()
  CTRL->>Q: close queue + notify
  Q->>Q: drain queued items
  CTRL->>CTRL: join worker
  MAIN->>NVM: MainFunction until complete
  MAIN->>PLATFORM: mode SHUTDOWN
```

Nếu hủy object trước khi join, worker có thể truy cập memory đã chết. Nếu shutdown trước NvM completion, last risk không được persist. Hai lỗi này minh họa vì sao lifecycle quan trọng như algorithm.

## 12. AUTOSAR/MICROSAR mapping

| Project | Hệ thống thật tương ứng gần nhất |
|---|---|
| `Platform_InjectRxPdu` | Can/CanIf/PduR/COM RX path hoặc sensor CDD |
| callback | COM notification/RTE/complex driver callout |
| `CPlatformAdapter` | SWC/RTE adapter hoặc integration layer |
| worker + queue | OS task/event/IOC; không nhất thiết `std::thread` |
| `Platform_DemReport` | DEM event report/debounce configuration |
| `Platform_NvM*` | NvM asynchronous block service/main functions |
| `Platform_ComSendAdasStatus` | COM signal packing/I-PDU scheduling |
| Unix IPC | Linux/Adaptive/service monitoring; không phải Classic MCAL |

Trong Toshiba MICROSAR, tên module/config/API vendor có thể khác. Học mapping theo trách nhiệm, rồi trace source/config thực tế; không giả định simulator là generated code thật.
