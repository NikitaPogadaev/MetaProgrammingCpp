#pragma once

#include <type_traits>
#include <cstdint>
#include <concepts>
#include <array>
#include <string_view>
#include <algorithm>

#include "../task3/type_tuples.hpp"
#include "../task3/value_types.hpp"






constexpr bool existEnum(const char* log){
    std::size_t ii = 0;
    for( ;log[ii] != '['; ++ii){}
    return (log[ii + 5] != '(');
}

template <auto T>
consteval auto helperValue(){
    return __PRETTY_FUNCTION__;
}

template <typename T>
constexpr auto helperType(){
    return __PRETTY_FUNCTION__;
}


template<type_tuples::TypeTuple TT1, type_tuples::TypeTuple TT2>
struct Concat;

template<typename... Pack1, typename... Pack2>
struct Concat<type_tuples::TTuple<Pack1...>, type_tuples::TTuple<Pack2...>>
{
  using Val = type_tuples::TTuple<Pack1..., Pack2...>;
};


template<int64_t L, int64_t R>
struct SignedIota{
    using Val = typename Concat<typename SignedIota<L, ((L + R) - int64_t((L + R)%2 != 0)) / 2>::Val, typename SignedIota<((L + R) - int64_t((L + R)%2 != 0)) / 2 + 1, R>::Val>::Val;
};

template<int64_t N>
struct SignedIota<N, N>{
    using Val = type_tuples::TTuple<value_types::ValueTag<N>>;
};


template<std::size_t L, std::size_t R>
struct UnsignedIota{
    using Val = typename Concat<typename UnsignedIota<L, (L + R)/ 2>::Val, typename UnsignedIota<(L + R) / 2 + 1, R>::Val>::Val;
};

template<std::size_t N>
struct UnsignedIota<N, N>{
    using Val = type_tuples::TTuple<value_types::ValueTag<N>>;
};





template <class Enum, std::size_t MAXN = 512>
	requires std::is_enum_v<Enum>
struct EnumeratorTraits {

    template<type_tuples::TypeTuple TT1, std::size_t N>
    struct LogHelper;

    template<typename... Args, std::size_t N>
    struct LogHelper<type_tuples::TTuple<Args...>, N> {
        static constexpr std::array<const char*, N> stuff {helperValue<static_cast<Enum>(
            static_cast<std::underlying_type_t<Enum>>(Args::Value)
            )>() ...};
    };


    static constexpr std::size_t size() noexcept {

        if constexpr(
            std::same_as<std::underlying_type_t<Enum>, signed char>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename SignedIota<-(int64_t)(std::min(std::size_t(128), MAXN)), (int64_t)(std::min(std::size_t(127), MAXN))>::Val, 
            std::min(std::size_t(128), MAXN) + 1 + std::min(std::size_t(127), MAXN)>::stuff;
            for(std::size_t i = 0; i <= std::min(std::size_t(128), MAXN) + std::min(std::size_t(127), MAXN); ++i){
                if(existEnum( logs[i])){
                    ++count;
                }
            }
            return count;
        }

        else if constexpr(
            std::same_as<std::underlying_type_t<Enum>, int>
            or std::same_as<std::underlying_type_t<Enum>, long long>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename SignedIota<-(int64_t)(MAXN), (int64_t)(MAXN)>::Val, 2*MAXN + 1>::stuff;

            for(std::size_t i = 0; i <= 2*MAXN; ++i){
                if(existEnum( logs[i])){
                    ++count;
                }
            }
            return count;
        }
        else if constexpr (
            !std::same_as<std::underlying_type_t<Enum>, unsigned char>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename UnsignedIota<0, MAXN>::Val, MAXN + 1>::stuff;
            for(std::size_t i = 0; i <= MAXN; ++i){
                if(existEnum( logs[i])) ++count;
            }
            return count;
        }
        else{
            std::size_t count = 0;
            const auto& logs = LogHelper<typename UnsignedIota<0, std::min(std::size_t(255), MAXN)>::Val, std::min(std::size_t(256), MAXN + 1)>::stuff;
            for(std::size_t i = 0; i <= std::min(std::size_t(255), MAXN); ++i){
                if(existEnum( logs[i])) ++count;
            }
            return count;
        }
    }

///////////////////////////////////////////////////////

    static constexpr Enum at(std::size_t ind) noexcept {
        if constexpr (
            std::same_as<std::underlying_type_t<Enum>, signed char>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename SignedIota<-(int64_t)(std::min(std::size_t(128), MAXN)), (int64_t)(std::min(std::size_t(127), MAXN))>::Val, 
            std::min(std::size_t(128), MAXN) + 1 + std::min(std::size_t(127), MAXN)>::stuff;
            for(std::size_t i = 0; i <= std::min(std::size_t(128), MAXN) + std::min(std::size_t(127), MAXN); ++i){
                if(existEnum( logs[i])) ++count;
                if(ind + 1 == count) return static_cast<Enum>((signed char)(i) - (signed char)(std::min(std::size_t(128), MAXN)));
            }
            return static_cast<Enum>(count);
        }
        else if constexpr(
            std::same_as<std::underlying_type_t<Enum>, int>
            or std::same_as<std::underlying_type_t<Enum>, long long>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename SignedIota<-(int64_t)(MAXN), (int64_t)(MAXN)>::Val, 2*MAXN + 1>::stuff;
            for(std::size_t i = 0; i <= 2*MAXN; ++i){
                if(existEnum( logs[i])) ++count;
                if(ind + 1 == count) return static_cast<Enum>((int64_t)(i) - (int64_t)(MAXN));
            }
            return static_cast<Enum>(count);
        }
        else if constexpr (
            !std::same_as<std::underlying_type_t<Enum>, unsigned char>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename UnsignedIota<0, MAXN>::Val, MAXN + 1>::stuff;
            for(std::size_t i = 0; i <= MAXN; ++i){
                if(existEnum( logs[i])) ++count;
                if(ind + 1 == count) return static_cast<Enum>(i);
            }
            return static_cast<Enum>(count);
        }
        else {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename UnsignedIota<0, std::min(std::size_t(255), MAXN)>::Val, std::min(std::size_t(256), MAXN + 1)>::stuff;
            for(std::size_t i = 0; i <= std::min(std::size_t(255), MAXN); ++i){
                if(existEnum( logs[i])) ++count;
                if(ind + 1 == count) return static_cast<Enum>(i);
            }
            return static_cast<Enum>(count);
        }
    }

////////////////////////////////////////////////////////////////////

    static constexpr std::string_view nameAt(std::size_t ind) noexcept {
        const char* chpretty = nullptr;


        if constexpr(
            std::same_as<std::underlying_type_t<Enum>, signed char>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename SignedIota<-(int64_t)(std::min(std::size_t(128), MAXN)), (int64_t)(std::min(std::size_t(127), MAXN))>::Val, 
            std::min(std::size_t(128), MAXN) + 1 + std::min(std::size_t(127), MAXN)>::stuff;
            for(std::size_t i = 0; i <= std::min(std::size_t(128), MAXN) + std::min(std::size_t(127), MAXN); ++i){
                if(existEnum( logs[i])) ++count;
                if(ind + 1 == count) {
                    chpretty = logs[i];
                    break;
                }
            }
        }

        else if constexpr(
            std::same_as<std::underlying_type_t<Enum>, int>
            or std::same_as<std::underlying_type_t<Enum>, long long>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename SignedIota<-(int64_t)(MAXN), (int64_t)(MAXN)>::Val, 2*MAXN + 1>::stuff;
            for(std::size_t i = 0; i <= 2*MAXN; ++i){
                if(existEnum( logs[i])) ++count;
                if(ind + 1 == count) {
                    chpretty = logs[i];
                    break;
                }
            }
        }
        else if constexpr(
            !std::same_as<std::underlying_type_t<Enum>, unsigned char>
        )
        {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename UnsignedIota<0, MAXN>::Val, MAXN + 1>::stuff;
            for(std::size_t i = 0; i <= MAXN; ++i){
                if(existEnum( logs[i])) ++count;
                if(ind + 1 == count) {
                    chpretty = logs[i];
                    break;
                }
            }
        }
        else {
            std::size_t count = 0;
            const auto& logs = LogHelper<typename UnsignedIota<0, std::min(std::size_t(255), MAXN)>::Val, std::min(std::size_t(256), MAXN + 1)>::stuff;
            for(std::size_t i = 0; i <= std::min(std::size_t(255), MAXN); ++i){
                if(existEnum( logs[i])) ++count;
                if(ind + 1 == count) {
                    chpretty = logs[i];
                    break;
                }
            }
        }


        std::size_t ii = 0;
        for( ;chpretty[ii] != '['; ++ii){}
        ii += 5;

        for(std::size_t kl = 1; chpretty[ii + kl] != ']'; ++kl){
            if(chpretty[ii + kl] == ':'){
                ii += (kl + 2);
                break;
            }
        }

        std::size_t jj = 0;
        for( ;chpretty[ii + jj] != ']'; ++jj){}

        return std::string_view(chpretty + ii, jj);
    }

private:




};