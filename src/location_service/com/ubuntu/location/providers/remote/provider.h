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
 * Authored by: Manuel de la Peña <manuel.delapena@canonical.com>
 */
#ifndef LOCATION_SERVICE_COM_UBUNTU_LOCATION_PROVIDERS_ESPOO_PROVIDER_H_
#define LOCATION_SERVICE_COM_UBUNTU_LOCATION_PROVIDERS_ESPOO_PROVIDER_H_

#include <com/ubuntu/location/provider.h>
#include <com/ubuntu/location/provider_factory.h>
#include <com/ubuntu/location/providers/remote/remote_interface.h>

#include <core/dbus/bus.h>

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

class Provider : public com::ubuntu::location::Provider
{
  public:
    // For integration with the Provider factory.
    static std::string class_name();

    // Instantiates a new provider instance, populating the configuration object
    // from the provided property bundle.
    static Provider::Ptr create_instance(const ProviderFactory::Configuration&);

    // structure that represents the configuration used in the remote provider
    struct Configuration
    {
        static std::string key_name() { return "name"; }
        static std::string key_path() { return "path"; }

        std::string name;
        std::string path;

        core::dbus::Bus::Ptr connection;

        Provider::Features features = Provider::Features::position;
        Provider::Requirements requirements = Provider::Requirements::cell_network |
            Provider::Requirements::data_network | Provider::Requirements::monetary_spending;
    };

    Provider(const Configuration& config);
    ~Provider() noexcept;

    virtual bool matches_criteria(const Criteria&);

    virtual void start_position_updates() override;
    virtual void stop_position_updates() override;

    virtual void start_heading_updates() override;
    virtual void stop_heading_updates() override;

    virtual void start_velocity_updates() override;
    virtual void stop_velocity_updates() override;

  private:
    struct Private;
    std::unique_ptr<Private> d;
};

}  // remote
}  // providers
}  // location
}  // ubuntu
}  // com
#endif // LOCATION_SERVICE_COM_UBUNTU_LOCATION_PROVIDERS_GEOCLUE_PROVIDER_H_