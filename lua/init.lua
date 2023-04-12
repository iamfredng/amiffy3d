local s2 = require('s2')
local imgui = require('imgui')
local log = require('log')

function init()
    -- for k, v in pairs(amiffy) do
    --     c_log_info(k, v)
    -- end
    -- for k, v in pairs(log) do
    --     c_log_info(k, v)
    -- end
    NK_WINDOW_BORDER = NK_FLAG(0)
    NK_WINDOW_MOVABLE = NK_FLAG(1)
    NK_WINDOW_SCALABLE = NK_FLAG(2)
    NK_WINDOW_CLOSABLE = NK_FLAG(3)
    NK_WINDOW_MINIMIZABLE = NK_FLAG(4)
    NK_WINDOW_NO_SCROLLBAR = NK_FLAG(5)
    NK_WINDOW_TITLE = NK_FLAG(6)
    NK_WINDOW_SCROLL_AUTO_HIDE = NK_FLAG(7)
    NK_WINDOW_BACKGROUND = NK_FLAG(8)
    NK_WINDOW_SCALE_LEFT = NK_FLAG(9)
    NK_WINDOW_NO_INPUT = NK_FLAG(10)

    imgui.change_bg_color(0.1, 0.3, 0.5, 1)
end

function update(width, height)

    if (imgui.begin_window('FW01', 10, 10, 300, 200, NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) then
        imgui.layout_row_dynamic(28, 2)
        -- amiffy_layout_row_static(28, 220, 2)
        if (imgui.button('点我')) then
            log.info('点了.....')
        end

        if (imgui.button('关闭')) then
            log.info('关闭.....')
        end
    end
    imgui.end_window()

end

log.info('init.lua 执行完毕')
