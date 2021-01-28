# Empire at War ModCheck

The ModCheck tool for Empire at War and Forces of Corruption allows mod makers
to check their mods for errors, like missing text entries, references or corrupted
models or map files.

The tool was originally developed by Mike Lankamp and is currently maintined by
the contributers to the Alamo Engine Tools GitHub organisation.


## Prerequisites

- Operating System: Windows 7 or newer
- A functional installation of Star Wars: Empire at War and Star Wars: Empire at War: Forces of Corruption

## Usage

1. Place the binary next to the mod's data folder.
2. Run `./ModCheck.exe` with one of the following arguments from a command line:    
    `/FOC` for Forces of Corruption mods    
    `/EAW` for base game mods    
3. Optionally add `>mod_check.log 2>&1` to write the report to a file (e.g. `./ModCheck.exe /FOC >mod_check.log 2>&1`)

## Changelog

- **2021-01-28 - Release 1.0.1: Legacy Release**
    - **Fixes**
        - Updated to the latest Visual Studio 2019 and fixes any compiler errors.
        - Updates the support for sub-directory resolving.
    - **Further Notes**
        - The tool is released _as is_. It builds under current Windows versions and can be used to validate the legacy
          versions of Empire at War and Forces of Corruption (Disc, gog.com, Origin).
        - The Legacy release is _not_ officially supported.
        - Any changes to the legacy release will have to be community driven.
