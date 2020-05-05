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

#ifndef ENUM_Behavior
#define ENUM_Behavior 1


struct Behavior
{
  enum type
  {
    Unsafe,Safe
  };
};


#endif // ENUM_Behavior
#ifndef ENUM_UnsafeTriggered
#define ENUM_UnsafeTriggered 1


struct UnsafeTriggered
{
  enum type
  {
    No,Yes
  };
};


#endif // ENUM_UnsafeTriggered

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
#ifndef ENUM_ILEDControl_LedState
#define ENUM_ILEDControl_LedState 1


  struct LedState
  {
    enum type
    {
      Red,Blue,Low
    };
  };


#endif // ENUM_ILEDControl_LedState

  struct
  {
    std::function< void()> initialise_framebuffer;
    std::function< void()> destruct_framebuffer;
    std::function< void()> light_led_red;
    std::function< void()> light_led_blue;
    std::function< void()> reset_led;
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
    if (! in.light_led_red) throw dzn::binding_error(meta, "in.light_led_red");
    if (! in.light_led_blue) throw dzn::binding_error(meta, "in.light_led_blue");
    if (! in.reset_led) throw dzn::binding_error(meta, "in.reset_led");


  }
};

inline void connect (ILEDControl& provided, ILEDControl& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}


#ifndef ENUM_TO_STRING_Behavior
#define ENUM_TO_STRING_Behavior 1
inline std::string to_string(::Behavior::type v)
{
  switch(v)
  {
    case ::Behavior::Unsafe: return "Behavior_Unsafe";
    case ::Behavior::Safe: return "Behavior_Safe";

  }
  return "";
}
#endif // ENUM_TO_STRING_Behavior
#ifndef ENUM_TO_STRING_UnsafeTriggered
#define ENUM_TO_STRING_UnsafeTriggered 1
inline std::string to_string(::UnsafeTriggered::type v)
{
  switch(v)
  {
    case ::UnsafeTriggered::No: return "UnsafeTriggered_No";
    case ::UnsafeTriggered::Yes: return "UnsafeTriggered_Yes";

  }
  return "";
}
#endif // ENUM_TO_STRING_UnsafeTriggered
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
#ifndef ENUM_TO_STRING_ILEDControl_LedState
#define ENUM_TO_STRING_ILEDControl_LedState 1
inline std::string to_string(::ILEDControl::LedState::type v)
{
  switch(v)
  {
    case ::ILEDControl::LedState::Red: return "LedState_Red";
    case ::ILEDControl::LedState::Blue: return "LedState_Blue";
    case ::ILEDControl::LedState::Low: return "LedState_Low";

  }
  return "";
}
#endif // ENUM_TO_STRING_ILEDControl_LedState

#ifndef STRING_TO_ENUM_Behavior
#define STRING_TO_ENUM_Behavior 1
inline ::Behavior::type to_Behavior(std::string s)
{
  static std::map<std::string, ::Behavior::type> m = {
    {"Behavior_Unsafe", ::Behavior::Unsafe},
    {"Behavior_Safe", ::Behavior::Safe},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_Behavior
#ifndef STRING_TO_ENUM_UnsafeTriggered
#define STRING_TO_ENUM_UnsafeTriggered 1
inline ::UnsafeTriggered::type to_UnsafeTriggered(std::string s)
{
  static std::map<std::string, ::UnsafeTriggered::type> m = {
    {"UnsafeTriggered_No", ::UnsafeTriggered::No},
    {"UnsafeTriggered_Yes", ::UnsafeTriggered::Yes},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_UnsafeTriggered
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
#ifndef STRING_TO_ENUM_ILEDControl_LedState
#define STRING_TO_ENUM_ILEDControl_LedState 1
inline ::ILEDControl::LedState::type to_ILEDControl_LedState(std::string s)
{
  static std::map<std::string, ::ILEDControl::LedState::type> m = {
    {"LedState_Red", ::ILEDControl::LedState::Red},
    {"LedState_Blue", ::ILEDControl::LedState::Blue},
    {"LedState_Low", ::ILEDControl::LedState::Low},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_ILEDControl_LedState


#endif // ILEDCONTROL_HH

/********************************** INTERFACE *********************************/


//version: 2.9.1
