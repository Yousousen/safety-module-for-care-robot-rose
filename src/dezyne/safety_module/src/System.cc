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
: dzn_meta{"","System",0,0,{& iLEDControl.meta,& iAccelerationSensor.meta,& iAngularVelocitySensor.meta},{& controller.dzn_meta,& kineticEnergyCheck.dzn_meta,& rotationalEnergyCheck.dzn_meta,& baseCaseCheck.dzn_meta},{[this]{iController.check_bindings();},[this]{iLEDControl.check_bindings();},[this]{iAccelerationSensor.check_bindings();},[this]{iAngularVelocitySensor.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)


, controller(dzn_locator)
, kineticEnergyCheck(dzn_locator)
, rotationalEnergyCheck(dzn_locator)
, baseCaseCheck(dzn_locator)

, iController(controller.iController)
, iLEDControl(controller.iLEDControl), iAccelerationSensor(kineticEnergyCheck.iAccelerationSensor), iAngularVelocitySensor(rotationalEnergyCheck.iAngularVelocitySensor)
{


  controller.dzn_meta.parent = &dzn_meta;
  controller.dzn_meta.name = "controller";
  kineticEnergyCheck.dzn_meta.parent = &dzn_meta;
  kineticEnergyCheck.dzn_meta.name = "kineticEnergyCheck";
  rotationalEnergyCheck.dzn_meta.parent = &dzn_meta;
  rotationalEnergyCheck.dzn_meta.name = "rotationalEnergyCheck";
  baseCaseCheck.dzn_meta.parent = &dzn_meta;
  baseCaseCheck.dzn_meta.name = "baseCaseCheck";


  connect(kineticEnergyCheck.iKineticEnergyCheck, controller.iNext);
  connect(rotationalEnergyCheck.iRotationalEnergyCheck, kineticEnergyCheck.iNext);
  connect(baseCaseCheck.iRoot, rotationalEnergyCheck.iNext);

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