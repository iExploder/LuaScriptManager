fun00 = function () print("function void -> void OK!") end
fun01 = function () print("function void -> int OK!") return 1 end
fun10 = function (i) print("function int -> void OK!") print("Input value:" .. i) end
fun11 = function (i) print("function int -> int OK!") print("Input value:" .. i) return i end