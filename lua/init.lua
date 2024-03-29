﻿local imgui = require('imgui')
local log = require('log')
local utils = require('utils')
--local p = require('amiffy_ex')

local app_ui_data = {
    background_color = '#7d94b5',
    top_left_dock = { show = true, height = 60, flags = NK_WINDOW_NO_SCROLLBAR },
    left_dock = { show = true, x = 0, y = 140, width = 160, height = 400, id = 'LD',
                  flags = NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_SCALABLE | NK_WINDOW_MOVABLE },
}

local app_capabilities = {
    left_dock = {
        buttons = {
            {
                label = '添加icon',
                callback = function(target)
                    log.debug('第一个 is clicked')
                    table.insert(target, {
                        label = '打印window尺寸',
                        callback = function()
                            log.debug('第2个 is clicked')
                            log.info('window ' .. tostring(window.width) .. 'x' .. tostring(window.height))
                        end
                    })
                end
            },
            {
                label = '删除icon',
                callback = function(target)
                    log.debug('第二个 is clicked')
                    table.remove(target)
                end
            },
        }
    },

    top_left_dock = {
        buttons = {},
    }
}

local app_ui = {
    showLeftDock = function(self)
        if (app_ui_data.left_dock.show) then
            if (imgui.begin_window(app_ui_data.left_dock.id, app_ui_data.left_dock.x, app_ui_data.left_dock.y,
                    app_ui_data.left_dock.width, app_ui_data.left_dock.height, app_ui_data.left_dock.flags)) then
                --local total = #(app_data.top_left_dock.buttons)
                for k, v in ipairs(app_capabilities.left_dock.buttons) do
                    imgui.layout_row_dynamic(30, 1)
                    if (imgui.button(v.label)) then
                        v.callback(app_capabilities.top_left_dock.buttons)
                    end
                end
            end
            imgui.end_window()
        end
    end,

    showTopLeftDock = function(self)
        if (app_ui_data.top_left_dock.show) then
            if (imgui.begin_window('top_left_dock', 0, 0, window.width,
                    app_ui_data.top_left_dock.height,
                    app_ui_data.top_left_dock.flags
            )) then
                local total = #(app_capabilities.top_left_dock.buttons)
                --log.debug('total:' .. tostring(total))
                imgui.layout_row_dynamic(app_ui_data.top_left_dock.height - 6, total)
                for k, v in ipairs(app_capabilities.top_left_dock.buttons) do
                    if (imgui.button(v.label)) then
                        v.callback()
                    end
                end
            end
            imgui.end_window()
        end
    end,
}

return {
    init = function()
        log.info('window ' .. window.width .. 'x' .. window.height)
        local r, g, b = utils.hex2rgb(app_ui_data.background_color)
        --imgui.change_bg_color(0, 0, 0, 0) -- 透明背景
        imgui.change_bg_color(r / 255, g / 255, b / 255, 1)
        --log.info('剪切板内容: ' .. p.get_clipboard_content())
        --log.info('cpath ' .. package.cpath)
    end,
    update = function()
        app_ui.showTopLeftDock()
        app_ui.showLeftDock()
    end
}

