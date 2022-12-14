// Report - An interface for report generation
//------------------------------------------------------------------------------
#pragma once

// Forward Declarations
//------------------------------------------------------------------------------
struct FBuildStats;

// Report
//------------------------------------------------------------------------------
class Report
{
public:
    virtual ~Report() {}

    virtual void Generate(const FBuildStats& stats) = 0;
    virtual void Save() const = 0;

};