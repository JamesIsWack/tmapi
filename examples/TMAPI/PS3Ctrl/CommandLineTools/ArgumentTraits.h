/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011, Sony Computer Entertainment Inc. / SN Systems Limited
//
/////////////////////////////////////////////////////////////////////////

#ifndef ARGUMENT_TRAITS_H
#define ARGUMENT_TRAITS_H

#include <string>
#include <windows.h>
namespace commandargutils
{
	struct PrimitiveType {
		typedef PrimitiveType TypeClass;
	};

	struct StringType {
		typedef StringType TypeClass;
	};

	struct IntegerType {
		typedef IntegerType TypeClass;
	};

	template<typename T>
	struct ArgTraits {
		typedef typename T::TypeClass TypeClass;
	};

	template<>
	struct ArgTraits<double> {
		typedef PrimitiveType TypeClass;
	};

	template<>
	struct ArgTraits<float> {
		typedef PrimitiveType TypeClass;
	};

	template<>
	struct ArgTraits<unsigned char> {
		typedef PrimitiveType TypeClass;
	};

	template<>
	struct ArgTraits<char> {
		typedef PrimitiveType TypeClass;
	};

	template<>
	struct ArgTraits<long> {
		typedef PrimitiveType TypeClass;
	};

	// strings
	template<>
	struct ArgTraits<std::string> {
		typedef StringType TypeClass;
	};

	// integers - use a special integer extraction function that supports hex reading
	template<>
	struct ArgTraits<UINT32> {
		typedef IntegerType TypeClass;
	};

	template<>
	struct ArgTraits<int> {
		typedef IntegerType TypeClass;
	};

	template<>
	struct ArgTraits<UINT64> {
		typedef IntegerType TypeClass;
	};

}; // end of namespace
#endif