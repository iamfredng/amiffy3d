local s2 = require('s2')
function update()
    amiffy_layout_row_dynamic(28, 2)
    if (amiffy_button('点我')) then
        c_log_info('点了.....')
    end

    if (amiffy_button('关闭')) then
        c_log_info('关闭.....')
    end

end

c_log_info('init.lua 执行完毕')
