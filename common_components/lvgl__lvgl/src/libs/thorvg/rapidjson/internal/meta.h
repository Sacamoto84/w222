// Tencent рада поддержать сообщество открытого исходного кода, созданного доступным RapidJSON.
//
// Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip.
//
// Лицензия MIT («Лицензия»); вы не можете использовать этот файл, за исключением
// в соответствии с Лицензией. Вы можете получить копию Лицензии по адресу
//
// http://opensource.org/licenses/MIT
//
// Если это не требуется действующим законодательством или не согласовано в письменной форме, распространяемое программное обеспечение
// по Лицензии распространяется на " AS IS " BASIS , WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND , явный или подразумеваемый. См. Лицензию на
// конкретный язык, регулирующий разрешения и ограничения по Лицензии.

#ifndef RAPIDJSON_INTERNAL_META_H_
#define RAPIDJSON_INTERNAL_META_H_

#include "../rapidjson.h"

#ifdef __GNUC__
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)
#endif

#if defined(_MSC_VER) && !defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(6334)
#endif

#if RAPIDJSON_HAS_CXX11_TYPETRAITS
#include <type_traits>
#endif

//@cond RAPIDJSON_INTERNAL
RAPIDJSON_NAMESPACE_BEGIN
namespace internal {

// Помощник для переноса/преобразования произвольных типов в пустоту, используемый для обсуждения произвольных типов.
template <typename T> struct Void { typedef void Type; };

///////////////////////////////////////////////////////////////////////////////
// БулТип, Истинный Тип, Ложный Тип
//
template <bool Cond> struct BoolType {
    static const bool Value = Cond;
    typedef BoolType Type;
};
typedef BoolType<true> TrueType;
typedef BoolType<false> FalseType;


///////////////////////////////////////////////////////////////////////////////
// SelectIf, BoolExpr, NotExpr, AndExpr, OrExpr
//

template <bool C> struct SelectIfImpl { template <typename T1, typename T2> struct Apply { typedef T1 Type; }; };
template <> struct SelectIfImpl<false> { template <typename T1, typename T2> struct Apply { typedef T2 Type; }; };
template <bool C, typename T1, typename T2> struct SelectIfCond : SelectIfImpl<C>::template Apply<T1,T2> {};
template <typename C, typename T1, typename T2> struct SelectIf : SelectIfCond<C::Value, T1, T2> {};

template <bool Cond1, bool Cond2> struct AndExprCond : FalseType {};
template <> struct AndExprCond<true, true> : TrueType {};
template <bool Cond1, bool Cond2> struct OrExprCond : TrueType {};
template <> struct OrExprCond<false, false> : FalseType {};

template <typename C> struct BoolExpr : SelectIf<C,TrueType,FalseType>::Type {};
template <typename C> struct NotExpr  : SelectIf<C,FalseType,TrueType>::Type {};
template <typename C1, typename C2> struct AndExpr : AndExprCond<C1::Value, C2::Value>::Type {};
template <typename C1, typename C2> struct OrExpr  : OrExprCond<C1::Value, C2::Value>::Type {};


///////////////////////////////////////////////////////////////////////////////
// Аддконст, возможно, аддконст, удалениеконст
template <typename T> struct AddConst { typedef const T Type; };
template <bool Constify, typename T> struct MaybeAddConst : SelectIfCond<Constify, const T, T> {};
template <typename T> struct RemoveConst { typedef T Type; };
template <typename T> struct RemoveConst<const T> { typedef T Type; };


///////////////////////////////////////////////////////////////////////////////
// IsSame, IsConst, IsMoreConst, IsPointer
//
template <typename T, typename U> struct IsSame : FalseType {};
template <typename T> struct IsSame<T, T> : TrueType {};

template <typename T> struct IsConst : FalseType {};
template <typename T> struct IsConst<const T> : TrueType {};

template <typename CT, typename T>
struct IsMoreConst
    : AndExpr<IsSame<typename RemoveConst<CT>::Type, typename RemoveConst<T>::Type>,
              BoolType<IsConst<CT>::Value >= IsConst<T>::Value> >::Type {};

template <typename T> struct IsPointer : FalseType {};
template <typename T> struct IsPointer<T*> : TrueType {};

///////////////////////////////////////////////////////////////////////////////
// IsBaseOf
//
#if RAPIDJSON_HAS_CXX11_TYPETRAITS

template <typename B, typename D> struct IsBaseOf
    : BoolType< ::std::is_base_of<B,D>::value> {};

#else // упрощенная версия, заимствованная из Boost

template<typename B, typename D> struct IsBaseOfImpl {
    RAPIDJSON_STATIC_ASSERT(sizeof(B) != 0);
    RAPIDJSON_STATIC_ASSERT(sizeof(D) != 0);

    typedef char (&Yes)[1];
    typedef char (&No) [2];

    template <typename T>
    static Yes Check(const D*, T);
    static No  Check(const B*, int);

    struct Host {
        operator const B*() const;
        operator const D*();
    };

    enum { Value = (sizeof(Check(Host(), 0)) == sizeof(Yes)) };
};

template <typename B, typename D> struct IsBaseOf
    : OrExpr<IsSame<B, D>, BoolExpr<IsBaseOfImpl<B, D> > >::Type {};

#endif // RAPIDJSON_HAS_CXX11_TYPETRAITS


//////////////////////////////////////////////////////////////////////////
// ВключитьЕсли/ВыключитьЕсли
//
template <bool Condition, typename T = void> struct EnableIfCond  { typedef T Type; };
template <typename T> struct EnableIfCond<false, T> { /* пустой */ };

template <bool Condition, typename T = void> struct DisableIfCond { typedef T Type; };
template <typename T> struct DisableIfCond<true, T> { /* пустой */ };

template <typename Condition, typename T = void>
struct EnableIf : EnableIfCond<Condition::Value, T> {};

template <typename Condition, typename T = void>
struct DisableIf : DisableIfCond<Condition::Value, T> {};

// SFINAE помощники
struct SfinaeTag {};
template <typename T> struct RemoveSfinaeTag;
template <typename T> struct RemoveSfinaeTag<SfinaeTag&(*)(T)> { typedef T Type; };

#define RAPIDJSON_REMOVEFPTR_(type) \
    typename ::RAPIDJSON_NAMESPACE::internal::RemoveSfinaeTag \
        < ::RAPIDJSON_NAMESPACE::internal::SfinaeTag&(*) type>::Type

#define RAPIDJSON_ENABLEIF(cond) \
    typename ::RAPIDJSON_NAMESPACE::internal::EnableIf \
        <RAPIDJSON_REMOVEFPTR_(cond)>::Type * = NULL

#define RAPIDJSON_DISABLEIF(cond) \
    typename ::RAPIDJSON_NAMESPACE::internal::DisableIf \
        <RAPIDJSON_REMOVEFPTR_(cond)>::Type * = NULL

#define RAPIDJSON_ENABLEIF_RETURN(cond,returntype) \
    typename ::RAPIDJSON_NAMESPACE::internal::EnableIf \
        <RAPIDJSON_REMOVEFPTR_(cond), \
         RAPIDJSON_REMOVEFPTR_(returntype)>::Type

#define RAPIDJSON_DISABLEIF_RETURN(cond,returntype) \
    typename ::RAPIDJSON_NAMESPACE::internal::DisableIf \
        <RAPIDJSON_REMOVEFPTR_(cond), \
         RAPIDJSON_REMOVEFPTR_(returntype)>::Type

} // внутреннее пространство имен
RAPIDJSON_NAMESPACE_END
//@endcond

#if defined(_MSC_VER) && !defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#ifdef __GNUC__
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_INTERNAL_META_H_
