# The `:;' works around a Bash 3.2 bug when the output is not writable.
%D%/package.m4: $(top_srcdir)/configure.ac
	:;{						\
	     echo '# Signature of the current package.'	\
	  && echo 'm4_define([AT_PACKAGE_NAME],'	\
	  && echo '  [$(PACKAGE_NAME)])'		\
	  && echo 'm4_define([AT_PACKAGE_TARNAME],'	\
	  && echo '  [$(PACKAGE_TARNAME)])'		\
	  && echo 'm4_define([AT_PACKAGE_VERSION],'	\
	  && echo '  [$(PACKAGE_VERSION)])'		\
	  && echo 'm4_define([AT_PACKAGE_STRING],'	\
	  && echo '  [$(PACKAGE_STRING)])'		\
	  && echo 'm4_define([AT_PACKAGE_BUGREPORT],'	\
	  && echo '  [$(PACKAGE_BUGREPORT)])'		\
	  && echo 'm4_define([AT_PACKAGE_URL],'		\
	  && echo '  [$(PACKAGE_URL)])'			\
	;} > $@

EXTRA_DIST += %D%/testsuite.at %D%/package.m4 %D%/$(TESTSUITE) %D%/atlocal.in
DISTCLEANFILES  += %D%/atconfig %D%/testsuite.log

TESTSUITE = testsuite

AUTOTEST_PATH = $(abs_builddir)/%D%

check-local: %D%/atconfig %D%/atlocal %D%/$(TESTSUITE)
	cd %D%; \
	if test -f '$(TESTSUITE)' ; then d=; else d='$(abs_top_srcdir)/%D%/' ; fi ;\
	AUTOTEST_PATH=`echo $(AUTOTEST_PATH) | sed 's/ :/:/g'` && \
	$(SHELL) $$d'$(TESTSUITE)' AUTOTEST_PATH="$$AUTOTEST_PATH" $(TESTSUITEFLAGS)

installcheck-local: %D%/atconfig  %D%/atlocal %D%/$(TESTSUITE)
	cd %D%; \
	if test -f '$(TESTSUITE)' ; then d=; else d='$(abs_top_srcdir)/%D%/' ; fi ;\
	AUTOTEST_PATH=`echo $(AUTOTEST_PATH) | sed 's/ :/:/g'` && \
	$(SHELL) $$d'$(TESTSUITE)' AUTOTEST_PATH="$$AUTOTEST_PATH" $(TESTSUITEFLAGS)

clean-local:
	test ! -f '%D%/$(TESTSUITE)' || $(SHELL) '%D%/$(TESTSUITE)' --clean

AUTOM4TE = $(SHELL) $(top_srcdir)/build-aux/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest

TESTSUITE_DEPS  = %D%/testsuite.at %D%/package.m4
%D%/$(TESTSUITE): $(TESTSUITE_DEPS)
	$(AUTOTEST) -I '$(srcdir)' -I '$(srcdir)/%D%' -I '%D%' -o $@.tmp $@.at
	mv $@.tmp $@

