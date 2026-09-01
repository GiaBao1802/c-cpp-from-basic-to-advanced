# Templates, STL và generic programming

## Template mechanics

Template được instantiate theo type/value. Definition thường phải thấy tại điểm instantiate. Explicit instantiation có thể kiểm soát build time/code bloat. Concepts (C++20) mô tả constraint rõ hơn substitution error dài.

Metaprogramming tools: `constexpr`, type traits, `if constexpr`, variadic templates, fold expression. Dùng để tạo zero-cost abstraction, nhưng tránh compile-time maze khó debug/qualification.

## STL mental model

Container sở hữu storage; iterator/reference có invalidation rule; algorithm hoạt động trên range; allocator quyết định allocation. Senior phải biết complexity và invalidation của operation đang dùng.

- `vector`: contiguous, reallocation invalidates all pointer/reference.
- `deque`: segmented; end insertion khác invalidation behavior.
- `list`: stable node iterator nhưng cache kém.
- associative container: ordering/comparator invariant.
- unordered container: rehash invalidation, collision behavior.

## Move không đảm bảo luôn rẻ

Move có thể O(1) khi chuyển pointer, nhưng `std::array` move từng element. `std::move` chỉ cast sang rvalue; operation thực tế phụ thuộc overload/type và có thể vẫn copy nếu object const.

## Allocator/PMR

Arena/pool giúp bound allocation và locality. `std::pmr` tách allocation policy khỏi container trên platform phù hợp. Hard real-time cần đo cả allocation/deallocation và exhaustion policy.

## Ranges, views và lifetime

View thường non-owning/lazy; dangling xảy ra nếu source chết. Pipeline đẹp không thay ownership analysis. Tránh trả view/span tới temporary.

## Automotive rule

Cho phép library component theo target: Classic safety MCU có subset/coding rule khác Linux ADAS. Ghi lại exception/RTTI/heap/STL policy, không tranh luận “C++ zero overhead” chung chung.
