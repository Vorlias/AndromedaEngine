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
end

-- function Test._update(self: Test, dt: number)
--     print("update", dt)

--     self.elapsed += dt
--     if self.lastTick + 5 < self.elapsed then
--         self.lastTick = self.elapsed
--         print("Updated in 5 seconds")
--     end
-- end

function Test._exit(self: Test)
    print("Entity go bye-bye!")
end

return Test