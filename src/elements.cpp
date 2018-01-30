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
 : ok_convert_ac3ita_aac(false),
   want_convert_ac3ita_aac(false),
   want_keep_ac3(false),
   want_keep_dolby(false)
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
   langid(),
   uid(),
   num(),
   tid(-1),
   codecname(),
   codecid(codecid_unknown),
   original_forced(false),
   want_forced(false),
   original_default(false),
   want_default(false)
 {}

