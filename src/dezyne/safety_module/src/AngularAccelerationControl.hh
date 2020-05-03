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
#ifndef IANGULARACCELERATIONCONTROL_HH
#define IANGULARACCELERATIONCONTROL_HH



struct IAngularAccelerationControl
{
#ifndef ENUM_IAngularAccelerationControl_State
#define ENUM_IAngularAccelerationControl_State 1


  struct State
  {
    enum type
    {
      Unsafe,Safe
    };
  };


#endif // ENUM_IAngularAccelerationControl_State

  struct
  {
    std::function< void()> check_acceleration;
    std::function< void(struct fb_t*&)> safe_acceleration;
    std::function< void(struct fb_t*&)> unsafe_acceleration;
    std::function< void()> stop;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IAngularAccelerationControl(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.check_acceleration) throw dzn::binding_error(meta, "in.check_acceleration");
    if (! in.safe_acceleration) throw dzn::binding_error(meta, "in.safe_acceleration");
    if (! in.unsafe_acceleration) throw dzn::binding_error(meta, "in.unsafe_acceleration");
    if (! in.stop) throw dzn::binding_error(meta, "in.stop");


  }
};

inline void connect (IAngularAccelerationControl& provided, IAngularAccelerationControl& required)
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
#ifndef ENUM_TO_STRING_IAngularAccelerationControl_State
#define ENUM_TO_STRING_IAngularAccelerationControl_State 1
inline std::string to_string(::IAngularAccelerationControl::State::type v)
{
  switch(v)
  {
    case ::IAngularAccelerationControl::State::Unsafe: return "State_Unsafe";
    case ::IAngularAccelerationControl::State::Safe: return "State_Safe";

  }
  return "";
}
#endif // ENUM_TO_STRING_IAngularAccelerationControl_State

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
#ifndef STRING_TO_ENUM_IAngularAccelerationControl_State
#define STRING_TO_ENUM_IAngularAccelerationControl_State 1
inline ::IAngularAccelerationControl::State::type to_IAngularAccelerationControl_State(std::string s)
{
  static std::map<std::string, ::IAngularAccelerationControl::State::type> m = {
    {"State_Unsafe", ::IAngularAccelerationControl::State::Unsafe},
    {"State_Safe", ::IAngularAccelerationControl::State::Safe},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_IAngularAccelerationControl_State


#endif // IANGULARACCELERATIONCONTROL_HH

/********************************** INTERFACE *********************************/


//version: 2.9.1
