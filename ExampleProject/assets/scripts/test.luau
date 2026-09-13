--!strict
const Test = {
    elapsed = 0,
    lastTick = 0,
}

type Test = typeof(Test)

function Test._init(self: Test)
    -- print("Called _init")
    -- print("name is", entity.name)

    -- entity.transform.position = Vector3.new(1, 2, 3)
    -- print("position is", entity.transform.position)
    
    local rect = Rect.new(0, 0, 100, 100)
    local rect2 = Rect.new(Vector2.new(10, 20), Vector2.new(250, 250))

    print(rect, rect2, Rect.new(50, 50, 100, 100).center)
end

-- function Test._update(self: Test, dt: number): ()
--     print("update", dt)
-- end

function Test._ready(self: Test)
    print("Called _ready!")
    
    print("the script on this is", entity.script.filePath)
end

function Test._enabled(self: Test)
    print("Called _enabled!")
end

function Test._disabled(self: Test)
    print("Called _disabled!");
end

function Test._remove(self: Test)
    print("Destroyed")
end

return Test