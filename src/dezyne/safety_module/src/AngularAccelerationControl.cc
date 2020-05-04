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
#include "AngularAccelerationControl.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



AngularAccelerationControl::AngularAccelerationControl(const dzn::locator& dzn_locator)
: dzn_meta{"","AngularAccelerationControl",0,0,{& iAngularAccelerationSensor.meta,& iResolver.meta},{},{[this]{iAngularAccelerationControl.check_bindings();},[this]{iAngularAccelerationSensor.check_bindings();},[this]{iResolver.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)

, iResolver(dzn_locator.get< IResolver>())

, iAngularAccelerationControl({{"iAngularAccelerationControl",this,&dzn_meta},{"",0,0}})

, iAngularAccelerationSensor({{"",0,0},{"iAngularAccelerationSensor",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;



  iAngularAccelerationControl.in.check_angular_acceleration = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iAngularAccelerationControl) = false; return iAngularAccelerationControl_check_angular_acceleration();}, this->iAngularAccelerationControl.meta, "check_angular_acceleration");};



}

::Behavior::type AngularAccelerationControl::iAngularAccelerationControl_check_angular_acceleration()
{

  {
    ::Behavior::type safetyState = ::Behavior::Safe;
    this->iAngularAccelerationSensor.in.retrieve_re_from_ang_acc();
    safetyState = this->iResolver.in.resolve_re_from_ang_acc();
    { this->reply_Behavior = safetyState; }
  }

  return this->reply_Behavior;
}


void AngularAccelerationControl::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void AngularAccelerationControl::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1