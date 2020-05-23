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
#ifndef IACCELERATIONSENSOR_HH
#define IACCELERATIONSENSOR_HH



struct IAccelerationSensor
{

  struct
  {
    std::function< void()> retrieve_ke_from_acc;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IAccelerationSensor(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.retrieve_ke_from_acc) throw dzn::binding_error(meta, "in.retrieve_ke_from_acc");


  }
};

inline void connect (IAccelerationSensor& provided, IAccelerationSensor& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}





#endif // IACCELERATIONSENSOR_HH

/********************************** INTERFACE *********************************/
/********************************** INTERFACE *********************************/
#ifndef IANGULARVELOCITYSENSOR_HH
#define IANGULARVELOCITYSENSOR_HH



struct IAngularVelocitySensor
{

  struct
  {
    std::function< void()> retrieve_re_from_ang_vel;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IAngularVelocitySensor(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.retrieve_re_from_ang_vel) throw dzn::binding_error(meta, "in.retrieve_re_from_ang_vel");


  }
};

inline void connect (IAngularVelocitySensor& provided, IAngularVelocitySensor& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}





#endif // IANGULARVELOCITYSENSOR_HH

/********************************** INTERFACE *********************************/
/********************************** INTERFACE *********************************/
#ifndef IARMFORCESENSOR_HH
#define IARMFORCESENSOR_HH



struct IArmForceSensor
{

  struct
  {
    std::function< void()> retrieve_arm_force;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IArmForceSensor(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.retrieve_arm_force) throw dzn::binding_error(meta, "in.retrieve_arm_force");


  }
};

inline void connect (IArmForceSensor& provided, IArmForceSensor& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}





#endif // IARMFORCESENSOR_HH

/********************************** INTERFACE *********************************/
/********************************** INTERFACE *********************************/
#ifndef IARMTORQUESENSOR_HH
#define IARMTORQUESENSOR_HH



struct IArmTorqueSensor
{

  struct
  {
    std::function< void()> retrieve_arm_torque;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IArmTorqueSensor(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.retrieve_arm_torque) throw dzn::binding_error(meta, "in.retrieve_arm_torque");


  }
};

inline void connect (IArmTorqueSensor& provided, IArmTorqueSensor& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}





#endif // IARMTORQUESENSOR_HH

/********************************** INTERFACE *********************************/
/********************************** INTERFACE *********************************/
#ifndef IARMPOSITIONSENSOR_HH
#define IARMPOSITIONSENSOR_HH



struct IArmPositionSensor
{

  struct
  {
    std::function< void()> retrieve_arm_pos;
  } in;

  struct
  {
  } out;

  dzn::port::meta meta;
  inline IArmPositionSensor(const dzn::port::meta& m) : meta(m) {}

  void check_bindings() const
  {
    if (! in.retrieve_arm_pos) throw dzn::binding_error(meta, "in.retrieve_arm_pos");


  }
};

inline void connect (IArmPositionSensor& provided, IArmPositionSensor& required)
{
  provided.out = required.out;
  required.in = provided.in;
  provided.meta.requires = required.meta.requires;
  required.meta.provides = provided.meta.provides;
}





#endif // IARMPOSITIONSENSOR_HH

/********************************** INTERFACE *********************************/


//version: 2.9.1
