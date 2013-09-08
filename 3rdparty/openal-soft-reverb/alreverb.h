/*
 * OpenAL Reverb Example
 *
 * Copyright (c) 2012 by Chris Robinson <chris.kcat@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* This file contains an example for applying reverb to a sound. */

#include <stdio.h>
#include <assert.h>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include "AL/efx-presets.h"

//#include "common/alhelpers.h"
//#include "common/sdl_sound.h"
#include "alhelpers.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LoadEffect loads the given reverb properties into a new OpenAL effect
 * object, and returns the new effect ID. */
ALuint LoadEffect(const EFXEAXREVERBPROPERTIES *reverb);

ALuint initEffect(const EFXEAXREVERBPROPERTIES *reverb, ALuint *effect, ALuint *slot);

/* LoadBuffer loads the named audio file into an OpenAL buffer object, and
 * returns the new buffer ID. */
ALuint LoadSound(const char *filename);

void loadFunctionPointers();

#ifdef __cplusplus
}
#endif
