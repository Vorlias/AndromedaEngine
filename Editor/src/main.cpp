#include "EditorApplication.h"
#include "ArgParse.h"
#include "Engine/Main.h"
#include "Engine/LinkedList.h"

using namespace andromeda;


Application* ApplicationMain(const ApplicationInit& ap) {
	int flags = EditorApplication::EDITOR_DEFAULT;

	EditorArgParser args(ap.args);
	args.Parse();

	std::filesystem::path projectPath;
	if (args.ProjectPath.empty()) {
		pfd::select_folder fold("Open Project", "", pfd::opt::force_path);
		auto result = fold.result();
		if (result.empty())
			return nullptr;
	}

	if (args.ProjectPath == ".") {
		projectPath = std::filesystem::current_path();
	} else {
		projectPath = args.ProjectPath;
	}

	return new EditorApplication(projectPath, (EditorApplication::EditorFlags)flags);
}