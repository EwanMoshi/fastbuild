// CompilerDriver_CL.h 
//------------------------------------------------------------------------------
#pragma once

// Includes
//------------------------------------------------------------------------------
#include "CompilerDriverBase.h"

// Forward Declarations
//------------------------------------------------------------------------------

// CLDriver
//------------------------------------------------------------------------------
class CompilerDriver_CL : public CompilerDriverBase
{
public:
    explicit CompilerDriver_CL( bool isClangCL );
    virtual ~CompilerDriver_CL() override;

protected:
    // Manipulate args if needed for various compilation modes
    virtual bool ProcessArg_PreprocessorOnly( const AString & token, size_t & index, const AString & nextToken, Args & outFullArgs ) override;
    virtual bool ProcessArg_CompilePreprocessed( const AString & token, size_t & index, const AString & nextToken, Args & outFullArgs ) override;
    virtual bool ProcessArg_Common( const AString & token, size_t & index, Args & outFullArgs ) override;

    // Add additional args
    virtual void AddAdditionalArgs_Preprocessor( Args & outFullArgs ) override;
    virtual void AddAdditionalArgs_Common( Args & outFullArgs ) override;

    static bool IsCompilerArg_MSVC( const AString & token, const char * arg );
    static bool IsStartOfCompilerArg_MSVC( const AString & token, const char * arg );
    static bool StripTokenWithArg_MSVC( const char * tokenToCheckFor, const AString & token, size_t & index );
    static bool StripToken_MSVC( const char * tokenToCheckFor, const AString & token, bool allowStartsWith = false );

    bool m_IsClangCL = false; // Using clang in CL compatibility mode?
};

//------------------------------------------------------------------------------