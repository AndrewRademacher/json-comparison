#include <benchmark/benchmark.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <fstream>
#include <rapidjson/schema.h>

using namespace rapidjson;

SchemaDocument load_schema(const char* schema_filename) {
    std::ifstream in(schema_filename);
    IStreamWrapper isw(in);
    Document json;
    if (json.ParseStream(isw).HasParseError()) {
        throw std::runtime_error("had schema parse error");
    }

    return SchemaDocument(json);
}

//////////////////////////////////////////////////////////////////////////////
// parse JSON from file
//////////////////////////////////////////////////////////////////////////////

static void ParseFile(benchmark::State& state, const char* filename, const char* schema_filename)
{
    SchemaDocument schema = load_schema(schema_filename);
    SchemaValidator validator(schema);

    while (state.KeepRunning())
    {
        state.PauseTiming();
        auto* f = new std::ifstream(filename);
        auto* j = new Document();
        state.ResumeTiming();

        IStreamWrapper isw(*f);
        j->ParseStream(isw);
        if (!j->Accept(validator)) {
            throw std::runtime_error("failed schema validation");
        }

        state.PauseTiming();
        delete f;
        delete j;
        state.ResumeTiming();
    }

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    state.SetBytesProcessed(state.iterations() * file.tellg());
}
//BENCHMARK_CAPTURE(ParseFile, jeopardy,              "../data/jeopardy/jeopardy.json");
BENCHMARK_CAPTURE(ParseFile, canada,                "../data/nativejson-benchmark/canada.json", "../schema/nativejson-benchmark/canada.json");
//BENCHMARK_CAPTURE(ParseFile, citm_catalog,          "../data/nativejson-benchmark/citm_catalog.json");
//BENCHMARK_CAPTURE(ParseFile, twitter,               "../data/nativejson-benchmark/twitter.json");
BENCHMARK_CAPTURE(ParseFile, floats,                "../data/numbers/floats.json",              "../schema/numbers/floats.json");
BENCHMARK_CAPTURE(ParseFile, signed_ints,           "../data/numbers/signed_ints.json",         "../schema/numbers/signed_ints.json");
BENCHMARK_CAPTURE(ParseFile, unsigned_ints,         "../data/numbers/unsigned_ints.json",       "../schema/numbers/small_signed_ints.json");
BENCHMARK_CAPTURE(ParseFile, small_signed_ints,     "../data/numbers/small_signed_ints.json",   "../schema/numbers/unsigned_ints.json");

//////////////////////////////////////////////////////////////////////////////
// parse JSON from string
//////////////////////////////////////////////////////////////////////////////

static void ParseString(benchmark::State& state, const char* filename, const char* schema_filename)
{
    std::ifstream f(filename);
    std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    SchemaDocument schema = load_schema(schema_filename);
    SchemaValidator validator(schema);

    while (state.KeepRunning())
    {
        state.PauseTiming();
        auto* j = new Document();
        state.ResumeTiming();

        j->Parse(str.data());
        if (!j->Accept(validator)) {
            throw std::runtime_error("failed schema validation");
        }

        state.PauseTiming();
        delete j;
        state.ResumeTiming();
    }

    state.SetBytesProcessed(state.iterations() * str.size());
}
//BENCHMARK_CAPTURE(ParseString, jeopardy,            "../data/jeopardy/jeopardy.json");
BENCHMARK_CAPTURE(ParseString, canada,              "../data/nativejson-benchmark/canada.json", "../schema/nativejson-benchmark/canada.json");
//BENCHMARK_CAPTURE(ParseString, citm_catalog,        "../data/nativejson-benchmark/citm_catalog.json");
//BENCHMARK_CAPTURE(ParseString, twitter,             "../data/nativejson-benchmark/twitter.json");
BENCHMARK_CAPTURE(ParseString, floats,              "../data/numbers/floats.json",              "../schema/numbers/floats.json");
BENCHMARK_CAPTURE(ParseString, signed_ints,         "../data/numbers/signed_ints.json",         "../schema/numbers/signed_ints.json");
BENCHMARK_CAPTURE(ParseString, unsigned_ints,       "../data/numbers/unsigned_ints.json",       "../schema/numbers/small_signed_ints.json");
BENCHMARK_CAPTURE(ParseString, small_signed_ints,   "../data/numbers/small_signed_ints.json",   "../schema/numbers/unsigned_ints.json");

BENCHMARK_MAIN();
