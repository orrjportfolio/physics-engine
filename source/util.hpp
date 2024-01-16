#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>

static char *strLoad(char const *path, size_t *oLen) {
	FILE *f = fopen(path, "rb");
	assert(f != nullptr);
	
	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	assert(len >= 0);
	
	char *str = (char*)malloc(len + 1);
	fseek(f, 0, SEEK_SET);
	fread(str, 1, len, f);
	str[len] = '\0';
	
	fclose(f);
	
	if (oLen) { *oLen = len; }
	
	return str;
}
