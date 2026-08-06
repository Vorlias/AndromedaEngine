#include "Engine/Time.h"
#include "Engine/Log.h"
#include <print>
#include <SDL3/SDL.h>

namespace andromeda {
	void sleep(Time time) {
		SDL_Delay(time.toMilliseconds());
	}

	Time Time::Zero;
} // namespace andromeda