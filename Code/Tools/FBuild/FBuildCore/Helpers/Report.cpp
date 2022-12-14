// Report
//------------------------------------------------------------------------------

// Includes
//------------------------------------------------------------------------------
#include "Report.h"

// FBuild
#include "Tools/FBuild/FBuildCore/FBuild.h"

// CONSTRUCTOR
//------------------------------------------------------------------------------
Report::Report(size_t initialCapacity, bool resizeable)
    : m_LibraryStats(512, true)
{
}

// DESTRUCTOR
//------------------------------------------------------------------------------
Report::~Report() {}

// GetLibraryStats
//------------------------------------------------------------------------------
void Report::GetLibraryStats(const FBuildStats& stats)
{
    // gather library stats, sorted by CPU cost
    GetLibraryStatsRecurse(m_LibraryStats, stats.GetRootNode(), nullptr);
    m_LibraryStats.SortDeref();
}

// GetLibraryStatsRecurse
//------------------------------------------------------------------------------
void Report::GetLibraryStatsRecurse(Array< LibraryStats* >& libStats, const Node* node, LibraryStats* currentLib) const
{
    // skip nodes we've already seen
    if (node->GetStatFlag(Node::STATS_REPORT_PROCESSED))
    {
        return;
    }
    node->SetStatFlag(Node::STATS_REPORT_PROCESSED);

    const Node::Type type = node->GetType();

    // object?
    if (type == Node::OBJECT_NODE)
    {
        if (currentLib == nullptr)
        {
            ASSERT(false); // should not be possible with a correctly formed dep graph
            return;
        }

        currentLib->objectCount++;

        const bool cacheHit = node->GetStatFlag(Node::STATS_CACHE_HIT);
        const bool cacheMiss = node->GetStatFlag(Node::STATS_CACHE_MISS);
        if (cacheHit || cacheMiss)
        {
            currentLib->objectCount_Cacheable++;

            if (cacheHit)
            {
                currentLib->objectCount_CacheHits++;
            }
            if (node->GetStatFlag(Node::STATS_CACHE_STORE))
            {
                currentLib->objectCount_CacheStores++;
                currentLib->cacheTimeMS += node->GetCachingTime();
            }
        }

        if (cacheHit || cacheMiss || node->GetStatFlag(Node::STATS_BUILT))
        {
            currentLib->objectCount_OutOfDate++;
            currentLib->cpuTimeMS += node->GetProcessingTime();
        }

        return; // Stop recursing at Objects
    }

    bool isLibrary = false;
    switch (type)
    {
    case Node::DLL_NODE:        isLibrary = true; break;
    case Node::LIBRARY_NODE:    isLibrary = true; break;
    case Node::OBJECT_LIST_NODE: isLibrary = true; break;
    case Node::CS_NODE:
    {
        isLibrary = node->GetName().EndsWithI(".dll"); // TODO:C - robustify this (could have an aribtrary extension)
        break;
    }
    default: break; // not a library
    }

    if (isLibrary)
    {
        currentLib = FNEW(LibraryStats);
        currentLib->library = node;
        currentLib->cpuTimeMS = 0;
        currentLib->objectCount = 0;
        currentLib->objectCount_OutOfDate = 0;
        currentLib->objectCount_Cacheable = 0;
        currentLib->objectCount_CacheHits = 0;
        currentLib->objectCount_CacheStores = 0;
        currentLib->cacheTimeMS = 0;

        // count time for library/dll itself
        if (node->GetStatFlag(Node::STATS_BUILT) || node->GetStatFlag(Node::STATS_FAILED))
        {
            currentLib->cpuTimeMS += node->GetProcessingTime();
        }

        libStats.Append(currentLib);

        // recurse into this new lib
    }

    // Dependencies
    GetLibraryStatsRecurse(libStats, node->GetPreBuildDependencies(), currentLib);
    GetLibraryStatsRecurse(libStats, node->GetStaticDependencies(), currentLib);
    GetLibraryStatsRecurse(libStats, node->GetDynamicDependencies(), currentLib);
}

// GetLibraryStatsRecurse
//------------------------------------------------------------------------------
void Report::GetLibraryStatsRecurse(Array< LibraryStats* >& libStats, const Dependencies& dependencies, LibraryStats* currentLib) const
{
    const Dependency* const end = dependencies.End();
    for (const Dependency* it = dependencies.Begin(); it != end; ++it)
    {
        GetLibraryStatsRecurse(libStats, it->GetNode(), currentLib);
    }
}