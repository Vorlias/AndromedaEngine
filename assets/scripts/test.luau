-- print(task.wait(2))

-- task.spawn(function(x)
--     local i = 0
--     while true do
--         print(i)
--         local t=  task.wait()
--         i+=t
--     end
-- end)

local testVec = Vector3.new(1, 2, 3)
print(testVec)
print(testVec.magnitude)