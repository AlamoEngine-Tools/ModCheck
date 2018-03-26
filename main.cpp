#include "Assets/Assets.h"
#include "General/Exceptions.h"
#include "General/Utils.h"
#include "Mod.h"
#include <iostream>
#include <windows.h>
#include <shlwapi.h>
using namespace std;

// Returns the base directory for a game, based on
// 1. Registry
// 2. Default path, expanded
static wstring GetExePath(const wchar_t* regkey, const wchar_t* valname, const wchar_t* defpath = NULL)
{
	// The games are 32-bit games, so make these calls in 32-bit mode
    wstring result;
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regkey, 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &hKey ) == ERROR_SUCCESS)
	{
		DWORD type, size = MAX_PATH;
        TCHAR path[MAX_PATH];
		if (RegQueryValueEx(hKey, valname, NULL, &type, (LPBYTE)path, &size) == ERROR_SUCCESS)
		{
            if (PathRemoveFileSpec(path) && PathIsDirectory(path))
            {
				result = path;
            }
		}
		RegCloseKey(hKey);
	}
    
    if (result.empty() && defpath != NULL)
    {
        // Use the default path
        TCHAR path[MAX_PATH];
        if (ExpandEnvironmentStrings(defpath, path, MAX_PATH) != 0)
        {
            if (PathIsDirectory(path))
            {
                result = path;
            }
        }
    }

    return result;
}

// Returns the base directory for a game
static wstring GetBaseDirForGame(GameID game)
{
    switch (game)
    {
        case GID_EAW_FOC:
            return GetExePath(L"Software\\LucasArts\\Star Wars Empire at War Forces of Corruption\\1.0", L"ExePath", L"%PROGRAMFILES%\\LucasArts\\Star Wars Empire at War Forces of Corruption");

        case GID_EAW:
            return GetExePath(L"Software\\LucasArts\\Star Wars Empire at War\\1.0", L"ExePath", L"%PROGRAMFILES%\\LucasArts\\Star Wars Empire at War\\GameData");
    }
    return L"";
}

int main(int argc, const char* argv[])
{
#ifndef NDEBUG
    // In debug mode we turn on memory checking
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif

#ifdef NDEBUG
    try
#endif
    {
        GameID game = GID_NONE;

        // Parse arguments
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '/' || argv[i][0] == '-')
            {
                if (_stricmp(argv[i] + 1, "EAW") == 0) {
                    game = GID_EAW;
                }
                else if (_stricmp(argv[i] + 1, "FOC") == 0) {
                    game = GID_EAW_FOC;
                }
            }
        }

        if (game == GID_NONE)
        {
            // Try to find game type based on current path
            TCHAR buffer[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, buffer);
            wstring curdir  = Utils::Uppercase(buffer);
            for (int i = 0; i < NUM_GAMES; i++)
            {
                wstring path = Utils::Uppercase(GetBaseDirForGame( (1 << i) ));
                if (curdir.compare(0, path.length(), path) == 0) {
                    game = (1 << i);
                    break;
                }
            }

            if (game == GID_NONE)
            {
                throw runtime_error("Unable to detect game type. Please specify /FOC or /EAW on the command line.");
            }
        }

        wstring old_path, main_path = GetBaseDirForGame(GID_EAW);
        if (game == GID_EAW_FOC) {
            old_path  = main_path;
            main_path = GetBaseDirForGame(GID_EAW_FOC);
        }

        ChecksumMap reference;

        // Load the reference objects without mod path
        Assets::Initialize(wstring(), main_path, old_path);
        Mod::LoadReferenceObjects(reference);

        // Now do everything, with mod path
        Assets::Initialize(L".", main_path, old_path);
        Mod mod(game, reference);

        Assets::Uninitialize();
    }
#ifdef NDEBUG
    catch (exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }
    catch (wexception& e)
    {
        wcerr << e.what() << endl;
        return 1;
    }
#endif
    return 0;
}