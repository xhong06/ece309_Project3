// tests/p3/test_p3.cpp — YOUR P3 TEST SUITE.
//
// The five tests below are worked examples. Keep them, and add your own
// until you have AT LEAST 15 (spec §6 lists what must be covered).
// Name every test <area>_<what>: reg_ for the registry, ws_ for the
// workspace, calc_ for the calculator.
//
// Build & run:   cmake --build build && ./build/test_p3
// One area:      ./build/test_p3 reg_        (names starting with "reg_")
// One test:      ./build/test_p3 =reg_add_and_find

#include "../mini_test.h"
#include "../test_helpers.h"     // TempDir, write_text, args1, ...

#include "tools/calculator.h"
#include "tools/tool_registry.h"
#include "tools/workspace.h"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

namespace {

// A tool whose name you choose — use it to fill the registry in tests.
class NamedTool : public Tool {
public:
    explicit NamedTool(std::string n) : n_(std::move(n)), schema_(JsonValue::object()) {}
    std::string name() const override { return n_; }
    std::string description() const override { return "test tool"; }
    const JsonValue& schema() const override { return schema_; }
    ToolResult invoke(const JsonValue&, ToolContext&) override { return ToolResult::success(n_); }
private:
    std::string n_;
    JsonValue   schema_;   // an empty JSON object: {}
};

// Tokenize with the provided tokenizer, then run YOUR calc_evaluate.
bool eval(const std::string& expr, double* value, std::string* error) {
    std::vector<CalcToken> tokens;
    if (!calc_tokenize(expr, &tokens, error)) return false;
    return calc_evaluate(tokens, value, error);
}

}  // namespace

// ---------------------------------------------------------------- examples

TEST(reg_add_and_find) {
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("alpha");
    Tool* raw = t.get();                        // remember the address before moving
    REQUIRE(r.add(std::move(t)));
    CHECK(r.size() == 1);
    CHECK(r.find("alpha") == raw);              // same object, not a copy
    CHECK(r.find("beta") == nullptr);
}

TEST(reg_duplicate_keeps_original) {
    ToolRegistry r;
    auto first = std::make_unique<NamedTool>("alpha");
    Tool* original = first.get();
    REQUIRE(r.add(std::move(first)));
    CHECK(!r.add(std::make_unique<NamedTool>("alpha")));
    CHECK(r.find("alpha") == original);
    CHECK(r.size() == 1);
}

TEST(ws_rejects_parent_escape) {
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;
    REQUIRE(ws.resolve("notes/todo.txt", &out, &reason));   // sanity: accepts *something*
    CHECK(out == ws.root() / "notes" / "todo.txt");
    CHECK(!ws.resolve("../etc/passwd", &out, &reason));
    CHECK(!reason.empty());
}

TEST(calc_precedence) {
    double v = 0;
    std::string err;
    REQUIRE(eval("2 + 3 * 4", &v, &err));
    CHECK(v == 14);
}

TEST(calc_division_by_zero) {
    double v = 0;
    std::string err;
    CHECK(!eval("1 / 0", &v, &err));
    CHECK(err == "division by zero");
}

// ------------------------------------------------------------- your tests
//
// Still to write (spec §6 has the full list). Suggested names:
//   reg_null_rejected, reg_for_each_visits_each_once
//   reg_10000_tools               REQUIRED: print bucket_count / load_factor /
//                                 max_probe_length, check the bound
//   reg_alternative_hash          REQUIRED: longest chain of a weak hash
//   ws_<case>                     REQUIRED: every row of the table in spec §4.4
//   calc_<case>                   left-to-right, parentheses, negatives

TEST(reg_add_then_find) {
    ToolRegistry r; 
    auto t = std::make_unique<NamedTool>("calculator");
    Tool* raw = t.get();                        // remember the address before moving
    REQUIRE(r.add(std::move(t)));
    CHECK(r.size() == 1);
    CHECK(r.find("calculator") == raw);              // same object, not a copy
    CHECK(r.find("teleport") == nullptr);
}

TEST(reg_rejects_duplicate) {
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("calculator");
    Tool* raw = t.get();
    auto t2 = std::make_unique<NamedTool>("calculator");

    REQUIRE(r.add(std::move(t)));
    REQUIRE(!(r.add(std::move(t2))));
    CHECK(r.size() == 1); // The number of tools in the hash table should be 1 since the second .add function 
    //should return false since it finds the first calculator tool already in the table.
    CHECK(r.find("calculator"));
    Tool* found_tool = r.find("calculator");
    CHECK(found_tool == raw);
    
}

TEST(reg_rejects_nullptr) {
    ToolRegistry r; 
    std::unique_ptr<NamedTool> empty;
    REQUIRE(!(r.add(std::move(empty))));
    CHECK(r.size() == 0); // Since the tool added was nullptr, it should return false immediately and not increment the size of the ToolRegistry.
    CHECK(r.add(std::move(empty)) == false); // Confirm that the .add function immediately returns false because the tool is nullptr.
}
    
TEST(reg_find_absent) {
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("calculator");
    auto t2 = std::make_unique<NamedTool>("camera");
    auto t3 = std::make_unique<NamedTool>("web-search");
    Tool* rp3 = t3.get();
    REQUIRE(r.add(std::move(t)));
    REQUIRE(r.add(std::move(t2)));
    REQUIRE(r.add(std::move(t3)));
    CHECK(r.size() == 3); // Confirm that the number of tools in the hash table is 3.
    CHECK(r.find("web-search") == rp3); // Confirm that a tool that was added into the hash table is in fact added correctly.
    CHECK(r.find("web-searc") == nullptr); // Near-miss edge case.
    CHECK(r.find("teleport") == nullptr); // Since teleport is not in the hash table, it will return nullptr.
}

TEST(reg_for_each_visits_once) {
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("calculator");
    auto t2 = std::make_unique<NamedTool>("camera");
    auto t3 = std::make_unique<NamedTool>("web-search");
    auto t4 = std::make_unique<NamedTool>("microwave");
    auto t5 = std::make_unique<NamedTool>("gps");
    auto t6 = std::make_unique<NamedTool>("video-call");
    auto t7 = std::make_unique<NamedTool>("timer");
    auto t8 = std::make_unique<NamedTool>("stopwatch");
    
    std::vector<std::string> names = {
        "calculator", "camera", "web-search", "microwave", "gps", "video-call",
        "timer", "stopwatch"
    };

    REQUIRE(r.add(std::move(t)));
    REQUIRE(r.add(std::move(t2)));
    REQUIRE(r.add(std::move(t3)));
    REQUIRE(r.add(std::move(t4)));
    REQUIRE(r.add(std::move(t5)));
    REQUIRE(r.add(std::move(t6)));
    REQUIRE(r.add(std::move(t7)));
    REQUIRE(r.add(std::move(t8)));

    std::vector<std::string> tool_visited;
    r.for_each([&](const Tool& tool) {               // a lambda: runs once per tool
        std::printf("%s\n", tool.name().c_str()); // Print out the tool names.
        tool_visited.push_back(tool.name()); // This not only keeps track of the number of 
        //tools but also keeps track of their names.
    });

    CHECK(r.size() == names.size());
    CHECK(tool_visited.size() == names.size());
    for (const auto& n : names) {
        CHECK(std::count(tool_visited.begin(), tool_visited.end(), n) == 1);
        //This function counts the amount of times each name in tool_visited appears and 
        //checks if it's equal to 1 since duplicates cannot happen in the hash table.
    }
}

TEST(reg_scale_10000) {
    ToolRegistry r;
    std::vector<std::string> tool_names;
    std::vector<Tool*> tool_pointers;
    for(int i = 0; i < 10000; i++) {
        auto t = std::make_unique<NamedTool>("tool_" + std::to_string(i));
        tool_names.push_back(t->name());
        REQUIRE(r.add(std::move(t)));
        tool_pointers.push_back(r.find(tool_names[i]));
        CHECK(r.load_factor() <= 1.0);
    }
    
    CHECK(r.size() == 10000);
    CHECK(r.max_probe_length() <= 64);
    for(std::size_t i = 0; i < tool_pointers.size(); i++) {
        CHECK(tool_pointers[i] != nullptr);
        CHECK(r.find(tool_names[i]) == tool_pointers[i]);
    }

    std::printf("Load Factor: %f\nMax Probe Length: %zu\nNumber of Buckets: %zu\n", r.load_factor(), r.max_probe_length(), r.bucket_count());

}

TEST(reg_weak_hash_comparison) {
    ToolRegistry r;
    
    // FIRST, we place 10000 tools into a FNV-1a Hash Table
    std::vector<std::string> tool_names;
    for(int i = 0; i < 10000; i++){
        auto t = std::make_unique<NamedTool>("tool_" + std::to_string(i));
        tool_names.push_back(t->name());
        REQUIRE(r.add(std::move(t)));
    }
    std::printf("FNV-1a:   Max Chain: %zu  Number of Buckets: %zu\n", r.max_probe_length(), r.bucket_count());
    
    // NEXT, we simulate a second hash table with a much weaker hash. This hash bases its placement off of the first character of the name and size of the name.
    std::vector<std::size_t> counts(r.bucket_count(), 0);
    for(int i = 0; i < 10000; i++){
        counts[(static_cast<unsigned char>(tool_names[i][0]) + tool_names[i].size()) % r.bucket_count()]++;
    }
    std::size_t fullest_bucket = *std::max_element(counts.begin(), counts.end());
    CHECK(fullest_bucket == 9000); // Our anticipated Max Probe Length for the weak hash is 9000 since there are 9000 entries (1000-9999) that have a name with the same length.
    CHECK(r.max_probe_length() < fullest_bucket); // This check confirms that the weaker hash is weaker: in the weaker hash, at least one bucket will be much more full than any of those in the FNV-1a table.
    std::printf("weak-hash:   Max Chain: %zu  Number of Buckets: %zu\n", fullest_bucket, r.bucket_count());
}






int main(int argc, char** argv) { return mt::run_all(argc, argv); }
