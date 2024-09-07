// ==WindhawkMod==
// @id                  ex7forw8-config
// @name                Explorer7 configurator
// @name:fr-FR          Paramétrage d'Explorer7
// @name:es-ES          Configuración de Explorer7
// @description         An easier way to configure explorer7 without registry
// @description:fr-FR   Une façon plus simple de configurer les paramètres d'Explorer7, sans modifier le Registre
// @description:es-ES   Un método muy fácil de modificar los ajustes de Explorer7 sin acceder al Registro
// @version             1.0
// @author              CatmanFan / Mr._Lechkar
// @github              https://github.com/CatmanFan
// @include             explorer.exe
// @architecture        x86
// @architecture        x86-64
// @compilerOptions     -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## Explorer7 configurator
Quick configuration mod created for Erizur's **[Explorer7](https://github.com/Erizur/explorer7-releases)** mod, which restores the Windows 7 explorer to Windows 8.1 and above.

This mod makes it so that you don't have to open the registry each time a modification is needed.

### Theme disclaimer
This will only change the basic/classic theme setting for the taskbar and start menu. If you're looking for a mod to change the system theme for all windows, use either **[Basic Themer](https://windhawk.net/mods/basic-themer)** by aubymori or **[Classic Theme Enable with extended compatibility](https://windhawk.net/mods/classic-theme-enable-with-extended-compatibility)** by Roland Pihlakas (both of them are available from the *Explore* section in Windhawk).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: aero
  $name: Theme filename
  $name:fr-FR: Nom du fichier de thème
  $name:es-ES: Nombre del archivo de tema
  $description: Name of the theme file to use, relative to the Explorer7 directory.
  $description:fr-FR: Nom du fichier de thème à utiliser, relatif au directoire où se trouve l'application Explorer7.
  $description:es-ES: Nombre del archivo de tema a utilizar, relativo al directorio donde se encuentra la aplicación Explorer7.
- disableComp: FALSE
  $name: Disable composition
  $name:fr-FR: Désactiver la composition
  $name:es-ES: Deshabilitar la composición de DWM
  $description: Disables Desktop Window Manager (DWM) detection. This allows for changing the theme to Windows 7 Basic.
  $description:fr-FR: Désactiver la détection du gestionnaire des fenêtres (Desktop Window Manager). Cette option permet ainsi de rendre le thème en Windows 7 Basic.
  $description:es-ES: Deshabilitar la detección de Desktop Window Manager. Esta opción permite cambiar el tema a Windows 7 Básico.
- classic: FALSE
  $name: Classic theme
  $name:fr-FR: Thème classique
  $name:es-ES: Tema clásico
  $description: Self-explanatory.
  $description:fr-FR: Cela dit tout.
  $description:es-ES: Así es.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <versionhelpers.h>
#include <stringapiset.h>
#include <sstream>

const std::wstring defKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

#pragma region ## Registry functions ##
/**
 * Reads a string value from a registry key within HKCU.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @param v The name of the value.
 * @return The string value if it is found, otherwise `NULL`.
 */
std::wstring read_SZ(std::wstring sk, std::wstring v, std::wstring defaultValue)
{
    const LPCTSTR subkey = sk.c_str();
    const LPCTSTR value = v.c_str();
    WCHAR szBuffer[512];
    DWORD size(sizeof(szBuffer));

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey);
    if (openRes == ERROR_SUCCESS)
    {
        LONG setRes = RegQueryValueEx(hKey, value, 0, NULL, (LPBYTE)&szBuffer, &size);
        RegCloseKey(hKey);

        if (setRes == ERROR_SUCCESS)
        {
            defaultValue = szBuffer;
        }
    }

    return defaultValue;
}

/**
 * Checks for the existence of a registry key within HKCU.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @return `TRUE` if found, otherwise `FALSE`.
 */
BOOL exists_Key(std::wstring sk)
{
    const LPCTSTR subkey = sk.c_str();

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey);
    if (openRes != ERROR_SUCCESS)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/**
 * Writes a DWORD value to a registry key within HKCU.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @param v The name of the value.
 * @param data The DWORD value to write.
 * @return `TRUE` if the operation succeeded, otherwise `FALSE`.
 */
BOOL set_DWORD(std::wstring sk, std::wstring v, unsigned long data)
{
    const LPCTSTR subkey = sk.c_str();
    const LPCTSTR value = v.c_str();

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey);
    if (openRes != ERROR_SUCCESS)
    {
        Wh_Log(L"Failed to open registry key");
        return FALSE;
    }

    LONG setRes = data < 0 ? RegDeleteValue(hKey, value) : RegSetValueEx(hKey, value, 0, REG_DWORD, (const BYTE*)&data, sizeof(data));
    RegCloseKey(hKey);

    if (setRes == ERROR_SUCCESS)
    {
        return TRUE;
    }
    else
    {
        Wh_Log(L"Failed writing to registry");
        return FALSE;
    }
}

/**
 * Writes an SZ value to a registry key within HKCU.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @param v The name of the value.
 * @param data The SZ value to write.
 * @return `TRUE` if the operation succeeded, otherwise `FALSE`.
 */
BOOL set_SZ(std::wstring sk, std::wstring v, LPCWSTR data)
{
    const LPCTSTR subkey = sk.c_str();
    const LPCTSTR value = v.c_str();

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey);
    if (openRes != ERROR_SUCCESS)
    {
        Wh_Log(L"Failed to open registry key");
        return FALSE;
    }

    LONG setRes = RegSetValueExW(hKey, value, 0, REG_SZ, (LPBYTE)data, sizeof(data));
    RegCloseKey(hKey);

    if (setRes == ERROR_SUCCESS)
    {
        return TRUE;
    }
    else
    {
        Wh_Log(L"Failed writing to registry");
        return FALSE;
    }
}
#pragma endregion

void go()
{
    LPCWSTR one = Wh_GetStringSetting(L"theme");
    DWORD two = Wh_GetIntSetting(L"disableComp");
    DWORD three = Wh_GetIntSetting(L"classic");

    /* ------------------------------------------------------------------------------------- */
    /* These values don't exist in the key by default and can be deleted if they're not used */
    /* ------------------------------------------------------------------------------------- */
    set_SZ(defKey, L"Theme", one);
    set_DWORD(defKey, L"DisableComposition", two);
    set_DWORD(defKey, L"ClassicTheme", three);

    Wh_FreeStringSetting(one);
    Wh_Log(L"We are donion rings");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    go();
    system("taskkill /F /IM explorer.exe & start explorer");
}

BOOL Wh_ModInit() {
    std::wstring username = read_SZ(L"Volatile Environment", L"USERNAME", L"???");
    if (username == L"???")
    {
        Wh_Log(L"Halt: Local username not detected");
        return FALSE;
    }

    if (!exists_Key(defKey))
    {
        Wh_Log(L"Halt: Key not detected");
        return FALSE;
    }

    go();
    return TRUE;
}