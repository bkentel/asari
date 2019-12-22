#include <array>
#include <memory>
#include <variant>
#include <vector>
#include <cstdint>

namespace asr
{

template <typename T>
struct point2
{
    T x;
    T y;
};

template <typename T>
struct rect
{
    point2<T> p0;
    point2<T> p1;
};

using rect_u32 = rect<uint32_t>;

class world
{
public:
    virtual ~world();

    virtual std::vector<int> const& terrain(rect_u32 const& region) const noexcept = 0;
};

[[nodiscard]] std::unique_ptr<world> make_world();

struct point2u
{
    uint32_t x;
    uint32_t y;
};

[[nodiscard]] constexpr point2u p2u(uint32_t const x, uint32_t const y) noexcept
{
    return point2u {x, y};
}

[[nodiscard]] constexpr size_t pow2(size_t const n) noexcept
{
    return size_t {1} << n;
}

inline constexpr size_t chunk_w = 256;
inline constexpr size_t chunk_h = 256;

template <typename T, size_t N>
struct static_block
{
    std::array<T, pow2(N) * pow2(N)> m_data;
};

//template <typename T>
//struct block;
//
//template <typename T>
//struct block_deleter
//{
//    constexpr explicit block_deleter(block<T> const& owner) noexcept
//        : m_owner(owner)
//    {
//    }
//
//    using pointer = T*;
//    void operator()(pointer const p) const noexcept
//    {
//        (void)p;
//    }
//
//    block<T> const* m_owner;
//};

struct allocator
{
    [[nodiscard]] virtual void* allocate(size_t const size, size_t n, size_t alignment) = 0;

    virtual void deallocate(void* p, size_t size, size_t n, size_t alignment) noexcept = 0;

    template <typename T, size_t Align = alignof(T)>
    [[nodiscard]] T* allocate(size_t const n = 1)
    {
        auto const memory = allocate(sizeof(T), n, Align);

        auto const stride = Align <= sizeof(T) ? sizeof(T) : Align;

        if constexpr (!std::is_trivial_v<T>)
        {
            for (size_t i = 0; i < n; ++i)
            {
                auto const p = static_cast<char*>(memory) + i * stride;
                ::new (p) T;
            }
        }
        else if constexpr (ASR_CONFIG_DEBUG)
        {
            std::memset(memory, 0, n * stride);
        }

        return static_cast<T*>(memory);
    }

    template <typename T, typename = std::enable_if_t<!std::is_void_v<T>>>
    void deallocate(T* const p, size_t const n = 1, size_t const alignment = alignof(T)) noexcept
    {
        deallocate(reinterpret_cast<void*>(p), sizeof(T), n, alignment);
    }
};

struct block_allocator : public allocator
{
    [[nodiscard]] void* allocate(size_t const size, size_t n, size_t alignment) override
    {
        return ::operator new(size * n, static_cast<std::align_val_t>(alignment));
    }

    void deallocate(void* p, size_t size, size_t n, size_t alignment) noexcept override
    {
        ::operator delete(p, size * n, static_cast<std::align_val_t>(alignment));
    }
};

template <typename T>
struct block
{
    block(uint8_t const n, allocator& alloc)
        : m_data(alloc.allocate<T>(pow2(n) * pow2(n)))
        , m_allocator(&alloc)
        , m_size(n)
    {
    }

    ~block()
    {
        m_allocator->deallocate(m_data, pow2(m_size) * pow2(m_size));
    }

    T*         m_data;
    allocator* m_allocator;
    uint8_t    m_size;
};



template <typename T>
struct uniform_chunk
{
    uniform_chunk(uint8_t const n, T const value) noexcept
        : m_value(value)
        , m_size(pow2(n))
    {
    }

    T        m_value;
    uint32_t m_size;
};


template <typename T>
struct dense_chunk
{
    dense_chunk(uint32_t const n, allocator& alloc)
        : m_data(alloc.allocate<T>(n * n))
        , m_allocator(&alloc)
        , m_size(n)
    {
    }

    ~dense_chunk()
    {
        m_allocator->deallocate(m_data, m_size * m_size);
    }

    T*         m_data;
    allocator* m_allocator;
    uint32_t   m_size;
};

template <typename T>
struct composite_chunk
{
    using node_t = std::variant<uniform_chunk<T>, dense_chunk<T>>;

    std::array<node_t, 4> m_data;
};




} // namespace asr
