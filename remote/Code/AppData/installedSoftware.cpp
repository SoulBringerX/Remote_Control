#ifdef WIN32

#include "installedsoftware.h"
#include <QVariantList>
#include <QDebug>
#include <shlwapi.h>

InstalledSoftware::InstalledSoftware(QObject *parent)
    : QObject(parent)
{
    refreshSoftwareList();
}

QVariantList InstalledSoftware::softwareList() const
{
    return softwareList_;
}

void InstalledSoftware::refreshSoftwareList()
{
    softwareList_.clear();

    HKEY hKey;
    QString path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, reinterpret_cast<const wchar_t*>(path.utf16()), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        wchar_t subkeyName[2048];
        DWORD subkeyNameSize = 2048;
        DWORD i = 0;

        while (RegEnumKeyExW(hKey, i, subkeyName, &subkeyNameSize, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
        {
            HKEY subKey;
            if (RegOpenKeyExW(hKey, subkeyName, 0, KEY_READ, &subKey) == ERROR_SUCCESS)
            {
                wchar_t displayName[2048];
                wchar_t displayVersion[2048];
                wchar_t installDate[2048];

                DWORD displayNameSize = 2048 * sizeof(wchar_t);
                DWORD displayVersionSize = 2048 * sizeof(wchar_t);
                DWORD installDateSize = 2048 * sizeof(wchar_t);

                QString name, version, date;

                if (RegQueryValueExW(subKey, L"DisplayName", nullptr, nullptr, reinterpret_cast<LPBYTE>(displayName), &displayNameSize) == ERROR_SUCCESS)
                {
                    name = QString::fromUtf16(reinterpret_cast<const char16_t*>(displayName));
                }
                if (RegQueryValueExW(subKey, L"DisplayVersion", nullptr, nullptr, reinterpret_cast<LPBYTE>(displayVersion), &displayVersionSize) == ERROR_SUCCESS)
                {
                    version = QString::fromUtf16(reinterpret_cast<const char16_t*>(displayVersion));
                }
                if (RegQueryValueExW(subKey, L"InstallDate", nullptr, nullptr, reinterpret_cast<LPBYTE>(installDate), &installDateSize) == ERROR_SUCCESS)
                {
                    date = QString::fromUtf16(reinterpret_cast<const char16_t*>(installDate));
                }

                if (!name.isEmpty())
                {
                    QVariantMap software;
                    software["name"] = name;
                    software["version"] = version;
                    software["installDate"] = date;
                    softwareList_.append(software);
                }

                RegCloseKey(subKey);
            }

            subkeyNameSize = 2048;
            i++;
        }
        RegCloseKey(hKey);
    }

    emit softwareListChanged();
}

#endif
