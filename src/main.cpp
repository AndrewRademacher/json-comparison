#include <spdlog/spdlog.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <nlohmann/json.hpp>

int main() {
    spdlog::info("Hello");

    // Nlohmann
    nlohmann::json njson{
            {"first_name", "John"},
            {"last_name", "Doe"},
            {"age", 34}
    };
    spdlog::info("Nlohmann: {}", njson.dump());

    // RapidJSON
    rapidjson::Document rjson(rapidjson::kObjectType);
    rjson.AddMember("first_name", "John", rjson.GetAllocator());
    rjson.AddMember("last_name", "Doe", rjson.GetAllocator());
    rjson.AddMember("age", 34, rjson.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    rjson.Accept(writer);
    spdlog::info("RapidJSON: {}", buffer.GetString());

    return 0;
}
