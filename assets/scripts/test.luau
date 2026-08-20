const Test = {
    elapsed = 0,
    lastTick = 0,
}

function Test:_init()
    print("hello, init!", self)
end

function Test:_update(dt: number)
    self.elapsed += dt
    
    if self.elapsed >= self.lastTick + 5 then
        self.lastTick = self.elapsed
        print("5 seconds elpased lol", self.elapsed)
    end
end

return Test