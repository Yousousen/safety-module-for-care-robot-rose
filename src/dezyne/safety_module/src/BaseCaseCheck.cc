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
#include "BaseCaseCheck.hh"

#include <dzn/locator.hh>
#include <dzn/runtime.hh>



BaseCaseCheck::BaseCaseCheck(const dzn::locator& dzn_locator)
: dzn_meta{"","BaseCaseCheck",0,0,{},{},{[this]{iRoot.check_bindings();}}}
, dzn_rt(dzn_locator.get<dzn::runtime>())
, dzn_locator(dzn_locator)


, iRoot({{"iRoot",this,&dzn_meta},{"",0,0}})



{
  dzn_rt.performs_flush(this) = true;



  iRoot.in.do_check = [&](){return dzn::call_in(this,[=]{ dzn_locator.get<dzn::runtime>().skip_block(&this->iRoot) = false; return iRoot_do_check();}, this->iRoot.meta, "do_check");};



}

::Behavior::type BaseCaseCheck::iRoot_do_check()
{
  { this->reply_Behavior = ::Behavior::Safe; }

  return this->reply_Behavior;
}


void BaseCaseCheck::check_bindings() const
{
  dzn::check_bindings(&dzn_meta);
}
void BaseCaseCheck::dump_tree(std::ostream& os) const
{
  dzn::dump_tree(os, &dzn_meta);
}




//version: 2.9.1