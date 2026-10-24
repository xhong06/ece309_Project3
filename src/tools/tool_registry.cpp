// src/tools/tool_registry.cpp — YOU IMPLEMENT (spec §4.3).
//
// A hand-written hash table. No std::unordered_map / std::map / std::set.
// Every function below is a stub so the starter compiles. Replace each body.

#include "tools/tool_registry.h"
#include <cstdint>


ToolRegistry::ToolRegistry() {
    size_ = 0; // The number of tools currently registered in the hash table is 0 to start.
    buckets_.resize(8); // Start with 8 buckets which is a power of 2 and within the range of 1 to 64.
}

ToolRegistry::~ToolRegistry() = default;   // unique_ptrs in your buckets free the tools

std::size_t ToolRegistry::index_for(std::string_view key) const {
    return (hash(key) & (buckets_.size() - 1)); // Use bitwise AND to compute the index for the bucket based on the key's hash and the buckets count. 
    //This is appropriate because the number of buckets is always a power of 2.
}

std::size_t ToolRegistry::hash(std::string_view key) noexcept {
    std::uint64_t h = 1469598103934665603ULL;                         // "offset basis"
    for (unsigned char c : key) { h ^= c; h *= 1099511628211ULL; }     // "FNV prime"
    return static_cast<std::size_t>(h);
}

bool ToolRegistry::add(std::unique_ptr<Tool> tool) {
    if(tool == nullptr) return false; // Check if the tool is null before even attempting to find/add it.
    
    std::string tool_name = tool->name(); // Get the name of the tool to be added.
    Tool * tool_found = find(tool_name); // Search for the new tool.
    if(tool_found != nullptr) { // If the tool is present,
        return false; // Then return false.
    } else { // Else, 
        buckets_[index_for(tool_name)].push_back(std::move(tool)); // Add the tool into the hash table.
        size_++; // Increment the number of tools in the hash table.
        return true; // Return true.
    }

}

Tool* ToolRegistry::find(std::string_view name) const {
    std::size_t index = index_for(name); // Computes the index using the hash function bitwise AND with the number of buckets minus 1. 
    //This gives the index of the bucket where the tool should be located.

    /* **** CANNOT USE THIS FOR LOOP SINCE buckets_[index].size() returns unsigned std::size_t AND NOT AN int.

        for(int i = 0; i < static_cast<int>(buckets_[index].size()); i++) { // Check each tool within the bucket at the found index to see if the tool searched for is present. 
            if(buckets_[index][i]->name() == name) { 
                return buckets_[index][i].get(); // If the tool is found, return a raw pointer pointing to the tool. The unique_ptr<Tool> is dereferenced using the .get() to get the raw pointer.
            }
        }
    */

    for(const auto& entry : buckets_[index]) { // Check each tool within the bucket at the found index to see if the tool searched for is present. 
        if(entry->name() == name) { 
            return entry.get(); // If the tool is found, return a raw pointer pointing to the tool. The unique_ptr<Tool> is dereferenced using the .get() function to get the raw pointer.
        }
    }
    return nullptr; // If the tool is not found in the bucket, return nullptr, showing that the tool is not registered in the hash table.
}

std::size_t ToolRegistry::size() const noexcept {
    return size_; // Return the number of tools registered in the hash table.
}

void ToolRegistry::for_each(const std::function<void(const Tool&)>& fn) const {
    for(const auto& bucket : buckets_) { // Iterate through each bucket in the hash table
        for(const auto& entry : bucket) { // Iterate through each tool in every bucket
            fn(*entry); // call fn for each tool
        }
    }
}

std::size_t ToolRegistry::bucket_count() const noexcept {
    return buckets_.size(); // Return the number of buckets currently in the hash table.
}

double ToolRegistry::load_factor() const noexcept {
    return static_cast<double>(size_) / buckets_.size(); // Return the LF which is the number of tools divided by the number of buckets. 
    // This is a measure of how full the hash table is.
    // Load factor can never divide by zero since the number of buckets always starts at 8 and only ever doubles.
}

std::size_t ToolRegistry::max_probe_length() const {
    std::size_t count = 0; // Set the inital longest length to 0.
    for(const auto& bucket : buckets_) { // Check each bucket in hash table.
        if(bucket.size() > count) count = bucket.size(); // If there is a bucket that is fuller than the previous ones, update count to the new tool count.
    }
    return count; // Return the longest count.
}
