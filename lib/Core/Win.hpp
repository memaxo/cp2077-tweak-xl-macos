#pragma once

// Windows-only includes
// This file should only be included on Windows builds

#if defined(_WIN32) || defined(_WIN64)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <wil/stl.h>
#include <wil/win32_helpers.h>

#else
// macOS: Include platform header instead
#include "Core/macOS.hpp"
#endif
