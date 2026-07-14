#include "slim/common/log.h"
bool test_can_print([[maybe_unused]] std::string_view consumer, std::string_view log_level, [[maybe_unused]] std::string_view file, [[maybe_unused]] std::string_view function) {
    if(log_level == "trace") {
        return true;
    }
    return false;
}
int main() {
    using namespace slim;
    using namespace slim::common;
    log::trace(log::Message(__func__, "works", __FILE__, __LINE__));
    log::debug(log::Message(__func__, "works", __FILE__, __LINE__));
    log::error(log::Message(__func__, "works", __FILE__, __LINE__));
    log::info("text only");
    log::trace(log::Message(__func__, "setting can_log_function", __FILE__, __LINE__));
    log::set_can_log(
        []([[maybe_unused]] std::string_view consumer, [[maybe_unused]] std::string_view log_level, [[maybe_unused]] std::string_view file, [[maybe_unused]] std::string_view function) { return false; }
    );
    log::info("info call still prints");
    log::trace(log::Message(__func__, "should not print", __FILE__, __LINE__));
    log::set_can_log(test_can_print);
    log::trace(log::Message(__func__, "tracing works", __FILE__, __LINE__));
    log::debug(log::Message(__func__, "debug should not work", __FILE__, __LINE__));
}
