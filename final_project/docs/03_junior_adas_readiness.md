# Học xong có apply Junior ADAS được chưa?

## Câu trả lời ngắn

**Có thể apply Junior ADAS Software/Embedded Software**, nếu bạn thật sự tự build, sửa và giải thích được project; chỉ đọc hết tài liệu thì chưa đủ. Project tạo nền software tốt nhưng chưa thay kiến thức perception/robotics/math hoặc kinh nghiệm hardware tùy JD.

## Năng lực project chứng minh

| Năng lực | Bằng chứng |
|---|---|
| C/C++ | C ABI, callback, OOP, RAII, smart pointer, template |
| Embedded | byte packing, fixed memory, lifecycle, error path |
| Concurrency | worker, queue, mutex, CV, atomic, shutdown |
| AUTOSAR | COM/RTE/DEM/NvM/EcuM responsibility mapping |
| Linux | Unix socket IPC và descriptor RAII |
| ADAS basics | radar/camera input, synchronization, fusion stub, TTC, degraded state |
| Engineering | requirements, ADR, tests, sanitizer CI, traceability |

## Phần còn thiếu theo loại JD

### Junior AUTOSAR/ADAS integration

Học thêm CAN/CAN FD, COM stack, DCM/DEM, OS scheduling, RTE/SWC, E2E, DaVinci/configuration và SIL/HIL. Với kinh nghiệm Bosch/Toshiba của bạn, đây là hướng phù hợp nhất.

### Junior ADAS algorithm/perception

Cần thêm linear algebra, probability, coordinate frames, Kalman filter, computer vision, OpenCV, camera calibration, radar association, Python/NumPy và dataset evaluation.

### Junior ADAS Linux/platform

Cần thêm POSIX, networking, CMake, GDB, sanitizers, Docker, IPC/shared memory, profiling và basic ROS 2/DDS nếu JD yêu cầu.

## Tiêu chí trước khi ghi project vào CV

Bạn phải làm được không nhìn tài liệu:

1. Vẽ architecture và nói rõ C/C++ boundary.
2. Giải thích ownership của controller, observer, worker và IPC publisher.
3. Debug một frame không tạo output theo từng tầng.
4. Thêm một requirement mới cùng unit/integration test.
5. Giải thích vì sao queue bounded và policy khi full.
6. Phân biệt simulator này với AUTOSAR/MICROSAR production.
7. Trình bày hạn chế của TTC/fusion demo và không gọi nó là AEB thật.

## Cách ghi CV trung thực

> Developed an AUTOSAR-inspired ADAS ECU reference project using C11 and C++17. Implemented a C platform boundary for communication, diagnostics, persistence and ECU lifecycle, integrated with a multithreaded C++ sensor-fusion pipeline using bounded queues, RAII, smart pointers and design patterns. Added Unix-domain IPC, requirement traceability, integration tests and sanitizer-based CI.

Không ghi “developed production ADAS/AEB” vì project là simulator học tập.

## Kế hoạch 4 tuần để sẵn sàng apply

| Tuần | Việc phải làm | Đầu ra |
|---|---|---|
| 1 | đọc guide, tự vẽ flow, chạy/debug tests | giải thích project 10 phút |
| 2 | thêm timeout, CRC8, TX packer và tests | PR/commit feature hoàn chỉnh |
| 3 | thêm timestamp buffer, metrics/benchmark | report latency/overload |
| 4 | mock interview, README tiếng Anh, demo video | portfolio link + 3 phút pitch |
