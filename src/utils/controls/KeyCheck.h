/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/

#pragma once

#include <cstdint>

#include "VirtualKeys.h"

class KeyCheck
{
public:
	static uint8_t m_ucCurrentStates[256];
	static uint8_t m_ucPrevStates[256];
	
	static void Update();

	static bool Check(uint32_t key);

	static bool CheckJustDown(uint32_t key);

	static bool CheckJustUp(uint32_t key);
};