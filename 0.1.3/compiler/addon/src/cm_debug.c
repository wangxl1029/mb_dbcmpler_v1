#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef WIN32
#include <windows.h>
#endif
int cm_debug_print(const char* f, size_t l, const char* fmt, ...)
{
	int len = 0;
	char buf[BUFSIZ];
	va_list varg;
	va_start(varg, fmt);
   #ifdef WIN32
	len = vsnprintf_s(buf, BUFSIZ, BUFSIZ, fmt, varg);
   #else
	len = vsnprintf(buf, BUFSIZ, fmt, varg);
   #endif
	va_end(varg);

	if (len > 0)
	{
      #ifdef WIN32
		OutputDebugStringA(buf);
      #else
      puts(buf);
      #endif
	}

	return len;
}