// src/tools/workspace.cpp — YOU IMPLEMENT (spec §4.4).
//
// Workspace::resolve: a purely lexical check, steps 1–6 of the spec.
// Do not touch the filesystem here (no exists(), canonical(), ...).
// The constructor, root() and check_symlinks() are provided in
// workspace_provided.cpp.

#include "tools/workspace.h"

namespace fs = std::filesystem;

bool Workspace::resolve(std::string_view user_path, fs::path* out,
                        std::string* reason) const {
    if(user_path == "") {   // Check if the user_path is empty.
        *reason = "empty path";
        return false;
    }
    if(user_path.find('\0') != std::string_view::npos) { // Check if '\0' is present preemptively in the user_path.
        *reason = "path contains a NUL byte";
        return false;
    }

    std::filesystem::path p{std::string(user_path)};
    if(p.is_absolute() || p.has_root_path()) {  // Check if the user_path is not a workspace-relative path.
        *reason = "absolute paths are not allowed; use a workspace-relative path";
        return false;
    }

    std::filesystem::path n = p.lexically_normal();
    if(*n.begin() == ".."){ // Check if the user_path starts with ".."; if so, reject.
        *reason = "path escapes the workspace";
        return false;
    }

    if(n == ".") {  // If the user_path starts with ".", then it is correct.
        *out = root();
        return true;
    } else {
        *out = root() / n;
        return true;
    }
}
