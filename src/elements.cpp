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
   isforced(false),
   isdefault(false)
 {}

item_t::item_t(const item_t& i)
 : type(i.type),
   name(i.name),
   lang(i.lang),
   langid(i.langid),
   uid(i.uid),
   num(i.num),
   tid(i.tid),
   codecname(i.codecname),
   codecid(i.codecid),
   isforced(i.isforced),
   isdefault(i.isdefault)
 {}

newitem_t::newitem_t(const item_t& backitem, size_t backn)
 : item_t(backitem),
   backitem(backitem),
   backn(backn),
   want(true)
{
	/*TODO error checking */
}
