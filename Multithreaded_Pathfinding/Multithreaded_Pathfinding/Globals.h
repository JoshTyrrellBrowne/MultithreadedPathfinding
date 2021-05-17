#pragma once

#include <iostream>
#include <thread>

const static int m_npcCount = 20;
static std::vector<std::thread::id> threadID_Vec = std::vector<std::thread::id>(m_npcCount, std::thread::id());
const int mapSize = 50; // the number of nodes wide and high we want the map
