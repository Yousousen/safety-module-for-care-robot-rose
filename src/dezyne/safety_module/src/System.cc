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



System::System(const dzn::locator& locator)
: dzn_meta{"","System",0,0,{& iLEDControl.meta,& iAccelerationSensor.meta,& iAngularAccelerationSensor.meta,& iGripArmSensor.meta},{& controller.dzn_meta,& accelerationControl.dzn_meta,& angularAccelerationControl.dzn_meta,& gripArmControl.dzn_meta},{[this]{iController.check_bindings();},[this]{iLEDControl.check_bindings();},[this]{iAccelerationSensor.check_bindings();},[this]{iAngularAccelerationSensor.check_bindings();},[this]{iGripArmSensor.check_bindings();}}}
, dzn_locator(locator.clone().set(dzn_rt).set(dzn_pump))


, controller(dzn_locator)
, accelerationControl(dzn_locator)
, angularAccelerationControl(dzn_locator)
, gripArmControl(dzn_locator)

, iController(controller.iController)
, iLEDControl(controller.iLEDControl), iAccelerationSensor(accelerationControl.iAccelerationSensor), iAngularAccelerationSensor(angularAccelerationControl.iAngularAccelerationSensor), iGripArmSensor(gripArmControl.iGripArmSensor)
, dzn_pump()
{
  controller.iController.meta.requires.port = "iController";

  controller.iLEDControl.meta.provides.port = "iLEDControl";
  accelerationControl.iAccelerationSensor.meta.provides.port = "iAccelerationSensor";
  angularAccelerationControl.iAngularAccelerationSensor.meta.provides.port = "iAngularAccelerationSensor";
  gripArmControl.iGripArmSensor.meta.provides.port = "iGripArmSensor";


  iController.in.initialise = [&] () {
    return dzn::shell(dzn_pump, [ & ] {return controller.iController.in.initialise();});
  };
  iController.in.destruct = [&] () {
    return dzn::shell(dzn_pump, [ & ] {return controller.iController.in.destruct();});
  };
  iController.in.reset = [&] () {
    return dzn::shell(dzn_pump, [ & ] {return controller.iController.in.reset();});
  };
  iController.in.do_checks = [&] () {
    return dzn::shell(dzn_pump, [ & ] {return controller.iController.in.do_checks();});
  };



  controller.iLEDControl.in.initialise_framebuffer = std::ref(iLEDControl.in.initialise_framebuffer);
  controller.iLEDControl.in.destruct_framebuffer = std::ref(iLEDControl.in.destruct_framebuffer);
  controller.iLEDControl.in.light_led_red = std::ref(iLEDControl.in.light_led_red);
  controller.iLEDControl.in.light_led_blue = std::ref(iLEDControl.in.light_led_blue);
  controller.iLEDControl.in.reset_led = std::ref(iLEDControl.in.reset_led);
  accelerationControl.iAccelerationSensor.in.retrieve_ke_from_acc = std::ref(iAccelerationSensor.in.retrieve_ke_from_acc);
  angularAccelerationControl.iAngularAccelerationSensor.in.retrieve_re_from_ang_acc = std::ref(iAngularAccelerationSensor.in.retrieve_re_from_ang_acc);
  gripArmControl.iGripArmSensor.in.retrieve_arm_str = std::ref(iGripArmSensor.in.retrieve_arm_str);


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

}

void System::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void System::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}



//version: 2.9.1