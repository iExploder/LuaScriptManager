fun00 = function () print("function void -> void OK!") end
fun01 = function () print("function void -> int OK!") return 1 end
fun10 = function (i) print("function int -> void OK!") print("Input value:" .. i) end
fun11 = function (i) print("function int -> int OK!") print("Input value:" .. i) return i end
fun20 = function (i,j) print("function (int, int) -> void OK!") print("Input value:" .. i .. ", " .. j) end
fun21 = function (i,j) print("function (int, int) -> int OK!") print("Input value:" .. i .. ", " .. j) return i+j end
fun30 = function (i,j,k) 
	print("function (int, int, int) -> void OK!") 
	print("Input value:" .. i .. ", " .. j .. ", " .. k) 
end
