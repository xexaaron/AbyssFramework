#include <AbyssFramework/Log.h>
#include <AbyssFramework/containers/BiMap.hpp>
#include <AbyssFramework/Macros.h>

int main() {
    auto& cfg = aby::log::Logger::get().config();
    cfg.add_callback([](const aby::log::Message& msg) {
        return "Oh No An Error Occured";
    });
    log_info("Hello World!");
}