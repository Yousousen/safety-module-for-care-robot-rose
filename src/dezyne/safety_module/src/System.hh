// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include <dzn/meta.hh>

namespace dzn {
  struct locator;
  struct runtime;
}



#include <iostream>
#include <map>

#ifndef ENUM_Behavior
#define ENUM_Behavior 1


struct Behavior
{
  enum type
  {
    Unsafe,Safe
  };
};


#endif // ENUM_Behavior
#ifndef ENUM_UnsafeTriggered
#define ENUM_UnsafeTriggered 1


struct UnsafeTriggered
{
  enum type
  {
    No,Yes
  };
};


#endif // ENUM_UnsafeTriggered

/***********************************  SHELL  ************************************/
#ifndef SYSTEM_HH
#define SYSTEM_HH


#include <dzn/locator.hh>
#include <dzn/runtime.hh>
#include <dzn/pump.hh>

#include "Controller.hh"
#include "AccelerationControl.hh"
#include "AngularAccelerationControl.hh"
#include "GripArmControl.hh"



struct System
{
  dzn::meta dzn_meta;
  dzn::runtime dzn_rt;
  dzn::locator dzn_locator;


  ::Controller controller;
  ::AccelerationControl accelerationControl;
  ::AngularAccelerationControl angularAccelerationControl;
  ::GripArmControl gripArmControl;

  ::IController iController;

  ::ILEDControl iLEDControl;
  ::IAccelerationSensor iAccelerationSensor;
  ::IAngularAccelerationSensor iAngularAccelerationSensor;
  ::IGripArmSensor iGripArmSensor;

  dzn::pump dzn_pump;
  System(const dzn::locator&);
  void check_bindings() const;
  void dump_tree(std::ostream& os=std::clog) const;
};

#endif // SYSTEM_HH

/***********************************  SHELL  ************************************/


//version: 2.9.1
