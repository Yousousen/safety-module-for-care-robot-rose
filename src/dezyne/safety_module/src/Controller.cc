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
: dzn_meta{"","Controller",0,0,{& iLEDControl.meta,& iNext.meta},{},{[this]{iController.check_bindings();},[this]{iLEDControl.check_bindings();},[this]{iNext.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)
, systemState(::Controller::State::Idle), unsafe_acknowledged(true), red("0x3000"), blue("0x0006")

, iController({{"iController",this,&dzn_meta},{"",0,0}})

, iLEDControl({{"",0,0},{"iLEDControl",this,&dzn_meta}})
, iNext({{"",0,0},{"iNext",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;

  iController.in.initialise = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_initialise();}, this->iController.meta, "initialise");};
  iController.in.destruct = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_destruct();}, this->iController.meta, "destruct");};
  iController.in.reset = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_reset();}, this->iController.meta, "reset");};


  iController.in.do_checks = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_do_checks();}, this->iController.meta, "do_checks");};



}

void Controller::iController_initialise()
{
  if (systemState == ::Controller::State::Idle) 
  {
    this->iLEDControl.in.initialise_framebuffer();
    this->iController.out.initialise_imu();
    this->iController.out.initialise_mutexes();
    this->iController.out.initialise_semaphores();
    systemState = ::Controller::State::Operating;
  }
  else if (!(systemState == ::Controller::State::Idle)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_destruct()
{
  if (systemState == ::Controller::State::Operating) 
  {
    this->iLEDControl.in.destruct_framebuffer();
    this->iController.out.destruct_mutexes();
    this->iController.out.destruct_semaphores();
    systemState = ::Controller::State::Idle;
  }
  else if (!(systemState == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_reset()
{
  if (systemState == ::Controller::State::Operating) 
  {
    this->iLEDControl.in.reset_led();
    unsafe_acknowledged = true;
  }
  else if (!(systemState == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
::UnsafeTriggered::type Controller::iController_do_checks()
{
  if (systemState == ::Controller::State::Operating) 
  {
    ::Behavior::type safetyState = this->iNext.in.do_check();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    {
      if (!(unsafe_acknowledged)) 
      {
        { this->reply_UnsafeTriggered = ::UnsafeTriggered::Yes; }
      }
      else 
      {
        { this->reply_UnsafeTriggered = ::UnsafeTriggered::No; }
      }
    }
  }
  else if (!(systemState == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return this->reply_UnsafeTriggered;
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