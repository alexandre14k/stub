#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <mutex>

using m_string = std::string;
static auto& m_cout = std::cout;
inline auto m_endl = std::endl<char, std::char_traits<char>>;