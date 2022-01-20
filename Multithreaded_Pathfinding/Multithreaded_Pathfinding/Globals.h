#pragma once

#include <iostream>
#include <thread>
#include <vector>

const static int m_npcCount = 50;
static std::vector<std::thread::id> threadID_Vec = std::vector<std::thread::id>(m_npcCount, std::thread::id());
const int mapSize = 100; // the number of nodes wide and high we want the map
extern bool isPathfindingFinished;
extern bool isTilesShowingPaths;
extern bool isWalkPath;

extern void initGlobals();
