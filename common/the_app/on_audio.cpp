
#include "the_app.h"

#include <motor/audio/util/frequency_bands.hpp>

using namespace demos;

//******************************************************************************************************
void_t the_app::on_audio( motor::audio::frontend_ptr_t fptr, audio_data_in_t ad ) noexcept
{
    if( ad.first_frame )
    {
        fptr->configure( motor::audio::capture_type::what_u_hear, &_co );
    }

    fptr->capture( &_co );
    _co.copy_samples_to( _aanl.captured_samples );
    _co.copy_frequencies_to( _aanl.captured_frequencies );

    float_t const bin_width = _co.get_bin_width_hz< float_t >();

    if( _aanl.captured_frequencies.empty() ) return;

    motor::audio::frequency_bands fb( bin_width, _aanl.captured_frequencies.size() );

    // low / kix ( 40 ; 250 Hz )
    {
        float_t sum = 0.0f;
        fb.for_each( 40.0f, 250.0f,
            [ & ]( size_t const, size_t const g ) { sum += _aanl.captured_frequencies[ g ]; } );        

        if( _kix_detector.update( sum, ad.sec_dt ) )
        {
            // detected
        }
    }
}