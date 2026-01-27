#pragma once

#ifndef MACRO_DEBUG_H
#define MACRO_DEBUG_H

#ifdef DEBUG
#define DEBUG_STATIC_CHECK_FALSE(cond) do { if (!(cond)) return; } while (0)
//#define DEBUG_STATIC_CHECK_FALSE(cond) if (!(cond)) return
#define DEBUG_STATIC_CHECK_FALSE_RET(cond, ret) do { if (!(cond)) return (ret); } while (0)
//#define DEBUG_STATIC_CHECK_FALSE_RET(cond, ret) if (!(cond)) return (ret)
#else
#define DEBUG_STATIC_CHECK_FALSE(cond) (void)0
#define DEBUG_STATIC_CHECK_FALSE_RET(cond, ret) (void)0
#endif

#endif // MACRO_DEBUG_H