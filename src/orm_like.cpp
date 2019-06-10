#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <fmt/format.h>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>

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
    const std::string& line_1() const {
        return _line1;
    }

    const std::string& line_2() const {
        return _line2;
    }

    const std::string& city() const {
        return _city;
    }

    const std::string& state() const {
        return _state;
    }

    uint32_t zip() const {
        return _zip;
    }

    void line_1(std::string line_1) {
        _line1 = std::move(line_1);
    }

    void line_2(std::string line_2) {
        _line2 = std::move(line_2);
    }

    void city(std::string city) {
        _city = std::move(city);
    }

    void state(std::string state) {
        _state = std::move(state);
    }

    void zip(uint32_t zip) {
        _zip = zip;
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
        const auto& line1 = doc["line_1"];
        a._line1 = std::string(line1.GetString(), line1.GetStringLength());
        const auto& line2 = doc["line_2"];
        a._line2 = std::string(line2.GetString(), line2.GetStringLength());
        const auto& city = doc["city"];
        a._city = std::string(city.GetString(), city.GetStringLength());
        const auto& state = doc["state"];
        a._state = std::string(state.GetString(), state.GetStringLength());
        const auto& zip = doc["zip"];
        a._zip = zip.GetInt();
        return a;
    }

private:
    std::string _line1;
    std::string _line2;
    std::string _city;
    std::string _state;
    uint32_t _zip;
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
