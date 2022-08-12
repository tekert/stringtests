
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

char** output = NULL;

void setup_c_out(const size_t strlen)
{
	//output = (char**)malloc((strlen + 1) * sizeof(char*));
	output = (char**)calloc(strlen / 2 + 1, sizeof(char*));
}
void free_c_strs(char** output)
{
	for (int i = 0; output[i]; i++)
	{
		free(output[i]);
	}
}

char** split_fromc(const char* str, const size_t strlen, const char* delims)
{
	//char** output = (char**)calloc(strlen/2 +1, sizeof(char*));
	//char** output = (char**)malloc((strlen / 2 + 1) * sizeof(char*));
	//free_c_strs(output);

	size_t i = 0;
	size_t size = 0;
	while (*str)
	{
		for (const char* delim2 = delims; *delim2; delim2++)
		{
			if (*str == *delim2)
			{
				if (size) {
					const char * substr = str - size;
					char* ss = malloc(size+1);

					memcpy(ss, substr, size);
					ss[size] = '\0';

					output[i++] = ss;

					size = 0;
				}
				goto aca;
			}
		}
		size++;
	aca:
		str++;
	}
	if (size) {
		const char* substr = str - size;
		char* ss = malloc(size + 1);

		memcpy(ss, substr, size);
		ss[size] = '\0';

		output[i++] = ss;
	}
	output[i] = NULL;

	return output;
}

inline static bool c_is_delim(const char tst, const char* DELIMS)
{
	//const char* DELIMS = " \n\t\r\f";
	//const char* DELIMS = g_delim;
	do // Delimiter string cannot be empty, so don't check for it
	{
		if (tst == *DELIMS)
			return true;
		++DELIMS;
	} while (*DELIMS);

	return false;
}

#if 0
std::vector<std::string> split_Cway2(const std::string& str, const std::string& delims = "or")
{
	std::vector<std::string> output;
	output.reserve(str.length() / 2);

	const char* p = str.c_str();
	const char* delim = delims.c_str();

	size_t size = 0;
	while (*p)
	{
		if (c_is_delim(*p, delim))
		{
			if (size) {
				output.emplace_back(p - size, size);
				size = 0;
			}
		}
		else
			size++;
		p++;
	}
	if (size) {
		output.emplace_back(p - size, size);
	}

	return output;
}
#endif