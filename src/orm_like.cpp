#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <fmt/format.h>
#include <string>
#include <string_view>
#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

const char* address_schema = R"({
    "type": "object",
    "properties": {
        "line_1": {
            "type": "string"
        },
        "line_2": {
            "type": "string"
        },
        "city": {
            "type": "string"
        },
        "state": {
            "type": "string"
        },
        "zip": {
            "type": "integer",
            "minimum": "10000"
        }
    }
})";

rapidjson::SchemaDocument parse_schema_document(const char* schema) {
    rapidjson::Document doc;
    doc.Parse(schema);
    if (doc.HasParseError()) {
        throw std::runtime_error(fmt::format("Failed to parse json schema document: {} at {}", doc.GetParseError(),
                doc.GetErrorOffset()));
    }

    return rapidjson::SchemaDocument(doc);
}

class address {
public:

    address()
            :_document(rapidjson::kObjectType) { }

    std::string_view line_1() const {
        const auto& val = _document["line_1"];
        return std::string_view(val.GetString(), val.GetStringLength());
    }

    std::string_view line_2() const {
        const auto& val = _document["line_2"];
        return std::string_view(val.GetString(), val.GetStringLength());
    }

    std::string_view city() const {
        const auto& val = _document["city"];
        return std::string_view(val.GetString(), val.GetStringLength());
    }

    std::string_view state() const {
        const auto& val = _document["state"];
        return std::string_view(val.GetString(), val.GetStringLength());
    }

    uint32_t zip() const {
        return _document["zip"].GetInt();
    }

    void line_1(const std::string& line_1) {
        _document.AddMember("line_1", line_1, _document.GetAllocator());
    }

    void line_2(const std::string& line_2) {
        _document.AddMember("line_2", line_2, _document.GetAllocator());
    }

    void city(const std::string& city) {
        _document.AddMember("city", city, _document.GetAllocator());
    }

    void state(const std::string& state) {
        _document.AddMember("state", state, _document.GetAllocator());
    }

    void zip(uint32_t zip) {
        _document.AddMember("zip", zip, _document.GetAllocator());
    }

    rapidjson::Document& to_json() {
        return _document;
    }

    static address from_json(const std::string& data) {
        static const rapidjson::SchemaDocument schema = parse_schema_document(address_schema);

        rapidjson::Document doc;
        doc.Parse(data.data());
        if (doc.HasParseError()) {
            throw std::runtime_error(fmt::format("Failed to parse json schema document: {} at {}", doc.GetParseError(),
                    doc.GetErrorOffset()));
        }

        rapidjson::SchemaValidator validator(schema);
        if (!doc.Accept(validator)) {
            rapidjson::StringBuffer buf;
            validator.GetInvalidSchemaPointer().StringifyUriFragment(buf);
            throw std::runtime_error(fmt::format("Failed to validate json document at: {}", buf.GetString()));
        }

        address a;
        a._document = std::move(doc);
        return a;
    }

private:
    rapidjson::Document _document;
};

TEST_CASE("basic address") {
    const char* data = R"({
        "line_1": "111 W. 2nd St.",
        "line_2": "#452",
        "city": "Kansas City",
        "state": "MO",
        "zip": 64111
    })";

    address a = address::from_json(data);
    REQUIRE(a.line_1() == "111 W. 2nd St.");
    REQUIRE(a.line_2() == "#452");
    REQUIRE(a.city() == "Kansas City");
    REQUIRE(a.state() == "MO");
    REQUIRE(a.zip() == 64111);
}

TEST_CASE("set address") {
    const char* expected_data = R"({
        "line_1": "111 W. 2nd St.",
        "line_2": "#452",
        "city": "Kansas City",
        "state": "MO",
        "zip": 64111
    })";

    address a;
    a.line_1("111 W. 2nd St.");
    a.line_2("#452");
    a.city("Kansas City");
    a.state("MO");
    a.zip(64111);
    rapidjson::Document& actual = a.to_json();
    rapidjson::Document expected;
    expected.Parse(expected_data);

    REQUIRE(actual == expected);
}
