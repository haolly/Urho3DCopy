//
// Created by liuhao on 2019-02-23.
//

#ifndef URHO3DCOPY_FLAGSET_HPP
#define URHO3DCOPY_FLAGSET_HPP

#include <type_traits>
#include <tclDecls.h>

namespace Urho3D
{

	template <typename T> struct IsFlagSet
	{
		constexpr static bool value_ = false;
	};

#define URHO3D_ENABLE_FLAGSET(enumName) \
	template<> struct IsFlagSet<enumName> { constexpr static bool value_ = true; }

#define URHO3D_FLAGSET(enumName, flagsetName) \
URHO3D_ENABLE_FLAGSET(enumName); \
using flagsetName = FlagSet<enumName>


	template <class E, class = typename std::enable_if<IsFlagSet<E>::value_>::type>
	class FlagSet
	{
	public:
		using Enum = E;
		using Integer = typename std::underlying_type<Enum>::type;

	public:
		explicit FlagSet(Integer value) :
						value_(value)
		{
		}

		FlagSet() = default;

		FlagSet(const FlagSet& another) = default;

		FlagSet(const Enum value) :
				value_(static_cast<Integer>(value))
		{}

		FlagSet&operator = (const FlagSet& rhs) = default;

		//todo

	protected:
		Integer value_;
	};

}



#endif //URHO3DCOPY_FLAGSET_HPP
