##============================================================================
## inst_vars.mk

INST_VARS_MK =

## our version of inst-vars.am

strip_dir	= `echo $$p | sed -e 's|^.*/||'`;
strip_sfx	= `echo $$p | sed -e 's|[.][^.]*$$||'`
mst__dirname    = `expr "$$p" : "\(.*\)/[^/]*"`
mst__basename   = `expr "$$p" : ".*/\([^/]*\)$$" || echo "$$p"`
mst__strip_sfx  = `expr "$$p" : "\(.*\)\.[^./]*"`
mst__strip_pfx  = `expr "$$p" : "^$$D\(.*\)"`
mst__dirstamp   = $(am_leading_dot)/dirstamp
