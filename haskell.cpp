#include <type_traits>


// Bit definition

struct Zero {};
struct One {};

template<class A>
concept bool Bit = std::is_same<A, Zero>::value || std::is_same<A, One>::value;

// Generic list definition

// The `:` operator known from Haskell
template <class Head, class Tail>
struct Cons {
    using head = Head;
    using tail = Tail;
};

struct Leaf {};


// Reversal

template <class List, class Agg>
struct ReverseHelper{
    using result = 
        typename ReverseHelper<
            typename List::tail,
            Cons<
                typename List::head,
                Agg
            >
        >::result;
};

template<class Agg>
struct ReverseHelper<Leaf, Agg>{
    using result = Agg;
};

template<class List>
using Reverse = typename ReverseHelper<List, Leaf>::result;

namespace {
    using a011 = Cons<Zero, Cons<One, Cons<One, Leaf>>>;
    using r011 = Reverse<a011>;
    static_assert(std::is_same<r011::head, One>::value, "a");
    static_assert(std::is_same<r011::tail::head, One>::value, "a");
    static_assert(std::is_same<r011::tail::tail::head, Zero>::value, "a");
    static_assert(std::is_same<r011::tail::tail::tail, Leaf>::value, "a");
};


// Pair - Syntactic sugar for two-element list

template<class Left, class Right>
struct Pair : public Cons<Left, Cons<Right, Leaf> > {
    using left = Left;
    using right = Right;
};


// ZipWithDefault

// Neither of the lists has ended
template<class ListA, class ListB, class Default, class Agg>
struct ZipWithDefaultHelper{
    using result = 
        typename ZipWithDefaultHelper<
            typename ListA::tail,
            typename ListB::tail,
            Default,
            Cons<
                Pair<typename ListA::head, typename ListB::head>,
                Agg
            >
        >::result;
};

// Both lists have ended
template<class Default, class Agg>
struct ZipWithDefaultHelper<Leaf, Leaf, Default, Agg> {
    using result = Reverse<Agg>;
};

// Left list has ended
template<class ListB, class Default, class Agg>
struct ZipWithDefaultHelper<Leaf, ListB, Default, Agg> {
    using result = 
        typename ZipWithDefaultHelper<
            Leaf,
            typename ListB::tail,
            Default,
            Cons<
                Pair<Default, typename ListB::head>,
                Agg
            >
        >::result;
};

// Right list has ended
template<class ListA, class Default, class Agg>
struct ZipWithDefaultHelper<ListA, Leaf, Default, Agg> {
    using result = 
        typename ZipWithDefaultHelper<
            typename ListA::tail,
            Leaf,
            Default,
            Cons<
                Pair<typename ListA::head, Default>,
                Agg
            >
        >::result;
};

template<class ListA, class ListB, class Default>
using ZipWithDefault = typename ZipWithDefaultHelper<ListA, ListB, Default, Leaf>::result;

namespace {
    using a101 = Cons<One, Cons<Zero, Cons<One, Leaf>>>;
    using b0011 = Cons<Zero, Cons<Zero, Cons<One, Cons<One, Leaf>>>>;
    using zipped = ZipWithDefault<a101, b0011, Zero>;
    static_assert(std::is_same<
        zipped::head,
        Pair<One, Zero>
    >::value, "a");
    static_assert(std::is_same<
        zipped::tail::head,
        Pair<Zero, Zero>
    >::value, "a");
    static_assert(std::is_same<
        zipped::tail::tail::head,
        Pair<One, One>
    >::value, "a");
    static_assert(std::is_same<
        zipped::tail::tail::tail::head,
        Pair<Zero, One>
    >::value, "a");
    static_assert(std::is_same<
        zipped::tail::tail::tail::tail,
        Leaf
    >::value, "a");
        
};



// Xor

template<Bit A>
struct Neg { };

template<>
struct Neg<Zero> { typedef One result; };

template<>
struct Neg<One> { typedef Zero result; };

template<Bit A, Bit B>
struct Xor { };

template<Bit B>
struct Xor<Zero, B> { typedef B result; };

template<Bit B>
struct Xor<One, B> { typedef typename Neg<B>::result result; };

namespace {
    static_assert(std::is_same<Xor<Zero,Zero>::result, Zero>::value, "a");
    static_assert(std::is_same<Xor<One,Zero>::result, One>::value, "a");
    static_assert(std::is_same<Xor<Zero,One>::result, One>::value, "a");
    static_assert(std::is_same<Xor<One,One>::result, Zero>::value, "a");
};


// FoldL

template<template <class, class> typename Func, class Acc, class List>
struct FoldL {
    using result = 
        typename FoldL<
            Func,
            typename Func<Acc, typename List::head>::result,
            typename List::tail
        >::result;
};

template<template <class, class> typename Func, class Acc>
struct FoldL<Func, Acc, Leaf> {
    using result = Acc;
};

namespace {
    using a = Leaf;
    using a1 = Cons<One, a>;
    using a01 = Cons<Zero, a1>;
    using a001 = Cons<Zero, a01>;
    using a1001 = Cons<One, a001>;
    template<class List>
    using XorFoldL = FoldL<Xor, Zero, List>;
    static_assert(std::is_same<XorFoldL<a>::result, Zero>::value, "a");
    static_assert(std::is_same<XorFoldL<a1>::result, One>::value, "a");
    static_assert(std::is_same<XorFoldL<a01>::result, One>::value, "a");
    static_assert(std::is_same<XorFoldL<a001>::result, One>::value, "a");
    static_assert(std::is_same<XorFoldL<a1001>::result, Zero>::value, "a");
};

// Addition

// Acc is a pair <result_so_far, carry-over>
/*
template<class Acc, class PairOfBits>
struct AddH {
    using su
};

template<class A, class B>
struct Add {
    using resultWithCarryOver = 
        typename FoldL<
            AddH,
            typename ZipWithDefault<
                typename Reverse<A>::result,
                typename Reverse<B>::result,
                Pair<Zero, Leaf>
            >::result,
            Leaf
        >::result;
};
*/

int main(int argc, char *argv[]) {
    return 0;
}

