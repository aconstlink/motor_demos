#include "audio_analysis.h"

using namespace demos ;

//*******************************************************************
void_t audio_analysis::analysis::write_kick( float_t const v ) noexcept
{
    kick_history[ ++kick_history_idx % kick_history.size() ] = v ;
    //if ( kick_history_idx >= kick_history.size() ) kick_history_idx = 0 ;
}

//*******************************************************************
void_t audio_analysis::analysis::write_midl( float_t const v ) noexcept
{
    midl_history[ ++midl_history_idx % midl_history.size() ] = v ;
    //if ( midl_history_idx >= midl_history.size() ) midl_history_idx = 0 ;
}

//*******************************************************************
void_t audio_analysis::analysis::recompute_average( void_t ) noexcept
{
    // kick
    {
        float_t accum = kick_history[ 0 ] ;
        for ( size_t i = 1; i < kick_history.size(); ++i )
        {
            accum += kick_history[ i ] ;
        }
        accum /= float_t( kick_history.size() ) ;
        kick_average = accum ;
    }

    // midl
    {
        float_t accum = midl_history[ 0 ] ;
        for ( size_t i = 1; i < midl_history.size(); ++i )
        {
            accum += midl_history[ i ] ;
        }
        accum /= float_t( midl_history.size() ) ;
        midl_average = accum ;
    }
}

//*******************************************************************
float_t audio_analysis::analysis::compute_kick_variance( void_t ) const noexcept
{
    float_t accum = 0.0f ;
    for ( size_t i = 0; i < kick_history.size(); ++i )
    {
        float_t const dif = kick_history[ i ] - kick_average ;
        float_t const sq = dif * dif ;
        accum += sq ;
    }
    return accum / float_t( kick_history.size() ) ;
}

//*******************************************************************
float_t audio_analysis::analysis::compute_midl_variance( void_t ) const noexcept
{
    float_t accum = 0.0f ;
    for ( size_t i = 0; i < midl_history.size(); ++i )
    {
        float_t const dif = midl_history[ i ] - midl_average ;
        float_t const sq = dif * dif ;
        accum += sq ;
    }
    return accum / float_t( midl_history.size() ) ;
}