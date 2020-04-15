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
#ifndef IPROGRAM_HH
#define IPROGRAM_HH



struct IProgram
{
#ifndef ENUM_IProgram_State
#define ENUM_IProgram_State 1


  struct State
  {
    enum type
    {
      Initialising,Operating,Destructing
    };
  };


#endif // ENUM_IProgram_State

  struct
  {
    std::function< void()> start;
    std::function< void()> stop;
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
  inline IProgram(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.start) throw dzn::binding_error(meta, "in.start");
    if (! in.stop) throw dzn::binding_error(meta, "in.stop");
    if (! in.reset) throw dzn::binding_error(meta, "in.reset");
    if (! in.trigger_red) throw dzn::binding_error(meta, "in.trigger_red");
    if (! in.trigger_blue) throw dzn::binding_error(meta, "in.trigger_blue");

    if (! out.initialise_framebuffer) throw dzn::binding_error(meta, "out.initialise_framebuffer");
    if (! out.destruct_framebuffer) throw dzn::binding_error(meta, "out.destruct_framebuffer");
    if (! out.light_led) throw dzn::binding_error(meta, "out.light_led");

  }
};

inline void connect (IProgram& provided, IProgram& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}


#ifndef ENUM_TO_STRING_IProgram_State
#define ENUM_TO_STRING_IProgram_State 1
inline std::string to_string(::IProgram::State::type v)
{
  switch(v)
  {
    case ::IProgram::State::Initialising: return "State_Initialising";
    case ::IProgram::State::Operating: return "State_Operating";
    case ::IProgram::State::Destructing: return "State_Destructing";

  }
  return "";
}
#endif // ENUM_TO_STRING_IProgram_State

#ifndef STRING_TO_ENUM_IProgram_State
#define STRING_TO_ENUM_IProgram_State 1
inline ::IProgram::State::type to_IProgram_State(std::string s)
{
  static std::map<std::string, ::IProgram::State::type> m = {
    {"State_Initialising", ::IProgram::State::Initialising},
    {"State_Operating", ::IProgram::State::Operating},
    {"State_Destructing", ::IProgram::State::Destructing},
  };
  return m.at(s);
}
#endif // STRING_TO_ENUM_IProgram_State


#endif // IPROGRAM_HH

/********************************** INTERFACE *********************************/
/********************************** COMPONENT *********************************/
#ifndef PROGRAM_HH
#define PROGRAM_HH




struct Program
{
  dzn::meta dzn_meta;
  dzn::runtime& dzn_rt;
  dzn::locator const& dzn_locator;
#ifndef ENUM_Program_State
#define ENUM_Program_State 1


  struct State
  {
    enum type
    {
      Initialising,Operating,Destructing
    };
  };


#endif // ENUM_Program_State

  ::Program::State::type state;
  unsigned color_red;
  unsigned color_blue;
  struct fb_t* fb;


  std::function<void ()> out_iProgram;

  ::IProgram iProgram;



  Program(const dzn::locator&);
  void check_bindings() const;
  void dump_tree(std::ostream& os) const;
  friend std::ostream& operator << (std::ostream& os, const Program& m)  {
    (void)m;
    return os << "[" << m.state <<", " << m.color_red <<", " << m.color_blue <<", " << m.fb <<"]" ;
  }
  private:
  void iProgram_start();
  void iProgram_stop();
  void iProgram_reset();
  void iProgram_trigger_red(struct fb_t*& fbx);
  void iProgram_trigger_blue(struct fb_t*& fbx);

};

#endif // PROGRAM_HH

/********************************** COMPONENT *********************************/


//version: 2.9.1
