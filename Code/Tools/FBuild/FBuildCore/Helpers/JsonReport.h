// JsonReport - Build JsonReport Generator
//------------------------------------------------------------------------------
#pragma once

// Includes
//------------------------------------------------------------------------------
#include "Core/Env/MSVCStaticAnalysis.h"
#include "Core/Mem/MemPoolBlock.h"
#include "Core/Strings/AString.h"
#include "Tools/FBuild/FBuildCore/Helpers/Report.h"


// Forward Declarations
//------------------------------------------------------------------------------
struct FBuildStats;
class Dependencies;
class Node;

// JsonReport
//------------------------------------------------------------------------------
class JsonReport : public Report
{
public:
    JsonReport();
    ~JsonReport();

    virtual void Generate( const FBuildStats & stats ) override;
    virtual void Save() const override;

private:
    // JsonReport sections
    void CreateOverview( const FBuildStats & stats );
    void DoCacheStats( const FBuildStats & stats );
    void DoCPUTimeByType( const FBuildStats & stats );
    void DoCPUTimeByItem( const FBuildStats & stats );
    void DoCPUTimeByLibrary();
    void DoIncludes();

    struct TimingStats
    {
        TimingStats( const char * l, float v, void * u = nullptr )
            : label( l )
            , value( v )
            , userData( u )
        {
        }

        const char *    label;
        float           value;
        void *          userData;

        bool operator < ( const TimingStats& other ) const { return value > other.value; }
    };
    
    struct IncludeStats
    {
        const Node *    node;
        uint32_t        count;
        bool            inPCH;

        bool operator < ( const IncludeStats & other ) const { return count > other.count; }

        IncludeStats *  m_Next; // in-place hash map chain
    };

    class IncludeStatsMap
    {
    public:
        IncludeStatsMap();
        ~IncludeStatsMap();

        IncludeStats * Find( const Node * node ) const;
        IncludeStats * Insert( const Node * node );

        void Flatten( Array< const IncludeStats * > & stats ) const;
    protected:
        IncludeStats * m_Table[ 65536 ];
        MemPoolBlock m_Pool;
    };

    // Helper to format some text
    void Write( MSVC_SAL_PRINTF const char * fmtString, ... ) FORMAT_STRING( 2, 3 );

    // gather stats
    // TODO: Extract into Report
    void GetIncludeFilesRecurse( IncludeStatsMap & incStats, const Node * node) const;
    void AddInclude( IncludeStatsMap & incStats, const Node * node, const Node * parentNode) const;

    // final output
    AString m_Output;
};

//------------------------------------------------------------------------------
