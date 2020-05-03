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
#include "AccelerationControl.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



AccelerationControl::AccelerationControl(const dzn::locator& dzn_locator)
: dzn_meta{"","AccelerationControl",0,0,{& iAccelerationSensor.meta,& iResolver.meta},{},{[this]{iAccelerationControl.check_bindings();},[this]{iAccelerationSensor.check_bindings();},[this]{iResolver.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)

, iResolver(dzn_locator.get< IResolver>())

, iAccelerationControl({{"iAccelerationControl",this,&dzn_meta},{"",0,0}})

, iAccelerationSensor({{"",0,0},{"iAccelerationSensor",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;



  iAccelerationControl.in.check_acceleration = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iAccelerationControl) = false; return iAccelerationControl_check_acceleration();}, this->iAccelerationControl.meta, "check_acceleration");};



}

::Behavior::type AccelerationControl::iAccelerationControl_check_acceleration()
{

  {
    ::Behavior::type safetyState = ::Behavior::Safe;
    this->iAccelerationSensor.in.retrieve_ke_from_acc();
    safetyState = this->iResolver.in.resolve_ke_from_acc();
    { this->reply_Behavior = safetyState; }
  }

  return this->reply_Behavior;
}


void AccelerationControl::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void AccelerationControl::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1