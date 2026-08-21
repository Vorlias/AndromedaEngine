-- local x = require("y")
-- print(x)

const Test = {
    elapsed = 0,
    lastTick = 0,
}

type Test = typeof(Test)

function Test._init(self: Test)
    print("Entity is initialized")

    task.spawn(function()
        while true do
            print("second")
            task.wait(1)
        end
    end)
end

function Test._ready(self: Test)
    print("Entity is ready :D")
end

function Test._update(self: Test, dt: number)
    self.elapsed += dt
    if self.lastTick + 5 < self.elapsed then
        self.lastTick = self.elapsed
        print("Waited 5 seconds", self.test)
    end
end

function Test._exit(self: Test)
    print("Entity go bye-bye!")
end

return Test