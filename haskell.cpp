#include <type_traits>

typedef unsigned char uint8_t;

// List of uint8_t definition:

// The `:` operator known from Haskell, working only on the uint8_t type
template <uint8_t H, class TAIL>
struct Cons {
    static constexpr uint8_t head = H;
    typedef TAIL tail;
};

struct Leaf {};

// Reversal

template <class List, class Agg>
struct Reverse{
    typedef
        typename Reverse<
            typename List::tail,
            Cons<
                List::head,
                Agg
            >
        >::result
    result;
};

template<class Agg>
struct Reverse<Leaf, Agg>{
    typedef
        Agg
    result;
};

namespace {
    typedef Cons<1, Cons<2, Cons<3, Leaf>>> a123;
    typedef typename Reverse<a123, Leaf>::result r123;
    static_assert(r123::head == 3, "a");
    static_assert(r123::tail::head == 2, "a");
    static_assert(r123::tail::tail::head == 1, "a");
    static_assert(std::is_same<r123::tail::tail::tail, Leaf>::value, "a");
};

int main(int argc, char *argv[]) {
    return 0;
}

