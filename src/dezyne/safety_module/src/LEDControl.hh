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
#include <dzn/meta.hh>

namespace dzn {
  struct locator;
  struct runtime;
}



#include <iostream>
#include <map>


/********************************** INTERFACE *********************************/
#ifndef ILEDCONTROL_HH
#define ILEDCONTROL_HH



struct ILEDControl
{
#ifndef ENUM_ILEDControl_State
#define ENUM_ILEDControl_State 1


  struct State
  {
    enum type
    {
      Idle,Operating
    };
  };


#endif // ENUM_ILEDControl_State

  struct
  {
    std::function< void()> initialise_framebuffer;
    std::function< void()> destruct_framebuffer;
    std::function< void(struct fb_t*,unsigned)> light_led;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline ILEDControl(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.initialise_framebuffer) throw dzn::binding_error(meta, "in.initialise_framebuffer");
    if (! in.destruct_framebuffer) throw dzn::binding_error(meta, "in.destruct_framebuffer");
    if (! in.light_led) throw dzn::binding_error(meta, "in.light_led");


  }
};

inline void connect (ILEDControl& provided, ILEDControl& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}


#ifndef ENUM_TO_STRING_ILEDControl_State
#define ENUM_TO_STRING_ILEDControl_State 1
inline std::string to_string(::ILEDControl::State::type v)
{
  switch(v)
  {
    case ::ILEDControl::State::Idle: return "State_Idle";
    case ::ILEDControl::State::Operating: return "State_Operating";

  }
  return "";
}
#endif // ENUM_TO_STRING_ILEDControl_State

#ifndef STRING_TO_ENUM_ILEDControl_State
#define STRING_TO_ENUM_ILEDControl_State 1
inline ::ILEDControl::State::type to_ILEDControl_State(std::string s)
{
  static std::map<std::string, ::ILEDControl::State::type> m = {
    {"State_Idle", ::ILEDControl::State::Idle},
    {"State_Operating", ::ILEDControl::State::Operating},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_ILEDControl_State


#endif // ILEDCONTROL_HH

/********************************** INTERFACE *********************************/


//version: 2.9.1
