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
#include "LEDControl.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



LEDControl::LEDControl(const dzn::locator& dzn_locator)
: dzn_meta{"","LEDControl",0,0,{},{},{[this]{iLEDControl.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)
, state(::LEDControl::State::Initialising), color_red(0x3000), color_blue(0x0006), fb()

, iLEDControl({{"iLEDControl",this,&dzn_meta},{"",0,0}})



{
  dzn_rt.performs_flush(this) = true;

  iLEDControl.in.initialise = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iLEDControl) = false; return iLEDControl_initialise();}, this->iLEDControl.meta, "initialise");};
  iLEDControl.in.destruct = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iLEDControl) = false; return iLEDControl_destruct();}, this->iLEDControl.meta, "destruct");};
  iLEDControl.in.reset = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iLEDControl) = false; return iLEDControl_reset();}, this->iLEDControl.meta, "reset");};
  iLEDControl.in.trigger_red = [&](struct fb_t*& fb){return dzn::call_in(this,[=, & fb]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iLEDControl) = false; return iLEDControl_trigger_red(fb);}, this->iLEDControl.meta, "trigger_red");};
  iLEDControl.in.trigger_blue = [&](struct fb_t*& fb){return dzn::call_in(this,[=, & fb]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iLEDControl) = false; return iLEDControl_trigger_blue(fb);}, this->iLEDControl.meta, "trigger_blue");};





}

void LEDControl::iLEDControl_initialise()
{
  if (state == ::LEDControl::State::Initialising) 
  {
    this->iLEDControl.out.initialise_framebuffer();
    state = ::LEDControl::State::Operating;
  }
  else if (state == ::LEDControl::State::Operating) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::LEDControl::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::LEDControl::State::Destructing) && (!(state == ::LEDControl::State::Operating) && !(state == ::LEDControl::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void LEDControl::iLEDControl_destruct()
{
  if (state == ::LEDControl::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::LEDControl::State::Operating) 
  {
    this->iLEDControl.out.destruct_framebuffer();
    state = ::LEDControl::State::Destructing;
  }
  else if (state == ::LEDControl::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::LEDControl::State::Destructing) && (!(state == ::LEDControl::State::Operating) && !(state == ::LEDControl::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void LEDControl::iLEDControl_reset()
{
  if (state == ::LEDControl::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::LEDControl::State::Operating) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::LEDControl::State::Destructing) 
  {
    state = ::LEDControl::State::Initialising;
  }
  else if ((!(state == ::LEDControl::State::Destructing) && (!(state == ::LEDControl::State::Operating) && !(state == ::LEDControl::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void LEDControl::iLEDControl_trigger_red(struct fb_t*& fbx)
{
  if (state == ::LEDControl::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::LEDControl::State::Operating) 
  {
    this->iLEDControl.out.light_led(fbx,color_red);
  }
  else if (state == ::LEDControl::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::LEDControl::State::Destructing) && (!(state == ::LEDControl::State::Operating) && !(state == ::LEDControl::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void LEDControl::iLEDControl_trigger_blue(struct fb_t*& fbx)
{
  if (state == ::LEDControl::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::LEDControl::State::Operating) 
  {
    this->iLEDControl.out.light_led(fbx,color_blue);
  }
  else if (state == ::LEDControl::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::LEDControl::State::Destructing) && (!(state == ::LEDControl::State::Operating) && !(state == ::LEDControl::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}


void LEDControl::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void LEDControl::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1