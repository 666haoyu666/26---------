#ifndef __COMPILE_CONFIG_H__
#define __COMPILE_CONFIG_H__

/*
 * 编译级配置：与"怎么编"相关的开关（Debug/Release、断言、优化提示等）。
 */

/* 构建类型：默认 Debug；Release 时可在工具链里定义 NDEBUG */
#ifndef NDEBUG
#define BUILD_DEBUG   1
#else
#define BUILD_DEBUG   0
#endif

/* 断言开关（实现待定，可接 RTT / 串口 / 死循环） */
#if BUILD_DEBUG
#define PLATFORM_ASSERT(expr)   do { if (!(expr)) { for (;;) {} } } while (0)
#else
#define PLATFORM_ASSERT(expr)   ((void)0)
#endif

#endif /* __COMPILE_CONFIG_H__ */
