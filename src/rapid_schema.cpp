#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <spdlog/spdlog.h>

#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;

TEST_CASE("simple validation") {
    const char* data = R"({"first_name":"John","last_name":"Doe","age":"34"})";
//    const char* data = R"({"first_name":"John","last_name":"Doe","age":34})";
//    const char* data = R"({"first_name":"John","last_name":"Doe"})";
    const char* schema_text = R"({
        "type": "object",
        "properties": {
            "first_name": {
                "type": "string"
            },
            "last_name": {
                "type": "string"
            },
            "age": {
                "type": "integer"
            }
        },
        "required": ["first_name", "last_name", "age"]
    })";

    Document sd;
    REQUIRE(!sd.Parse(schema_text).HasParseError());
    SchemaDocument schema(sd);
    SchemaValidator validator(schema);

    Document json;
    REQUIRE(!json.Parse(data).HasParseError());
    if (!json.Accept(validator)) {
        StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        spdlog::error("Invalid schema: {}", sb.GetString());
        spdlog::error("Invalid keyword: {}", validator.GetInvalidSchemaKeyword());
        sb.Clear();
        validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        spdlog::error("Invalid document: {}", sb.GetString());
    }
}
