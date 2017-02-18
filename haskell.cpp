#include <type_traits>


// Bit definition

struct Zero {};
struct One {};


// Generic list definition

// The `:` operator known from Haskell
template <class Head, class Tail>
struct Cons {
    typedef Head head;
    typedef Tail tail;
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


// Pair - Syntactic sugar for two-element list

template<class Left, class Right>
struct Pair : public Cons<Left, Cons<Right, Leaf> > {
    typedef Left left;
    typedef Right right;
};


// ZipWithDefault

// Neither of the lists has ended
template<class ListA, class ListB, class Default, class Agg>
struct ZipWithDefaultHelper{
    typedef
        typename ZipWithDefaultHelper<
            typename ListA::tail,
            typename ListB::tail,
            Default,
            Cons<
                Pair<typename ListA::head, typename ListB::head>,
                Agg
            >
        >::result
    result;
};

// Both lists have ended
template<class Default, class Agg>
struct ZipWithDefaultHelper<Leaf, Leaf, Default, Agg> {
    typedef
        typename Reverse<Agg>::result
    result;
};

// Left list has ended
template<class ListB, class Default, class Agg>
struct ZipWithDefaultHelper<Leaf, ListB, Default, Agg> {
    typedef
        typename ZipWithDefaultHelper<
            Leaf,
            typename ListB::tail,
            Default,
            Cons<
                Pair<Default, typename ListB::head>,
                Agg
            >
        >::result
    result;
};

// Right list has ended
template<class ListA, class Default, class Agg>
struct ZipWithDefaultHelper<ListA, Leaf, Default, Agg> {
    typedef
        typename ZipWithDefaultHelper<
            typename ListA::tail,
            Leaf,
            Default,
            Cons<
                Pair<typename ListA::head, Default>,
                Agg
            >
        >::result
    result;
};

template<class ListA, class ListB, class Default>
using ZipWithDefault = ZipWithDefaultHelper<ListA, ListB, Default, Leaf>;

namespace {
    typedef Cons<One, Cons<Zero, Cons<One, Leaf>>> a101;
    typedef Cons<Zero, Cons<Zero, Cons<One, Cons<One, Leaf>>>> b0011;
    typedef typename ZipWithDefault<a101, b0011, Zero>::result zipped;
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

template<class A>
struct Neg { };

template<>
struct Neg<Zero> { typedef One result; };

template<>
struct Neg<One> { typedef Zero result; };

template<class A, class B>
struct Xor { };

template<class B>
struct Xor<Zero, B> { typedef B result; };

template<class B>
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
    typedef
        typename FoldL<
            Func,
            typename Func<Acc, typename List::head>::result,
            typename List::tail
        >::result
    result;
};

template<template <class, class> typename Func, class Acc>
struct FoldL<Func, Acc, Leaf> {
    typedef Acc result;
};

namespace {
    typedef Leaf a;
    typedef Cons<One, a> a1;
    typedef Cons<Zero, a1> a01;
    typedef Cons<Zero, a01> a001;
    typedef Cons<One, a001> a1001;
    template<class List>
    using XorFoldL = FoldL<Xor, Zero, List>;
    static_assert(std::is_same<XorFoldL<a>::result, Zero>::value, "a");
    static_assert(std::is_same<XorFoldL<a1>::result, One>::value, "a");
    static_assert(std::is_same<XorFoldL<a01>::result, One>::value, "a");
    static_assert(std::is_same<XorFoldL<a001>::result, One>::value, "a");
    static_assert(std::is_same<XorFoldL<a1001>::result, Zero>::value, "a");
};

// Addition

/*
template<class A, class B>
struct Add {
    typedef
        typename FoldL<
            AddH,
            typename ZipWithDefault<
                typename Reverse<A>::result,
                typename Reverse<B>::result,
                Zero
            >::result,
            Leaf
    result;
};
*/

int main(int argc, char *argv[]) {
    return 0;
}

