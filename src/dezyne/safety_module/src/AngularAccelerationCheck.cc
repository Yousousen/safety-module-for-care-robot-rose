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
#include "AngularAccelerationCheck.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



AngularAccelerationCheck::AngularAccelerationCheck(const dzn::locator& dzn_locator)
: dzn_meta{"","AngularAccelerationCheck",0,0,{& iAngularAccelerationSensor.meta,& iNext.meta,& iResolver.meta},{},{[this]{iAngularAccelerationSensor.check_bindings();},[this]{iAngularAccelerationCheck.check_bindings();},[this]{iNext.check_bindings();},[this]{iResolver.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)

, iResolver(dzn_locator.get< IResolver>())

, iAngularAccelerationCheck({{"iAngularAccelerationCheck",this,&dzn_meta},{"",0,0}})

, iAngularAccelerationSensor({{"",0,0},{"iAngularAccelerationSensor",this,&dzn_meta}})
, iNext({{"",0,0},{"iNext",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;



  iAngularAccelerationCheck.in.do_check = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iAngularAccelerationCheck) = false; return iAngularAccelerationCheck_do_check();}, this->iAngularAccelerationCheck.meta, "do_check");};



}

::Behavior::type AngularAccelerationCheck::iAngularAccelerationCheck_do_check()
{

  {
    ::Behavior::type currSafetyState = ::Behavior::Safe;
    this->iAngularAccelerationSensor.in.retrieve_re_from_ang_acc();
    currSafetyState = this->iResolver.in.resolve_re_from_ang_acc();
    ::Behavior::type nextSafetyState = this->iNext.in.do_check();
    ::Behavior::type res = and_safety_states(currSafetyState,nextSafetyState);
    { this->reply_Behavior = res; }
  }

  return this->reply_Behavior;
}

::Behavior::type AngularAccelerationCheck::and_safety_states (::Behavior::type current,::Behavior::type next) 
{
  {
    if ((current == ::Behavior::Unsafe || next == ::Behavior::Unsafe)) 
    {
      return ::Behavior::Unsafe;
    }
  }
  return ::Behavior::Safe;
}

void AngularAccelerationCheck::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void AngularAccelerationCheck::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1