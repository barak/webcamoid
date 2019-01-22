/* Webcamoid, webcam capture application.
 * Copyright (C) 2017  Gonzalo Exequiel Pedone
 *
 * Webcamoid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Webcamoid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: http://webcamoid.github.io/
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <CoreMediaIO/CMIOHardwarePlugIn.h>
#include <CoreMedia/CMFormatDescription.h>

#include "VCamUtils/src/image/videoformattypes.h"

namespace AkVCam
{
    bool uuidEqual(const REFIID &uuid1, const CFUUIDRef uuid2);
    std::string enumToString(UInt32 value);
    FourCharCode formatToCM(PixelFormat format);
    PixelFormat formatFromCM(FourCharCode format);
}

#endif // UTILS_H
