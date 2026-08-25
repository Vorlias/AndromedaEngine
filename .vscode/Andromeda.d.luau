declare extern type Transform with
    --- The position of the object
    position: Vector3
    --- The scale of the object
    scale: Vector3
    --- The euler-angle rotation of the object
    rotation: Vector3
end

declare extern type LuauScriptComponent with
    enabled: boolean
    read filePath: string
end

declare extern type Entity with
    --- The parent of the entity
    read parent: Entity?
    --- The entity's id
    read id: number
    --- The name of the entity
    name: string
    --- The transform of the entity
    read transform: Transform
    --- This script
    read script: LuauScriptComponent
end

--- The entity that this script is attached to
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
    read magnitude: number
    read normalized: Vector2
end
declare Vector2: {
    new: (x: number?, y: number?) -> Vector2
}

declare extern type Vector3 with
    x: number
    y: number
    z: number
    --- the magnitude of this vector
    read magnitude: number
    --- this vector as a normalized vector
    read normalized: Vector3

	function __add(self, other: Vector3): Vector3
	function __div(self, other: Vector3 | number): Vector3
	function __idiv(self, other: Vector3 | number): Vector3
	function __mul(self, other: Vector3 | number): Vector3
	function __sub(self, other: Vector3): Vector3
	function __unm(self): Vector3
end
declare Vector3: {
    new: (x: number?, y: number?, z: number?) -> Vector3
}

type EntityCtor = {
    new: (name: string?) -> Entity
}

declare Entity: EntityCtor