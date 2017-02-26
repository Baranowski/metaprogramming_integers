// For testing

template<class A, class B>
struct IsSame {
    static constexpr bool value = false;
};

template<class A>
struct IsSame<A, A> {
    static constexpr bool value = true;
};


// Id

template<class A>
struct Id { using result = A; };

// Bit definition

struct Zero {};
struct One {};

template<class A>
concept bool Bit = IsSame<A, Zero>::value || IsSame<A, One>::value;

// Generic list definition

// The `:` operator known from Haskell
template <class Head, class Tail>
struct Cons {
    using head = Head;
    using tail = Tail;
    using result = Cons<Head, Tail>;
};

struct Leaf {};

template<class List>
struct Head {
    using result = typename List::head;
};


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
using Reverse = ReverseHelper<List, Leaf>;

namespace test_Reverse {
    using a011 = Cons<Zero, Cons<One, Cons<One, Leaf>>>;
    using r011 = typename Reverse<a011>::result;
    static_assert(IsSame<r011::head, One>::value);
    static_assert(IsSame<r011::tail::head, One>::value);
    static_assert(IsSame<r011::tail::tail::head, Zero>::value);
    static_assert(IsSame<r011::tail::tail::tail, Leaf>::value);
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
    using result = typename Reverse<Agg>::result;
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
using ZipWithDefault = ZipWithDefaultHelper<ListA, ListB, Default, Leaf>;

namespace test_ZipWithDefault {
    using a101 = Cons<One, Cons<Zero, Cons<One, Leaf>>>;
    using b0011 = Cons<Zero, Cons<Zero, Cons<One, Cons<One, Leaf>>>>;
    using zipped = typename ZipWithDefault<a101, b0011, Zero>::result;
    static_assert(IsSame<
        zipped::head,
        Pair<One, Zero>
    >::value);
    static_assert(IsSame<
        zipped::tail::head,
        Pair<Zero, Zero>
    >::value);
    static_assert(IsSame<
        zipped::tail::tail::head,
        Pair<One, One>
    >::value);
    static_assert(IsSame<
        zipped::tail::tail::tail::head,
        Pair<Zero, One>
    >::value);
    static_assert(IsSame<
        zipped::tail::tail::tail::tail,
        Leaf
    >::value);
        
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

namespace test_Xor {
    static_assert(IsSame<Xor<Zero,Zero>::result, Zero>::value);
    static_assert(IsSame<Xor<One,Zero>::result, One>::value);
    static_assert(IsSame<Xor<Zero,One>::result, One>::value);
    static_assert(IsSame<Xor<One,One>::result, Zero>::value);
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

namespace test_FoldL {
    using a = Leaf;
    using a1 = Cons<One, a>;
    using a01 = Cons<Zero, a1>;
    using a001 = Cons<Zero, a01>;
    using a1001 = Cons<One, a001>;
    template<class List>
    using XorFoldL = FoldL<Xor, Zero, List>;
    static_assert(IsSame<XorFoldL<a>::result, Zero>::value);
    static_assert(IsSame<XorFoldL<a1>::result, One>::value);
    static_assert(IsSame<XorFoldL<a01>::result, One>::value);
    static_assert(IsSame<XorFoldL<a001>::result, One>::value);
    static_assert(IsSame<XorFoldL<a1001>::result, Zero>::value);
};


// Map

template<template <class> class Func, class List>
struct Map {
    using result =
        Cons<
            typename Func<
                typename List::head
            >::result,
            typename Map<
                Func,
                typename List::tail
            >::result
        >;
};

template<template <class> class Func>
struct Map<Func, Leaf> {
    using result = Leaf;
};

namespace test_Map {
    using empty = Leaf;
    static_assert(IsSame<empty, Map<Neg, empty>::result>::value);

    using a1 = Cons<One, Leaf>;
    using a0 = Cons<Zero, Leaf>;
    static_assert(IsSame<a0, Map<Neg, a1>::result>::value);

    using a1101 = Cons<One, Cons<One, Cons<Zero, Cons<One, Leaf>>>>;
    using negated = Map<Neg, a1101>::result;
    using a0010 = Cons<Zero, Cons<Zero, Cons<One, Cons<Zero, Leaf>>>>;
    static_assert(IsSame<negated, a0010>::value);
};


// IfSameThenElse

template<class Left, class Right, class Then, class Else>
struct IfSameThenElse {
    using result = Else;
};

template<class Both, class Then, class Else>
struct IfSameThenElse<Both, Both, Then, Else> {
    using result = Then;
};

namespace test_IfSameThenElse {
    static_assert(IsSame<IfSameThenElse<One, One, One, Zero>::result, One>::value);
    static_assert(IsSame<IfSameThenElse<One, Zero, One, Zero>::result, Zero>::value);
};

// Addition


// Just a helper value used internally for adding
struct Two {};

template<class A>
struct SumBits { };

template<class A>
struct SumBits<Pair<Zero, A>> {
    using result = A;
};

template<>
struct SumBits<Pair<One, Zero>> {
    using result = One;
};

template<>
struct SumBits<Pair<One, One>> {
    using result = Two;
};

template<class CarryOverAndAcc, class NextBit>
struct AddHelper {};

template<class CarryOver, class Acc>
struct AddHelper<Pair<CarryOver, Acc>, Two> {
    using result =
        Pair<
            One,
            Cons<CarryOver, Acc>
        >;
};

template<class Acc>
struct AddHelper<Pair<One, Acc>, One> {
    using result =
        typename AddHelper<Pair<Zero, Acc>, Two>::result;
};

template<class Acc>
struct AddHelper<Pair<Zero, Acc>, One> {
    using result =
        Pair<
            Zero,
            Cons<One, Acc>
        >;
};

template<class CarryOver, class Acc>
struct AddHelper<Pair<CarryOver, Acc>, Zero> {
    using result =
        Pair<
            Zero,
            Cons<CarryOver, Acc>
        >;
};

template<class A, class B>
struct Add {
    using zippedReversed = 
        typename ZipWithDefault<
            typename Reverse<A>::result,
            typename Reverse<B>::result,
            Zero
        >::result;

    using resultReversedWithTwos = 
        typename Map<
            SumBits,
            zippedReversed
        >::result;

    using carryOverAndResult =
        typename FoldL<
            AddHelper,
            Pair<Zero, Leaf>,
            resultReversedWithTwos
        >::result;

    using result =
        typename IfSameThenElse<
            typename carryOverAndResult::left,
            One,
            Cons<One, typename carryOverAndResult::right>,
            typename carryOverAndResult::right
        >::result;
};

namespace test_Add {
    using a1 = Cons<One, Leaf>;
    using a1plus1 = Add<a1, a1>;
    static_assert(IsSame<a1plus1::zippedReversed, Cons<Pair<One, One>, Leaf>>::value);
    static_assert(IsSame<a1plus1::resultReversedWithTwos, Cons<Two, Leaf>>::value);

    using a10011 = Cons<One, Cons<Zero, Cons<Zero, Cons<One, Cons<One, Leaf>>>>>;
    using a11 = Cons<One, Cons<One, Leaf>>;
    using a10110 = Cons<One, Cons<Zero, Cons<One, Cons<One, Cons<Zero, Leaf>>>>>;
    static_assert(IsSame<Add<a10011, a11>::result, a10110>::value);

    using a01100 = typename Map<Neg, a10011>::result;
    using a11111 = Cons<One, Cons<One, Cons<One, Cons<One, Cons<One, Leaf>>>>>;
    static_assert(IsSame<Add<a01100, a10011>::result, a11111>::value);

    using a101011 = Cons<One, Cons<Zero, Cons<One, Cons<Zero, Cons<One, Cons<One, Leaf>>>>>>;
    static_assert(IsSame<Add<a01100, a11111>::result, a101011>::value);
};


// Concat

template<class A, class B>
struct ConcatHelper {
    using result = Cons<B, A>;
};

template<class A, class B>
struct Concat {
    using reversed =
        typename FoldL<
            ConcatHelper,
            typename Reverse<A>::result,
            B
        >::result;

    using result = typename Reverse< reversed >::result;
};

namespace test_Concat {
    using a010 = Cons<Zero, Cons<One, Cons<Zero, Leaf>>>;
    using a11 = Cons<One, Cons<One, Leaf>>;
    using a01011 = Cons<Zero, Cons<One, Cons<Zero, Cons<One, Cons<One, Leaf>>>>>;
    static_assert(IsSame<Concat<a010, a11>::result, a01011>::value);
};

// Filter

template<template <class A> class Func, class Standard, class List>
struct Filter {
    using subresult = typename Filter<Func, Standard, typename List::tail>::result;
    using result =
        typename IfSameThenElse<
            typename Func<
                typename List::head
            >::result,
            Standard,
            Cons<
                typename List::head,
                subresult
            >,
            subresult
        >::result;
};

template<template <class A> class Func, class Standard>
struct Filter<Func, Standard, Leaf> {
    using result = Leaf;
};

namespace test_Filter {

    using a10110 = Cons<One, Cons<Zero, Cons<One, Cons<One, Cons<Zero, Leaf>>>>>;
    using a111 = Cons<One, Cons<One, Cons<One, Leaf>>>;
    using a00 = Cons<Zero, Cons<Zero, Leaf>>;

    static_assert(IsSame<Filter<Id, One, a10110>::result, a111>::value);
};


// Tails

template<class List>
struct Tails {
    using subresult = typename Tails<typename List::tail>::result;
    using result =
        Cons<
            List,
            subresult
        >;
};

template<>
struct Tails<Leaf> {
    using result = Leaf;
};

namespace test_Tails {
    using a110 = Cons<One, Cons<One, Cons<Zero, Leaf>>>;
    static_assert(IsSame<Tails<a110>::result, Cons<a110, Cons<a110::tail, Cons<a110::tail::tail, Leaf>>>>::value);
};

// Multiply

/*
template<class A, class B>
struct Multiply{
    using significant_tails =
        typename Filter<
            Head,
            One,
            typename Tails<B>::result
        >::result;

    using zeroes = Map<TrimAndZeroify, significant_tails>::result;
    template<class Right>
    ConcatA = Concat<A, Right>;
    using shifted = Map<ConcatA, zeroes>::result;
    using result = typename FoldL<Add, Leaf, shifted>::result;
};
*/

int main(int argc, char *argv[]) {
    return 0;
}

