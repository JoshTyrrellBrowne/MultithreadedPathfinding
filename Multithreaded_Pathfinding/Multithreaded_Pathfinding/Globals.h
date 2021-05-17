#pragma once

#include <iostream>
#include <thread>

const static int m_npcCount = 30;
static std::vector<std::thread::id> threadID_Vec = std::vector<std::thread::id>(m_npcCount, std::thread::id());
