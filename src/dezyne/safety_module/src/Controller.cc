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
#include "Controller.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



Controller::Controller(const dzn::locator& dzn_locator)
: dzn_meta{"","Controller",0,0,{& iLEDControl.meta,& iAccelerationControl.meta,& iAngularAccelerationControl.meta},{},{[this]{iController.check_bindings();},[this]{iLEDControl.check_bindings();},[this]{iAccelerationControl.check_bindings();},[this]{iAngularAccelerationControl.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)
, unsafe_triggered(false), state(::Controller::State::Idle)

, iController({{"iController",this,&dzn_meta},{"",0,0}})

, iLEDControl({{"",0,0},{"iLEDControl",this,&dzn_meta}})
, iAccelerationControl({{"",0,0},{"iAccelerationControl",this,&dzn_meta}})
, iAngularAccelerationControl({{"",0,0},{"iAngularAccelerationControl",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;

  iController.in.initialise = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_initialise();}, this->iController.meta, "initialise");};
  iController.in.destruct = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_destruct();}, this->iController.meta, "destruct");};
  iController.in.reset = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_reset();}, this->iController.meta, "reset");};
  iController.in.do_checks = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_do_checks();}, this->iController.meta, "do_checks");};





}

void Controller::iController_initialise()
{
  if (state == ::Controller::State::Idle) 
  {
    this->iLEDControl.in.initialise_framebuffer();
    state = ::Controller::State::Operating;
  }
  else if (!(state == ::Controller::State::Idle)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_destruct()
{
  if (state == ::Controller::State::Operating) 
  {
    this->iLEDControl.in.destruct_framebuffer();
    state = ::Controller::State::Idle;
  }
  else if (!(state == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_reset()
{
  if (state == ::Controller::State::Operating) 
  {
    this->iLEDControl.in.reset_led();
    unsafe_triggered = false;
  }
  else if (!(state == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_do_checks()
{
  if (state == ::Controller::State::Operating) 
  {
    ::Behavior::type safetyState = this->iAccelerationControl.in.check_acceleration();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red();
        unsafe_triggered = true;
      }
      else {
        if (!(unsafe_triggered)) 
        {
          this->iLEDControl.in.light_led_blue();
        }
      }
    }
    safetyState = this->iAngularAccelerationControl.in.check_angular_acceleration();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red();
        unsafe_triggered = true;
      }
      else {
        if (!(unsafe_triggered)) 
        {
          this->iLEDControl.in.light_led_blue();
        }
      }
    }
  }
  else if (!(state == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}


void Controller::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void Controller::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1