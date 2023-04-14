local imgui = require('imgui')
local log = require('log')

local app_ui_config = {
    top_left_dock = { show = true, height = 40, flags = NK_WINDOW_NO_SCROLLBAR },
    left_dock = { show = true, width = 240, flags = NK_WINDOW_NO_SCROLLBAR },
}

local app_data = {
    top_left_dock = {
        buttons = {
            {
                label = '打印window尺寸',
                callback = function()
                    log.debug('第一个 is clicked')
                    log.info('window ' .. tostring(window.width) .. 'x' .. tostring(window.height))
                end
            },
            {
                label = '输出lua栈',
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
    showLeftDock = function()
        if (app_ui_config.left_dock.show) then
            if (imgui.begin_window('left_dock',
                    0,
                    app_ui_config.top_left_dock.height,
                    app_ui_config.left_dock.width,
                    window.height - app_ui_config.top_left_dock.height,
                    app_ui_config.left_dock.flags)) then
                local total = #(app_data.top_left_dock.buttons)
                imgui.layout_row_dynamic(27, total)
                for k, v in ipairs(app_data.top_left_dock.buttons) do
                    if (imgui.button(v.label)) then
                        v.callback()
                    end
                end
            end
            imgui.end_window()
        end
    end,
    showTopLeftDock = function()
        if (app_ui_config.top_left_dock.show) then
            if (imgui.begin_window('top_left_dock',
                    0,
                    0,
                    window.width,
                    app_ui_config.top_left_dock.height,
                    app_ui_config.top_left_dock.flags
            )) then
                local total = #(app_data.top_left_dock.buttons)
                imgui.layout_row_dynamic(33, total)
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

local function HEXtoRGB(hexArg)

    hexArg = hexArg:gsub('#', '')

    if (string.len(hexArg) == 3) then
        return tonumber('0x' .. hexArg:sub(1, 1)) * 17, tonumber('0x' .. hexArg:sub(2, 2)) * 17, tonumber('0x' .. hexArg:sub(3, 3)) * 17
    elseif (string.len(hexArg) == 6) then
        return tonumber('0x' .. hexArg:sub(1, 2)), tonumber('0x' .. hexArg:sub(3, 4)), tonumber('0x' .. hexArg:sub(5, 6))
    else
        return 0, 0, 0
    end

end

return {
    init = function()
        log.info('window ' .. tostring(window.width) .. 'x' .. tostring(window.height))
        local r, g, b = HEXtoRGB('#7d94b5')
        log.info('rgb' .. tostring(r) .. ' ' .. tostring(g) .. ' ' .. tostring(b))
        imgui.change_bg_color(r / 255, g / 255, b / 255, 1)
    end,
    update = function()
        app_ui.showTopLeftDock()
        app_ui.showLeftDock()
    end
}

