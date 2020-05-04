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
#include "GripArmControl.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



GripArmControl::GripArmControl(const dzn::locator& dzn_locator)
: dzn_meta{"","GripArmControl",0,0,{& iGripArmSensor.meta,& iResolver.meta},{},{[this]{iGripArmControl.check_bindings();},[this]{iGripArmSensor.check_bindings();},[this]{iResolver.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)

, iResolver(dzn_locator.get< IResolver>())

, iGripArmControl({{"iGripArmControl",this,&dzn_meta},{"",0,0}})

, iGripArmSensor({{"",0,0},{"iGripArmSensor",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;



  iGripArmControl.in.check_arm_position = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iGripArmControl) = false; return iGripArmControl_check_arm_position();}, this->iGripArmControl.meta, "check_arm_position");};
  iGripArmControl.in.check_arm_strength = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iGripArmControl) = false; return iGripArmControl_check_arm_strength();}, this->iGripArmControl.meta, "check_arm_strength");};



}

::Behavior::type GripArmControl::iGripArmControl_check_arm_position()
{

  {
    ::Behavior::type safetyState = ::Behavior::Safe;
    this->iGripArmSensor.in.retrieve_arm_pos();
    safetyState = this->iResolver.in.resolve_arm_pos();
    { this->reply_Behavior = safetyState; }
  }

  return this->reply_Behavior;
}
::Behavior::type GripArmControl::iGripArmControl_check_arm_strength()
{

  {
    ::Behavior::type safetyState = ::Behavior::Safe;
    this->iGripArmSensor.in.retrieve_arm_str();
    safetyState = this->iResolver.in.resolve_arm_str();
    { this->reply_Behavior = safetyState; }
  }

  return this->reply_Behavior;
}


void GripArmControl::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void GripArmControl::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1