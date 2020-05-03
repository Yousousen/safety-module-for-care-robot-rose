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

/********************************** INTERFACE *********************************/
#ifndef IGRIPARMCONTROL_HH
#define IGRIPARMCONTROL_HH



struct IGripArmControl
{
#ifndef ENUM_IGripArmControl_State
#define ENUM_IGripArmControl_State 1


  struct State
  {
    enum type
    {
      Unsafe,Safe
    };
  };


#endif // ENUM_IGripArmControl_State

  struct
  {
    std::function< void()> check_folded;
    std::function< void()> check_strength;
    std::function< void(struct fb_t*&)> arm_folded;
    std::function< void(struct fb_t*&)> arm_unfolded;
    std::function< void()> stop;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IGripArmControl(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.check_folded) throw dzn::binding_error(meta, "in.check_folded");
    if (! in.check_strength) throw dzn::binding_error(meta, "in.check_strength");
    if (! in.arm_folded) throw dzn::binding_error(meta, "in.arm_folded");
    if (! in.arm_unfolded) throw dzn::binding_error(meta, "in.arm_unfolded");
    if (! in.stop) throw dzn::binding_error(meta, "in.stop");


  }
};

inline void connect (IGripArmControl& provided, IGripArmControl& required)
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
#ifndef ENUM_TO_STRING_IGripArmControl_State
#define ENUM_TO_STRING_IGripArmControl_State 1
inline std::string to_string(::IGripArmControl::State::type v)
{
  switch(v)
  {
    case ::IGripArmControl::State::Unsafe: return "State_Unsafe";
    case ::IGripArmControl::State::Safe: return "State_Safe";

  }
  return "";
}
#endif // ENUM_TO_STRING_IGripArmControl_State

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
#ifndef STRING_TO_ENUM_IGripArmControl_State
#define STRING_TO_ENUM_IGripArmControl_State 1
inline ::IGripArmControl::State::type to_IGripArmControl_State(std::string s)
{
  static std::map<std::string, ::IGripArmControl::State::type> m = {
    {"State_Unsafe", ::IGripArmControl::State::Unsafe},
    {"State_Safe", ::IGripArmControl::State::Safe},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_IGripArmControl_State


#endif // IGRIPARMCONTROL_HH

/********************************** INTERFACE *********************************/


//version: 2.9.1
