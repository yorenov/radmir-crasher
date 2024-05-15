#pragma once

#include "models/LoadStage.h"

class LoadController
{
private:
	std::list<LoadStage> stages;
	bool                 finished;
	int                  currentStage;

public:
	/// <summary>
	/// Process initialization queue
	/// </summary>
	/// <returns>Loading is finished</returns>
	bool process();

	/// <summary>
	/// Returns true if all init stages ends.
	/// Otherwise, false.
	/// </summary>
	/// <returns>Is loading finished</returns>
	[[nodiscard]] bool isFinished() const;

	/// <summary>
	/// Add new load stage to queue.
	/// </summary>
	void add(const std::function<bool()>& callback, const std::optional<std::string>& stageName = std::nullopt);

	/// <summary>
	/// Default constructor.
	/// </summary>
	LoadController();
};
