/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
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
 *
 */

#include <location/cmds/provider.h>

#include <location/provider_factory.h>

#include <location/dbus/stub/service.h>
#include <location/glib/runtime.h>

#include <core/posix/signal.h>

namespace cli = location::util::cli;

namespace
{
void die_if(bool b, std::ostream& out, const std::string& message)
{
    if (b) { out << message << std::endl; std::exit(EXIT_FAILURE); }
}
}

location::cmds::Provider::Provider()
    : CommandWithFlagsAndAction{cli::Name{"provider"}, cli::Usage{"provider"}, cli::Description{"executes a built-in provider"}},
      bus{dbus::Bus::system}
{
    flag(cli::make_flag(cli::Name{"bus"}, cli::Description{"bus instance to connect to, defaults to system"}, bus));
    flag(cli::make_flag(cli::Name{"id"}, cli::Description{"id of the actual provider implementation."}, id));

    action([this](const Context& ctxt)
    {
        die_if(not id, ctxt.cout, "name of actual provider implementation is missing");

        glib::Runtime runtime;

        location::dbus::stub::Service::create(bus, [this](const location::Result<location::dbus::stub::Service::Ptr>& result)
        {
            if (!result)
            {
                glib::Runtime::instance()->stop();
                return;
            }

            service = result.value();
            service->add_provider(location::ProviderFactory::instance().create_provider_for_name_with_config(
                                      *id, location::Configuration{}));
        });

        return runtime.run();
    });
}
