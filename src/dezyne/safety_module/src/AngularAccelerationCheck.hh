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
#ifndef IANGULARACCELERATIONSENSOR_HH
#define IANGULARACCELERATIONSENSOR_HH



struct IAngularAccelerationSensor
{

  struct
  {
    std::function< void()> retrieve_re_from_ang_acc;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IAngularAccelerationSensor(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.retrieve_re_from_ang_acc) throw dzn::binding_error(meta, "in.retrieve_re_from_ang_acc");


  }
};

inline void connect (IAngularAccelerationSensor& provided, IAngularAccelerationSensor& required)
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


#endif // IANGULARACCELERATIONSENSOR_HH

/********************************** INTERFACE *********************************/
/********************************** COMPONENT *********************************/
#ifndef ANGULARACCELERATIONCHECK_HH
#define ANGULARACCELERATIONCHECK_HH

#include "ISafetyCheck.hh"
#include "ISafetyCheck.hh"
#include "Resolver.hh"



struct AngularAccelerationCheck
{
  dzn::meta dzn_meta;
  dzn::runtime& dzn_rt;
  dzn::locator const& dzn_locator;


  ::Behavior::type reply_Behavior;

  std::function<void ()> out_iAngularAccelerationCheck;

  ::ISafetyCheck iAngularAccelerationCheck;

  ::IAngularAccelerationSensor iAngularAccelerationSensor;
  ::ISafetyCheck iNext;
  ::IResolver iResolver;


  AngularAccelerationCheck(const dzn::locator&);
  void check_bindings() const;
  void dump_tree(std::ostream& os) const;
  friend std::ostream& operator << (std::ostream& os, const AngularAccelerationCheck& m)  {
    (void)m;
    return os << "[" << "]" ;
  }
  private:
  ::Behavior::type iAngularAccelerationCheck_do_check();

  ::Behavior::type and_safety_states (::Behavior::type current,::Behavior::type next);
};

#endif // ANGULARACCELERATIONCHECK_HH

/********************************** COMPONENT *********************************/


//version: 2.9.1