/***************************************************************************
                          enginespectralback.cpp  -  description
                             -------------------
    begin                : Sun Aug 5 2001
    copyright            : (C) 2001 by Tue Haste Andersen
    email                : haste@diku.dk
 ***************************************************************************/


#include "enginespectralback.h"

/* -------- -----------------------------------------------------------------
   Purpose: Initializes EngineSpectralBack object, for performing inverse fft's
   using the FFTW library.
   Input:   size  - size of FFT to perform
   Output:  -
   -------- ----------------------------------------------------------------- */
EngineSpectralBack::EngineSpectralBack(int Length, CSAMPLE *_window)
{
    l  = Length;
    l2 = Length*2;
    l_half = Length/2;

    window = _window;

    // Create plans for use in FFT calculations.
    plan_backward  = rfftw_create_plan(l,FFTW_COMPLEX_TO_REAL,FFTW_ESTIMATE);

    // Allocate temporary buffer. Double size arrays of the input size is used,
    // because of the FFT's
    tmp = new fftw_real[l];
    samples = new fftw_real[l];
}

/* -------- -----------------------------------------------------------------
   Purpose: Destroys ESpectral object.
   Input:   -
   Output:  -
   -------- ----------------------------------------------------------------- */
EngineSpectralBack::~EngineSpectralBack()
{
    // Destroy fft plans
    rfftw_destroy_plan(plan_backward);

    // Deallocate temporary buffer
    delete [] tmp;
    delete [] samples;
}

/* -------- -----------------------------------------------------------------
   Purpose: Performs inverse fft on data pointed to by p.
   Input:   p is expected to hold a pointer to an array of size l2. the first
   part of length l, is supposed to hold valid data. The first
            half contains the power, the second the phase (see FFTW
            documentation for exact bounderies). These values are transformed
   into real and imaginary parts, before the IFFT is calculated.
   Output:  Pointer to an array of CSAMPLES of length l.
   -------- ----------------------------------------------------------------- */
CSAMPLE *EngineSpectralBack::process(CSAMPLE *p, const int)
{
    CSAMPLE *p2 = (CSAMPLE *)p;

    // Subtract linear phase incriment caused by window
    // for (int i=l_half; i<l; i++)
    //  p2[i] -= (i-l_half)*pi;

    // Convert from (mag,phase) to (real,imag).
    tmp[0]      = sqrt(p2[0]);
    tmp[l_half] = sqrt(p2[l_half]);

    for (int i=1; i<l_half; i++)
    {
        tmp[i]    = cos(p2[l-i])*p2[i];
        tmp[l-i]  = sin(p2[l-i])*p2[i];
    }

    // No conversion
    //for (int i=0; i<l; i++)
    // tmp[i]=p2[i];

    // Perform FFT
    rfftw_one(plan_backward,tmp,samples);

    // Descale the data by window length and divide by window
    for (i=0; i<l; i++)
        samples[i] /= (l*window[i]);

    return (CSAMPLE *) samples;
}
