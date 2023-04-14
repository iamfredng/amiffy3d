local imgui = require('imgui')
local log = require('log')

local app_ui_config = {
    top_left_dock = { show = true, x = 0, y = 0, width = 480, height = 70, flags = NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER }
}

local app_data = {
    top_left_dock = {
        buttons = {
            {
                label = '第一个',
                callback = function()
                    log.debug('第一个 is clicked')
                    log.info('window ' .. tostring(window.width) .. 'x' .. tostring(window.height))
                end
            },
            {
                label = '第二个',
                callback = function()
                    log.debug('第二个 is clicked')
                    c_print_stack();
                end
            },
            {
                label = '关闭',
                callback = function()
                    app_ui_config.top_left_dock.show = false
                end
            },
        },

    }
}

local app_ui = {
    showTopLeftDock = function()
        if (app_ui_config.top_left_dock.show) then

            if (imgui.begin_window('top_left_dock',
                    app_ui_config.top_left_dock.x,
                    app_ui_config.top_left_dock.y,
                    app_ui_config.top_left_dock.width,
                    app_ui_config.top_left_dock.height,
                    app_ui_config.top_left_dock.flags
            )) then
                local total = #(app_data.top_left_dock.buttons)
                imgui.layout_row_dynamic(25, total)
                for k, v in ipairs(app_data.top_left_dock.buttons) do
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
        log.info('window ' .. tostring(window.width) .. 'x' .. tostring(window.height))
        imgui.change_bg_color(0.1, 0.3, 0.5, 1)
    end,
    update = function()
        app_ui.showTopLeftDock()
    end
}

