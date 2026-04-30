/** 
 * @file  Environment.h
 *
 * @brief Declaration file for Environment-related routines.
 */
#pragma once

#include "UnicodeString.h"

namespace env
{

void SetTemporaryPath(const String& path);
String GetTemporaryPath();
String GetTemporaryFileName(const String& lpPathName, const String& lpPrefixString,
		int * pnerr = nullptr);
String GetTempChildPath();
void SetProgPath(const String& path);
String GetProgPath();

String GetWindowsDirectory();
String GetMyDocuments();
String GetAppDataPath();
String GetConfigPath();
String GetDataPath();
String GetSystemTempPath();

String GetPerInstanceString(const String& name);

String ExpandEnvironmentVariables(const String& text);

}
