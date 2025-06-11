#pragma once
/*--------------------------------------------------------------------
  chunk_vector.h --- A general‑purpose fixed‑chunk dynamic container
  --------------------------------------------------------------------
  *   Stable pointers/iterators: elements are never relocated after
  *   construction (except explicit erase()).
  *   Fast O(1) random access with index   : obj = cv[idx];
  *   No external dependencies (header‑only / C++17).
  *   Optional template parameters: ChunkSize, Allocator.
  *
  *  Typical usage:
  *    chunk_vector<MyType, 512> cv;
  *    cv.emplace_back(args...);
  *    for (auto& v : cv) { ... }
  *
  *  Author: ChoEngine project (2025)
--------------------------------------------------------------------*/
#include <vector>
#include <memory>
#include <cstddef>
#include <climits>
#include <cassert>
#include <iterator>
#include <new>
#include <type_traits>

namespace cho
{

    // ---------------------------------------------------------------
    //  chunk_vector  (primary template)
    // ---------------------------------------------------------------

    template <typename T,
        std::size_t ChunkSize = 1024,
        typename Allocator = std::allocator<std::byte>>
        class chunk_vector
    {
        static_assert(std::is_trivially_destructible_v<T> || std::is_destructible_v<T>,
            "T must be destructible");
        static_assert((ChunkSize& (ChunkSize - 1)) == 0,
            "ChunkSize must be a power of two");

        using alloc_traits = std::allocator_traits<Allocator>;

        struct Chunk
        {
            using storage_t = std::aligned_storage_t<sizeof(T), alignof(T)>;
            storage_t data[ChunkSize];
        };

    public:
        // -------------------- public type aliases -------------------
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;

    private:
        // bit magic helpers
        static constexpr size_type kChunkMask = ChunkSize - 1;
        static constexpr size_type kChunkShift = [] {
            size_type s = 0;
            while ((1ULL << s) < ChunkSize) ++s;
            return s;
            }();

        // container state
        std::vector<Chunk*> m_chunks;            // owned chunk pointers
        std::vector<size_type> m_free;           // recycled indices
        std::vector<uint8_t>  m_alive;           // occupancy flags (0/1)
        allocator_type        m_alloc{};         // byte‑wise allocator
        size_type             m_nextIndex = 0;   // monotonic index source
        size_type             m_active = 0;   // living element count

        // ---- internal helpers -------------------------------------
        static constexpr size_type chunk_of(size_type idx) noexcept { return idx >> kChunkShift; }
        static constexpr size_type offset_of(size_type idx) noexcept { return idx & kChunkMask; }

        void ensure_chunk(size_type cidx)
        {
            while (m_chunks.size() <= cidx)
            {
                // allocate raw memory for one chunk
                using chunk_alloc = typename alloc_traits::template rebind_alloc<Chunk>;
                chunk_alloc ca(m_alloc);
                Chunk* raw = alloc_traits::allocate(ca, 1);
                m_chunks.push_back(raw);
                // mark occupancy for new chunk
                m_alive.resize(m_chunks.size() * ChunkSize, 0);
            }
        }

        pointer ptr_from_index(size_type idx) noexcept
        {
            size_type c = chunk_of(idx);
            size_type o = offset_of(idx);
            return std::launder(reinterpret_cast<pointer>(&m_chunks[c]->data[o]));
        }
        const_pointer ptr_from_index(size_type idx) const noexcept
        {
            size_type c = chunk_of(idx);
            size_type o = offset_of(idx);
            return std::launder(reinterpret_cast<const_pointer>(&m_chunks[c]->data[o]));
        }

    public:
        // -------------------- ctors / dtors -------------------------
        chunk_vector() = default;
        explicit chunk_vector(const allocator_type& a) : m_alloc(a) {}

        chunk_vector(const chunk_vector&) = delete;
        chunk_vector& operator=(const chunk_vector&) = delete;

        chunk_vector(chunk_vector&& other) noexcept
            : m_chunks(std::move(other.m_chunks)),
            m_free(std::move(other.m_free)),
            m_alive(std::move(other.m_alive)),
            m_alloc(std::move(other.m_alloc)),
            m_nextIndex(other.m_nextIndex),
            m_active(other.m_active)
        {
            other.m_nextIndex = other.m_active = 0;
        }

        chunk_vector& operator=(chunk_vector&& rhs) noexcept
        {
            if (this == &rhs) return *this;
            clear();
            m_chunks = std::move(rhs.m_chunks);
            m_free = std::move(rhs.m_free);
            m_alive = std::move(rhs.m_alive);
            m_alloc = std::move(rhs.m_alloc);
            m_nextIndex = rhs.m_nextIndex;
            m_active = rhs.m_active;
            rhs.m_nextIndex = rhs.m_active = 0;
            return *this;
        }

        ~chunk_vector() { clear(); for (auto* ch : m_chunks) alloc_traits::deallocate(m_alloc, reinterpret_cast<std::byte*>(ch), sizeof(Chunk)); }

        // -------------------- element modifiers ---------------------
        template<class... Args>
        pointer emplace_back(Args&&... args)
        {
            size_type idx;
            if (!m_free.empty())
            {
                idx = m_free.back();
                m_free.pop_back();
            }
            else
            {
                idx = m_nextIndex++;
                ensure_chunk(chunk_of(idx));
            }
            pointer p = ptr_from_index(idx);
            std::construct_at(p, std::forward<Args>(args)...);
            if (m_alive.size() <= idx) m_alive.resize(idx + 1, 0);
            m_alive[idx] = 1;
            ++m_active;
            return p;
        }

        pointer push_back(const T& v) { return emplace_back(v); }
        pointer push_back(T&& v) { return emplace_back(std::move(v)); }

        void erase(size_type idx)
        {
            if (idx >= m_alive.size() || !m_alive[idx]) return; // already empty
            pointer p = ptr_from_index(idx);
            std::destroy_at(p);
            m_alive[idx] = 0;
            m_free.push_back(idx);
            --m_active;
        }

        void clear()
        {
            for (size_type i = 0; i < m_nextIndex; ++i)
            {
                if (m_alive[i])
                {
                    std::destroy_at(ptr_from_index(i));
                }
            }
            m_free.clear();
            m_alive.assign(m_alive.size(), 0);
            m_nextIndex = m_active = 0;
        }

        // -------------------- capacity / size -----------------------
        [[nodiscard]] size_type size() const noexcept { return m_active; }
        [[nodiscard]] size_type capacity() const noexcept { return m_chunks.size() * ChunkSize; }
        [[nodiscard]] bool      empty() const noexcept { return m_active == 0; }

        void reserve(size_type desired)
        {
            size_type chunksNeeded = (desired + ChunkSize - 1) >> kChunkShift;
            while (m_chunks.size() < chunksNeeded)
            {
                ensure_chunk(m_chunks.size());
            }
        }

        // -------------------- element access ------------------------
        reference operator[](size_type idx) noexcept { assert(m_alive[idx]); return *ptr_from_index(idx); }
        const_reference operator[](size_type idx) const noexcept { assert(m_alive[idx]); return *ptr_from_index(idx); }

        pointer       get(size_type idx) noexcept { return m_alive[idx] ? ptr_from_index(idx) : nullptr; }
        const_pointer get(size_type idx) const noexcept { return m_alive[idx] ? ptr_from_index(idx) : nullptr; }

        // -------------------- iterator implementation ---------------
        class iterator
        {
            chunk_vector* m_parent;
            size_type     m_idx;
            void advance()
            {
                while (m_idx < m_parent->m_nextIndex && !m_parent->m_alive[m_idx]) ++m_idx;
            }
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            iterator(chunk_vector* p, size_type idx) : m_parent(p), m_idx(idx) { advance(); }
            reference operator*()  const { return (*m_parent)[m_idx]; }
            pointer   operator->() const { return &(*m_parent)[m_idx]; }

            iterator& operator++() { ++m_idx; advance(); return *this; }
            iterator  operator++(int) { iterator tmp(*this); ++(*this); return tmp; }

            friend bool operator==(const iterator& a, const iterator& b) { return a.m_idx == b.m_idx; }
            friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }
        };

        class const_iterator
        {
            const chunk_vector* m_parent;
            size_type           m_idx;
            void advance()
            {
                while (m_idx < m_parent->m_nextIndex && !m_parent->m_alive[m_idx]) ++m_idx;
            }
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = const T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T*;
            using reference = const T&;

            const_iterator(const chunk_vector* p, size_type idx) : m_parent(p), m_idx(idx) { advance(); }
            reference operator*()  const { return (*m_parent)[m_idx]; }
            pointer   operator->() const { return &(*m_parent)[m_idx]; }

            const_iterator& operator++() { ++m_idx; advance(); return *this; }
            const_iterator  operator++(int) { const_iterator tmp(*this); ++(*this); return tmp; }

            friend bool operator==(const const_iterator& a, const const_iterator& b) { return a.m_idx == b.m_idx; }
            friend bool operator!=(const const_iterator& a, const const_iterator& b) { return !(a == b); }
        };

        iterator begin() { return iterator(this, 0); }
        iterator end() { return iterator(this, m_nextIndex); }

        const_iterator begin() const { return const_iterator(this, 0); }
        const_iterator end()   const { return const_iterator(this, m_nextIndex); }

        const_iterator cbegin() const { return begin(); }
        const_iterator cend()   const { return end(); }
    };

} // namespace cho