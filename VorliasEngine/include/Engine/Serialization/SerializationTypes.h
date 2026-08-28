#pragma once
#include "yaml-cpp/yaml.h"

#include "Engine/Data/Vector.h"
#include "Engine/Data/Color.h"
#include "Engine/Data/Rect.h"

namespace YAML {
	template<>
	struct convert<andromeda::Vector3> {
		static Node encode(const andromeda::Vector3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, andromeda::Vector3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<andromeda::Color> {
		static Node encode(const andromeda::Color& rhs) {
			Node node;
			node.push_back(rhs.r);
			node.push_back(rhs.g);
			node.push_back(rhs.b);
			node.push_back(rhs.a);
			return node;
		}

		static bool decode(const Node& node, andromeda::Color& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.r = node[0].as<float>();
			rhs.g = node[1].as<float>();
			rhs.b = node[2].as<float>();
			rhs.a = node[2].as<float>();
			return true;
		}
	};
} // namespace YAML

namespace andromeda {
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Color& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.r << v.g << v.b << v.a << YAML::EndSeq;
		return out;
	}
}