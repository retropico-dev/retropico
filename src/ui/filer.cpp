//
// Created by cpasjuste on 14/06/23.
//

#include "main.h"
#include "filer.h"
#include "retropico_colors.h"
#include "../bootloader/bootloader.h"

using namespace p2d;
using namespace retropico;

Filer::Filer(const Utility::Vec2i &pos, const Utility::Vec2i &size) : Widget(pos, size) {
    p_platform = Ui::getInstance()->getPlatform();

    m_line_height = UI_FONT_HEIGHT + 6; // font height + margin
    m_max_lines = (int16_t) (Filer::getSize().y / m_line_height);
    if (m_max_lines * m_line_height < Filer::getSize().y) {
        m_line_height = (int16_t) (Filer::getSize().y / m_max_lines);
    }

    // add highlight
    p_highlight = new Rectangle({1, 1}, {Filer::getSize().x, (int16_t) (m_line_height + 4)}, true);
    p_highlight->setColor(Red);
    p_highlight->setOutlineColor(YellowLight);
    p_highlight->setOutlineThickness(1);
    Filer::add(p_highlight);

    // add lines
    for (int i = 0; i < m_max_lines; i++) {
        auto line = new Text(3, (int16_t) (m_line_height * i + 6),
                             (int16_t) (Filer::getSize().x - 10), (int16_t) m_line_height, "");
        line->setColor(YellowLight);
        p_lines.push_back(line);
        Filer::add(line);
    }

    //
    p_no_rom_text = new Text((int16_t) (size.x / 2), (int16_t) (size.y / 2),
                             "! NO ROM FOUND !\r\n\r\n    CHECK SDCARD");
    p_no_rom_text->setColor(Red);
    p_no_rom_text->setOrigin(Origin::Center);
    p_no_rom_text->setVisibility(Visibility::Hidden);
    add(p_no_rom_text);

    // set repeat delay for ui
    p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
}

void Filer::load() {
    // load roms lists from sdcard
    for (int i = 0; i < ROMS_FOLDER_COUNT; i++) {
        // TODO: filtering
        m_files[i] = Io::getList(Core::getRomsPath(i));
        printf("Filer::load: loaded %lu roms from %s\r\n", m_files[i].size(), Core::getRomsPath(i).c_str());
    }

    // set current browsing directory (core) to latest one
    m_core = static_cast<Core::Type>(Ui::getInstance()->getConfig()->getFilerCurrentCore());
    setSelection(Ui::getInstance()->getConfig()->getFilerCurrentCoreIndex());

    // set no rom message if needed
    const auto &empty = m_files[m_core].empty();
    p_highlight->setVisibility(empty ? Visibility::Hidden : Visibility::Visible);
    p_no_rom_text->setVisibility(empty ? Visibility::Visible : Visibility::Hidden);

    refresh();
}

bool Filer::onInput(const uint16_t &buttons) {
    if (!isVisible() || Ui::getInstance()->getMenu()->isVisible()) return false;

    if (buttons & Input::Button::UP) {
        int index = m_file_index + m_highlight_index;
        int middle = m_max_lines / 2;
        if (m_highlight_index <= middle && index - middle > 0) {
            m_file_index--;
        } else {
            m_highlight_index--;
        }
        if (m_highlight_index < 0) {
            m_highlight_index =
                    m_files[m_core].size() < m_max_lines ? m_files[m_core].size() - 1 : m_max_lines - 1;
            m_file_index = m_files[m_core].size() - 1 - m_highlight_index;
        }
        refresh();
        return true;
    }

    if (buttons & Input::Button::DOWN) {
        int index = m_file_index + m_highlight_index;
        int middle = m_max_lines / 2;
        if (m_highlight_index >= middle && index + middle < m_files[m_core].size()) {
            m_file_index++;
        } else {
            m_highlight_index++;
        }
        if (m_highlight_index >= m_max_lines || m_file_index + m_highlight_index >= m_files[m_core].size()) {
            m_file_index = 0;
            m_highlight_index = 0;
        }
        refresh();
        return true;
    }

    if (buttons & Input::Button::LEFT) {
        int index = m_file_index + m_highlight_index - m_max_lines;
        if (index < 0) index = 0;
        setSelection(index);
        refresh();
        return true;
    }

    if (buttons & Input::Button::RIGHT) {
        int index = m_file_index + m_highlight_index + m_max_lines;
        if (index > m_files[m_core].size() - 1) index = m_files[m_core].size() - 1;
        setSelection(index);
        refresh();
        return true;
    }

    if (buttons & Input::Button::B1 && m_files[m_core].size() > m_file_index + m_highlight_index) {
        // remove old rom
        const auto list = Io::getList("flash:/rom/");
        for (auto &f: list) {
            printf("Filer: deleting previous rom (%s)\r\n", std::string("flash:/rom/" + f.name).c_str());
            Io::remove("flash:/rom/" + f.name);
        }

        // copy new rom
        const auto file = m_files[m_core].at(m_file_index + m_highlight_index);
        const std::string path = Core::getRomsPath(m_core) + "/" + file.name;
        printf("Filer: copying %s to flash:/rom/%s\r\n", path.c_str(), file.name.c_str());
        Ui::getInstance()->getOverlay()->getInfoBox()->show("Loading...", 0, p_platform);
        const auto success = Io::copy(path, "flash:/rom/" + file.name, [this](const uint8_t progress) {
            if (progress % 5 == 0) {
                //printf("copy: %i\r\n", progress);
                const std::string msg = "Loading... " + std::to_string(progress) + "%";
                Ui::getInstance()->getOverlay()->getInfoBox()->show(msg, 0, p_platform);
            }
        });

        if (success) {
            printf("Filer: copy done... writing config to flash...\r\n");
            // write config
            Ui::getInstance()->getConfig()->save();

            // write bootloader "config"
            char magic[FLASH_SECTOR_SIZE];
            if (m_core == Core::Type::Nes) {
                strcpy(magic, "NES");
            } else if (m_core == Core::Type::Gb) {
                strcpy(magic, "GB");
            } else {
                strcpy(magic, "SMS");
            }
            io_flash_write_sector(FLASH_TARGET_OFFSET_CONFIG, (const uint8_t *) magic);

            printf("Filer: done... rebooting to bootloader...\r\n");
            m_done = true;
            return true;
        }

        printf("Filer: failed to load rom (%s)\r\n", path.c_str());
        Ui::getInstance()->getOverlay()->getInfoBox()->hide();
    }

    return Widget::onInput(buttons);
}

void Filer::refresh() {
    // update "lines"
    for (int i = 0; i < m_max_lines; i++) {
        if (m_file_index + i >= m_files[m_core].size()) {
            p_lines[i]->setVisibility(Visibility::Hidden);
        } else {
            p_lines[i]->setVisibility(Visibility::Visible);
            p_lines[i]->setString(m_files[m_core].at(i + m_file_index).name);
            if (i == m_highlight_index) {
                p_lines[i]->setColor(Yellow);
                p_highlight->setPosition(p_highlight->getPosition().x, (int16_t) (p_lines[i]->getPosition().y - 5));
            } else {
                p_lines[i]->setColor(YellowLight);
            }
        }
    }

    // set config
    Ui::getInstance()->getConfig()->setFilerCurrentCoreIndex(m_file_index + m_highlight_index);
}

void Filer::setSelection(int index) {
    if (index < m_max_lines / 2) {
        m_file_index = 0;
        m_highlight_index = 0;
    } else if (index > m_files[m_core].size() - m_max_lines / 2) {
        m_highlight_index = m_max_lines - 1;
        m_file_index = m_files[m_core].size() - 1 - m_highlight_index;
        if (m_highlight_index >= m_files[m_core].size()) {
            m_highlight_index = m_files[m_core].size() - 1;
            m_file_index = 0;
        }
    } else {
        m_highlight_index = m_max_lines / 2;
        m_file_index = index - m_highlight_index;
    }
}

void Filer::setCore(const Core::Type &core) {
    Ui::getInstance()->getConfig()->setFilerCurrentCores((uint8_t) core);
    m_core = core;
    m_file_index = 0;
    m_highlight_index = 0;

    const auto &empty = m_files[m_core].empty();
    p_highlight->setVisibility(empty ? Visibility::Hidden : Visibility::Visible);
    p_no_rom_text->setVisibility(empty ? Visibility::Visible : Visibility::Hidden);

    refresh();
}

#ifdef USE_FLASH_LIST_BUFFER
Io::ListBuffer *Filer::getListBuffer(uint8_t index) {
    return Ui::getInstance()->getConfig()->getListBuffer(index);
}
#endif
