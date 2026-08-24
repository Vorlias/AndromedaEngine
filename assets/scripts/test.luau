--!strict

const Test = {
    elapsed = 0,
    lastTick = 0,
}

type Test = typeof(Test)

function Test._init(self: Test)
    print("Entity initialized", entity.name, entity.id, entity)
end

function Test._ready(self: Test)
    -- entity.script.enabled = false
    
    task.spawn(function()
        task.spawn(function()
            task.wait(10)
            print("This still counts thankfully")
        end)

        task.wait(6)
        print(entity.name)
    end)
end

function Test._enabled(self: Test)
    print("Luau Script was enabled!")
end

function Test._disabled(self: Test)
    print("Luau Script was disabled!")
end

function Test._update(self: Test, dt: number)
    self.elapsed += dt
    if self.lastTick + 2 < self.elapsed then
        self.lastTick = self.elapsed
        print("2 seconds passed")
    end
end

function Test._remove(self: Test)
    print("Script removed lol")
end

return Test