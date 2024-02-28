//
// Created by cpasjuste on 14/06/23.
//

#include "main.h"
#include "filer.h"
#include "../bootloader/flashloader.h"

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
    p_highlight = new Rectangle({0, 0}, {Filer::getSize().x, (int16_t) (m_line_height + 4)}, true);
    p_highlight->setColor(Ui::Color::Red);
    p_highlight->setOutlineColor(Ui::Color::YellowLight);
    p_highlight->setOutlineThickness(1);
    Filer::add(p_highlight);

    // add lines
    for (int i = 0; i < m_max_lines; i++) {
        auto line = new Text(3, (int16_t) (m_line_height * i + 5),
                             (int16_t) (Filer::getSize().x - 10), (int16_t) m_line_height, "");
        line->setColor(Ui::Color::YellowLight);
        p_lines.push_back(line);
        Filer::add(line);
    }

    //
    p_no_rom_text = new Text((int16_t) (size.x / 2), (int16_t) (size.y / 2),
                             "! NO ROM FOUND !\r\n\r\n    CHECK SDCARD");
    p_no_rom_text->setColor(Ui::Color::Red);
    p_no_rom_text->setOrigin(Origin::Center);
    p_no_rom_text->setVisibility(Visibility::Hidden);
    add(p_no_rom_text);

    // set repeat delay for ui
    p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
}

void Filer::load() {
    // buffer roms lists
    if (Ui::getInstance()->getConfig()->getListBuffer(0)->data == nullptr) {
        uint32_t offset = FLASH_TARGET_OFFSET_CACHE;
        for (int i = 0; i < ROMS_FOLDER_COUNT; i++) {
            printf("Filer::load: buffering roms list %i...", i);
            Io::ListBuffer listBuffer = Io::getBufferedList(Core::getRomsPath(i), offset);
            listBuffer.copy(getListBuffer(i));
            offset += listBuffer.data_size;
            printf(" found %i roms\r\n", listBuffer.count);
        }

        Ui::getInstance()->getConfig()->save();

        printf("Filer::load: loaded %lu bytes in flash\r\n",
               offset + getListBuffer(Core::Gg)->data_size - FLASH_TARGET_OFFSET_CACHE);
    }

    // set current browsing directory (core) to latest one
    m_core = (Core::Type) Ui::getInstance()->getConfig()->getFilerCurrentCore();
    setSelection(Ui::getInstance()->getConfig()->getFilerCurrentCoreIndex());

    // set no rom message if needed
    p_highlight->setVisibility(getListBuffer(m_core)->count ? Visibility::Visible : Visibility::Hidden);
    p_no_rom_text->setVisibility(getListBuffer(m_core)->count ? Visibility::Hidden : Visibility::Visible);

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
                    getListBuffer(m_core)->count < m_max_lines ? getListBuffer(m_core)->count - 1 : m_max_lines - 1;
            m_file_index = getListBuffer(m_core)->count - 1 - m_highlight_index;
        }
        refresh();
        return true;
    } else if (buttons & Input::Button::DOWN) {
        int index = m_file_index + m_highlight_index;
        int middle = m_max_lines / 2;
        if (m_highlight_index >= middle && index + middle < getListBuffer(m_core)->count) {
            m_file_index++;
        } else {
            m_highlight_index++;
        }
        if (m_highlight_index >= m_max_lines || m_file_index + m_highlight_index >= getListBuffer(m_core)->count) {
            m_file_index = 0;
            m_highlight_index = 0;
        }
        refresh();
        return true;
    } else if (buttons & Input::Button::LEFT) {
        int index = m_file_index + m_highlight_index - m_max_lines;
        if (index < 0) index = 0;
        setSelection(index);
        refresh();
        return true;
    } else if (buttons & Input::Button::RIGHT) {
        int index = m_file_index + m_highlight_index + m_max_lines;
        if (index > getListBuffer(m_core)->count - 1) index = getListBuffer(m_core)->count - 1;
        setSelection(index);
        refresh();
        return true;
    } else if (buttons & Input::Button::B1 && getListBuffer(m_core)->count > m_file_index + m_highlight_index) {
        std::string name = getListBuffer(m_core)->at(m_file_index + m_highlight_index);
        std::string path = Core::getRomsPath(m_core) + "/" + name;
        // remove old rom
        auto list = Io::getList("flash:/rom/");
        for (auto &f: list) {
            printf("Filer: deleting previous rom (%s)\r\n", std::string("flash:/rom/" + f.name).c_str());
            Io::remove("flash:/rom/" + f.name);
        }
        // copy new rom
        printf("Filer: copying %s to flash:/rom/%s\r\n", path.c_str(), name.c_str());
        Ui::getInstance()->getInfoBox()->show("Loading...");
        auto success = Io::copy(path, "flash:/rom/" + name);
        if (success) {
            printf("Filer: copy done... writing config to flash...\r\n");
            // write bootloader "config"
            char magic[FLASH_SECTOR_SIZE];
            if (m_core == Core::Type::Nes) {
                strcpy(magic, "NES");
                io_flash_write_sector(FLASH_TARGET_OFFSET_CONFIG, (const uint8_t *) magic);
            } else if (m_core == Core::Type::Gb) {
                strcpy(magic, "GB");
                io_flash_write_sector(FLASH_TARGET_OFFSET_CONFIG, (const uint8_t *) magic);
            } else {
                strcpy(magic, "SMS");
                io_flash_write_sector(FLASH_TARGET_OFFSET_CONFIG, (const uint8_t *) magic);
            }
            printf("Filer: done... rebooting to bootloader...\r\n");
            m_done = true;
            return true;
        } else {
            printf("Filer: failed to load rom (%s)\r\n", path.c_str());
            Ui::getInstance()->getInfoBox()->hide();
        }
    }

    return Widget::onInput(buttons);
}

void Filer::refresh() {
    // update "lines"
    for (int i = 0; i < m_max_lines; i++) {
        if (m_file_index + i >= getListBuffer(m_core)->count) {
            p_lines[i]->setVisibility(Visibility::Hidden);
        } else {
            p_lines[i]->setVisibility(Visibility::Visible);
            p_lines[i]->setString(getListBuffer(m_core)->at(i + m_file_index));
            if (i == m_highlight_index) {
                p_lines[i]->setColor(Ui::Color::Yellow);
                p_highlight->setPosition(p_highlight->getPosition().x, (int16_t) (p_lines[i]->getPosition().y - 5));
            } else {
                p_lines[i]->setColor(Ui::Color::YellowLight);
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
    } else if (index > getListBuffer(m_core)->count - m_max_lines / 2) {
        m_highlight_index = m_max_lines - 1;
        m_file_index = getListBuffer(m_core)->count - 1 - m_highlight_index;
        if (m_highlight_index >= getListBuffer(m_core)->count) {
            m_highlight_index = getListBuffer(m_core)->count - 1;
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

    p_highlight->setVisibility(getListBuffer(m_core)->count ? Visibility::Visible : Visibility::Hidden);
    p_no_rom_text->setVisibility(getListBuffer(m_core)->count ? Visibility::Hidden : Visibility::Visible);

    refresh();
}

p2d::Io::ListBuffer *Filer::getListBuffer(uint8_t index) {
    return Ui::getInstance()->getConfig()->getListBuffer(index);
}
