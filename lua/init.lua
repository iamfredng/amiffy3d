local s2 = require('s2')

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

function update(width, height)

    if (amiffy_begin_window('FW01', 10, 10, 300, 200, NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
        NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) then
        amiffy_layout_row_dynamic(28, 2)
        --amiffy_layout_row_static(28, 220, 2)
        if (amiffy_button('点我')) then
            c_log_info('点了.....')
        end

        if (amiffy_button('关闭')) then
            c_log_info('关闭.....')
        end
    end
    amiffy_end_window()

end

c_log_info('init.lua 执行完毕')
