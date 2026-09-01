# 10 — Bài tập và project

## Bài C theo cấp độ

1. Viết `set_field(reg, mask, shift, value)` và không làm đổi bit ngoài mask.
2. Pack/unpack ba signal vào payload 8 byte, có unit test boundary.
3. Viết state machine `INIT/RUN/DEGRADED/SAFE` không dùng global public.
4. Viết ring buffer fixed-size, phát hiện full/empty.
5. Fake ADC driver và test module scaling/invalid handling.
6. Viết diagnostic parser nhận SID + payload length và reject malformed request.

## Bài C++

1. Class fixed-capacity signal history không heap.
2. RAII guard cho resource lock/unlock; cấm copy.
3. Template saturating integer wrapper và boundary tests.
4. Object pool fixed-size trả handle có ownership rõ.
5. ADAS pipeline ba stage với bounded queue và dropped-frame counter.

## Project 1 — Mini AUTOSAR signal gateway

Input CAN-like frame → unpack signal → validate counter/checksum → application mapping → pack output frame. Tách module Driver Stub, COM-like packer, gateway logic, DEM-like event recorder. Có test normal, timeout, invalid length, bad counter và checksum.

## Project 2 — Cooperative ECU scheduler

Mô phỏng task 1/5/10 ms, ISR event, watchdog checkpoint, RUN/POST_RUN và asynchronous NvM job. In timeline; phát hiện deadline miss và shutdown trước khi write complete.

## Project 3 — ADAS object pipeline

Nhận object list recorded/synthetic, validate timestamp/range, tính time-to-collision đơn giản, xuất request + quality. Không dynamic allocation; test missing frame, NaN, capacity overflow và degraded state.

## Definition of done

- Warning sạch với `-Wall -Wextra -Werror`.
- Không UB hiển nhiên; test boundary/error.
- README mô tả architecture và ownership.
- Có trace requirement → design → test.
- Giải thích được tại sao chọn C/C++, allocation và synchronization.

## Senior capstone — Linux ADAS gateway

Xây daemon C++20 nhận hai stream qua socket, parse frame an toàn, đưa vào bounded queues, đồng bộ timestamp, chạy fusion stub và publish qua Unix domain socket. Dùng object pool hoặc PMR, graceful shutdown, metrics và structured logs. Viết một C adapter cho CAN-like transport và build bằng CMake.

Artifacts bắt buộc:

1. Context/component/sequence diagram.
2. Ownership và lock-order table.
3. Protocol specification có version/length/endian.
4. Unit, integration, stress và sanitizer test.
5. Benchmark p50/p95/p99, memory high-water và overload result.
6. Failure injection: peer disconnect, malformed frame, queue full, slow consumer.
7. Design review ghi rõ alternative đã loại và lý do.

## Senior review drills

- Một callback được gọi từ ISR nhưng implementation dùng mutex: phân tích failure.
- `shared_ptr` cycle giữa pipeline stages: phát hiện và redesign ownership.
- Shared-memory reader crash khi writer update layout: thiết kế versioning/commit marker.
- Linker báo RAM overflow 12 KiB: đọc map, phân loại `.data/.bss/stack/heap` và đề xuất.
- CAN gateway miss deadline sau thêm logging: thiết kế measurement và fix không làm mất observability.

## Câu hỏi phỏng vấn

1. Array khác pointer thế nào?
2. `volatile` làm gì và không làm gì?
3. Vì sao struct không nên map thẳng network frame?
4. Stack overflow được phát hiện/giảm thiểu ra sao?
5. Reentrant khác thread-safe thế nào?
6. `static` ở file scope có tác dụng gì?
7. RAII có ích nếu exception bị tắt không?
8. Khi nào không dùng `std::vector` trên ECU?
9. ISR nên làm gì và không nên làm gì?
10. Trace CAN TX từ SWC xuống bus.
