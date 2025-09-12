#pragma once

// ------------------------------------
// Resursive template to call function of each type
// ------------------------------------

// Usage
//  callback: return false to stop loop, param is nullptr to pass type
// 
//  IterateType::Iterate_Types<FirstType, SecondType, ...>(
//      []<typename Type>(Type * pType) {}
//  );
// 
//  it's recommend to use a macro for types to simplify call
//  #define TYPE FirstType, SecondType, ...
// 
//  you can wrap it to iterate sub types that depend on main type
// 
//  static void Iterate_SubTypes(auto callback) {
//      IterateType::Iterate_Types<TYPE>([&]<typename Type>(Type* pType) {
//          static_assert(HasSubType<Type>, "No Sub Type");
//      
//          using SubType = std::remove_cvref_t<decltype(pType->GetSubType())>;
//          SubType* pSubType = nullptr;
//      
//          return callback(pSubType);
//      });
//  }

namespace IterateType {
	template <typename T, typename... ParamTypes>
	void Iterate_TypesImpl(auto callback) {
		if (T* p = nullptr; !callback(p)) { return; }

		if constexpr (sizeof...(ParamTypes) > 0) {
			Iterate_TypesImpl<ParamTypes...>(callback);
		}
	}

	template <typename... ParamTypes>
	void Iterate_Types(auto callback) {
		if constexpr (sizeof...(ParamTypes) > 0) {
			Iterate_TypesImpl<ParamTypes...>(callback);
		}
	}
}
