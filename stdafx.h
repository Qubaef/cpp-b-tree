#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>	// min()
#include <fstream>
#include <windows.h>
#include <ctime>

constexpr auto DISK_PAGE_SIZE = 4096;
constexpr auto BTREE_D = 2;
constexpr auto MEMORY_NULL = 65535;