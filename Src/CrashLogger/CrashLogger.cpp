/**
 * @file  CrashLogger.cpp
 *
 * @brief Disabled crash logging implementation.
 */
#include "StdAfx.h"
#include "CrashLogger.h"

namespace CrashLogger
{
	bool Install()
	{
		return false;
	}

	void Disable()
	{
	}

	bool HasPreviousCrash()
	{
		return false;
	}

	void CheckAndReportPreviousCrash()
	{
	}
}
