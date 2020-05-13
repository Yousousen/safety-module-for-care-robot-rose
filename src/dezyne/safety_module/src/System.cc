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
: dzn_meta{"","System",0,0,{& iLEDControl.meta,& iAccelerationSensor.meta,& iAngularAccelerationSensor.meta,& iGripArmPositionSensor.meta,& iGripArmStrengthSensor.meta},{& controller.dzn_meta,& accelerationCheck.dzn_meta,& angularAccelerationCheck.dzn_meta,& gripArmPositionCheck.dzn_meta,& gripArmStrengthCheck.dzn_meta,& baseCaseCheck.dzn_meta},{[this]{iController.check_bindings();},[this]{iLEDControl.check_bindings();},[this]{iAccelerationSensor.check_bindings();},[this]{iAngularAccelerationSensor.check_bindings();},[this]{iGripArmPositionSensor.check_bindings();},[this]{iGripArmStrengthSensor.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)


, controller(dzn_locator)
, accelerationCheck(dzn_locator)
, angularAccelerationCheck(dzn_locator)
, gripArmPositionCheck(dzn_locator)
, gripArmStrengthCheck(dzn_locator)
, baseCaseCheck(dzn_locator)

, iController(controller.iController)
, iLEDControl(controller.iLEDControl), iAccelerationSensor(accelerationCheck.iAccelerationSensor), iAngularAccelerationSensor(angularAccelerationCheck.iAngularAccelerationSensor), iGripArmPositionSensor(gripArmPositionCheck.iGripArmPositionSensor), iGripArmStrengthSensor(gripArmStrengthCheck.iGripArmStrengthSensor)
{


  controller.dzn_meta.parent = &dzn_meta;
  controller.dzn_meta.name = "controller";
  accelerationCheck.dzn_meta.parent = &dzn_meta;
  accelerationCheck.dzn_meta.name = "accelerationCheck";
  angularAccelerationCheck.dzn_meta.parent = &dzn_meta;
  angularAccelerationCheck.dzn_meta.name = "angularAccelerationCheck";
  gripArmPositionCheck.dzn_meta.parent = &dzn_meta;
  gripArmPositionCheck.dzn_meta.name = "gripArmPositionCheck";
  gripArmStrengthCheck.dzn_meta.parent = &dzn_meta;
  gripArmStrengthCheck.dzn_meta.name = "gripArmStrengthCheck";
  baseCaseCheck.dzn_meta.parent = &dzn_meta;
  baseCaseCheck.dzn_meta.name = "baseCaseCheck";


  connect(accelerationCheck.iAccelerationCheck, controller.iNext);
  connect(angularAccelerationCheck.iAngularAccelerationCheck, accelerationCheck.iNext);
  connect(gripArmPositionCheck.iGripArmPositionCheck, angularAccelerationCheck.iNext);
  connect(gripArmStrengthCheck.iGripArmStrengthCheck, gripArmPositionCheck.iNext);
  connect(baseCaseCheck.iRoot, gripArmStrengthCheck.iNext);

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