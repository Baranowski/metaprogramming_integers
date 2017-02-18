#include <type_traits>

typedef unsigned char uint8_t;

// Bit definition

struct Zero {};
struct One {};

// Generic list definition

// The `:` operator known from Haskell
template <class HEAD, class TAIL>
struct Cons {
    typedef HEAD head;
    typedef TAIL tail;
};

struct Leaf {};

// Reversal

template <class List, class Agg>
struct ReverseHelper{
    typedef
        typename ReverseHelper<
            typename List::tail,
            Cons<
                typename List::head,
                Agg
            >
        >::result
    result;
};

template<class Agg>
struct ReverseHelper<Leaf, Agg>{
    typedef
        Agg
    result;
};

template<class List>
using Reverse = ReverseHelper<List, Leaf>;

namespace {
    typedef Cons<Zero, Cons<One, Cons<One, Leaf>>> a011;
    typedef typename Reverse<a011>::result r011;
    static_assert(std::is_same<r011::head, One>::value, "a");
    static_assert(std::is_same<r011::tail::head, One>::value, "a");
    static_assert(std::is_same<r011::tail::tail::head, Zero>::value, "a");
    static_assert(std::is_same<r011::tail::tail::tail, Leaf>::value, "a");
};

int main(int argc, char *argv[]) {
    return 0;
}

