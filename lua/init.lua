local s2 = require('s2')
add = function(a, b)
    local c = a + b + big_a(a, b)
    return c
end
uuuu = function(inp)
    print(inp)
end
c_log_info(tostring(s2))
c_log_info('hello world 中文输入')

c_log_info(package.path);

c_log_info(tostring(s2.fuc))
