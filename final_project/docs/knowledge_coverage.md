# Knowledge coverage matrix

| Kiến thức | Implementation/evidence |
|---|---|
| C types, pointer, array, endian | `Platform_InjectRxPdu`, byte unpack |
| function pointer + context | `SensorRxCallback` và registration |
| opaque/module state | private `PlatformState` trong `.c` |
| C/C++ ABI | `extern "C"` trong `platform_c.h` |
| modular build/static library | `adas_platform`, `adas_domain` CMake targets |
| templates/DSA | `BoundedQueue<T, Capacity>` ring buffer |
| RAII | thread join, mutex locks, Unix FD destructor |
| smart pointers | fusion `unique_ptr`, observer `shared/weak_ptr` |
| move semantics | queue/controller constructor ownership transfer |
| OOP/polymorphism | Fusion Strategy, Observer, IPC interface |
| design patterns | Adapter, Strategy, Observer, State, Facade |
| multithreading | worker, mutex, condition variable, atomics |
| IPC/Linux | non-blocking Unix datagram publisher |
| AUTOSAR lifecycle | STARTUP/RUN/POST_RUN/SHUTDOWN |
| COM/RTE-like boundary | unpack callback và TX façade |
| DEM/NvM | event counters và asynchronous-like persistence |
| unit/integration test | fusion, callback, controller, DEM, NvM |
| requirements/ADR | traceability document |

Các chủ đề compiler/preprocessor/linker/MISRA nằm ở build và các chương senior của repo, không nên ép mọi feature vào runtime. Ví dụ, inheritance sâu, `shared_ptr` mọi nơi, exception hay lock-free queue được cố ý **không dùng** vì không phù hợp design này; năng lực senior gồm cả quyết định không dùng feature.
