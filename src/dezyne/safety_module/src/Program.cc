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
#include "Program.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



Program::Program(const dzn::locator& dzn_locator)
: dzn_meta{"","Program",0,0,{},{},{[this]{iProgram.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)
, state(::Program::State::Initialising), color_red(0x3000), color_blue(0x0006), fb()

, iProgram({{"iProgram",this,&dzn_meta},{"",0,0}})



{
  dzn_rt.performs_flush(this) = true;

  iProgram.in.start = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iProgram) = false; return iProgram_start();}, this->iProgram.meta, "start");};
  iProgram.in.stop = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iProgram) = false; return iProgram_stop();}, this->iProgram.meta, "stop");};
  iProgram.in.reset = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iProgram) = false; return iProgram_reset();}, this->iProgram.meta, "reset");};
  iProgram.in.trigger_red = [&](struct fb_t*& fb){return dzn::call_in(this,[=, & fb]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iProgram) = false; return iProgram_trigger_red(fb);}, this->iProgram.meta, "trigger_red");};
  iProgram.in.trigger_blue = [&](struct fb_t*& fb){return dzn::call_in(this,[=, & fb]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iProgram) = false; return iProgram_trigger_blue(fb);}, this->iProgram.meta, "trigger_blue");};





}

void Program::iProgram_start()
{
  if (state == ::Program::State::Initialising) 
  {
    this->iProgram.out.initialise_framebuffer();
    state = ::Program::State::Operating;
  }
  else if (state == ::Program::State::Operating) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Program::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::Program::State::Destructing) && (!(state == ::Program::State::Operating) && !(state == ::Program::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Program::iProgram_stop()
{
  if (state == ::Program::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Program::State::Operating) 
  {
    this->iProgram.out.destruct_framebuffer();
    state = ::Program::State::Destructing;
  }
  else if (state == ::Program::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::Program::State::Destructing) && (!(state == ::Program::State::Operating) && !(state == ::Program::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Program::iProgram_reset()
{
  if (state == ::Program::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Program::State::Operating) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Program::State::Destructing) 
  {
    state = ::Program::State::Initialising;
  }
  else if ((!(state == ::Program::State::Destructing) && (!(state == ::Program::State::Operating) && !(state == ::Program::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Program::iProgram_trigger_red(struct fb_t*& fbx)
{
  if (state == ::Program::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Program::State::Operating) 
  {
    this->iProgram.out.light_led(fbx,color_red);
  }
  else if (state == ::Program::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::Program::State::Destructing) && (!(state == ::Program::State::Operating) && !(state == ::Program::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}
void Program::iProgram_trigger_blue(struct fb_t*& fbx)
{
  if (state == ::Program::State::Initialising) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if (state == ::Program::State::Operating) 
  {
    this->iProgram.out.light_led(fbx,color_blue);
  }
  else if (state == ::Program::State::Destructing) dzn_locator.get<dzn::illegal_handler>().illegal();
  else if ((!(state == ::Program::State::Destructing) && (!(state == ::Program::State::Operating) && !(state == ::Program::State::Initialising)))) dzn_locator.get<dzn::illegal_handler>().illegal();
  else dzn_locator.get<dzn::illegal_handler>().illegal();

  return;

}


void Program::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void Program::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1