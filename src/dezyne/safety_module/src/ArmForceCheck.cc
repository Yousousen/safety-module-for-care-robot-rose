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
#include "ArmForceCheck.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



ArmForceCheck::ArmForceCheck(const dzn::locator& dzn_locator)
: dzn_meta{"","ArmForceCheck",0,0,{& iArmForceSensor.meta,& iNext.meta,& iResolver.meta},{},{[this]{iArmForceSensor.check_bindings();},[this]{iArmForceCheck.check_bindings();},[this]{iNext.check_bindings();},[this]{iResolver.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)

, iResolver(dzn_locator.get< IResolver>())

, iArmForceCheck({{"iArmForceCheck",this,&dzn_meta},{"",0,0}})

, iArmForceSensor({{"",0,0},{"iArmForceSensor",this,&dzn_meta}})
, iNext({{"",0,0},{"iNext",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;



  iArmForceCheck.in.do_check = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iArmForceCheck) = false; return iArmForceCheck_do_check();}, this->iArmForceCheck.meta, "do_check");};



}

::Behavior::type ArmForceCheck::iArmForceCheck_do_check()
{

  {
    ::Behavior::type currSafetyState = ::Behavior::Safe;
    this->iArmForceSensor.in.retrieve_arm_force();
    currSafetyState = this->iResolver.in.resolve_arm_force();
    ::Behavior::type nextSafetyState = this->iNext.in.do_check();
    ::Behavior::type res = and_safety_states(currSafetyState,nextSafetyState);
    { this->reply_Behavior = res; }
  }

  return this->reply_Behavior;
}

::Behavior::type ArmForceCheck::and_safety_states (::Behavior::type current,::Behavior::type next) 
{
  {
    if ((current == ::Behavior::Unsafe || next == ::Behavior::Unsafe)) 
    {
      return ::Behavior::Unsafe;
    }
  }
  return ::Behavior::Safe;
}

void ArmForceCheck::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void ArmForceCheck::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1