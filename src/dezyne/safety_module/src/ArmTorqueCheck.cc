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
#include "ArmTorqueCheck.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



ArmTorqueCheck::ArmTorqueCheck(const dzn::locator& dzn_locator)
: dzn_meta{"","ArmTorqueCheck",0,0,{& iArmTorqueSensor.meta,& iNext.meta,& iResolver.meta},{},{[this]{iArmTorqueSensor.check_bindings();},[this]{iArmTorqueCheck.check_bindings();},[this]{iNext.check_bindings();},[this]{iResolver.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)

, iResolver(dzn_locator.get< IResolver>())

, iArmTorqueCheck({{"iArmTorqueCheck",this,&dzn_meta},{"",0,0}})

, iArmTorqueSensor({{"",0,0},{"iArmTorqueSensor",this,&dzn_meta}})
, iNext({{"",0,0},{"iNext",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;



  iArmTorqueCheck.in.do_check = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iArmTorqueCheck) = false; return iArmTorqueCheck_do_check();}, this->iArmTorqueCheck.meta, "do_check");};



}

::Behavior::type ArmTorqueCheck::iArmTorqueCheck_do_check()
{

  {
    ::Behavior::type currSafetyState = ::Behavior::Safe;
    this->iArmTorqueSensor.in.retrieve_arm_torque();
    currSafetyState = this->iResolver.in.resolve_arm_torque();
    ::Behavior::type nextSafetyState = this->iNext.in.do_check();
    ::Behavior::type res = and_safety_states(currSafetyState,nextSafetyState);
    { this->reply_Behavior = res; }
  }

  return this->reply_Behavior;
}

::Behavior::type ArmTorqueCheck::and_safety_states (::Behavior::type current,::Behavior::type next) 
{
  {
    if ((current == ::Behavior::Unsafe || next == ::Behavior::Unsafe)) 
    {
      return ::Behavior::Unsafe;
    }
  }
  return ::Behavior::Safe;
}

void ArmTorqueCheck::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void ArmTorqueCheck::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1