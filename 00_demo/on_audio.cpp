
#include "main.h"

using namespace demos ;

#define USE_HISTORY_BUFFER 1

void_t the_app::on_audio( motor::audio::frontend_ptr_t fptr, audio_data_in_t ad ) noexcept
{
    if ( ad.first_frame )
    {
        fptr->configure( motor::audio::capture_type::what_u_hear, &_co ) ;
    }

    fptr->capture( &_co ) ;
    _co.copy_samples_to( _aanl.captured_samples ) ;
    _co.copy_frequencies_to( _aanl.captured_frequencies ) ;

    if ( _aanl.captured_frequencies_avg.size() != _aanl.captured_frequencies.size() )
    {
        {
            _aanl.captured_frequencies_avg.resize( _aanl.captured_frequencies.size() )  ;
            for ( size_t i = 0; i < _aanl.captured_frequencies.size(); ++i )
                _aanl.captured_frequencies_avg[ i ] = 0.0f ;
        }
        {
            _aanl.captured_frequencies_var.resize( _aanl.captured_frequencies.size() )  ;
            for ( size_t i = 0; i < _aanl.captured_frequencies.size(); ++i )
                _aanl.captured_frequencies_var[ i ] = 0.0f ;
        }
    }

    for ( size_t i = 0; i < _aanl.captured_frequencies.size(); ++i )
    {
        {
            _aanl.captured_frequencies_avg[ i ] += _aanl.captured_frequencies[ i ] ;
            _aanl.captured_frequencies_avg[ i ] /= 2.0f ;
        }

        {
            float_t const var = _aanl.captured_frequencies[ i ] - _aanl.captured_frequencies_avg[ i ] ;
            _aanl.captured_frequencies_var[ i ] += var * var ;
            _aanl.captured_frequencies_var[ i ] /= 2.0f ;
        }
    }

    // [0,1] : kick frequencies
    // [2,3] : mid low frequencies
    motor::vector< size_t > const bands =
    {
        size_t( _aanl.asys.kick_start ),
        size_t( _aanl.asys.kick_start + _aanl.asys.kick_band ),
        300, 750
    } ;
    size_t const num_bands = bands.size() >> 1 ;
    size_t const band_width = _co.get_band_width() ;

    if ( band_width == 0 ) return ;

    // kick average
    {
        size_t const kick_idx_0 = bands[ 0 ] / band_width ;
        size_t const kick_idx_1 = bands[ 1 ] / band_width ;
        size_t const kick_idx_range = ( kick_idx_1 - kick_idx_0 ) + 1 ;

        float_t accum_frequencies = 0.0f ;
        for ( size_t i = kick_idx_0; i <= kick_idx_1; ++i )
        {
            accum_frequencies += _aanl.captured_frequencies[ i ] ;
        }
        accum_frequencies /= float_t( kick_idx_range ) ;

        #if USE_HISTORY_BUFFER
        _aanl.asys.kick_history_single = accum_frequencies ;
        _aanl.asys.write_kick( accum_frequencies ) ;
        #else
        _aanl.asys.kick_history_single = accum_frequencies ;
        _aanl.asys.kick_average += accum_frequencies ;
        _aanl.asys.kick_average /= 2.0f ;
        #endif
    }

    // mid low average
    {
        size_t const midl_idx_0 = bands[ 2 ] / band_width ;
        size_t const midl_idx_1 = bands[ 3 ] / band_width ;
        size_t const midl_idx_range = ( midl_idx_1 - midl_idx_0 ) + 1 ;

        float_t accum_frequencies = 0.0f ;
        for ( size_t i = midl_idx_0; i <= midl_idx_1; ++i )
        {
            accum_frequencies += _aanl.captured_frequencies[ i ] ;
        }
        accum_frequencies /= float_t( midl_idx_range ) ;

        #if USE_HISTORY_BUFFER
        _aanl.asys.midl_history_single = accum_frequencies ;
        _aanl.asys.write_midl( accum_frequencies ) ;
        #else
        _aanl.asys.midl_history_single = accum_frequencies ;
        _aanl.asys.midl_average += accum_frequencies ;
        _aanl.asys.midl_average /= 2.0f ;
        #endif
    }

    {
        #if USE_HISTORY_BUFFER
        _aanl.asys.recompute_average() ;
        #endif
    }

    // do threshold
    {
        #if USE_HISTORY_BUFFER
        float_t const kick_var = _aanl.asys.compute_kick_variance() ;
        float_t const midl_var = _aanl.asys.compute_midl_variance() ;
        #else
        float_t const kick_dif = _aanl.asys.kick_history_single - _aanl.asys.kick_average ;
        float_t const midl_dif = _aanl.asys.midl_history_single - _aanl.asys.midl_average ;

        float_t const kick_var = kick_dif * kick_dif ;
        float_t const midl_var = midl_dif * midl_dif ;
        #endif
        auto beat_threshold = [&] ( float_t const v )
        {
            return -15.0f * v + 1.55f ;
        } ;

        _aanl.asys.is_kick = ( _aanl.asys.kick_history_single - 0.05f ) > beat_threshold( kick_var ) * _aanl.asys.kick_average ;
        _aanl.asys.is_lowm = ( _aanl.asys.midl_history_single - 0.005f ) > beat_threshold( midl_var ) * _aanl.asys.midl_average ;
    }

    // do value degradation
    {
        if ( _aanl.asys.is_kick ) _aanl.asys.kick = 1.0f ;
        if ( _aanl.asys.is_lowm ) _aanl.asys.midl = 1.0f ;

        float_t const deg = ad.sec_dt * 0.5f ;

        _aanl.asys.kick -= deg ;
        _aanl.asys.midl -= deg ;

        _aanl.asys.kick = std::max( _aanl.asys.kick, 0.0f ) ;
        _aanl.asys.midl = std::max( _aanl.asys.midl, 0.0f ) ;
    }
}