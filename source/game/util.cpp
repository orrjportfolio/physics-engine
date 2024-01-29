#include "util.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>

namespace Util {
	char *loadStr(char const *path, size_t *oLen) {
		auto f = fopen(path, "rb");
		assert(f != nullptr);
		
		fseek(f, 0, SEEK_END);
		auto len = ftell(f);
		
		auto str = (char*)malloc(len + 1);
		fseek(f, 0, SEEK_SET);
		fread(str, 1, len, f);
		str[len] = '\0';
		
		if (oLen) { *oLen = len; }
		return str;
	}
}
