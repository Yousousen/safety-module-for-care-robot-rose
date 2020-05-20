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
#include "RotationalEnergyCheck.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



RotationalEnergyCheck::RotationalEnergyCheck(const dzn::locator& dzn_locator)
: dzn_meta{"","RotationalEnergyCheck",0,0,{& iAngularVelocitySensor.meta,& iNext.meta,& iResolver.meta},{},{[this]{iAngularVelocitySensor.check_bindings();},[this]{iRotationalEnergyCheck.check_bindings();},[this]{iNext.check_bindings();},[this]{iResolver.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)

, iResolver(dzn_locator.get< IResolver>())

, iRotationalEnergyCheck({{"iRotationalEnergyCheck",this,&dzn_meta},{"",0,0}})

, iAngularVelocitySensor({{"",0,0},{"iAngularVelocitySensor",this,&dzn_meta}})
, iNext({{"",0,0},{"iNext",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;



  iRotationalEnergyCheck.in.do_check = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iRotationalEnergyCheck) = false; return iRotationalEnergyCheck_do_check();}, this->iRotationalEnergyCheck.meta, "do_check");};



}

::Behavior::type RotationalEnergyCheck::iRotationalEnergyCheck_do_check()
{

  {
    ::Behavior::type currSafetyState = ::Behavior::Safe;
    this->iAngularVelocitySensor.in.retrieve_re_from_ang_vel();
    currSafetyState = this->iResolver.in.resolve_re_from_ang_vel();
    ::Behavior::type nextSafetyState = this->iNext.in.do_check();
    ::Behavior::type res = and_safety_states(currSafetyState,nextSafetyState);
    { this->reply_Behavior = res; }
  }

  return this->reply_Behavior;
}

::Behavior::type RotationalEnergyCheck::and_safety_states (::Behavior::type current,::Behavior::type next) 
{
  {
    if ((current == ::Behavior::Unsafe || next == ::Behavior::Unsafe)) 
    {
      return ::Behavior::Unsafe;
    }
  }
  return ::Behavior::Safe;
}

void RotationalEnergyCheck::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void RotationalEnergyCheck::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1