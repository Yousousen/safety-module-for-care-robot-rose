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
#ifndef IRESOLVER_HH
#define IRESOLVER_HH



struct IResolver
{

  struct
  {
    std::function< ::Behavior::type()> resolve_ke_from_acc;
    std::function< ::Behavior::type()> resolve_re_from_ang_vel;
    std::function< ::Behavior::type()> resolve_arm_str;
    std::function< ::Behavior::type()> resolve_arm_pos;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IResolver(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.resolve_ke_from_acc) throw dzn::binding_error(meta, "in.resolve_ke_from_acc");
    if (! in.resolve_re_from_ang_vel) throw dzn::binding_error(meta, "in.resolve_re_from_ang_vel");
    if (! in.resolve_arm_str) throw dzn::binding_error(meta, "in.resolve_arm_str");
    if (! in.resolve_arm_pos) throw dzn::binding_error(meta, "in.resolve_arm_pos");


  }
};

inline void connect (IResolver& provided, IResolver& required)
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


#endif // IRESOLVER_HH

/********************************** INTERFACE *********************************/


//version: 2.9.1
