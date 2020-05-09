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
#include "System.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>


//SYSTEM

System::System(const dzn::locator& dzn_locator)
: dzn_meta{"","System",0,0,{& iLEDControl.meta,& iAccelerationSensor.meta,& iAngularAccelerationSensor.meta,& iGripArmSensor.meta},{& controller.dzn_meta,& accelerationControl.dzn_meta,& angularAccelerationControl.dzn_meta,& gripArmControl.dzn_meta},{[this]{iController.check_bindings();},[this]{iLEDControl.check_bindings();},[this]{iAccelerationSensor.check_bindings();},[this]{iAngularAccelerationSensor.check_bindings();},[this]{iGripArmSensor.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)


, controller(dzn_locator)
, accelerationControl(dzn_locator)
, angularAccelerationControl(dzn_locator)
, gripArmControl(dzn_locator)

, iController(controller.iController)
, iLEDControl(controller.iLEDControl), iAccelerationSensor(accelerationControl.iAccelerationSensor), iAngularAccelerationSensor(angularAccelerationControl.iAngularAccelerationSensor), iGripArmSensor(gripArmControl.iGripArmSensor)
{


  controller.dzn_meta.parent = &dzn_meta;
  controller.dzn_meta.name = "controller";
  accelerationControl.dzn_meta.parent = &dzn_meta;
  accelerationControl.dzn_meta.name = "accelerationControl";
  angularAccelerationControl.dzn_meta.parent = &dzn_meta;
  angularAccelerationControl.dzn_meta.name = "angularAccelerationControl";
  gripArmControl.dzn_meta.parent = &dzn_meta;
  gripArmControl.dzn_meta.name = "gripArmControl";


  connect(accelerationControl.iAccelerationControl, controller.iAccelerationControl);
  connect(angularAccelerationControl.iAngularAccelerationControl, controller.iAngularAccelerationControl);
  connect(gripArmControl.iGripArmControl, controller.iGripArmControl);

  dzn::rank(iController.meta.provides.meta, 0);

}

void System::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void System::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}

////////////////////////////////////////////////////////////////////////////////



//version: 2.9.1