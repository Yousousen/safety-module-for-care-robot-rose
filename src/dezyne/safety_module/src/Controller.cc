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
: dzn_meta{"","Controller",0,0,{& iLEDControl.meta,& iAccelerationControl.meta,& iAngularAccelerationControl.meta,& iGripArmControl.meta},{},{[this]{iController.check_bindings();},[this]{iLEDControl.check_bindings();},[this]{iAccelerationControl.check_bindings();},[this]{iAngularAccelerationControl.check_bindings();},[this]{iGripArmControl.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)
, systemState(::Controller::State::Idle), unsafe_acknowledged(true), acc_triggered(), angacc_triggered(), str_triggered(), pos_triggered(), red("0x3000"), blue("0x0006")

, iController({{"iController",this,&dzn_meta},{"",0,0}})

, iLEDControl({{"",0,0},{"iLEDControl",this,&dzn_meta}})
, iAccelerationControl({{"",0,0},{"iAccelerationControl",this,&dzn_meta}})
, iAngularAccelerationControl({{"",0,0},{"iAngularAccelerationControl",this,&dzn_meta}})
, iGripArmControl({{"",0,0},{"iGripArmControl",this,&dzn_meta}})


{
  dzn_rt.performs_flush(this) = true;

  iController.in.initialise = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_initialise();}, this->iController.meta, "initialise");};
  iController.in.destruct = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_destruct();}, this->iController.meta, "destruct");};
  iController.in.reset = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_reset();}, this->iController.meta, "reset");};


  iController.in.do_checks = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_do_checks();}, this->iController.meta, "do_checks");};
  iController.in.check_acc = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_check_acc();}, this->iController.meta, "check_acc");};
  iController.in.check_angacc = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_check_angacc();}, this->iController.meta, "check_angacc");};
  iController.in.check_str = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_check_str();}, this->iController.meta, "check_str");};
  iController.in.check_pos = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_check_pos();}, this->iController.meta, "check_pos");};



}

void Controller::iController_initialise()
{
  if (systemState == ::Controller::State::Idle) 
  {
    this->iLEDControl.in.initialise_framebuffer();
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
    acc_triggered = false;
    angacc_triggered = false;
    str_triggered = false;
    pos_triggered = false;
    ::Behavior::type safetyState = this->iAccelerationControl.in.check_acceleration();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
        acc_triggered = true;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    safetyState = this->iAngularAccelerationControl.in.check_angular_acceleration();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
        angacc_triggered = true;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    safetyState = this->iGripArmControl.in.check_arm_strength();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
        str_triggered = true;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    safetyState = this->iGripArmControl.in.check_arm_position();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
        pos_triggered = true;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    this->iController.out.what_triggered(acc_triggered,angacc_triggered,str_triggered,pos_triggered);
    {
      if (!(unsafe_acknowledged)) { this->reply_UnsafeTriggered = ::UnsafeTriggered::Yes; }
      else { this->reply_UnsafeTriggered = ::UnsafeTriggered::No; }
    }
  }
  else if (!(systemState == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return this->reply_UnsafeTriggered;
}
::UnsafeTriggered::type Controller::iController_check_acc()
{
  if (systemState == ::Controller::State::Operating) 
  {
    acc_triggered = false;
    ::Behavior::type safetyState = this->iAccelerationControl.in.check_acceleration();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
        acc_triggered = true;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    this->iController.out.what_triggered(acc_triggered,angacc_triggered,str_triggered,pos_triggered);
    {
      if (!(unsafe_acknowledged)) { this->reply_UnsafeTriggered = ::UnsafeTriggered::Yes; }
      else { this->reply_UnsafeTriggered = ::UnsafeTriggered::No; }
    }
  }
  else if (!(systemState == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return this->reply_UnsafeTriggered;
}
::UnsafeTriggered::type Controller::iController_check_angacc()
{
  if (systemState == ::Controller::State::Operating) 
  {
    angacc_triggered = false;
    ::Behavior::type safetyState = this->iAngularAccelerationControl.in.check_angular_acceleration();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
        angacc_triggered = true;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    this->iController.out.what_triggered(acc_triggered,angacc_triggered,str_triggered,pos_triggered);
    {
      if (!(unsafe_acknowledged)) { this->reply_UnsafeTriggered = ::UnsafeTriggered::Yes; }
      else { this->reply_UnsafeTriggered = ::UnsafeTriggered::No; }
    }
  }
  else if (!(systemState == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return this->reply_UnsafeTriggered;
}
::UnsafeTriggered::type Controller::iController_check_str()
{
  if (systemState == ::Controller::State::Operating) 
  {
    str_triggered = false;
    ::Behavior::type safetyState = this->iGripArmControl.in.check_arm_strength();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
        str_triggered = true;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    this->iController.out.what_triggered(acc_triggered,angacc_triggered,str_triggered,pos_triggered);
    {
      if (!(unsafe_acknowledged)) { this->reply_UnsafeTriggered = ::UnsafeTriggered::Yes; }
      else { this->reply_UnsafeTriggered = ::UnsafeTriggered::No; }
    }
  }
  else if (!(systemState == ::Controller::State::Operating)) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return this->reply_UnsafeTriggered;
}
::UnsafeTriggered::type Controller::iController_check_pos()
{
  if (systemState == ::Controller::State::Operating) 
  {
    pos_triggered = false;
    ::Behavior::type safetyState = this->iGripArmControl.in.check_arm_position();
    {
      if (safetyState == ::Behavior::Unsafe) 
      {
        this->iLEDControl.in.light_led_red(red);
        unsafe_acknowledged = false;
        pos_triggered = true;
      }
      else {
        if (unsafe_acknowledged) 
        {
          this->iLEDControl.in.light_led_blue(blue);
        }
      }
    }
    this->iController.out.what_triggered(acc_triggered,angacc_triggered,str_triggered,pos_triggered);
    {
      if (!(unsafe_acknowledged)) { this->reply_UnsafeTriggered = ::UnsafeTriggered::Yes; }
      else { this->reply_UnsafeTriggered = ::UnsafeTriggered::No; }
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