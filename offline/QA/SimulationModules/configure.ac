AC_INIT(simqa_modules,[1.00])
AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE

AC_PROG_CXX(CC g++)

LT_INIT([disable-static])

CXXFLAGS="$CXXFLAGS -Wall -Wextra -Wshadow -Werror"
case $CXX in
 clang++)
  CXXFLAGS="$CXXFLAGS -Wno-macro-redefined -Wno-deprecated-copy -Wno-bitwise-instead-of-logical -Wno-unknown-warning-option"
 ;;
esac

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
