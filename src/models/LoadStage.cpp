#include "LoadStage.h"
#include <cassert>

#include <utility>

LoadStage::LoadStage(const std::function<bool()>& callback, std::optional<std::string> name)
{
	assert(callback != nullptr);

	this->callback  = callback;
	this->name = std::move(name);
}

const std::optional<std::string>& LoadStage::getName() const
{
	return this->name;
}

bool LoadStage::process()
{
	return this->callback();
}
