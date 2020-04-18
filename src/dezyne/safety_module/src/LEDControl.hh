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
      Initialising,Operating,Destructing
    };
  };


#endif // ENUM_ILEDControl_State

  struct
  {
    std::function< void()> initialise;
    std::function< void()> destruct;
    std::function< void()> reset;
    std::function< void(struct fb_t*&)> trigger_red;
    std::function< void(struct fb_t*&)> trigger_blue;
  } in;

  struct
  {
    std::function< void()> initialise_framebuffer;
    std::function< void()> destruct_framebuffer;
    std::function< void(struct fb_t*,unsigned)> light_led;
  } out;

  dzn::port::meta meta;
  inline ILEDControl(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.initialise) throw dzn::binding_error(meta, "in.initialise");
    if (! in.destruct) throw dzn::binding_error(meta, "in.destruct");
    if (! in.reset) throw dzn::binding_error(meta, "in.reset");
    if (! in.trigger_red) throw dzn::binding_error(meta, "in.trigger_red");
    if (! in.trigger_blue) throw dzn::binding_error(meta, "in.trigger_blue");

    if (! out.initialise_framebuffer) throw dzn::binding_error(meta, "out.initialise_framebuffer");
    if (! out.destruct_framebuffer) throw dzn::binding_error(meta, "out.destruct_framebuffer");
    if (! out.light_led) throw dzn::binding_error(meta, "out.light_led");

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
    case ::ILEDControl::State::Initialising: return "State_Initialising";
    case ::ILEDControl::State::Operating: return "State_Operating";
    case ::ILEDControl::State::Destructing: return "State_Destructing";

  }
  return "";
}
#endif // ENUM_TO_STRING_ILEDControl_State

#ifndef STRING_TO_ENUM_ILEDControl_State
#define STRING_TO_ENUM_ILEDControl_State 1
inline ::ILEDControl::State::type to_ILEDControl_State(std::string s)
{
  static std::map<std::string, ::ILEDControl::State::type> m = {
    {"State_Initialising", ::ILEDControl::State::Initialising},
    {"State_Operating", ::ILEDControl::State::Operating},
    {"State_Destructing", ::ILEDControl::State::Destructing},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_ILEDControl_State


#endif // ILEDCONTROL_HH

/********************************** INTERFACE *********************************/
/********************************** COMPONENT *********************************/
#ifndef LEDCONTROL_HH
#define LEDCONTROL_HH




struct LEDControl
{
  dzn::meta dzn_meta;
  dzn::runtime& dzn_rt;
  dzn::locator const& dzn_locator;
#ifndef ENUM_LEDControl_State
#define ENUM_LEDControl_State 1


  struct State
  {
    enum type
    {
      Initialising,Operating,Destructing
    };
  };


#endif // ENUM_LEDControl_State

  ::LEDControl::State::type state;
  unsigned color_red;
  unsigned color_blue;
  struct fb_t* fb;


  std::function<void ()> out_iLEDControl;

  ::ILEDControl iLEDControl;



  LEDControl(const dzn::locator&);
  void check_bindings() const;
  void dump_tree(std::ostream& os) const;
  friend std::ostream& operator << (std::ostream& os, const LEDControl& m)  {
    (void)m;
    return os << "[" << m.state <<", " << m.color_red <<", " << m.color_blue <<", " << m.fb <<"]" ;
  }
  private:
  void iLEDControl_initialise();
  void iLEDControl_destruct();
  void iLEDControl_reset();
  void iLEDControl_trigger_red(struct fb_t*& fbx);
  void iLEDControl_trigger_blue(struct fb_t*& fbx);

};

#endif // LEDCONTROL_HH

/********************************** COMPONENT *********************************/


//version: 2.9.1
