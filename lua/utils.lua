return {
    hex2rgb = function(hexArg)
        hexArg = hexArg:gsub('#', '')
        if (string.len(hexArg) == 3) then
            return tonumber('0x' .. hexArg:sub(1, 1)) * 17, tonumber('0x' .. hexArg:sub(2, 2)) * 17, tonumber('0x' .. hexArg:sub(3, 3)) * 17
        elseif (string.len(hexArg) == 6) then
            return tonumber('0x' .. hexArg:sub(1, 2)), tonumber('0x' .. hexArg:sub(3, 4)), tonumber('0x' .. hexArg:sub(5, 6))
        else
            return 0, 0, 0
        end
    end
}