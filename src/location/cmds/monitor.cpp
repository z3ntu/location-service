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

#include <location/cmds/monitor.h>

#include <location/criteria.h>
#include <location/logging.h>
#include <location/dbus/stub/service.h>
#include <location/glib/runtime.h>
#include <location/runtime.h>

#include <type_traits>

namespace cli = location::util::cli;

location::cmds::Monitor::PrintingDelegate::PrintingDelegate(std::ostream& out) : out{out}
{
}

void location::cmds::Monitor::PrintingDelegate::on_new_position(const Update<Position>& pos)
{
    last_position_update = pos;
    print_row();
}

void location::cmds::Monitor::PrintingDelegate::on_new_heading(const Update<units::Degrees>& heading)
{
    last_heading_update = heading;
    print_row();
}

void location::cmds::Monitor::PrintingDelegate::on_new_velocity(const Update<units::MetersPerSecond>& velocity)
{
    last_velocity_update = velocity;
    print_row();
}

void location::cmds::Monitor::PrintingDelegate::print_header()
{
    out << std::left << std::setw(15) << std::setfill(' ') << "lat.[deg]"
        << std::left << std::setw(15) << std::setfill(' ') << "lon.[deg]"
        << std::left << std::setw(15) << std::setfill(' ') << "hor.acc.[m]"
        << std::left << std::setw(15) << std::setfill(' ') << "alt.[m]"
        << std::left << std::setw(15) << std::setfill(' ') << "ver.acc.[m]"
        << std::left << std::setw(15) << std::setfill(' ') << "heading[deg]"
        << std::left << std::setw(15) << std::setfill(' ') << "vel.[m/s]" << std::endl;
}

void location::cmds::Monitor::PrintingDelegate::print_row()
{
    if (last_position_update)
    {
        out << std::left << std::setw(15) << std::setfill(' ') << std::fixed << std::setprecision(5) << last_position_update->value.latitude().value();
        out << std::left << std::setw(15) << std::setfill(' ') << std::fixed << std::setprecision(5) << last_position_update->value.longitude().value();

        if (last_position_update->value.accuracy().horizontal())
            out << std::left << std::setw(15) << std::setfill(' ') << std::fixed << std::setprecision(2) << last_position_update->value.accuracy().horizontal()->value() ;
        else
            out << std::left << std::setw(15) << std::setfill(' ') << "n/a" ;

        if (last_position_update->value.altitude())
            out << std::left << std::setw(15) << std::setfill(' ') << std::fixed << std::setprecision(2) << last_position_update->value.altitude()->value() ;
        else
            out << std::left << std::setw(15) << std::setfill(' ') << "n/a" ;

        if (last_position_update->value.accuracy().vertical())
            out << std::left << std::setw(15) << std::setfill(' ') << std::fixed << std::setprecision(2) << last_position_update->value.accuracy().vertical()->value() ;
        else
            out << std::left << std::setw(15) << std::setfill(' ') << "n/a" ;
    }
    else
    {
        out << std::left << std::setw(15) << std::setfill(' ') << "n/a"   // latitude
            << std::left << std::setw(15) << std::setfill(' ') << "n/a"   // longitude
            << std::left << std::setw(15) << std::setfill(' ') << "n/a"   // horizontal accuracy
            << std::left << std::setw(15) << std::setfill(' ') << "n/a"   // altitude
            << std::left << std::setw(15) << std::setfill(' ') << "n/a" ; // vertical accuracy
    }

    if (last_heading_update)
    {
        out << std::left << std::setw(15) << std::setfill(' ') << last_heading_update->value.value() ;
    }
    else
    {
        out << std::left << std::setw(15) << std::setfill(' ') << "n/a" ;
    }

    if (last_velocity_update)
    {
        out << std::left << std::setw(15) << std::setfill(' ') << last_velocity_update->value.value();
    }
    else
    {
        out << std::left << std::setw(15) << std::setfill(' ') << "n/a";
    }

    out << std::endl;

}

location::cmds::Monitor::Monitor(const std::shared_ptr<Delegate>& delegate)
    : CommandWithFlagsAndAction{cli::Name{"monitor"}, cli::Usage{"monitor"}, cli::Description{"monitors the daemon"}},
      delegate{delegate},
      bus{dbus::Bus::system}
{
    flag(cli::make_flag(cli::Name{"bus"}, cli::Description{"bus instance to connect to, defaults to system"}, bus));
    action([this](const Context& ctxt)
    {
        glib::Runtime runtime{glib::Runtime::WithOwnMainLoop{}};
        runtime.redirect_logging();

        location::dbus::stub::Service::create(bus, [this, &ctxt](const Result<location::dbus::stub::Service::Ptr>& result) mutable
        {
            if (!result)
            {
                glib::Runtime::instance()->stop();
                return;
            }

            auto service = result.value();
            service->create_session_for_criteria(location::Criteria{}, [this, &ctxt, service](const Result<Service::Session::Ptr>& result)
            {
                if (!result)
                {
                    glib::Runtime::instance()->stop();
                    return;
                }

                auto session = result.value();

                session->updates().position.changed().connect([this, session](const location::Update<location::Position>& pos)
                {
                    Monitor::delegate->on_new_position(pos);
                });

                session->updates().heading.changed().connect([this, session](const location::Update<location::units::Degrees>& heading)
                {
                    Monitor::delegate->on_new_heading(heading);
                });

                session->updates().velocity.changed().connect([this, session](const location::Update<location::units::MetersPerSecond>& velocity)
                {
                    Monitor::delegate->on_new_velocity(velocity);
                });

                session->updates().position_status = location::Service::Session::Updates::Status::enabled;
                session->updates().heading_status = location::Service::Session::Updates::Status::enabled;
                session->updates().velocity_status = location::Service::Session::Updates::Status::enabled;

                LOG(INFO) << "Enabled position/heading/velocity updates..." << std::endl;
            });
        });

        return runtime.run();
    });
}
