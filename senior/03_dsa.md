# Data Structures & Algorithms cho systems engineer

## Complexity không chỉ là Big-O

Big-O mô tả tăng trưởng, nhưng embedded/real-time còn quan tâm worst-case bound, allocation, cache locality, branch predictability và fragmentation. Array linear search nhỏ đôi khi nhanh/deterministic hơn hash tree phức tạp.

| Structure | Search | Insert/remove | Điểm hệ thống |
|---|---:|---:|---|
| array/vector | O(n), index O(1) | O(n) giữa | contiguous, cache tốt |
| linked list | O(n) | O(1) khi có node | pointer/heap, cache kém |
| stack/queue | O(1) ends | O(1) | scheduling/buffering |
| ring buffer | O(1) | O(1) | fixed memory, stream/ISR |
| binary heap | top O(1) | O(log n) | priority queue |
| balanced tree | O(log n) | O(log n) | ordered, node allocation |
| hash table | average O(1) | average O(1) | collision/worst case |

## Core implementations cần tự viết

Dynamic array với capacity/size/growth; singly/doubly list; stack/queue; ring buffer; heap; hash table open addressing; binary search/tree traversal; graph BFS/DFS/Dijkstra; sorting và stable sort.

Không phải để thay STL trong production, mà để hiểu iterator invalidation, ownership, exception safety, allocator và complexity.

## Ring buffer invariant

```text
0 <= head, tail < capacity
empty: head == tail (nếu chừa một slot)
full:  next(head) == tail
producer chỉ ghi slot rồi publish head
consumer đọc published slot rồi publish tail
```

Multithread làm invariant phức tạp vì memory ordering. Bản single-thread/ISR protected phải đúng trước.

## Algorithm engineering

Xác định input bound và failure policy trước. Chọn representation; chứng minh invariant; xử lý empty/one/max/duplicate/overflow; benchmark dữ liệu gần thực tế; fuzz parser/container.

ADAS dùng spatial indexing, queues, graph/assignment và matrix; nhưng production thường dùng library đã tối ưu/qualified. Engineer vẫn phải hiểu complexity và numerical/memory behavior.
