find_path(PCRECPP_INCLUDE_DIR NAMES pcrecpp.h)
mark_as_advanced(PCRECPP_INCLUDE_DIR)

find_library(PCRECPP_LIBRARY NAMES libpcrecpp.a)
mark_as_advanced(PCRECPP_LIBRARY)

find_library(PCRE_LIBRARY NAMES libpcre.a)
mark_as_advanced(PCRE_LIBRARY)

# # handle the QUIETLY and REQUIRED arguments and set PCRECPP_FOUND to TRUE if
# # all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PCRECPP DEFAULT_MSG PCRECPP_LIBRARY PCRE_LIBRARY PCRECPP_INCLUDE_DIR)

#message("HAVE ${PCRECPP_FOUND} ${PCRECPP_INCLUDE_DIRS} ${PCRECPP_LIBRARY} ${PCRE_LIBRARY}")
if(PCRECPP_FOUND)
  set(PCRECPP_LIBRARIES ${PCRECPP_LIBRARY} ${PCRE_LIBRARY})
  set(PCRECPP_INCLUDE_DIRS ${PCRECPP_INCLUDE_DIR})
endif(PCRECPP_FOUND)
