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

  struct
  {
    std::function< ::Behavior::type()> check_arm_position;
    std::function< ::Behavior::type()> check_arm_strength;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IGripArmControl(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.check_arm_position) throw dzn::binding_error(meta, "in.check_arm_position");
    if (! in.check_arm_strength) throw dzn::binding_error(meta, "in.check_arm_strength");


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


#endif // IGRIPARMCONTROL_HH

/********************************** INTERFACE *********************************/
/********************************** INTERFACE *********************************/
#ifndef IGRIPARMSENSOR_HH
#define IGRIPARMSENSOR_HH



struct IGripArmSensor
{

  struct
  {
    std::function< void()> retrieve_arm_str;
    std::function< void()> retrieve_arm_pos;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IGripArmSensor(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.retrieve_arm_str) throw dzn::binding_error(meta, "in.retrieve_arm_str");
    if (! in.retrieve_arm_pos) throw dzn::binding_error(meta, "in.retrieve_arm_pos");


  }
};

inline void connect (IGripArmSensor& provided, IGripArmSensor& required)
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


#endif // IGRIPARMSENSOR_HH

/********************************** INTERFACE *********************************/
/********************************** COMPONENT *********************************/
#ifndef GRIPARMCONTROL_HH
#define GRIPARMCONTROL_HH

#include "Resolver.hh"



struct GripArmControl
{
  dzn::meta dzn_meta;
  dzn::runtime& dzn_rt;
  dzn::locator const& dzn_locator;


  ::Behavior::type reply_Behavior;

  std::function<void ()> out_iGripArmControl;

  ::IGripArmControl iGripArmControl;

  ::IGripArmSensor iGripArmSensor;
  ::IResolver iResolver;


  GripArmControl(const dzn::locator&);
  void check_bindings() const;
  void dump_tree(std::ostream& os) const;
  friend std::ostream& operator << (std::ostream& os, const GripArmControl& m)  {
    (void)m;
    return os << "[" << "]" ;
  }
  private:
  ::Behavior::type iGripArmControl_check_arm_position();
  ::Behavior::type iGripArmControl_check_arm_strength();

};

#endif // GRIPARMCONTROL_HH

/********************************** COMPONENT *********************************/


//version: 2.9.1
