--!strict

const Test = {
    elapsed = 0,
    lastTick = 0,
}

type Test = typeof(Test)

function Test._init(self: Test)
    entity.name = "test"
    print(entity, entity.name, (entity.parent :: Entity).name, entity.id, (entity.parent ::Entity).id)
end

function Test._ready(self: Test)
    print("Entity is ready :D")
    
    task.wait(2)
    entity.script.enabled = false
end

function Test._enabled(self: Test)
    print("Was enabled")
end

function Test._disabled(self: Test)
    print("Entity was disabled lmfao")
end

function Test._update(self: Test, dt: number)

end

function Test._exit(self: Test)
    print("Entity go bye-bye!")
end

return Test