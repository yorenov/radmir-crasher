#include "LoadController.h"
#include <cassert>

#include "defines.h"

bool LoadController::process()
{
	if (finished)
		return true;

	if (auto initStage = stages.front();
            initStage.process())
	{
		stages.pop_front();
		stages.emplace_back(initStage);

		const auto& stageName = initStage.getName();

		debugHWL("INIT", "The load stage has been successfully completed! [ID: %i | Name: %s]", currentStage,
				stageName.has_value() ? stageName.value().c_str() : "(unnamed)");

		++currentStage;

		if (static_cast<size_t>(currentStage) >= stages.size())
			return finished = true;
	}

	return false;
}

bool LoadController::isFinished() const
{
	return finished;
}

void LoadController::add(const std::function<bool()>& callback, const std::optional<std::string>& stageName)
{
	assert(callback != nullptr);

	debugHWL("INIT", "New stage! [ID: %i | Name: %s]", stages.size(),
             stageName.has_value() ? stageName.value().c_str() : "(unnamed)");

	stages.emplace_back(callback, stageName);
}

LoadController::LoadController()
{
    finished = false;
}
