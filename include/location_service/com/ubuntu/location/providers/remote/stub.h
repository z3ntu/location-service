/*
 * Copyright © 2014 Canonical Ltd.
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
#ifndef LOCATION_SERVICE_COM_UBUNTU_LOCATION_SERVICE_PROVIDERS_REMOTE_STUB_H_
#define LOCATION_SERVICE_COM_UBUNTU_LOCATION_SERVICE_PROVIDERS_REMOTE_STUB_H_

#include <com/ubuntu/location/provider.h>

namespace core { namespace dbus {
class Bus;
class Object;
}}

namespace com
{
namespace ubuntu
{
namespace location
{
namespace providers
{
namespace remote
{
namespace stub
{
/** @brief All creation time arguments go here. */
struct Configuration
{
    /** @brief Remote object implementing remote::Interface. */
    std::shared_ptr<core::dbus::Object> object;
};

/** @brief Create a stub instance referring to a remote provider instance. */
Provider::Ptr create_with_configuration(const Configuration& configuration);
}
}
}
}
}
}
#endif // LOCATION_SERVICE_COM_UBUNTU_LOCATION_SERVICE_PROVIDERS_REMOTE_STUB_H_
