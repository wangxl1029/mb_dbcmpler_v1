#pragma once

#define CM_LOG_INFO(...) cm_debug_print(__FILE__, __LINE__, __VA_ARGS__);
#define CM_LOG_WARNING(...) cm_debug_print(__FILE__, __LINE__, __VA_ARGS__);
#define CM_LOG_ERROR(...) cm_debug_print(__FILE__, __LINE__, __VA_ARGS__);
#define CM_LOG_DEBUG(...) cm_debug_print(__FILE__, __LINE__, __VA_ARGS__);


#ifdef __cplusplus
extern "C"{
#endif
	int cm_debug_print(const char* f, size_t l, const char* fmt, ...);
#ifdef __cplusplus
};
#endif




