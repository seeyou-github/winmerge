// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  AccentColor.cpp
 *
 * @brief Implementation of the CAccentColor class
 */

#include "StdAfx.h"
#include "AccentColor.h"

CAccentColor::CAccentColor()
	: m_accentColor(CLR_NONE)
	, m_accentColorInactive(CLR_NONE)
	, m_colorPrevalence(false)
{
	Reload();
}

CAccentColor& CAccentColor::Get()
{
	static CAccentColor s_accentColor;
	return s_accentColor;
}

void CAccentColor::Reload()
{
	m_accentColor = CLR_NONE;
	m_accentColorInactive = CLR_NONE;
	m_colorPrevalence = false;
}
