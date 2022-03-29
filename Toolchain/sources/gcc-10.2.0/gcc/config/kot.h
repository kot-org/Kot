#undef TARGET_KOT
#define TARGET_KOT 1

#undef LIB_SPEC
#define LIB_SPEC "-lc" 

#undef STANDARD_STARTFILE_PREFIX
#define STANDARD_STARTFILE_PREFIX "/lib/"

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "%{!shared:crt0.o%s} crti.o%s %{shared:crtbeginS.o%s;:crtbegin.o%s}"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "%{shared:crtendS.o%s;:crtend.o%s} crtn.o%s"

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()          \
  do{                                     \
    builtin_define ("__kot__");           \
    builtin_define ("__unix__");          \
    builtin_assert ("system=kot");        \
    builtin_assert ("system=unix");       \
    builtin_assert ("system=posix");      \
  }while(0);