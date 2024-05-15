#include <functional>
#include <optional>
#include <string>

class LoadStage
{
private:
    std::function<bool()> callback;
    std::optional<std::string> name;

public:
    LoadStage(const std::function<bool()>& callback, std::optional<std::string> name );
    [[nodiscard]] const std::optional<std::string>& getName() const;

    bool process();
};