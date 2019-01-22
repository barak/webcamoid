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

#ifndef AKVCAMUTILS_RCNAME_H
#define AKVCAMUTILS_RCNAME_H

#include <string>
#include <cstdint>

namespace AkVCam
{
    struct RcName
    {
        uint16_t m_size;
        uint32_t m_hash;
        uint16_t *m_data;

        static std::string read(const unsigned char *rcName);
    };
}

#endif // AKVCAMUTILS_RCNAME_H
