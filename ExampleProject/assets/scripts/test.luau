--!strict
const Test = {
    elapsed = 0,
    lastTick = 0,
}

type Test = typeof(Test)

function Test._init(self: Test)
    print("Called _init")
    print("name is", entity.name)

    entity.transform.position = Vector3.new(1, 2, 3)
    print("position is", entity.transform.position)
end

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