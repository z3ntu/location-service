/*
 * Copyright © 2012-2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */
#ifndef LOCATION_SERVICE_COM_UBUNTU_LOCATION_CONNECTIVITY_WIRELESS_NETWORK_H_
#define LOCATION_SERVICE_COM_UBUNTU_LOCATION_CONNECTIVITY_WIRELESS_NETWORK_H_

#include <com/ubuntu/location/connectivity/bounded_integer.h>

#include <string>

namespace com
{
namespace ubuntu
{
namespace location
{
namespace connectivity
{
struct WirelessNetwork
{
    enum class Mode
    {
        /** Mode is unknown. */
        unknown = 0,
        /** Indicates the object is part of an Ad-Hoc 802.11 network without a central coordinating access point. */
        adhoc = 1,
        /** The wireless device or access point is in infrastructure mode. */
        infrastructure = 2
    };

    enum class Domain
    {
        frequency,
        channel,
        strength
    };

    typedef BoundedInteger<2412, 5825, static_cast<int>(Domain::frequency)> Frequency;
    typedef BoundedInteger<1, 165, static_cast<int>(Domain::channel)> Channel;
    typedef BoundedInteger<0, 100, static_cast<int>(Domain::strength)> Strength;

    bool operator==(const WirelessNetwork& rhs) const
    {
        return bssid == rhs.bssid &&
               ssid == rhs.ssid &&
               frequency == rhs.frequency &&
               channel == rhs.channel &&
               strength == rhs.strength &&
               snr == rhs.snr;
    }

    friend std::ostream& operator<<(std::ostream& out, const WirelessNetwork& wifi)
    {
        return out << "("
                   << "bssid: " << wifi.bssid << ", "
                   << "ssid: " << wifi.ssid << ", "
                   << "frequency: " << wifi.frequency << ", "
                   << "channel: " << wifi.channel << ", "
                   << "strength: " << wifi.strength << ", "
                   << "snr: " << wifi.snr << ")";
    }

    std::string bssid; ///< The BSSID of the network.
    std::string ssid; ///< The SSID of the network.
    Frequency frequency; ///< Frequency of the network/AP.
    Channel channel; ///< Channel of the network/AP.
    Strength strength; ///< Signal quality of the network/AP in percent.
    float snr; ///< Signal-noise ratio of the specific network.
};
}
}
}
}

#endif // LOCATION_SERVICE_COM_UBUNTU_LOCATION_CONNECTIVITY_WIRELESS_NETWORK_H_
