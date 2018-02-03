#include <cstdlib>
#include "model.h"
#include "elements.h"

media_t::media_t()
 : isinit(false),
   isready(false),
   name(),
   directory(),
   path(),
   outname(),
   outdirectory(),
   outpath(),
   title(),
   origitems(),
   destitems(),
   pt(),
   err()
 {}

err_t::err_t()
 : scan(false),
   scan_description(),
   conv(false),
   conv_description()
 {}

coreitem_t::coreitem_t()
 : type(itemtype_unknown),
   name(),
   lang(),
   langid(),
   uid(),
   num(),
   tid(-1),
   codecname(),
   codecid(codecid_unknown),
   isforced(false),
   isdefault(false)
 {}

destitem_t::destitem_t(const origitem_t& orig)
 : coreitem_t(orig),
   orig(orig),
   want(true),
   outpath()
 {}

