/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <modm/board.hpp>
#include <modm/platform.hpp>

using namespace Board;
using namespace modm::platform;

/*
 * Blinks the green user LED with 1 Hz.
 * It is on for 90% of the time and off for 10% of the time.
 */
int main()
{
#if 1
    Board::initialize();
#else
    Rcc::enableInternalClock();

    // switch system clock to PLL output
    Rcc::enableSystemClock( Rcc::SystemClockSource::InternalClock );

    // AHB has max 8MHz
    Rcc::setAhbPrescaler( Rcc::AhbPrescaler::Div1 );

    // APB1 has max. 8MHz
    Rcc::setApb1Prescaler( Rcc::Apb1Prescaler::Div1 );

    // APB2 has max. 8MHz
    Rcc::setApb2Prescaler( Rcc::Apb2Prescaler::Div1 );

    // update frequencies for busy-wait delay functions
    Rcc::updateCoreFrequency<Rcc::BootFrequency>();

    extern void modm_dwt_enable( void );

    modm_dwt_enable();
#endif

    LedGreen::reset();

    while ( true )
    {
        LedGreen::set();

        modm::delay( 500ms );

        LedGreen::reset();

        modm::delay( 500ms );
    }
}
