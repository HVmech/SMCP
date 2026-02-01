#pragma once

#ifndef SMCP_DEBUG_ASSERT_H
#define SMCP_DEBUG_ASSERT_H

#ifdef DEBUG

#define DEBUG_ASSERT(expr) do { if (!(expr)) { assert(__FILE__, __LINE__); } } while (0)
void assert(const char* file, int line);

#else // DEBUG

#define DEBUG_ASSERT(expr) (void)0

#endif // DEBUG

#endif // SMCP_DEBUG_ASSERT_H