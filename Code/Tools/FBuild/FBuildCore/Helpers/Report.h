// Report - An interface for report generation
//------------------------------------------------------------------------------
#pragma once

// Includes
//------------------------------------------------------------------------------
#include "Core/Containers/Array.h"

// Forward Declarations
//------------------------------------------------------------------------------
struct FBuildStats;
class Dependencies;
class Node;

// Report
//------------------------------------------------------------------------------
class Report
{
public:
    Report(size_t initialCapacity, bool resizeable);
    virtual ~Report();

    virtual void Generate(const FBuildStats& stats) = 0;
    virtual void Save() const = 0;
    
protected:
    struct LibraryStats
    {
        const Node *    library;
        uint32_t        cpuTimeMS;
        uint32_t        objectCount;
        uint32_t        objectCount_OutOfDate;
        uint32_t        objectCount_Cacheable;
        uint32_t        objectCount_CacheHits;
        uint32_t        objectCount_CacheStores;
        uint32_t        cacheTimeMS;

        bool operator < ( const LibraryStats & other ) const { return cpuTimeMS > other.cpuTimeMS; }
    };
    
    // gather stats
    void GetLibraryStats( const FBuildStats & stats );
    void GetLibraryStatsRecurse( Array< LibraryStats * > & libStats, const Node * node, LibraryStats * currentLib ) const;
    void GetLibraryStatsRecurse( Array< LibraryStats * > & libStats, const Dependencies & dependencies, LibraryStats * currentLib ) const;

    // intermediate collected data
    Array< LibraryStats * > m_LibraryStats;
};