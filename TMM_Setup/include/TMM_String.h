#pragma once

#include <memory>

namespace TMM
{
	
	class String
	{
		char* mpStr = nullptr;
		uint64_t mStrSize = 0;
		bool mOwner = false;

		bool IsOwner() { return mOwner; }

	public:

		String() = default;
		String(uint64_t size);	
		String(const char* pchars);
		String(char* pchars, uint64_t size);
		String(const String& other);				

		virtual ~String();

		char* data() { return mpStr; }
		const char* data() const { return mpStr; }
		uint64_t size() const { return mStrSize; }

		void Destroy();
		void Move(String& other);
		void Move(char* pChars, uint64_t size);

		String operator + (const String& other);
		String operator + (const char* other);

		void operator = (const char* other);
		void operator = (const String& other);

		char& operator[](uint64_t index);
	};

}
