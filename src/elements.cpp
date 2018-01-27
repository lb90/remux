#include <cstdlib>
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
   opt(),
   items(),
   pt(),
   err()
 {}

opt_t::opt_t()
 : convert_ac3ita_aac(false),
   leave_ac3(false),
   leave_dolby(false)
 {}

err_t::err_t()
 : scan(false),
   scan_description(),
   conv(false),
   conv_description()
 {}

item_t::item_t()
 : type(itemtype_unknown),
   name(),
   lang(),
   uid(),
   num(),
   tid(-1),
   codecname(),
   codectype(codectype_unknown),
   orig_forced(false),
   want_forced(false),
   orig_default(false),
   want_default(false)
 {}

