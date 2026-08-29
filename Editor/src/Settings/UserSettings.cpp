#include "UserSettings.h"
#define TOML_IMPLEMENTATION
#include "toml.hpp"
#include <fstream>

// static andromeda::Vector2 toVector2(toml::array* array) {
//     auto val1 = array->get(0)->as_floating_point();
//     auto val2 = array->get(0)->as_floating_point();

//     std::cout << val1 << ", " << val2 << std::endl;

//     return andromeda::Vector2(&val1, &val2);
// }

bool andromeda::UserSettings::Load(const std::filesystem::path& path) {
	if (std::filesystem::exists(path)) {
		toml::table tbl;
		try {
			tbl = toml::parse_file(path.string());

			auto window = tbl["window"];
			{
				auto windowSize = window["size"];
				{
					auto x = windowSize[0].value<float>();
					auto y = windowSize[1].value<float>();

					if (x && y) {
						this->windowSize = Vector2(x.value(), y.value());
					}
				}

				auto windowPos = window["position"];
				{
					auto x = windowPos[0].value<float>();
					auto y = windowPos[1].value<float>();

					if (x && y) {
						this->windowPosition = Vector2(x.value(), y.value());
					}
				}

                this->maximized = window["maximized"].value<bool>().value_or(true);
			}

			return true;
		} catch (const toml::parse_error& err) {
			andromeda::error("Failed to load configuration file {}", path.string());
			return false;
		}
	}

	return false;
}

void andromeda::UserSettings::Save(const std::filesystem::path& path) {
	auto windowSettings = toml::table{
		{"size", toml::array{windowSize.x, windowSize.y}}, {"position", toml::array{windowPosition.x, windowPosition.y}}, {"maximized", maximized}
	};

	auto tbl = toml::table{{"window", windowSettings}};

	auto res = toml::toml_formatter(tbl);
	std::ofstream file(path);
	file << res;
	file.flush();
	file.close();
}