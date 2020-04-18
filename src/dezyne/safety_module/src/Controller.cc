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
: dzn_meta{"","Controller",0,0,{},{},{[this]{iController.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)
, state(::Controller::State::Initialising), color_red(0x3000), color_blue(0x0006), fb()

, iController({{"iController",this,&dzn_meta},{"",0,0}})



{
  dzn_rt.performs_flush(this) = true;

  iController.in.initialise = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_initialise();}, this->iController.meta, "initialise");};
  iController.in.destruct = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_destruct();}, this->iController.meta, "destruct");};
  iController.in.reset = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_reset();}, this->iController.meta, "reset");};
  iController.in.trigger_red = [&](struct fb_t*& fb){return dzn::call_in(this,[=, & fb]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_trigger_red(fb);}, this->iController.meta, "trigger_red");};
  iController.in.trigger_blue = [&](struct fb_t*& fb){return dzn::call_in(this,[=, & fb]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iController) = false; return iController_trigger_blue(fb);}, this->iController.meta, "trigger_blue");};





}

void Controller::iController_initialise()
{
  if (state == ::Controller::State::Initialising) 
  {
    this->iController.out.initialise_framebuffer();
    state = ::Controller::State::Operating;
  }
  else if (state == ::Controller::State::Operating) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Controller::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::Controller::State::Destructing) && (!(state == ::Controller::State::Operating) && !(state == ::Controller::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_destruct()
{
  if (state == ::Controller::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Controller::State::Operating) 
  {
    this->iController.out.destruct_framebuffer();
    state = ::Controller::State::Destructing;
  }
  else if (state == ::Controller::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::Controller::State::Destructing) && (!(state == ::Controller::State::Operating) && !(state == ::Controller::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_reset()
{
  if (state == ::Controller::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Controller::State::Operating) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Controller::State::Destructing) 
  {
    state = ::Controller::State::Initialising;
  }
  else if ((!(state == ::Controller::State::Destructing) && (!(state == ::Controller::State::Operating) && !(state == ::Controller::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_trigger_red(struct fb_t*& fbx)
{
  if (state == ::Controller::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Controller::State::Operating) 
  {
    this->iController.out.light_led(fbx,color_red);
  }
  else if (state == ::Controller::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::Controller::State::Destructing) && (!(state == ::Controller::State::Operating) && !(state == ::Controller::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Controller::iController_trigger_blue(struct fb_t*& fbx)
{
  if (state == ::Controller::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Controller::State::Operating) 
  {
    this->iController.out.light_led(fbx,color_blue);
  }
  else if (state == ::Controller::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::Controller::State::Destructing) && (!(state == ::Controller::State::Operating) && !(state == ::Controller::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
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