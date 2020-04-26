#pragma once

#include <string>
#include <fstream>
#include <streambuf>

#define FILE_READER(F, M) std::string((std::istreambuf_iterator<char>(std::ifstream(F, M | std::ios::in).rdbuf())), std::istreambuf_iterator<char>())
#define FILE_WRITER(D, F, M) std::ofstream(F, M | std::ios::out).write((D).data(), (D).size())