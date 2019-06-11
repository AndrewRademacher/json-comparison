#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <fmt/format.h>
#include <string>
#include <string_view>
#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <boost/preprocessor.hpp>

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

class message {
public:
    message()
            :_document(rapidjson::kObjectType) { }

    rapidjson::Document& to_json() {
        return _document;
    }

protected:
    rapidjson::Document _document;
};

#define MSG_PROP_STRING(name)\
    std::string_view name() const {\
        const auto& val = _document[#name];\
        return std::string_view(val.GetString(), val.GetStringLength());\
    }\
    void name(const std::string& name) {\
        _document.AddMember(#name, name, _document.GetAllocator());\
    }\

#define MSG_PROP_UINT32(name)\
    uint32_t name() const {\
        return _document[#name].GetInt();\
    }\
    void name(uint32_t name) {\
        _document.AddMember(#name, name, _document.GetAllocator());\
    }\

class address : public message {
public:

    MSG_PROP_STRING(line_1);
    MSG_PROP_STRING(line_2);
    MSG_PROP_STRING(city);
    MSG_PROP_STRING(state);
    MSG_PROP_UINT32(zip);

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
