/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include "KeyCheck.h"

#include <Windows.h>

void KeyCheck::Update()
{
	memcpy_s(m_ucPrevStates, 256, m_ucCurrentStates, 256);
	GetKeyboardState(m_ucCurrentStates);
}

bool KeyCheck::Check(const uint32_t key)
{
	return key < 256 && (m_ucCurrentStates[key] & 0x80);
}

bool KeyCheck::CheckJustDown(const uint32_t key)
{
	return key < 256 && (m_ucCurrentStates[key] & 0x80) && !(m_ucPrevStates[key] & 0x80);
}

bool KeyCheck::CheckJustUp(const uint32_t key)
{
	return key < 256 && !(m_ucCurrentStates[key] & 0x80) && (m_ucPrevStates[key] & 0x80);
}

uint8_t KeyCheck::m_ucCurrentStates[256] = {0};
uint8_t KeyCheck::m_ucPrevStates[256] = {0};