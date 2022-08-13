#include "Configuration.h"
#include <filesystem>
#include <fstream>
#include <QSettings>
#include <QStandardPaths>
#include <json/json.h>

using namespace NickvisionTubeConverter::Update;

namespace NickvisionTubeConverter::Models
{
    Configuration::Configuration() : m_configDir{ QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() }, m_theme{ Theme::System }, m_alwaysStartOnHomePage{ true }, m_ytdlpVersion{ "0.0.0" }, m_ffmpegVersion{ "0.0.0" }, m_previousSaveFolder { "" }, m_previousFileFormat{MediaFileType::MP4}
    {
        if (!std::filesystem::exists(m_configDir))
        {
            std::filesystem::create_directories(m_configDir);
        }
        std::ifstream configFile{ m_configDir + "/config.json" };
        if (configFile.is_open())
        {
            Json::Value json;
            configFile >> json;
            try
            {
                m_theme = static_cast<Theme>(json.get("Theme", 2).asInt());
                m_alwaysStartOnHomePage = json.get("AlwaysStartOnHomePage", true).asBool();
                m_ytdlpVersion = { json.get("YtdlpVersion", "0.0.0").asString() };
                m_ffmpegVersion = { json.get("FfmpegVersion", "0.0.0").asString() };
                m_previousSaveFolder = json.get("PreviousSaveFolder", "").asString();
                m_previousFileFormat = static_cast<MediaFileType::Value>(json.get("PreviousFileFormat", 0).asInt());
            }
            catch (...) { }
        }
    }

    Configuration& Configuration::getInstance()
    {
        static Configuration instance;
        return instance;
    }

    Theme Configuration::getTheme(bool calculateSystemTheme) const
    {
        if (!calculateSystemTheme || m_theme != Theme::System)
        {
            return m_theme;
        }
        QSettings regKeyTheme{ "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat };
        return regKeyTheme.value("AppsUseLightTheme").toBool() ? Theme::Light : Theme::Dark;
    }

    void Configuration::setTheme(Theme theme)
    {
        m_theme = theme;
    }

    bool Configuration::getAlwaysStartOnHomePage() const
    {
        return m_alwaysStartOnHomePage;
    }

    void Configuration::setAlwaysStartOnHomePage(bool alwaysStartOnHomePage)
    {
        m_alwaysStartOnHomePage = alwaysStartOnHomePage;
    }

    const Version& Configuration::getYtdlpVersion() const
    {
        return m_ytdlpVersion;
    }

    void Configuration::setYtdlpVersion(const Version& ytdlpVersion)
    {
        m_ytdlpVersion = ytdlpVersion;
    }

    const Version& Configuration::getFfmpegVersion() const
    {
        return m_ffmpegVersion;
    }

    void Configuration::setFfmpegVersion(const Version& ffmpegVersion)
    {
        m_ffmpegVersion = ffmpegVersion;
    }

    const std::string& Configuration::getPreviousSaveFolder() const
    {
        return m_previousSaveFolder;
    }

    void Configuration::setPreviousSaveFolder(const std::string& previousSaveFolder)
    {
        m_previousSaveFolder = previousSaveFolder;
    }

    const MediaFileType& Configuration::getPreviousFileFormat() const
    {
        return m_previousFileFormat;
    }

    void Configuration::setPreviousFileForamt(const MediaFileType& previousFileFormat)
    {
        m_previousFileFormat = previousFileFormat;
    }

    void Configuration::save() const
    {
        std::ofstream configFile{ m_configDir + "/config.json" };
        if (configFile.is_open())
        {
            Json::Value json;
            json["Theme"] = static_cast<int>(m_theme);
            json["AlwaysStartOnHomePage"] = m_alwaysStartOnHomePage;
            json["YtdlpVersion"] = m_ytdlpVersion.toString();
            json["FfmpegVersion"] = m_ffmpegVersion.toString();
            json["PreviousSaveFolder"] = m_previousSaveFolder;
            json["PreviousFileFormat"] = static_cast<int>(m_previousFileFormat);
            configFile << json;
        }
    }
}