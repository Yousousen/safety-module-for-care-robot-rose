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
#ifndef ICONTROLLER_HH
#define ICONTROLLER_HH



struct IController
{
#ifndef ENUM_IController_State
#define ENUM_IController_State 1


  struct State
  {
    enum type
    {
      Initialising,Operating,Destructing
    };
  };


#endif // ENUM_IController_State

  struct
  {
    std::function< void()> initialise;
    std::function< void()> destruct;
    std::function< void()> reset;
    std::function< void(struct fb_t*&)> light_red;
    std::function< void(struct fb_t*&)> light_blue;
    std::function< void(struct fb_t*&)> safe_acceleration;
    std::function< void(struct fb_t*&)> unsafe_acceleration;
  } in;

  struct
  {
    std::function< void()> initialise_framebuffer;
    std::function< void()> destruct_framebuffer;
    std::function< void(struct fb_t*,unsigned)> light_led;
  } out;

  dzn::port::meta meta;
  inline IController(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.initialise) throw dzn::binding_error(meta, "in.initialise");
    if (! in.destruct) throw dzn::binding_error(meta, "in.destruct");
    if (! in.reset) throw dzn::binding_error(meta, "in.reset");
    if (! in.light_red) throw dzn::binding_error(meta, "in.light_red");
    if (! in.light_blue) throw dzn::binding_error(meta, "in.light_blue");
    if (! in.safe_acceleration) throw dzn::binding_error(meta, "in.safe_acceleration");
    if (! in.unsafe_acceleration) throw dzn::binding_error(meta, "in.unsafe_acceleration");

    if (! out.initialise_framebuffer) throw dzn::binding_error(meta, "out.initialise_framebuffer");
    if (! out.destruct_framebuffer) throw dzn::binding_error(meta, "out.destruct_framebuffer");
    if (! out.light_led) throw dzn::binding_error(meta, "out.light_led");

  }
};

inline void connect (IController& provided, IController& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}


#ifndef ENUM_TO_STRING_IController_State
#define ENUM_TO_STRING_IController_State 1
inline std::string to_string(::IController::State::type v)
{
  switch(v)
  {
    case ::IController::State::Initialising: return "State_Initialising";
    case ::IController::State::Operating: return "State_Operating";
    case ::IController::State::Destructing: return "State_Destructing";

  }
  return "";
}
#endif // ENUM_TO_STRING_IController_State

#ifndef STRING_TO_ENUM_IController_State
#define STRING_TO_ENUM_IController_State 1
inline ::IController::State::type to_IController_State(std::string s)
{
  static std::map<std::string, ::IController::State::type> m = {
    {"State_Initialising", ::IController::State::Initialising},
    {"State_Operating", ::IController::State::Operating},
    {"State_Destructing", ::IController::State::Destructing},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_IController_State


#endif // ICONTROLLER_HH

/********************************** INTERFACE *********************************/
/********************************** COMPONENT *********************************/
#ifndef CONTROLLER_HH
#define CONTROLLER_HH




struct Controller
{
  dzn::meta dzn_meta;
  dzn::runtime& dzn_rt;
  dzn::locator const& dzn_locator;
#ifndef ENUM_Controller_State
#define ENUM_Controller_State 1


  struct State
  {
    enum type
    {
      Initialising,Operating,Destructing,Safe,Unsafe
    };
  };


#endif // ENUM_Controller_State

  ::Controller::State::type state;
  unsigned color_red;
  unsigned color_blue;
  struct fb_t* fb;


  std::function<void ()> out_iController;

  ::IController iController;



  Controller(const dzn::locator&);
  void check_bindings() const;
  void dump_tree(std::ostream& os) const;
  friend std::ostream& operator << (std::ostream& os, const Controller& m)  {
    (void)m;
    return os << "[" << m.state <<", " << m.color_red <<", " << m.color_blue <<", " << m.fb <<"]" ;
  }
  private:
  void iController_initialise();
  void iController_destruct();
  void iController_reset();
  void iController_light_red(struct fb_t*& fbx);
  void iController_light_blue(struct fb_t*& fbx);
  void iController_safe_acceleration(struct fb_t*& fbx);
  void iController_unsafe_acceleration(struct fb_t*& fbx);

  void trigger_red (struct fb_t*& fb);
  void trigger_blue (struct fb_t*& fb);
};

#endif // CONTROLLER_HH

/********************************** COMPONENT *********************************/


//version: 2.9.1
