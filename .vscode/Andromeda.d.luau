declare extern type Transform with
end
declare extern type LuauScriptComponent with
    enabled: boolean
end

declare extern type Entity with
    parent: Entity?
    id: number
    name: string
    transform: Transform?
    script: LuauScriptComponent
end

declare entity: Entity

declare task: {
    -- defer: <A..., R...>(threadOrFunction: thread | (A...) -> R..., A...) -> thread,
    spawn: <A..., R...>(threadOrFunction: thread | (A...) -> R..., A...) -> thread,
    -- delay: <A..., R...>(duration: number?, threadOrFunction: thread | (A...) -> R..., A...) -> thread,
    wait: (duration: number?) -> number,
    -- cancel: (thread: thread) -> (),
}

declare extern type Vector2 with
    x: number
    y: number
    magnitude: number
    normalized: Vector2
end
declare Vector2: {
    new: (x: number?, y: number?) -> Vector2
}

declare extern type Vector3 with
    x: number
    y: number
    z: number
    magnitude: number
    normalized: Vector3
end
declare Vector3: {
    new: (x: number?, y: number?, z: number?) -> Vector3
}

type EntityCtor = {
    new: (name: string?) -> Entity
}

declare Entity: EntityCtor