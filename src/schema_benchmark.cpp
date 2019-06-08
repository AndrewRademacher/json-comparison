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
//BENCHMARK_CAPTURE(ParseFile, canada,                "../data/nativejson-benchmark/canada.json");
//BENCHMARK_CAPTURE(ParseFile, citm_catalog,          "../data/nativejson-benchmark/citm_catalog.json");
//BENCHMARK_CAPTURE(ParseFile, twitter,               "../data/nativejson-benchmark/twitter.json");
BENCHMARK_CAPTURE(ParseFile, floats,                "../data/numbers/floats.json",            "../schema/numbers/floats.json");
BENCHMARK_CAPTURE(ParseFile, signed_ints,           "../data/numbers/signed_ints.json",       "../schema/numbers/signed_ints.json");
BENCHMARK_CAPTURE(ParseFile, unsigned_ints,         "../data/numbers/unsigned_ints.json",     "../schema/numbers/small_signed_ints.json");
BENCHMARK_CAPTURE(ParseFile, small_signed_ints,     "../data/numbers/small_signed_ints.json", "../schema/numbers/unsigned_ints.json");

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
//BENCHMARK_CAPTURE(ParseString, canada,              "../data/nativejson-benchmark/canada.json");
//BENCHMARK_CAPTURE(ParseString, citm_catalog,        "../data/nativejson-benchmark/citm_catalog.json");
//BENCHMARK_CAPTURE(ParseString, twitter,             "../data/nativejson-benchmark/twitter.json");
BENCHMARK_CAPTURE(ParseString, floats,              "../data/numbers/floats.json",            "../schema/numbers/floats.json");
BENCHMARK_CAPTURE(ParseString, signed_ints,         "../data/numbers/signed_ints.json",       "../schema/numbers/signed_ints.json");
BENCHMARK_CAPTURE(ParseString, unsigned_ints,       "../data/numbers/unsigned_ints.json",     "../schema/numbers/small_signed_ints.json");
BENCHMARK_CAPTURE(ParseString, small_signed_ints,   "../data/numbers/small_signed_ints.json", "../schema/numbers/unsigned_ints.json");


//////////////////////////////////////////////////////////////////////////////
// serialize JSON
//////////////////////////////////////////////////////////////////////////////

static void Dump(benchmark::State& state, const char* filename, int indent)
{
    std::ifstream f(filename);
    std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    Document j;
    j.Parse(str.data());

    while (state.KeepRunning())
    {
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        j.Accept(writer);
    }

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    j.Accept(writer);
    state.SetBytesProcessed(state.iterations() * buffer.GetSize());
}
//BENCHMARK_CAPTURE(Dump, jeopardy / -,      "../data/jeopardy/jeopardy.json",                 -1);
//BENCHMARK_CAPTURE(Dump, jeopardy / 4,      "../data/jeopardy/jeopardy.json",                 4);
//BENCHMARK_CAPTURE(Dump, canada / -,        "../data/nativejson-benchmark/canada.json",       -1);
//BENCHMARK_CAPTURE(Dump, canada / 4,        "../data/nativejson-benchmark/canada.json",       4);
//BENCHMARK_CAPTURE(Dump, citm_catalog / -,  "../data/nativejson-benchmark/citm_catalog.json", -1);
//BENCHMARK_CAPTURE(Dump, citm_catalog / 4,  "../data/nativejson-benchmark/citm_catalog.json", 4);
//BENCHMARK_CAPTURE(Dump, twitter / -,       "../data/nativejson-benchmark/twitter.json",      -1);
//BENCHMARK_CAPTURE(Dump, twitter / 4,       "../data/nativejson-benchmark/twitter.json",      4);
//BENCHMARK_CAPTURE(Dump, floats / -,        "../data/numbers/floats.json",                    -1);
//BENCHMARK_CAPTURE(Dump, floats / 4,        "../data/numbers/floats.json",                    4);
//BENCHMARK_CAPTURE(Dump, signed_ints / -,   "../data/numbers/signed_ints.json",               -1);
//BENCHMARK_CAPTURE(Dump, signed_ints / 4,   "../data/numbers/signed_ints.json",               4);
//BENCHMARK_CAPTURE(Dump, unsigned_ints / -, "../data/numbers/unsigned_ints.json",             -1);
//BENCHMARK_CAPTURE(Dump, unsigned_ints / 4, "../data/numbers/unsigned_ints.json",             4);
//BENCHMARK_CAPTURE(Dump, small_signed_ints / -,   "../data/numbers/small_signed_ints.json",   -1);
//BENCHMARK_CAPTURE(Dump, small_signed_ints / 4,   "../data/numbers/small_signed_ints.json",   4);


BENCHMARK_MAIN();
