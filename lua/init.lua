local imgui = require('imgui')
local log = require('log')

local app_ui_config = {
    top_left_dock = { x = 0, y = 0, width = 480, height = 70, flags = NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER }
}

local app_data = {
    top_left_dock = {
        show = true,
    }
}

local app_ui = {
    showTopLeftDock = function()
        if (app_data.top_left_dock.show) then
            if (imgui.begin_window('top_left_dock',
                    app_ui_config.top_left_dock.x,
                    app_ui_config.top_left_dock.y,
                    app_ui_config.top_left_dock.width,
                    app_ui_config.top_left_dock.height,
                    app_ui_config.top_left_dock.flags
            )) then
                imgui.layout_row_dynamic(25, 3)
                -- amiffy_layout_row_static(28, 220, 2)
                if (imgui.button('点我')) then
                    log.info('点了.....')
                end

                if (imgui.button('点我')) then
                    log.info('点了.....')
                end

                if (imgui.button('关闭')) then
                    app_data.top_left_dock.show = false
                    log.info('关闭.....')
                end
            end
            imgui.end_window()
        end
    end,
}

return {
    init = function()
        imgui.change_bg_color(0.1, 0.3, 0.5, 1)
    end,
    update = function(width, height)
        app_ui.showTopLeftDock()
    end
}

